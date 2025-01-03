/*
 * Copyright (C) 2024-2026 Texas Instruments Incorporated - https://www.ti.com/
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <plat_scmi_def.h>

#include <drivers/scmi-msg.h>
#include <drivers/scmi.h>
#include <lib/utils.h>
#include <lib/utils_def.h>

#include <ti_clk.h>
#include <ti_clocks.h>
#include <ti_device.h>
#include <ti_device_clk.h>
#include <ti_device_handler.h>
#include <ti_device_pm.h>
#include <ti_devices.h>
#include <ti_sci.h>

#include <scmi_pd_data.h>

#define POWER_STATE_ON  (0 << 30)
#define POWER_STATE_OFF BIT(30)

size_t plat_scmi_pd_count(unsigned int agent_id __unused)
{
	return ARRAY_SIZE(scmi_power_domains);
}

const char *plat_scmi_pd_get_name(unsigned int agent_id __unused,
				  unsigned int pd_id)
{
	if (pd_id >= ARRAY_SIZE(scmi_power_domains)) {
		return NULL;
	}

	return scmi_power_domains[pd_id].name;
}

unsigned int plat_scmi_pd_get_state(unsigned int agent_id __unused,
				    unsigned int pd_id)
{
	bool is_on;

	if (pd_id >= ARRAY_SIZE(scmi_power_domains)) {
		return POWER_STATE_OFF;
	}

	is_on = ti_get_device_handler(scmi_power_domains[pd_id].id);

	return is_on ? POWER_STATE_ON : POWER_STATE_OFF;
}

int32_t plat_scmi_pd_set_state(unsigned int agent_id __unused,
			       unsigned int flags __unused,
			       unsigned int pd_id,
			       unsigned int state)
{
	int32_t ret = SCMI_SUCCESS;
	bool current_state;
	unsigned int current_power_state;

	if (pd_id >= ARRAY_SIZE(scmi_power_domains)) {
		return SCMI_NOT_FOUND;
	}

	current_state = ti_get_device_handler(scmi_power_domains[pd_id].id);
	current_power_state = current_state ? POWER_STATE_ON : POWER_STATE_OFF;

	if ((current_power_state == POWER_STATE_ON) && (state == POWER_STATE_OFF)) {
		VERBOSE("\n%s: Disabling PD: agent_id = %u, pd = %u, state to set = 0x%x\n",
			__func__, agent_id, pd_id, state);
		ret = ti_set_device_handler(scmi_power_domains[pd_id].id, false);
	} else if ((current_power_state == POWER_STATE_OFF) && (state == POWER_STATE_ON)) {
		VERBOSE("\n%s: Enabling PD: agent_id = %u, pd = %u, state to set = 0x%x\n",
			__func__, agent_id, pd_id, state);
		ret = ti_set_device_handler(scmi_power_domains[pd_id].id, true);
	} else {
		ret = SCMI_SUCCESS;
	}

	return ret;
}
