# SPDX-License-Identifier: Apache-2.0

if(CONFIG_PM)
  zephyr_sources(pm.c constraint.c)
  zephyr_sources_ifdef(CONFIG_PM_STATS pm_stats.c)
endif()

zephyr_sources_ifdef(CONFIG_PM_DEVICE device.c)
zephyr_sources_ifdef(CONFIG_PM_DEVICE_RUNTIME device_runtime.c)
add_subdirectory(policy)
