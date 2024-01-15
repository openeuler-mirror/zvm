/*
 * Copyright 2021-2022 HNU
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZVM_VM_IRQ_H_
#define ZEPHYR_INCLUDE_ZVM_VM_IRQ_H_

#include <stdint.h>
#include <sys/dlist.h>
#include <arch/arm64/lib_helpers.h>
#include <virtualization/zvm.h>
#include <virtualization/arm/trap_handler.h>
#include <virtualization/vm_dev.h>

/**
 * Software irq flags, which is used to
 * set the status of virt irq desc:
 * @VIRQ_HW_FLAG: this irq desc has a related hardware device,
 * not a fully emualted interrupt.
 * @VIRQ_PENDING_FLAG: irq desc is pending, and when eret to
 * vm, this irq will assert. (1)This flag is set when virt irq is
 * set to vm or virt irq is not process by vm(but vm exit). (2)This
 * flag is unset when virt irq is inject to hardware device, which
 * will assert vm's irq.
*/
#define VIRQ_HW_FLAG                BIT(0)      /*@TODO: HW_FLAG may not enbaled for each spi.*/
#define VIRQ_PENDING_FLAG		    BIT(1)
#define VIRQ_ACTIVED_FLAG           BIT(2)
#define VIRQ_ENABLED_FLAG		    BIT(3)
#define VIRQ_WAKEUP_FLAG            BIT(4)
#define VIRQ_NOUSED_FLAG            BIT(5)

/* Hardware irq states */
#define VIRQ_STATE_INVALID		        (0b00)
#define VIRQ_STATE_PENDING		        (0b01)
#define VIRQ_STATE_ACTIVE		        (0b10)
#define VIRQ_STATE_ACTIVE_AND_PENDING	(0b11)

/* VM's injuct irq num, bind to the register */
#define VM_INVALID_DESC_ID	            (0xFF)

/* VM's irq prio */
#define VM_DEFAULT_LOCAL_VIRQ_PRIO      (0x20)

/* irq number for arm64 system. */
#define VM_LOCAL_VIRQ_NR	(VM_SGI_VIRQ_NR + VM_PPI_VIRQ_NR)
#define VM_GLOBAL_VIRQ_NR   (VM_LOCAL_VIRQ_NR + VM_SPI_VIRQ_NR)

struct vm;
struct vcpu;
struct virt_dev;

/**
 * @brief Description for each virt irq descripetor.
 */
struct virt_irq_desc {
    /* Id that describes the irq. */
    uint8_t id;
    uint8_t vcpu_id;
    uint8_t vm_id;
    uint8_t prio;
    /* software dev trigger level flag */
    uint8_t vdev_trigger;

    /* irq level type */
    uint8_t type;
    uint8_t src_cpu;

    /* hardware virq states */
    uint8_t virq_states;
    /* software virq flags */
    uint32_t virq_flags;

    /**
     * If physical irq is existed, pirq_num has
     * a value, otherwise, it is set to 0xFFFFFFFF
    */
    uint32_t pirq_num;
    uint32_t virq_num;

    sys_dnode_t desc_node;
};

/**
 * @brief vm's irq block to describe this all the device interrup
 * for vm. In this struct, it called `VM_LOCAL_VIRQ_NR`;
*/
struct vcpu_virt_irq_block {
    /**
     * record the virt irq counts which is actived,
     * when a virt irq is sent to vm, zvm should record
     * it and it means there is a virt irq need to process.
    */
    uint32_t virq_pending_counts;

    struct virt_irq_desc vcpu_virt_irq_desc[VM_LOCAL_VIRQ_NR];

    struct k_spinlock spinlock;

    sys_dlist_t active_irqs;
    sys_dlist_t pending_irqs;
};

/**
 * @brief vm's irq block to describe this all the device interrup
 * for vm. In this struct, it called `VM_GLOBAL_VIRQ_NR-VM_LOCAL_VIRQ_NR`;
*/
struct vm_virt_irq_block {

    bool enabled;
    bool irq_bitmap[VM_GLOBAL_VIRQ_NR];

    /* interrupt control block flag */
    uint32_t flags;
    uint32_t irq_num;
    uint32_t cpu_num;

	uint32_t irq_target[VM_GLOBAL_VIRQ_NR];
	uint32_t ipi_vcpu_source[CONFIG_MP_NUM_CPUS][VM_SGI_VIRQ_NR];

    /* desc for this vm */
    struct virt_irq_desc vm_virt_irq_desc[VM_GLOBAL_VIRQ_NR-VM_LOCAL_VIRQ_NR];

    /* bind to interrupt controller */
    void *virt_priv_date;
};

/**
 * @brief init the irq desc when add @vm_dev.
*/
void vm_device_irq_init(struct vm *vm, struct virt_dev *vm_dev);

/**
 * @brief init irq block for vm.
 */
int vm_irq_block_init(struct vm *vm);

#endif /* ZEPHYR_INCLUDE_ZVM_VM_IRQ_H_ */
