/*
 * Copyright 2021-2022 HNU
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ZVM_OS_LINUX_H_
#define __ZVM_OS_LINUX_H_

#include <zephyr.h>
#include <stdint.h>
#include <virtualization/zvm.h>
#include <virtualization/arm/mm.h>
#include "os.h"

/**
 * LINUX_VM_IMAGE_BASE presents that the linux image base
 * in the ddr(@TODO: In the disk better). And LINUX_VM_IMAGE_SIZE
 * presents the linux image size in the ddr.
*/
#define LINUX_VM_IMAGE_BASE     DT_REG_ADDR(DT_NODELABEL(linux_ddr))
#define LINUX_VM_IMAGE_SIZE     DT_REG_SIZE(DT_NODELABEL(linux_ddr))
#define LINUX_VMSYS_BASE        DT_PROP(DT_NODELABEL(linux_ddr), vm_reg_base)
#define LINUX_VMSYS_SIZE        DT_PROP(DT_NODELABEL(linux_ddr), vm_reg_size)

#ifdef CONFIG_DTB_FILE_INPUT
#define LINUX_DTB_MEM_BASE        DT_PROP(DT_INST(0, linux_vm), dtb_address)
#define LINUX_DTB_MEM_SIZE        DT_PROP(DT_INST(0, linux_vm), dtb_size)
#endif /* CONFIG_DTB_FILE_INPUT */

struct vm_mem_domain;


/**
 * @brief load linux image from other memory address to allocated address
 *
 * @param vm
 * @return int :
 * -1 - error type
 * 0 -- success
 */
int load_linux_image(struct vm_mem_domain *vmem_domain);



#endif  //__ZVM_OS_LINUX_H_