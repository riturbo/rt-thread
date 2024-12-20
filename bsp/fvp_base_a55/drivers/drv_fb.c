/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020/12/31     Bernard      Add license info
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <dfs_file.h>
#include <rtthread.h>
#ifdef RT_USING_SMART
#include <lwp.h>
#include <lwp_user_mm.h>
#endif
#include <board.h>

#include "drv_fb.h"
#include "drivers/lcd.h"

#define CLCD_WIDTH  (BSP_LCD_WIDTH)
#define CLCD_HEIGHT (BSP_LCD_HEIGHT)

#define CLCD_DEVICE(dev)    (struct drv_clcd_device*)(dev)


struct drv_clcd_device
{
    struct rt_device parent;

    int width;
    int height;

    uint8_t *fb;
};
struct drv_clcd_device _lcd;

void init_lcd_ve( void )
{
    // VE System Register 32-bit word offsets
    const int VE_SYS_CFG_DATA = (0xA0/4);
    const int VE_SYS_CFG_CTRL = (0xA4/4);

    volatile unsigned int*  ve_sysreg =(unsigned int*)rt_ioremap((void*)0x1C010000, 0x1000);

    // Set CLCD clock
    //   SYS_CFG_DATA sets oscillator rate value as 5.4MHz
    //   SYS_CFG_CTRL( start=1 | write=1 | function=1 | site=0 | position=0 | device=1 )
    ve_sysreg[ VE_SYS_CFG_DATA ] = 5400000;
    ve_sysreg[ VE_SYS_CFG_CTRL ] = 0x80000000 | (1<<30) | (1<<20) | (0<<16) | (0<<12) | (1<<0);

    // Set DVI mux for correct MMB
    //   SYS_CFG_CTRL( start=1 | write=1 | function=7 | site=0 | position=0 | device=0 )
    ve_sysreg[ VE_SYS_CFG_DATA ] = 0;  // VE_DVI_MUX
    ve_sysreg[ VE_SYS_CFG_CTRL ] = 0x80000000 | (1<<30) | (7<<20) | (0<<16) | (0<<12) | (0<<0);
}


// Configure PL111 CLCD
void init_pl111( unsigned int  width,
                 unsigned int  height,
                 unsigned int  frame_base )
{
    volatile unsigned int*  clcd  = (unsigned int*)rt_ioremap(0x1C1F0000,0x1000);  // CS3 + 0x1F0000

    // PL111 register offsets (32-bit words)
    const int PL111_TIM0 = (0x00/4);
    const int PL111_TIM1 = (0x04/4);
    const int PL111_TIM2 = (0x08/4);
    const int PL111_TIM3 = (0x0C/4);
    const int PL111_UBAS = (0x10/4);
    const int PL111_LBAS = (0x14/4);
    const int PL111_CNTL = (0x18/4);
    const int PL111_IENB = (0x1C/4);

    // Timing number for an 8.4" LCD screen for use on a VGA screen
    unsigned int TIM0_VAL = ( (((width/16)-1)<<2) | (63<<8) | (31<<16) | (63<<8) );
    unsigned int TIM1_VAL = ( (height - 1) | (24<<10) | (11<<16) | (9<<24) );
    unsigned int TIM2_VAL = ( (0x7<<11) | ((width - 1)<<16) | (1<<26) );

    // Program the CLCD controller registers and start the CLCD
    clcd[ PL111_TIM0 ] = TIM0_VAL;
    clcd[ PL111_TIM1 ] = TIM1_VAL;
    clcd[ PL111_TIM2 ] = TIM2_VAL;
    clcd[ PL111_TIM3 ] = 0;
    clcd[ PL111_UBAS ] = frame_base;
    clcd[ PL111_LBAS ] = 0;
    clcd[ PL111_IENB ] = 0;

    // Set the control register: 16BPP 5:6:5, Power OFF
    clcd[ PL111_CNTL ] = (1<<0) | (6<<1) | (1<<5);

    // Power ON
    clcd[ PL111_CNTL ] |= (1<<11);
}
static rt_err_t drv_clcd_init(struct rt_device *device)
{
    struct drv_clcd_device *lcd = CLCD_DEVICE(device);

    (void)lcd; /* nothing, right now */
    return RT_EOK;
}

