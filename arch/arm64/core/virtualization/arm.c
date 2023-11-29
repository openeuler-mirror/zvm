/*
 * Copyright 2021-2022 HNU
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <kernel.h>
#include <kernel_internal.h>
#include <kernel_structs.h>
#include <sys/atomic.h>
#include <syscall_handler.h>
#include <spinlock.h>
#include <timeout_q.h>
#include <toolchain.h>
#include <arch/cpu.h>
#include <arch/arm64/lib_helpers.h>
#include <irq.h>
#include <logging/log.h>

#include <virtualization/zvm.h>
#include <virtualization/vm.h>
#include <virtualization/arm/sysreg.h>
#include <virtualization/arm/switch.h>
#include <virtualization/arm/cpu.h>
#include <virtualization/vdev/vgic_v3.h>
#include <virtualization/arm/vtimer.h>
#include <virtualization/os/os_linux.h>

LOG_MODULE_DECLARE(ZVM_MODULE_NAME);

static void hyp_memory_map(void)
{
    /*TODO: support split mode.*/
}

/**
 * @brief Check whether Hyp and vhe are supported
 */
static bool is_basic_hardware_support(void)
{
    if (!is_el_implemented(MODE_EL2)) {
        ZVM_LOG_ERR("Hyp mode not available on this system.\n");
        return false;
    }

    if (is_el2_vhe_supported()) {
        return true;
    } else {
        hyp_memory_map();
        return false;
    }
}

static bool is_gicv3_device_support(void)
{
#if defined(CONFIG_GIC_V3)
    return true;
#else
    return false;
#endif
}

static int vcpu_virq_init(struct vcpu *vcpu)
{
    struct gicv3_vcpuif_ctxt *ctxt;

    /* init vgicv3 context */
    ctxt = (struct gicv3_vcpuif_ctxt *)k_malloc(sizeof(struct gicv3_vcpuif_ctxt));
    if(!ctxt) {
        ZVM_LOG_ERR("Init vcpu context failed");
        return -ENXIO;
    }
    memset(ctxt, 0, sizeof(struct gicv3_vcpuif_ctxt));

    vcpu_gicv3_init(ctxt);
    vcpu->arch->virq_data = ctxt;

    return 0;
}

static void vcpu_vgic_save(struct vcpu *vcpu)
{
    vgicv3_state_save(vcpu, (struct gicv3_vcpuif_ctxt *)vcpu->arch->virq_data);
}

static void vcpu_vgic_load(struct vcpu *vcpu)
{
    vgicv3_state_load(vcpu, (struct gicv3_vcpuif_ctxt *)vcpu->arch->virq_data);
}

static void vcpu_vtimer_save(struct vcpu *vcpu)
{
    uint64_t vcycles, pcycles;
    k_timeout_t vticks, pticks;
    struct virt_timer_context *timer_ctxt = vcpu->arch->vtimer_context;

#ifdef CONFIG_HAS_ARM_VHE_EXTN
    /* virt timer save */
    timer_ctxt->cntv_ctl = read_cntv_ctl_el02();
    write_cntv_ctl_el02(timer_ctxt->cntv_ctl & ~CNTV_CTL_ENABLE_BIT);
    timer_ctxt->cntv_cval = read_cntv_cval_el02();
    /* phys timer save */
    timer_ctxt->cntp_ctl = read_cntp_ctl_el02();
    write_cntp_ctl_el02(timer_ctxt->cntp_ctl & ~CNTP_CTL_ENABLE_BIT);
    timer_ctxt->cntp_cval = read_cntp_cval_el02();

    if (timer_ctxt->cntv_ctl & CNTV_CTL_ENABLE_BIT && !(timer_ctxt->cntv_ctl & CNTV_CTL_IMASK_BIT)) {
        vcycles = read_cntvct_el0();
        if (timer_ctxt->cntv_cval <= vcycles) {
            vticks.ticks = 0;
        } else {
            vticks.ticks = (timer_ctxt->cntv_cval - vcycles) / HOST_CYC_PER_TICK;
        }
        z_add_timeout(&timer_ctxt->vtimer_timeout, timer_ctxt->vtimer_timeout.fn, vticks);
    }
    if (timer_ctxt->cntp_ctl & CNTP_CTL_ENABLE_BIT && !(timer_ctxt->cntp_ctl & CNTP_CTL_IMASK_BIT)) {
        pcycles = read_cntpct_el0();
        if (timer_ctxt->cntp_cval <= pcycles) {
            pticks.ticks = 0;
        } else {
            pticks.ticks = (timer_ctxt->cntp_cval - pcycles) / HOST_CYC_PER_TICK;
        }
        z_add_timeout(&timer_ctxt->ptimer_timeout, timer_ctxt->ptimer_timeout.fn, pticks);
    }
#else
    timer_ctxt->cntv_ctl = read_cntv_ctl_el0();
    write_cntv_ctl_el0(timer_ctxt->cntv_ctl & ~CNTV_CTL_ENABLE_BIT);
    timer_ctxt->cntv_cval = read_cntv_cval_el0();
#endif
    dsb();
}

