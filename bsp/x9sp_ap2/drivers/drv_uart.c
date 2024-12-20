/*
 * serial.c UART driver
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-03-30     Bernard      the first verion
 */

#include <rthw.h>
#include <rtdevice.h>

#include "board.h"
#include "mmu.h"
#include "DW_apb_uart_reg.h"

struct hw_uart_device
{
    rt_size_t hw_base;
    rt_size_t irqno;
};


#define readl(x) (*(uint32_t *)(x))
#define writel(a,x)  (*(uint32_t *)(x) = a)

static void rt_hw_uart_isr(int irqno, void *param)
{
    struct rt_serial_device *serial = (struct rt_serial_device *)param;

    rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
}

static rt_err_t uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    return RT_EOK;
}

static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct hw_uart_device *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct hw_uart_device *)serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        //UART_IMSC(uart->hw_base) &= ~UARTIMSC_RXIM;
        break;

    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        //UART_IMSC(uart->hw_base) |= UARTIMSC_RXIM;
        rt_hw_interrupt_umask(uart->irqno);
        break;

    default:
        return -1;
    }

    return RT_EOK;
}

static int uart_putc(struct rt_serial_device *serial, char c)
{
    struct hw_uart_device *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct hw_uart_device *)serial->parent.user_data;


    while (!(readl(uart->hw_base + USR_OFF) & (BM_USR_TFNF | BM_USR_TFE)));

    writel(c, uart->hw_base + THR_OFF);

    return 1;
}

static int uart_getc(struct rt_serial_device *serial)
{
    int ch;
    struct hw_uart_device *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct hw_uart_device *)serial->parent.user_data;

    ch = -1;

    if ((readl(uart->hw_base + USR_OFF) & (BM_USR_RFNE | BM_USR_RFF))) {
        ch = (uint8_t)readl(uart->hw_base + RBR_OFF);
    }

    return ch;
}

static const struct rt_uart_ops _uart_ops =
{
    uart_configure,
    uart_control,
    uart_putc,
    uart_getc,
};

#ifdef RT_USING_UART0
/* UART device driver structure */
static struct hw_uart_device _uart0_device =
{
    PL011_UART0_BASE,
    PL011_UART0_IRQNUM,
};
static struct rt_serial_device _serial0;
#endif

int rt_hw_uart_init(void)
{
    struct hw_uart_device *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

#ifdef RT_USING_UART0
    _uart0_device.hw_base = (rt_size_t)rt_ioremap((void*)_uart0_device.hw_base, PL011_UART0_SIZE);
    uart = &_uart0_device;

    _serial0.ops    = &_uart_ops;
    _serial0.config = config;

    /* register UART1 device */
    rt_hw_serial_register(&_serial0, "uart0",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          uart);
    rt_hw_interrupt_install(uart->irqno, rt_hw_uart_isr, &_serial0, "uart0");
    /* enable Rx and Tx of UART */

#endif

    return 0;
}