static rt_err_t drv_clcd_control(struct rt_device *device, int cmd, void *args)
{
    struct drv_clcd_device *lcd = CLCD_DEVICE(device);

    switch (cmd)
    {
    case RTGRAPHIC_CTRL_RECT_UPDATE:
        {
            struct rt_device_rect_info *info = (struct rt_device_rect_info*)args;

            info = info; /* nothing, right now */
        }
        break;

    case RTGRAPHIC_CTRL_GET_INFO:
        {
            struct rt_device_graphic_info* info = (struct rt_device_graphic_info*)args;

            RT_ASSERT(info != RT_NULL);
            info->pixel_format  = RTGRAPHIC_PIXEL_FORMAT_RGB565;
            info->bits_per_pixel= 16;
            info->width         = lcd->width;
            info->height        = lcd->height;
            info->framebuffer   = lcd->fb;
        }
        break;

    case FBIOGET_FSCREENINFO:
    {
#ifdef RT_USING_SMART
        struct fb_fix_screeninfo *info = (struct fb_fix_screeninfo *)args;
        strncpy(info->id, "fb0", sizeof(info->id));
        info->smem_len    = lcd->width * lcd->height * 2;
        info->smem_start  = lwp_map_user_phy(lwp_self(), RT_NULL, lcd->fb,
            info->smem_len, 1);
        info->line_length = lcd->width * 2;
#endif
    }
        break;

    case FBIOGET_VSCREENINFO:
    {
        struct fb_var_screeninfo *info = (struct fb_var_screeninfo *)args;
        info->bits_per_pixel = 16;
        info->xres = lcd->width;
        info->yres = lcd->height;
    }
        break;

    case FBIOGET_DISPINFO:
        break;
    case RT_FIOMMAP2:
        {
            struct dfs_mmap2_args *mmap2 = (struct dfs_mmap2_args *)args;
            if(mmap2)
            {
                mmap2->ret = lwp_map_user_phy(lwp_self(), RT_NULL, rt_kmem_v2p(_lcd.fb), mmap2->length, 1);
            }
            else
            {
                return -EIO;
            }
            break;
        }
    case FBIOGET_PIXELINFO:
        int *pixel_info = (int *)args;
        *pixel_info = RTGRAPHIC_PIXEL_FORMAT_RGB565;
        break;
    }

    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops clcd_ops =
{
    drv_clcd_init,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    drv_clcd_control
};
#endif

int drv_clcd_hw_init(void)
{
    struct rt_device *device = &_lcd.parent;

    /* memset _lcd to zero */
    memset(&_lcd, 0x0, sizeof(_lcd));

    _lcd.width  = CLCD_WIDTH;
    _lcd.height = CLCD_HEIGHT;
    rt_kprintf("try to allocate fb... | w - %d, h - %d | ", _lcd.width, _lcd.height);
#ifdef RT_USING_SMART
    _lcd.fb = rt_pages_alloc(rt_page_bits(_lcd.width * _lcd.height * 2));
#else
    _lcd.fb = rt_malloc(_lcd.width * _lcd.height * 2);
#endif
    rt_kprintf("done!\n");
    rt_kprintf("fb => 0x%08x\n", _lcd.fb);
    if (_lcd.fb == NULL)
    {
        rt_kprintf("initialize frame buffer failed!\n");
        return -1;
    }
    memset(_lcd.fb, 0xffff, _lcd.width * _lcd.height * 2);

    init_lcd_ve();
    init_pl111(_lcd.width, _lcd.height, (uint32_t)_lcd.fb + PV_OFFSET);


    device->type    = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    device->ops     = &clcd_ops;
#else
    device->init    = drv_clcd_init;
    device->control = drv_clcd_control;
#endif

    rt_device_register(device, "fb0", RT_DEVICE_FLAG_RDWR);

    return 0;
}
INIT_DEVICE_EXPORT(drv_clcd_hw_init);

