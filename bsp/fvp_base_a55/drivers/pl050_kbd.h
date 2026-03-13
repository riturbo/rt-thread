/*
 * ARM PL050 Keyboard Controller Driver Header
 *
 * Copyright (c) 2026, RT-Thread Development Team
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PL050_KBD_H__
#define __PL050_KBD_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/input.h>

#define PL050_KBD_BASE   0x1C060000
#define PL050_KBD_IRQ    44

struct pl050_kbd
{
    rt_size_t *base;
    struct rt_input_device input;
};

int pl050_kbd_init(void);

#endif /* __PL050_KBD_H__ */
