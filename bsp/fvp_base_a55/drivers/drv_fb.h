/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020/12/31     Bernard      Add license info
 */
#ifndef DRV_FB_H__
#define DRV_FB_H__

#include <rtthread.h>
#include <rtdevice.h>


#define BSP_LCD_WIDTH   320

#define BSP_LCD_HEIGHT  160

int drv_clcd_hw_init(void);

#endif