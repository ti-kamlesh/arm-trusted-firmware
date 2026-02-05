/*
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * TI Device Clock Driver
 *
 * This driver provides clock nodes that are derived from device states,
 * allowing the clock framework to query and track device power states.
 * It implements a clock driver that reports frequency and state based on
 * the associated device's power state, enabling clock tree dependencies
 * on device power domains.
 */

#include <assert.h>
#include <ti_clk.h>
#include <ti_clk_dev.h>
#include <ti_device.h>
#include <ti_device_clk.h>
#include <ti_device_pm.h>
#include <ti_container_of.h>

/* It might be useful to prevent the device from disabling in this case */
static bool clk_from_device_set_state(struct ti_clk *clkp __maybe_unused,
				      bool enabled __maybe_unused)
{
	assert(clkp != NULL);

	return true;
}

static uint32_t clk_from_device_get_state(struct ti_clk *clkp)
{
	const struct ti_clk_data_from_dev *from_device;
	struct ti_device *dev;
	uint32_t ret;

	assert(clkp != NULL);

	from_device = container_of((const struct ti_clk_drv_data *)clkp->data,
				   const struct ti_clk_data_from_dev, data);

	dev = ti_device_lookup(from_device->dev);

	if ((dev == NULL) || ((dev->initialized) == 0U)) {
		ret = TI_CLK_HW_STATE_DISABLED;
	} else {
		uint32_t state;

		state = ti_device_get_state(dev);
		if (state == 0U) {
			ret = TI_CLK_HW_STATE_DISABLED;
		} else if (state == 2U) {
			ret = TI_CLK_HW_STATE_TRANSITION;
		} else {
			struct ti_dev_clk *dev_clkp;

			dev_clkp = ti_get_dev_clk(dev, from_device->clk_idx);
			if (!dev_clkp) {
				ret = TI_CLK_HW_STATE_DISABLED;
			} else if ((dev_clkp->flags & TI_DEV_CLK_FLAG_DISABLE) != 0U) {
				ret = TI_CLK_HW_STATE_DISABLED;
			} else {
				ret = TI_CLK_HW_STATE_ENABLED;
			}
		}
	}

	return ret;
}

const struct ti_clk_drv ti_clk_drv_from_device = {
	.get_freq = ti_clk_value_get_freq,
	.set_state = clk_from_device_set_state,
	.get_state = clk_from_device_get_state,
};