static void vcpu_vtimer_load(struct vcpu *vcpu)
{
    struct virt_timer_context *timer_ctxt = vcpu->arch->vtimer_context;

    z_abort_timeout(&timer_ctxt->vtimer_timeout);
    z_abort_timeout(&timer_ctxt->ptimer_timeout);

#ifdef CONFIG_HAS_ARM_VHE_EXTN
    write_cntvoff_el2(timer_ctxt->timer_offset);
#else
    write_cntvoff_el2(timer_ctxt->timer_offset);
    write_cntv_cval_el0(timer_ctxt->cntv_cval);
    write_cntv_ctl_el0(timer_ctxt->cntv_ctl);
#endif
    dsb();
}

static void arch_vcpu_sys_regs_init(struct vcpu *vcpu)
{
    struct zvm_vcpu_context *aarch64_c = &vcpu->arch->ctxt;

    aarch64_c->sys_regs[VCPU_MPIDR_EL1] = 0x81000000;   /* read_mpidr_el1(); */
    aarch64_c->sys_regs[VCPU_CPACR_EL1] = 0x03 << 20;
    aarch64_c->sys_regs[VCPU_VPIDR] = 0x410fc050;

    aarch64_c->sys_regs[VCPU_TTBR0_EL1] = 0;
    aarch64_c->sys_regs[VCPU_TTBR1_EL1] = 0;
    aarch64_c->sys_regs[VCPU_MAIR_EL1] = 0;
    aarch64_c->sys_regs[VCPU_TCR_EL1] = 0;
    aarch64_c->sys_regs[VCPU_PAR_EL1] = 0;
    aarch64_c->sys_regs[VCPU_AMAIR_EL1] = 0;

    aarch64_c->sys_regs[VCPU_TPIDR_EL0] = read_tpidr_el0();
    aarch64_c->sys_regs[VCPU_TPIDRRO_EL0] = read_tpidrro_el0();

    aarch64_c->sys_regs[VCPU_CSSELR_EL1] = read_csselr_el1();
    aarch64_c->sys_regs[VCPU_SCTLR_EL1] = 0x30C50838;
    aarch64_c->sys_regs[VCPU_ESR_EL1] = 0;
    aarch64_c->sys_regs[VCPU_AFSR0_EL1] = 0;
    aarch64_c->sys_regs[VCPU_AFSR1_EL1] = 0;
    aarch64_c->sys_regs[VCPU_FAR_EL1] = 0;
    aarch64_c->sys_regs[VCPU_VBAR_EL1] = 0;
    aarch64_c->sys_regs[VCPU_CONTEXTIDR_EL1] = 0;
    aarch64_c->sys_regs[VCPU_CNTKCTL_EL1] = 0;
    aarch64_c->sys_regs[VCPU_ELR_EL1] = 0;
    aarch64_c->sys_regs[VCPU_SPSR_EL1] = SPSR_MODE_EL1H;
}

static void arch_vcpu_common_regs_init(struct vcpu *vcpu)
{
    struct zvm_vcpu_context *ctxt;

    ctxt = &vcpu->arch->ctxt;
    memset(&ctxt->regs, 0, sizeof(struct arch_commom_regs));

    ctxt->regs.pc = vcpu->vm->os->code_entry_point;
    ctxt->regs.pstate = (SPSR_MODE_EL1H | DAIF_DBG_BIT | DAIF_ABT_BIT |
                            DAIF_IRQ_BIT | DAIF_FIQ_BIT );
}

