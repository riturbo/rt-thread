/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-17     GuEe-GUI     the first version
 */

#ifndef VIRT_H__
#define VIRT_H__

#include <rtdef.h>
#include <ioremap.h>

#ifdef RT_USING_SMART
#include <mmu.h>
#endif

#define __REG32(x)          (*((volatile unsigned int *)(x)))
#define __REG16(x)          (*((volatile unsigned short *)(x)))

/* UART */
#define PL011_UART0_BASE    0x60520000  //uart15
#define PL011_UART0_SIZE    0x00010000
#define PL011_UART0_IRQNUM  (32 + 25)

/* RTC */
#define PL031_RTC_BASE      0x1C170000
#define PL031_RTC_SIZE      0x00010000
#define PL031_RTC_IRQNUM    (32 + 2)

/* GPIO */
//#define PL061_GPIO_BASE     0x09030000
//#define PL061_GPIO_SIZE     0x00001000
//#define PL061_GPIO_IRQNUM   (32 + 7)

/* GIC */
#define MAX_HANDLERS        300
#define GIC_IRQ_START       0
#define ARM_GIC_NR_IRQS     300
#define ARM_GIC_MAX_NR      1

#define IRQ_ARM_IPI_KICK    0
#define IRQ_ARM_IPI_CALL    1

/* GICv2 */
#define GIC_PL390_DISTRIBUTOR_PPTR      0x63E81000
#define GIC_PL390_CONTROLLER_PPTR       0x63E82000
#define GIC_PL390_HYPERVISOR_BASE       0x08030000
#define GIC_PL390_VIRTUAL_CPU_BASE      0x08040000

/* GICv3 */
#define GIC_PL500_DISTRIBUTOR_PPTR      GIC_PL390_DISTRIBUTOR_PPTR
#define GIC_PL500_REDISTRIBUTOR_PPTR    0x2F100000
#define GIC_PL500_CONTROLLER_PPTR       GIC_PL390_CONTROLLER_PPTR
#define GIC_PL500_ITS_PPTR              0x2F020000

/* the basic constants and interfaces needed by gic */
rt_inline rt_ubase_t platform_get_gic_dist_base(void)
{
#ifdef BSP_USING_GICV2
    return GIC_PL390_DISTRIBUTOR_PPTR;
#else
    return GIC_PL500_DISTRIBUTOR_PPTR;
#endif
}

rt_inline rt_ubase_t platform_get_gic_redist_base(void)
{
    return GIC_PL500_REDISTRIBUTOR_PPTR;
}

rt_inline rt_ubase_t platform_get_gic_cpu_base(void)
{
#ifdef BSP_USING_GICV2
    return GIC_PL390_CONTROLLER_PPTR;
#else
    return GIC_PL500_CONTROLLER_PPTR;
#endif
}

rt_inline rt_ubase_t platform_get_gic_its_base(void)
{
    return GIC_PL500_ITS_PPTR;
}

#endif
