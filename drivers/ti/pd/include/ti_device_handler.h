/*
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Device Handler API
 *
 * This header provides device state handler functions and constants for
 * managing device states, retention modes, and exclusive device ownership
 * as part of the power management subsystem.
 */

#ifndef DEVICE_HANDLER_H
#define DEVICE_HANDLER_H

/**
 * Claim a device exclusively. When passed with STATE_RETENTION or STATE_ON,
 * it will claim the device exclusively. If another host already has this
 * device set to STATE_RETENTION or STATE_ON, the message will fail. Once
 * successful, other hosts attempting to set STATE_RETENTION or STATE_ON
 * will fail.
 */
#define TI_DEVICE_EXCLUSIVE	      BIT(10)

/**
 * Turn device off when possible. This must be used in conjunction with
 * SoC dependencies to identify the overall power domain state being
 * achieved.
 */
#define TI_DEVICE_SW_STATE_AUTO_OFF    0

/** Disable device but keep in retention. */
#define TI_DEVICE_SW_STATE_RETENTION   1

/** Turn device on for usage. */
#define TI_DEVICE_SW_STATE_ON	    2

/**
 * Hardware state indicating device is on.
 */
#define TI_DEVICE_HW_STATE_ON	    1

/**
 *  \brief  Set Device State Handler PM Function.
 *
 *  \param  dev_id   Device ID to set state for
 *  \param  enable   true to enable device, false to disable
 *  \return ret      0 if the API executed successfully.
 *                   EFAIL   if the API failed to execute.
 */
int32_t ti_set_device_handler(uint32_t dev_id, bool enable);

/**
 *  \brief  Get Device State Handler PM Function.
 *
 *  \param  dev_id      Device ID to get state for
 *  \return is_enabled  true if enabled, false if disable
 */
bool ti_get_device_handler(uint32_t dev_id);

#endif /* DEVICE_HANDLER_H */
