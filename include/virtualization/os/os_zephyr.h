/*
 * Copyright 2021-2022 HNU
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ZVM_OS_ZEPHYR_H_
#define __ZVM_OS_ZEPHYR_H_

#include <zephyr.h>
#include <stdint.h>
#include <virtualization/arm/mm.h>

#define ZEPHYR_VM_MEM_BASE  DT_REG_ADDR(DT_NODELABEL(zephyr_ddr))
#define ZEPHYR_VM_MEM_SIZE  DT_REG_SIZE(DT_NODELABEL(zephyr_ddr))
#define ZEPHYR_VMSYS_ENTRY  DT_PROP(DT_NODELABEL(zephyr_ddr), vm_reg)
#define ZEPHYR_VM_IMG_SIZE  DT_PROP(DT_NODELABEL(zephyr_ddr), img_sz)

/**
 * @brief load zephyr image from other memory address to allocated address
 */
int load_zephyr_image(struct vm_mem_domain *vmem_domain);

#endif /* __ZVM_OS_ZEPHYR_H_ */
