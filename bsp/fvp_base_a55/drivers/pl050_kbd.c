/*
 * ARM PL050 Keyboard Controller Driver
 *
 * Copyright (c) 2026, RT-Thread Development Team
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pl050_kbd.h"
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/input.h>

#define KBD_REG(offset) (*(volatile rt_uint32_t *)((rt_size_t)kbd->base + (offset)))

#define KBD_CTRL      0x00
#define KBD_STATUS    0x04
#define KBD_DATA      0x08
#define KBD_CLKDIV    0x0C
#define KBD_INT       0x10

#define KBD_STATUS_RXFULL (1 << 4)
#define KBD_CTRL_EN       (1 << 2)
#define KBD_INT_RX        (1 << 4)

static struct pl050_kbd *kbd = RT_NULL;

static void pl050_kbd_isr(int vector, void *param)
{
    static rt_uint32_t last_status = 0;
    rt_uint32_t status;
    rt_uint8_t scancode;

    RT_UNUSED(vector);
    RT_UNUSED(param);

    status = KBD_REG(KBD_STATUS);
    while (status & KBD_STATUS_RXFULL)
    {
        scancode = (rt_uint8_t)KBD_REG(KBD_DATA);
        if (scancode == 0xF0)
        {   
            last_status = 0xF0;
        }
        else
        {
            if(last_status == 0xF0)
            {
                //rt_kprintf("Kr:0x%02X\n", scancode);
                rt_input_event(&kbd->input, EV_KEY, scancode, 0); // Key release
                rt_input_sync(&kbd->input);
                last_status = 0;
            }
            else
            {
                // Simple: send scancode as key event
                //rt_kprintf("Kp:0x%02X\n", scancode);
                rt_input_event(&kbd->input, EV_KEY, scancode, 1);  // Key press
                rt_input_sync(&kbd->input);
            }
        }
        // For release, you may need to decode protocol
        status = KBD_REG(KBD_STATUS);
    }
}

static rt_err_t pl050_kbd_poll(struct rt_input_device *idev)
{
    // Optional: polling mode, not used if interrupt
    return RT_EOK;
}

int pl050_kbd_init(void)
{
    rt_err_t err;
    rt_uint16_t scancode;

    if (kbd)
    {
        return -RT_EBUSY;
    }

    kbd = rt_calloc(1, sizeof(struct pl050_kbd));
    if (!kbd)
    {
        return -RT_ENOMEM;
    }

    kbd->base = (unsigned int*)rt_ioremap((void*)PL050_KBD_BASE, 0x1000);
    if (!kbd->base)
    {
        err = -RT_ENOMEM;
        goto fail_alloc;
    }

    // Enable controller
    KBD_REG(KBD_CTRL) = KBD_CTRL_EN | KBD_INT_RX;

    // Register interrupt
    rt_hw_interrupt_install(PL050_KBD_IRQ, pl050_kbd_isr, RT_NULL, "pl050_kbd");
    rt_hw_interrupt_umask(PL050_KBD_IRQ);

    // Setup input device
    kbd->input.parent.type = RT_Device_Class_Char;
    kbd->input.parent.rx_indicate = RT_NULL;
    kbd->input.parent.tx_complete = RT_NULL;
    kbd->input.poller = RT_NULL;
    kbd->input.trigger = RT_NULL;
    for (scancode = 0; scancode <= 0xFF; ++scancode)
    {
        rt_input_set_capability(&kbd->input, EV_KEY, scancode);
    }

    err = rt_input_device_register(&kbd->input);
    if (err != RT_EOK)
    {
        goto fail_ioremap;
    }

    return 0;

fail_ioremap:
    rt_iounmap(kbd->base);
fail_alloc:
    rt_free(kbd);
    kbd = RT_NULL;

    return err;
}

INIT_DEVICE_EXPORT(pl050_kbd_init);