static void arch_vcpu_fp_regs_init(struct vcpu *vcpu)
{
    /*TODO: support fpu later. */
    ARG_UNUSED(vcpu);
}

void arch_vcpu_context_save(struct vcpu *vcpu)
{
    vcpu_vgic_save(vcpu);
    vcpu_vtimer_save(vcpu);
    vcpu_sysreg_save(vcpu);
}

void arch_vcpu_context_load(struct vcpu *vcpu)
{
    int cpu = _current_cpu->id;
    vcpu->cpu = cpu;

    vcpu_sysreg_load(vcpu);
    vcpu_vtimer_load(vcpu);
    vcpu_vgic_load(vcpu);

#ifdef CONFIG_SCHED_CPU_MASK_PIN_ONLY
	vcpu->arch->hcr_el2 &= ~HCR_TWE_BIT;
    vcpu->arch->hcr_el2 &= ~HCR_TWI_BIT;
#else
    vcpu->arch->hcr_el2 |= HCR_TWE_BIT;
    vcpu->arch->hcr_el2 |= HCR_TWI_BIT;
#endif
}

int arch_vcpu_init(struct vcpu *vcpu)
{
    int ret = 0;
    struct vcpu_arch *vcpu_arch = vcpu->arch;
    struct vm_arch *vm_arch = vcpu->vm->arch;
    k_spinlock_key_t key;

    key = k_spin_lock(&vcpu->vm->vm_vcpu_id.vcpu_id_lock);
    vcpu->vcpu_id = vcpu->vm->vm_vcpu_id.totle_vcpu_id++;
    k_spin_unlock(&vcpu->vm->vm_vcpu_id.vcpu_id_lock, key);

    vcpu->cpu = _current_cpu->id;

    vcpu_arch->hcr_el2 = HCR_VM_FLAGS ;
    vcpu_arch->guest_mdcr_el2 = 0;
    vcpu_arch->host_mdcr_el2 = 0;
    vcpu_arch->list_regs_map = 0;
    vcpu_arch->pause = 0;
    vcpu_arch->vcpu_sys_register_loaded = false;

    /* init vm_arch here */
    vm_arch->vtcr_el2 = (0x20 | BIT(6) | BIT(8) | BIT(10) | BIT(12) | BIT(13));
    vm_arch->vttbr = (vcpu->vm->vmid | vm_arch->vm_pgd_base);

    arch_vcpu_common_regs_init(vcpu);
    arch_vcpu_sys_regs_init(vcpu);
    arch_vcpu_fp_regs_init(vcpu);

    ret = vcpu_virq_init(vcpu);
    if(ret) {
        return ret;
    }

    ret = arch_vcpu_timer_init(vcpu);
    if(ret) {
        return ret;
    }

#ifdef CONFIG_DTB_FILE_INPUT
    /* passing argu to linux, like fdt and others */
    vcpu_arch->ctxt.regs.esf_handle_regs.x0 = LINUX_DTB_MEM_BASE;
    vcpu_arch->ctxt.regs.esf_handle_regs.x1 = 0;
    vcpu_arch->ctxt.regs.esf_handle_regs.x2 = 0;
    vcpu_arch->ctxt.regs.esf_handle_regs.x3 = 0;
    vcpu_arch->ctxt.regs.callee_saved_regs.x20 = LINUX_DTB_MEM_BASE;
    vcpu_arch->ctxt.regs.callee_saved_regs.x21 = 0;
    vcpu_arch->ctxt.regs.callee_saved_regs.x22 = 0;
    vcpu_arch->ctxt.regs.callee_saved_regs.x23 = 0;
#endif
    return ret;
}

int zvm_arch_init(void *op)
{
    int ret = 0;

    /* Is hyp、vhe available? */
    if(!is_basic_hardware_support()){
        return -EVMODE;
    }

    if(!is_gicv3_device_support()){
        return -ENOVDEV;
    }

    ret = zvm_arch_vtimer_init(op);
    if(ret) {
        ZVM_LOG_ERR("Vtimer subsystem do not supported! \n");
        return ret;
    }
    return ret;
}
