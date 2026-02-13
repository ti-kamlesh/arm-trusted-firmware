#ifndef SCMI_CLOCK_H
#define SCMI_CLOCK_H

#include <stdint.h>
#include <common.h>

typedef struct ti_scmi_clock {
	uint32_t dev_id;
	uint32_t clock_id;
	unsigned long *rates;
	char name[64];
} ti_scmi_clock_t;

#endif /* SCMI_CLOCK_H */
