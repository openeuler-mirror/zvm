/*
 * Copyright (c) 2019 Carlo Caione <ccaione@baylibre.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ARCH_ARM64_CPU_H_
#define ZEPHYR_INCLUDE_ARCH_ARM64_CPU_H_

#include <sys/util.h>

#define	RK3568_ASM_PRINTK	1

#define DAIFSET_FIQ_BIT		BIT(0)
#define DAIFSET_IRQ_BIT		BIT(1)
#define DAIFSET_ABT_BIT		BIT(2)
#define DAIFSET_DBG_BIT		BIT(3)

#define DAIFCLR_FIQ_BIT		BIT(0)
#define DAIFCLR_IRQ_BIT		BIT(1)
#define DAIFCLR_ABT_BIT		BIT(2)
#define DAIFCLR_DBG_BIT		BIT(3)

#define DAIF_FIQ_BIT		BIT(6)
#define DAIF_IRQ_BIT		BIT(7)
#define DAIF_ABT_BIT		BIT(8)
#define DAIF_DBG_BIT		BIT(9)

#define SPSR_DAIF_SHIFT		(6)
#define SPSR_DAIF_MASK		(0xf << SPSR_DAIF_SHIFT)

#define SPSR_MODE_EL0T		(0x0)
#define SPSR_MODE_EL1T		(0x4)
#define SPSR_MODE_EL1H		(0x5)
#define SPSR_MODE_EL2T		(0x8)
#define SPSR_MODE_EL2H		(0x9)
#define SPSR_MODE_MASK		(0xf)

/* Additional SPSR bits */
#define SPSR_IL_BIT			BIT(20)


#define SCTLR_EL3_RES1		(BIT(29) | BIT(28) | BIT(23) | \
				 BIT(22) | BIT(18) | BIT(16) | \
				 BIT(11) | BIT(5)  | BIT(4))

#define SCTLR_EL2_RES1		(BIT(29) | BIT(28) | BIT(23) | \
				 BIT(22) | BIT(18) | BIT(16) | \
				 BIT(11) | BIT(5)  | BIT(4))

#define SCTLR_EL1_RES1		(BIT(29) | BIT(28) | BIT(23) | \
				 BIT(22) | BIT(20) | BIT(11))

#define SCTLR_M_BIT		BIT(0)
#define SCTLR_A_BIT		BIT(1)
#define SCTLR_C_BIT		BIT(2)
#define SCTLR_SA_BIT		BIT(3)
#define SCTLR_I_BIT		BIT(12)

#define CPACR_EL1_ZEN_ENEL1		BIT(16)
#define CPACR_EL1_ZEN			(0x3 << 16)
#define CPACR_EL1_FPEN_NOTRAP	(0x3 << 20)
#define CPACR_EL1_DFT			(CPACR_EL1_ZEN_ENEL1 | CPACR_EL1_FPEN_NOTRAP)

#define SCR_NS_BIT		BIT(0)
#define SCR_IRQ_BIT		BIT(1)
#define SCR_FIQ_BIT		BIT(2)
#define SCR_EA_BIT		BIT(3)
#define SCR_SMD_BIT		BIT(7)
#define SCR_HCE_BIT		BIT(8)
#define SCR_RW_BIT		BIT(10)
#define SCR_ST_BIT		BIT(11)

#define SCR_RES1		(BIT(4) | BIT(5))

/* MPIDR */
#define MPIDR_AFFLVL_MASK	(0xff)

#define MPIDR_AFF0_SHIFT	(0)
#define MPIDR_AFF1_SHIFT	(8)
#define MPIDR_AFF2_SHIFT	(16)
#define MPIDR_AFF3_SHIFT	(32)

