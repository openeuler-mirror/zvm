/*
 * Copyright 2021-2022 HNU
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZVM_ARM_VGIC_COMMON_H_
#define ZEPHYR_INCLUDE_ZVM_ARM_VGIC_COMMON_H_

#include <zephyr.h>
#include <kernel.h>
#include <devicetree.h>
#include <virtualization/vm.h>
#include <virtualization/arm/trap_handler.h>

struct virt_dev;

#define VGIC_CONTROL_BLOCK_ID		vgic_control_block
#define VGIC_CONTROL_BLOCK_NAME		vm_irq_control_block

/* GIC version */
#define VGIC_V2     BIT(0)
#define VGIC_V3     BIT(8)

/* GIC dev type */
#define TYPE_GIC_GICD		BIT(0)
#define TYPE_GIC_GICR_RD	BIT(1)
#define TYPE_GIC_GICR_SGI	BIT(2)
#define TYPE_GIC_GICR_VLPI	BIT(3)
#define TYPE_GIC_INVAILD	(0xFF)

/* GIC device macro here */
#define VGIC_DIST_BASE	DT_REG_ADDR_BY_IDX(DT_INST(0, arm_gic), 0)
#define VGIC_DIST_SIZE	DT_REG_SIZE_BY_IDX(DT_INST(0, arm_gic), 0)
#define VGIC_RDIST_BASE	DT_REG_ADDR_BY_IDX(DT_INST(0, arm_gic), 1)
#define VGIC_RDIST_SIZE	DT_REG_SIZE_BY_IDX(DT_INST(0, arm_gic), 1)

/* GICD registers offset from DIST_base(n) */
#define VGICD_CTLR			0x0000
#define VGICD_TYPER			0x0004
#define VGICD_IIDR			0x0008
#define VGICD_STATUSR		0x0010

#define VGIC_RESERVED		0x0F30
#define VGIC_INMIRn			0x0f80
#define VGICD_PIDR2			0xFFE8

/* Vgic control block flag */
#define VIRQ_HW_SUPPORT				BIT(1)

#define VGIC_VIRQ_IN_SGI			(0x0)
#define VGIC_VIRQ_IN_PPI			(0x1)
/* Sorting virt irq to SGI/PPI/SPI */
#define VGIC_VIRQ_LEVEL_SORT(irq)	((irq)/VM_SGI_VIRQ_NR)

/* VGIC Type for virtual interrupt control */
#define VGIC_TYPER_REGISTER		(read_sysreg(ICH_VTR_EL2))
#define VGIC_TYPER_LR_NUM 		((VGIC_TYPER_REGISTER & 0x1F) + 1)
#define VGIC_TYPER_PRIO_NUM		(((VGIC_TYPER_REGISTER >> 29) & 0x07) + 1)

/* 64k frame */
#define VGIC_RD_BASE_SIZE		(64 * 1024)
#define VGIC_SGI_BASE_SIZE		(64 * 1024)
#define VGIC_RD_SGI_SIZE		(VGIC_RD_BASE_SIZE+VGIC_SGI_BASE_SIZE)

/* virtual gic device register operation */
#define vgic_sysreg_read32(base, offset)			sys_read32((long unsigned int)(base+((offset)/4)))
#define vgic_sysreg_write32(data, base, offset)		sys_write32(data, (long unsigned int)(base+((offset)/4)))
#define vgic_sysreg_read64(data, base, offset)		sys_read64(data, (long unsigned int)(base+((offset)/4)))
#define vgic_sysreg_write64(data, base, offset)		sys_write64(data, (long unsigned int)(base+((offset)/4)))

/**
 * @brief Virtual generatic interrupt controller distributor
 * struct for each vm.
*/
struct virt_gic_gicd {
	/* gicd address base and size */
	uint32_t gicd_base;
	uint32_t gicd_size;
	/* virtual gicr for emulating device for vm. */
	uint32_t *gicd_regs_base;

