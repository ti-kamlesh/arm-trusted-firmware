/*
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SOC_DEVICE_H
#define SOC_DEVICE_H

#include <ti_psc_soc_device.h>

/**
 * This index defines the device ID assigned to a given piece of IP. Device
 * IDs are assigned statically and are exported as part of the ABI.
 *
 * This value corresponds to the device field in power management messages.
 */
typedef uint8_t dev_idx_t;

#endif