#define MPIDR_AFFLVL(mpidr, aff_level) \
		(((mpidr) >> MPIDR_AFF##aff_level##_SHIFT) & MPIDR_AFFLVL_MASK)

#define GET_MPIDR()		read_sysreg(mpidr_el1)

#if defined(CONFIG_SOC_FVP_BASE_A55X4_A75X2) || defined(CONFIG_SOC_RK3568)
#define MPIDR_TO_CORE(mpidr)	MPIDR_AFFLVL(mpidr, 1)
#else
#define MPIDR_TO_CORE(mpidr)	MPIDR_AFFLVL(mpidr, 0)
#endif

#define IS_PRIMARY_CORE()	(!MPIDR_TO_CORE(GET_MPIDR()))

#define MODE_EL_SHIFT		(0x2)
#define MODE_EL_MASK		(0x3)

#define MODE_EL3		(0x3)
#define MODE_EL2		(0x2)
#define MODE_EL1		(0x1)
#define MODE_EL0		(0x0)

#define GET_EL(_mode)		(((_mode) >> MODE_EL_SHIFT) & MODE_EL_MASK)

#define ESR_EC_SHIFT		(26)
#define ESR_EC_MASK		BIT_MASK(6)
#define ESR_ISS_SHIFT		(0)
#define ESR_ISS_MASK		BIT_MASK(25)
#define ESR_IL_SHIFT		(25)
#define ESR_IL_MASK		BIT_MASK(1)

#define GET_ESR_EC(esr)		(((esr) >> ESR_EC_SHIFT) & ESR_EC_MASK)
#define GET_ESR_IL(esr)		(((esr) >> ESR_IL_SHIFT) & ESR_IL_MASK)
#define GET_ESR_ISS(esr)	(((esr) >> ESR_ISS_SHIFT) & ESR_ISS_MASK)

#define CNTV_CTL_ENABLE_BIT	BIT(0)
#define CNTV_CTL_IMASK_BIT	BIT(1)
#define CNTV_CTL_ISTAT_BIT  BIT(2)

#define CNTP_CTL_ENABLE_BIT	BIT(0)
#define CNTP_CTL_IMASK_BIT	BIT(1)
#define CNTP_CTL_ISTAT_BIT  BIT(2)

#define ID_AA64PFR0_EL0_SHIFT	(0)
#define ID_AA64PFR0_EL1_SHIFT	(4)
#define ID_AA64PFR0_EL2_SHIFT	(8)
#define ID_AA64PFR0_EL3_SHIFT	(12)
#define ID_AA64PFR0_ELX_MASK	(0xf)
#define ID_AA64PFR0_GIC_SHIFT	(24)
#define ID_AA64PFR0_SVE_SHIFT	(32)
#define ID_AA64PFR0_SEL2_SHIFT	(36)
#define ID_AA64PFR0_AMU_SHIFT	(44)
#define ID_AA64PFR0_SEL2_MASK	(0xf)

/*
 * TODO: ACTLR is of class implementation defined. All core implementations
 * in armv8a have the same implementation so far w.r.t few controls.
 * When there will be differences we have to create core specific headers.
 */
#define ACTLR_EL3_CPUACTLR_BIT	BIT(0)
#define ACTLR_EL3_CPUECTLR_BIT	BIT(1)
#define ACTLR_EL3_L2CTLR_BIT	BIT(4)
#define ACTLR_EL3_L2ECTLR_BIT	BIT(5)
#define ACTLR_EL3_L2ACTLR_BIT	BIT(6)

#define CPTR_EZ_BIT		BIT(8)
#define CPTR_TFP_BIT		BIT(10)
#define CPTR_TTA_BIT		BIT(20)
#define CPTR_TCPAC_BIT		BIT(31)

#define CPTR_EL2_RES1		BIT(13) | BIT(12) | BIT(9) | (0xff)
#define CPTR_EL2_TZ			(1 << 8)

#define HCR_VM_BIT		BIT(0)
#define HCR_SWIO_BIT	BIT(1)
#define HCR_PTW_BIT		BIT(2)
#define HCR_FMO_BIT		BIT(3)
#define HCR_IMO_BIT		BIT(4)
#define HCR_AMO_BIT		BIT(5)
#define HCR_VF_BIT		BIT(6)
#define HCR_VI_BIT		BIT(7)
#define HCR_VSE_BIT		BIT(8)
#define HCR_FB_BIT		BIT(9)
#define HCR_BSU_IS_BIT	BIT(10)
#define HCR_BSU_BIT		(3 << 10)
#define HCR_DC_BIT		BIT(12)
#define HCR_TWI_BIT		BIT(13)
#define HCR_TWE_BIT		BIT(14)
#define HCR_TID0_BIT	BIT(15)
#define HCR_TID1_BIT	BIT(16)
#define HCR_TID2_BIT	BIT(17)
#define HCR_TID3_BIT	BIT(18)
#define HCR_TSC_BIT		BIT(19)
#define HCR_TIDCP_BIT	BIT(20)
#define HCR_TAC_BIT	    BIT(21)
#define HCR_TSW_BIT		BIT(22)
#define HCR_TPC_BIT		BIT(23)
#define HCR_TPU_BIT		BIT(24)
#define HCR_TTLB_BIT	BIT(25)
#define HCR_TVM_BIT		BIT(26)
#define HCR_TGE_BIT		BIT(27)
#define HCR_TDZ_BIT		BIT(28)
#define HCR_HCDV_BIT	BIT(29)
#define HCR_TRVM_BIT	BIT(30)
#define HCR_RW_BIT		BIT(31)
#define HCR_CD_BIT		BIT(32)
#define HCR_ID_BIT		BIT(33)
#define HCR_E2H_BIT		BIT(34)
#define HCR_TLOR_BIT	BIT(35)
#define HCR_TERR_BIT	BIT(36)
#define HCR_TEA_BIT		BIT(37)
#define HCR_APK_BIT		BIT(40)
#define HCR_API_BIT		BIT(41)
#define HCR_FWB_BIT		BIT(46)
#define HCR_FIEN_BIT	BIT(47)
#define HCR_AMVOFFEN_BIT	BIT(51)
#define HCR_ATA_BIT		BIT(56)
#define HCR_DCT_BIT		BIT(57)
#define HCR_TID5_BIT	BIT(58)
#define HCR_RES0	(BIT(48) | BIT(39))


/* System register interface to GICv3 */
#define ICC_IGRPEN1_EL1		S3_0_C12_C12_7
#define ICC_SGI1R		S3_0_C12_C11_5
#define ICC_SRE_EL1		S3_0_C12_C12_5
#define ICC_SRE_EL2		S3_4_C12_C9_5
#define ICC_SRE_EL3		S3_6_C12_C12_5
#define ICC_BPR0_EL1	S2_0_C12_C8_3
#define ICC_BPR1_EL1	S2_0_C12_C12_3
#define ICC_CTLR_EL1		S3_0_C12_C12_4
#define ICC_CTLR_EL3		S3_6_C12_C12_4
#define ICC_PMR_EL1		S3_0_C4_C6_0
#define ICC_RPR_EL1		S3_0_C12_C11_3
#define ICC_IGRPEN1_EL3		S3_6_C12_C12_7
#define ICC_IGRPEN0_EL1		S3_0_C12_C12_6
#define ICC_HPPIR0_EL1		S3_0_C12_C8_2
#define ICC_HPPIR1_EL1		S3_0_C12_C12_2
#define ICC_IAR0_EL1		S3_0_C12_C8_0
#define ICC_IAR1_EL1		S3_0_C12_C12_0
#define ICC_EOIR0_EL1		S3_0_C12_C8_1
#define ICC_EOIR1_EL1		S3_0_C12_C12_1
#define ICC_SGI0R_EL1		S3_0_C12_C11_7
#define ICC_DIR_EL1			S3_0_C12_C11_1

/* register constants */
#define ICC_SRE_ELx_SRE_BIT	BIT(0)
#define ICC_SRE_ELx_DFB_BIT	BIT(1)
#define ICC_SRE_ELx_DIB_BIT	BIT(2)
#define ICC_SRE_EL3_EN_BIT	BIT(3)

/* ICC SGI macros */
#define SGIR_TGT_MASK		(0xffff)
#define SGIR_AFF1_SHIFT		(16)
#define SGIR_AFF2_SHIFT		(32)
#define SGIR_AFF3_SHIFT		(48)
#define SGIR_AFF_MASK		(0xf)
#define SGIR_INTID_SHIFT	(24)
#define SGIR_INTID_MASK		(0xf)
#define SGIR_IRM_SHIFT		(40)
#define SGIR_IRM_MASK		(0x1)
#define SGIR_IRM_TO_AFF		(0)

#define GICV3_SGIR_VALUE(_aff3, _aff2, _aff1, _intid, _irm, _tgt)	\
	((((uint64_t) (_aff3) & SGIR_AFF_MASK) << SGIR_AFF3_SHIFT) |	\
	 (((uint64_t) (_irm) & SGIR_IRM_MASK) << SGIR_IRM_SHIFT) |	\
	 (((uint64_t) (_aff2) & SGIR_AFF_MASK) << SGIR_AFF2_SHIFT) |	\
	 (((_intid) & SGIR_INTID_MASK) << SGIR_INTID_SHIFT) |		\
	 (((_aff1) & SGIR_AFF_MASK) << SGIR_AFF1_SHIFT) |		\
	 ((_tgt) & SGIR_TGT_MASK))

/* Implementation defined register definations */
#if defined(CONFIG_CPU_CORTEX_A72)

#define CORTEX_A72_L2CTLR_EL1				S3_1_C11_C0_2
#define CORTEX_A72_L2CTLR_DATA_RAM_LATENCY_SHIFT	(0)
#define CORTEX_A72_L2CTLR_DATA_RAM_SETUP_SHIFT		(5)
#define CORTEX_A72_L2CTLR_TAG_RAM_LATENCY_SHIFT		(6)
#define CORTEX_A72_L2CTLR_TAG_RAM_SETUP_SHIFT		(9)

#define CORTEX_A72_L2_DATA_RAM_LATENCY_3_CYCLES		(2)
#define CORTEX_A72_L2_DATA_RAM_LATENCY_MASK		(0x7)
#define CORTEX_A72_L2_DATA_RAM_SETUP_1_CYCLE		(1)
#define CORTEX_A72_L2_TAG_RAM_LATENCY_2_CYCLES		(1)
#define CORTEX_A72_L2_TAG_RAM_LATENCY_3_CYCLES		(2)
#define CORTEX_A72_L2_TAG_RAM_LATENCY_MASK		(0x7)
#define CORTEX_A72_L2_TAG_RAM_SETUP_1_CYCLE		(1)

#define CORTEX_A72_L2ACTLR_EL1				S3_1_C15_C0_0
#define CORTEX_A72_L2ACTLR_DISABLE_ACE_SH_OR_CHI_BIT	BIT(6)

#endif /* CONFIG_CPU_CORTEX_A72 */

#define L1_CACHE_SHIFT		(6)
#define L1_CACHE_BYTES		BIT(L1_CACHE_SHIFT)
#define ARM64_CPU_INIT_SIZE	L1_CACHE_BYTES

/* spsel bit for stack pointer chosen */
#define SPSEL_EL0			(0)
#define SPSEL_ELX			(1)

/* debug system related register */
#define ID_AA64DFR0_PMUVER_SHIFT		(8)
#define	ID_AA64DFR0_TRBE_SHIFT			(44)

/* Below codes may have copyright risk */
/*
 * Below code is to add some macor for EL2 init
 */
/* sysreg_ops for c code asm extention */
#define sysreg_ops(op0, op1, crn, crm, op2) (((op0) << 19) | ((op1) << 16) | \
	 ((crn) << 12) | ((crm) << 8) | ((op2) << 5))

#define ENDIAN_SET_EL2		 (0)

#define INIT_SCTLR_EL2_MMU_OFF (SCTLR_EL2_RES1 | ENDIAN_SET_EL2)

/* TRBE Registers */
/* Below codes may be change to original status in next stage */
#define LORC_EL1			sysreg_ops(3, 0, 10, 4, 3)

#define SYS_SCTLR_EL2			S3_4_C1_C0_0
#define HAFGRTR_EL2			sysreg_ops(3, 4, 3, 1, 6)
#define HFGRTR_EL2			sysreg_ops(3, 4, 1, 1, 4)
#define HFGWTR_EL2			sysreg_ops(3, 4, 1, 1, 5)
#define HFGITR_EL2			sysreg_ops(3, 4, 1, 1, 6)
#define SYS_ZCR_EL2				S3_4_C1_C2_0
#define SYS_TRFCR_EL2			S3_4_C1_C2_1
#define SYS_DACR32_EL2			S3_4_C3_C0_0
#define HDFGRTR_EL2			sysreg_ops(3, 4, 3, 1, 4)
#define HDFGWTR_EL2			sysreg_ops(3, 4, 3, 1, 5)

#define TRBIDR_PROG				BIT(4)

#define SYS_TRBIDR_EL1		sysreg_ops(3, 0, 9, 11, 7)

/* id_aa64dfr0_EL1 */
#define ID_AA64DFR0_PMSVER_SHIFT		(32)

/* id_aa64mmfr0 */
#define ID_AA64MMFR0_FGT_SHIFT			(56)

/* id_aa64mmfr1 */
#define ID_AA64MMFR1_LOR_SHIFT			(16)

/* ID register */
#define SYS_PMBIDR_EL1			sysreg_ops(3, 0, 9, 10, 7)
#define SYS_PMBIDR_EL1_P_SHIFT			(4)

/* sampling controls register */
#define SYS_PMSCR_EL2			sysreg_ops(3, 4, 9, 9, 0)
#define SYS_PMSCR_EL2_PA_SHIFT			(4)
#define SYS_PMSCR_EL2_PCT_SHIFT			(6)

/* hyp related register */
#define MDCR_EL2_E2PB_MASK				(3)
#define MDCR_EL2_E2PB_SHIFT				(12)
#define MDCR_EL2_E2TB_SHIFT				(24)

/* These are for GICv2 emulation only */
#define GICH_LR_VIRTUALID				(0x3ffUL << 0)
#define GICH_LR_PHYSID_CPUID			(7UL << 10)

#define ICC_IAR1_EL1_SPURIOUS			(0x3ff)


/*
 * The ZCR_ELx_LEN_* definitions intentionally include bits [8:4] which
 * are reserved by the SVE architecture for future expansion of the LEN
 * field, with compatible semantics.
 */
#define ZCR_ELx_LEN_SHIFT				(0)
#define ZCR_ELx_LEN_SIZE				(9)
#define ZCR_ELx_LEN_MASK				(0x1ff)



/* This code will be moved to gic.h file in last stage! */
/* These are for GICv2 emulation only */
#define ICC_SRE_EL2_SRE			BIT(0)
#define ICC_SRE_EL2_ENABLE		BIT(3)
/*********************-------*******************************/

/*  This code will be moved to prace.h file in last stage! */
#define INIT_PSTATE_EL1  (0x00000200 | 0x00000100 | 0x00000080 | 0x00000040 | 0x00000005)
#define INIT_PSTATE_EL2  (0x00000200 | 0x00000100 | 0x00000080 | 0x00000040 | 0x00000009)



#define SCTLR_EL1_SET	(BIT(0)    | BIT(2)    | BIT(3)    |\
			 BIT(4)   | BIT(8)   | BIT(12)     |\
			 BIT(14)   | BIT(15)   | BIT(16)  |\
			 BIT(18)  | BIT(21)  | BIT(23)  |\
			 BIT(0)  | BIT(26)   | SCTLR_EL1_RES1 )
/***********************/
/* Above codes may have copyright risk */

#endif /* ZEPHYR_INCLUDE_ARCH_ARM64_CPU_H_ */