	/* gicd spin lock */
	struct k_spinlock gicd_lock;
};


typedef int (*vgic_gicd_read_32_t)(const struct device *dev, struct vcpu *vcpu,
					uint32_t offset, uint32_t *value);

typedef int (*vgic_gicrrd_read_32_t)(const struct device *dev, struct vcpu *vcpu,
					uint32_t offset, uint32_t *value);

typedef int (*vgic_gicd_write_32_t)(const struct device *dev, struct vcpu *vcpu,
					uint32_t offset, uint32_t *value);

typedef int (*vgic_gicrrd_write_32_t)(const struct device *dev, struct vcpu *vcpu,
					uint32_t offset, uint32_t *value);

__subsystem struct vgic_common_api {

	vgic_gicd_read_32_t 	vgicd_read_32;
	vgic_gicd_write_32_t	vgicd_write_32;

	vgic_gicrrd_read_32_t	vgicr_rd_read_32;
	vgic_gicrrd_write_32_t	vgicr_write_32;

};

typedef int (*vm_irq_exit_t)(struct device *dev, struct vcpu *vcpu, void *data);

typedef int (*vm_irq_enter_t)(struct device *dev, struct vcpu *vcpu, void *data);

__subsystem struct vm_irq_handler_api {
    vm_irq_exit_t irq_exit_from_vm;
    vm_irq_enter_t irq_enter_to_vm;
};

void z_ready_thread(struct k_thread *thread);

/**
 * @brief test and set vgic bits.
 */
void vgic_irq_test_and_set_bit(struct vcpu *vcpu, uint32_t spi_nr_count, uint32_t *value,
						uint32_t bit_size, bool enable);

/**
 * @brief Just for enable menopoly irq for vcpu.
 */
void arch_vdev_irq_enable(struct vcpu *vcpu);

/**
 * @brief Just for disable menopoly irq for vcpu.
 */
void arch_vdev_irq_disable(struct vcpu *vcpu);


int vgic_vdev_mem_read(struct virt_dev *vdev, uint64_t addr, uint64_t *value);

int vgic_vdev_mem_write(struct virt_dev *vdev, uint64_t addr, uint64_t *value);

/**
 * @brief send a virt irq signal to a vcpu.
 */
int set_virq_to_vcpu(struct vcpu *vcpu, uint32_t virq_num);

/**
 * @brief send a virq to vm that bind to this vcpu.
 */
int set_virq_to_vm(struct vm *vm, uint32_t virq_num);


int virt_irq_sync_vgic(struct vcpu *vcpu);

int virt_irq_flush_vgic(struct vcpu *vcpu);

/**
 * @brief Get the virq desc object.
 */
struct virt_irq_desc *get_virt_irq_desc(struct vcpu *vcpu, uint32_t virq);

/**
 * @brief Create vm's interrupt controller.
 */
int vm_intctrl_vdev_create(struct vm *vm);

/**
 * @brief When vcpu is loop on idel mode, we must send virq
 * to activate it.
 */
static ALWAYS_INLINE void wakeup_target_vcpu(struct vcpu *vcpu, struct virt_irq_desc *desc)
{
    ARG_UNUSED(desc);
    /* Set thread into runnig queue */
    z_ready_thread(vcpu->work->vcpu_thread);
}

/**
 * @brief Check that whether this vm can recieve virq?
 */
static ALWAYS_INLINE bool is_vm_irq_valid(struct vm *vm, uint32_t flag)
{
    if (vm->vm_status == VM_STATE_NEVER_RUN) {
		return false;
	}

    if (vm->vm_status == VM_STATE_PAUSE) {
        if (flag & VIRQ_WAKEUP_FLAG) {
			return true;
		} else {
			return false;
		}
    }
    return true;
}

#endif /* ZEPHYR_INCLUDE_ZVM_ARM_VGIC_COMMON_H_ */
