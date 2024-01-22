/*
 * Copyright 2020 NXP
 * Copyright 2022 HNU-ESNL
 * Copyright 2022 openEuler SIG-Zephyr
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <devicetree.h>
#include <sys/util.h>
#include <arch/arm64/arm_mmu.h>

static const struct arm_mmu_region mmu_regions[] = {

	MMU_REGION_FLAT_ENTRY("GIC",
			      DT_REG_ADDR_BY_IDX(DT_NODELABEL(gic), 0),
			      DT_REG_SIZE_BY_IDX(DT_NODELABEL(gic), 0),
			      MT_DEVICE_nGnRnE | MT_P_RW_U_RW | MT_NS),

	MMU_REGION_FLAT_ENTRY("GIC",
			      DT_REG_ADDR_BY_IDX(DT_NODELABEL(gic), 1),
			      DT_REG_SIZE_BY_IDX(DT_NODELABEL(gic), 1),
			      MT_DEVICE_nGnRnE | MT_P_RW_U_RW | MT_NS),

	MMU_REGION_FLAT_ENTRY("UART2",
			      DT_REG_ADDR(DT_NODELABEL(uart2)),
			      DT_REG_SIZE(DT_NODELABEL(uart2)),
			      MT_DEVICE_nGnRnE | MT_P_RW_U_RW | MT_NS),

#if defined(CONFIG_ZVM)
	MMU_REGION_FLAT_ENTRY("UART3",
			      DT_REG_ADDR(DT_NODELABEL(uart3)),
			      DT_REG_SIZE(DT_NODELABEL(uart3)),
			      MT_DEVICE_nGnRnE | MT_P_RW_U_NA | MT_DEFAULT_SECURE_STATE),

	MMU_REGION_FLAT_ENTRY("UART9",
			      DT_REG_ADDR(DT_NODELABEL(uart9)),
			      DT_REG_SIZE(DT_NODELABEL(uart9)),
			      MT_DEVICE_nGnRnE | MT_P_RW_U_NA | MT_DEFAULT_SECURE_STATE),
#endif
};

const struct arm_mmu_config mmu_config = {
	.num_regions = ARRAY_SIZE(mmu_regions),
	.mmu_regions = mmu_regions,
};
