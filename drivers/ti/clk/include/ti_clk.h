/*
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * TI Clock Framework API Header
 *
 * This header defines the core clock framework data structures and API
 * functions. It provides clock data types, clock driver interfaces, parent-
 * child relationships, frequency ranges, and the main API for clock
 * operations including get/put, frequency control, and state management.
 */

#ifndef TI_CLK_H
#define TI_CLK_H

#include <stddef.h>

#include <common/debug.h>
#include <lib/utils_def.h>

#include <ti_pm_types.h>

/**
 * \brief Generates a 32-bit mask that covers all set bits in a number
 */
#define TI_MASK_COVER_FOR_NUMBER(number) ((1UL << (32 - __builtin_clz(number))) - 1UL)

/** \brief Converts frequency from GHz to Hz */
#define FREQ_GHZ(freq)	((uint32_t) ((double) freq * 1000000000.0))

/** \brief Converts frequency from MHz to Hz */
#define FREQ_MHZ(freq)	((uint32_t) ((double) freq * 1000000.0))

/** \brief Converts frequency from KHz to Hz */
#define FREQ_KHZ(freq)	((uint32_t) ((double) freq * 1000.0))

/** \brief Converts frequency from Hz to Hz (no-op for consistency) */
#define FREQ_HZ(freq)	((uint32_t) (freq))

/**
 * \brief Initializes a clock range entry with min and max frequencies
 */
#define TI_CLK_RANGE(id, min, max)[(id)] =				\
		{ .min_hz = (uint32_t) (min),			\
		  .max_hz = (uint32_t) (max) }

/**
 * \brief Initializes a clock default frequency entry with min, target, and max
 */
#define TI_CLK_DEFAULT(id, min, target, max)[(id)] =		\
		{ .min_hz = (uint32_t) (min),			\
		  .target_hz = (uint32_t) (target),		\
		  .max_hz = (uint32_t) (max) }

/** \brief Clock type identifier for divider clocks */
#define TI_CLK_TYPE_DIV	1U

/** \brief Clock type identifier for multiplexer clocks */
#define TI_CLK_TYPE_MUX	2U

/** \brief Allows clock to modify its parent's frequency */
#define TI_CLK_DATA_FLAG_MODIFY_PARENT_FREQ	BIT(0)

/** \brief Skips hardware reinitialization during clock setup */
#define TI_CLK_DATA_FLAG_NO_HW_REINIT		BIT(2)

/** \brief Clock should be enabled during power-up */
#define TI_CLK_FLAG_PWR_UP_EN			((uint8_t) BIT(0))

/** \brief Clock has been initialized */
#define TI_CLK_FLAG_INITIALIZED			((uint8_t) BIT(2))

/**
 * Clock has a cached frequency value stored in soc_clock_values[] array.
 * Used for clocks like PLLs where the frequency is programmed and stored
 * rather than derived from parent clocks or hardware registers.
 */
#define TI_CLK_FLAG_CACHED				((uint8_t) BIT(3))

/** \brief Clock hardware is disabled */
#define TI_CLK_HW_STATE_DISABLED	0U

/** \brief Clock hardware is enabled */
#define TI_CLK_HW_STATE_ENABLED	1U

/** \brief Clock hardware is in transition state */
#define TI_CLK_HW_STATE_TRANSITION 2U

/**
 * \brief Defines a frequency range with minimum and maximum values
 */
struct ti_clk_range {
	uint32_t min_hz;	/** Minimum frequency in Hz */
	uint32_t max_hz;	/** Maximum frequency in Hz */
};

/**
 * \brief Defines a default frequency configuration with min, target, and max
 */
struct ti_clk_default {
	uint32_t min_hz;	/** Minimum acceptable frequency in Hz */
	uint32_t target_hz;	/** Target frequency in Hz */
	uint32_t max_hz;	/** Maximum acceptable frequency in Hz */
};

/**
 * \brief Represents a clock's parent and associated divider
 */
struct ti_clk_parent {
	uint16_t clk;	/** Parent clock ID */
	uint8_t div;	/** Divider value applied to parent frequency */
};

/**
 * \brief Clock driver-specific data (placeholder for extensibility)
 */
struct ti_clk_drv_data {
};

/**
 * \brief Clock structure combining runtime state and configuration
 *
 * This structure merges what was previously ti_clk (runtime) and ti_clk_data
 * (configuration). Since everything runs from RAM on this platform, there's no
 * benefit to separating them into parallel arrays.
 */
struct ti_clk {
	/* Runtime state - modified during operation */
	uint8_t ref_count;		/** Reference count for clock usage */
	uint8_t flags;			/** Runtime flags (TI_CLK_FLAG_*) */

	/* Configuration - set at initialization, read-only afterwards */
	const struct ti_clk_drv *drv;		/** Clock driver operations */
	const struct ti_clk_drv_data *data;	/** Driver-specific data */
	struct ti_clk_parent parent;		/** Parent clock and divider */
	uint16_t freq_idx;			/** soc_clock_values[] index for cached clocks */
	uint8_t range_idx;			/** Index into shared range table */
	uint8_t type;				/** Clock type (TI_CLK_TYPE_*) */
	uint8_t data_flags;			/** Configuration flags (TI_CLK_DATA_FLAG_*) */
};

struct ti_clk_drv {
	/** Perform any necessary intitialization */
	int32_t (*init)(struct ti_clk *clkp);

	/**
	 * \brief Set the current state of a clock.
	 *
	 * \param clk The clock to modify
	 * \param enabled True to enable, false to gate/power down
	 *
	 * \return true if the action succeeded
	 */
	bool (*set_state)(struct ti_clk *clkp, bool enabled);

	/**
	 * \brief Get the current state of a clock.
	 *
	 * \param clk The clock to query
	 *
	 * \return true if the clock is running
	 */
	uint32_t (*get_state)(struct ti_clk *clkp);

	/**
	 * \brief Program a clock to run at a given frequency. The minimum
	 * and maximum frequency parameters allow the best nearest match
	 * to be chosen. The clock tree is walked to accomplish this.
	 *
	 * \param clk The clock to modify
	 * \param target_hz The target frequency in Hz
	 * \param min_hz The minimum acceptable frequency
	 * \param max_hz The maximum acceptable frequency
	 * \param changed Output parameter indicating if frequency changed
	 *
	 * \return Best frequency found in Hz, returns 0 for failure
	 */
	uint32_t (*set_freq)(struct ti_clk *clkp, uint32_t target_hz,
			     uint32_t min_hz, uint32_t max_hz,
			     bool *changed);

	/**
	 * \brief Return the frequency this clock runs at.
	 *
	 * \param clk The clock to query
	 *
	 * \return The frequency in Hz
	 */
	uint32_t (*get_freq)(struct ti_clk *clkp);

};

/** The table of clock structures */
extern struct ti_clk soc_clocks[];
extern const size_t soc_clock_count;

/** The table of shared const clock ranges */
extern const struct ti_clk_range soc_clock_ranges[];
extern const size_t soc_clock_range_count;

/** The table of default frequencies */
extern const struct ti_clk_default soc_clock_freq_defaults[];
extern const size_t soc_clock_freq_defaults_count;

/**
 * The table of cached clock frequency values.
 * Some clocks (e.g., PLLs) store their programmed frequency in this array
 * indexed by clk->freq_idx, rather than deriving it from parent clocks or
 * hardware registers. Clocks using this mechanism have TI_CLK_FLAG_CACHED set.
 */
extern uint32_t soc_clock_values[];
extern const size_t soc_clock_value_count;

/**
 * \brief Sets frequency for clocks with cached frequency values
 *
 * Used by clocks that store their frequency in soc_clock_values[] array
 * (e.g., PLLs). The frequency is validated against min/max range and stored
 * at the index specified by clk->freq_idx.
 *
 * \param clkp The clock to modify (must have valid freq_idx)
 * \param target_hz Target frequency in Hz
 * \param min_hz Minimum acceptable frequency
 * \param max_hz Maximum acceptable frequency
 * \param changed Output parameter indicating if frequency changed
 *
 * \return Target frequency in Hz on success, 0 on failure
 */
uint32_t ti_clk_value_set_freq(struct ti_clk *clkp, uint32_t target_hz,
			    uint32_t min_hz, uint32_t max_hz,
			    bool *changed);

/**
 * \brief Gets the cached frequency value for a clock
 *
 * Retrieves the frequency stored in soc_clock_values[] array for clocks
 * that cache their frequency (e.g., PLLs). The value is read from the index
 * specified by clk->freq_idx.
 *
 * \param clkp The clock to query (must have valid freq_idx)
 *
 * \return The cached frequency in Hz, or 0 if freq_idx is out of bounds
 */
uint32_t ti_clk_value_get_freq(struct ti_clk *clkp);

/**
 * \brief Gets the parent clock's current frequency
 *
 * \param clkp The clock whose parent frequency to retrieve
 *
 * \return Parent frequency in Hz
 */
uint32_t ti_clk_get_parent_freq(struct ti_clk *clkp);


/**
 * \brief Generic frequency setter that adjusts parent frequency
 *
 * \param clkp The clock to modify
 * \param parent The parent clock
 * \param target_hz Target frequency in Hz
 * \param min_hz Minimum acceptable frequency
 * \param max_hz Maximum acceptable frequency
 * \param changed Output parameter indicating if frequency changed
 * \param div Divider from parent to child (child = parent/div).
 *            Used to translate child frequency requirements to parent frequency.
 *
 * \return Best frequency found in Hz, 0 on failure
 */
uint32_t ti_clk_generic_set_freq_parent(struct ti_clk *clkp, struct ti_clk *parent,
				     uint32_t target_hz, uint32_t min_hz,
				     uint32_t max_hz,
				     bool *changed, uint32_t div);

/**
 * \brief Sets a clock's frequency
 *
 * \param clkp The clock to modify
 * \param target_hz Target frequency in Hz
 * \param min_hz Minimum acceptable frequency
 * \param max_hz Maximum acceptable frequency
 * \param changed Output parameter indicating if frequency changed
 *
 * \return Best frequency found in Hz, 0 on failure
 */
uint32_t ti_clk_set_freq(struct ti_clk *clkp, uint32_t target_hz, uint32_t min_hz,
			 uint32_t max_hz, bool *changed);

/**
 * \brief Gets a clock's current frequency
 *
 * \param clkp The clock to query
 *
 * \return Current frequency in Hz
 */
uint32_t ti_clk_get_freq(struct ti_clk *clkp);

/**
 * \brief Gets a clock's hardware state
 *
 * \param clkp The clock to query
 *
 * \return Clock state (TI_CLK_HW_STATE_*)
 */
uint32_t ti_clk_get_state(struct ti_clk *clkp);

/**
 * \brief Sets a clock's hardware state (enable/disable)
 *
 * \param clkp The clock to modify
 * \param enable True to enable, false to disable
 *
 * \return True if successful
 */
bool ti_clk_set_state(struct ti_clk *clkp, bool enable);

/**
 * \brief Increments a clock's reference count and enables it
 *
 * \param clkp The clock to get
 *
 * \return True if successful
 */
bool ti_clk_get(struct ti_clk *clkp);

/**
 * \brief Decrements a clock's reference count and disables if zero
 *
 * \param clkp The clock to put
 */
void ti_clk_put(struct ti_clk *clkp);

/**
 * \brief Initializes the TI clock framework
 *
 * Performs one pass of clock initialization. Clocks are processed in array
 * order. Each clock is skipped if its parent clock is not yet initialized,
 * allowing the function to handle parent-child dependencies.
 *
 * Design for arbitrary clock ordering:
 * - The function can be called multiple times to initialize clocks with
 *   multi-level dependencies
 * - Each call initializes clocks whose parent dependencies are now met
 * - Returns 0 if progress was made, -EAGAIN if no progress possible
 *
 * Current implementation:
 * - The clock table is topologically sorted (parents before children)
 * - A single call initializes all clocks
 * - Multiple calls are not necessary but are safe (subsequent calls
 *   detect all clocks initialized and return 0 quickly)
 *
 * \return 0 on success (clocks initialized or no clocks need init),
 *         -EAGAIN if no progress made (circular dependency - fatal error),
 *         -ENODEV if a power-up clock failed to enable,
 *         or other negative error code on failure
 */
int32_t ti_clk_init(void);

/**
 * \brief Clears power-up enable flag on all clocks
 */
void ti_clk_drop_pwr_up_en(void);

/**
 * \brief Determine if a clock ID is valid.
 *
 * \param id
 * The internal clock API ID.
 *
 * \return
 * True if the clock ID refers to a valid SoC clock, false otherwise
 */
static inline bool ti_clk_id_valid(ti_clk_idx_t id)
{
	return (id < soc_clock_count) && (soc_clocks[id].drv);
}

/**
 * \brief Lookup an internal clock based on a clock ID
 *
 * This does a lookup of a clock based on the clock ID.
 *
 * \param id
 * The internal clock API ID.
 *
 * \return
 * The clock pointer, or NULL if the clock ID was not valid.
 */
static inline struct ti_clk *ti_clk_lookup(ti_clk_idx_t id)
{
	return ti_clk_id_valid(id) ? (soc_clocks + id) : NULL;
}

/**
 * \brief Return the clock ID based on a clock pointer.
 *
 * The clock pointer is just an index into the array of clocks. This is
 * used to return a clock ID. This function has no error checking for NULL
 * pointers.
 *
 * \param clk
 * The clock pointer.
 *
 * \return
 * The clock ID.
 */
static inline ti_clk_idx_t ti_clk_id(struct ti_clk *clkp)
{
	return (ti_clk_idx_t) (clkp - soc_clocks);
}

/**
 * \brief Return a shared clock range with bounds checking.
 *
 * The clock infrastructure contains a set of shared clock ranges. Many
 * clocks and PLLs have min/max ranges. However, these min/max ranges can
 * take up a significant amount of storage. As many clocks share the same
 * ranges, we use a shared table of ranges. The clocks can then index that
 * table with a small uint8_t.
 *
 * \param idx
 * The index into the range table.
 *
 * \return
 * The range associated with the index, or NULL if index is out of bounds.
 */
static inline const struct ti_clk_range *ti_clk_get_range(ti_clk_idx_t idx)
{
	if (idx >= soc_clock_range_count) {
		return NULL;
	}
	return soc_clock_ranges + idx;
}

/**
 * \brief Set a cached clock frequency value with bounds checking.
 *
 * Stores a frequency value in the soc_clock_values[] array for clocks that
 * cache their frequency (e.g., PLLs with TI_CLK_FLAG_CACHED set). Validates
 * the index is within array bounds before writing.
 *
 * \param idx
 * The index into soc_clock_values[] array (from clk->freq_idx).
 *
 * \param value_hz
 * The frequency value in Hz to cache.
 *
 * \return
 * True if the value was cached successfully, false if index is out of bounds.
 */
static inline bool ti_clk_set_value(uint16_t idx, uint32_t value_hz)
{
	if (idx >= soc_clock_value_count) {
		return false;
	}
	soc_clock_values[idx] = value_hz;
	return true;
}

/**
 * \brief Get a cached clock frequency value with bounds checking.
 *
 * Retrieves a frequency value from the soc_clock_values[] array for clocks
 * that cache their frequency (e.g., PLLs with TI_CLK_FLAG_CACHED set).
 * Validates the index is within array bounds before reading.
 *
 * \param idx
 * The index into soc_clock_values[] array (from clk->freq_idx).
 *
 * \return
 * The cached frequency value in Hz, or 0 if index is out of bounds.
 */
static inline uint32_t ti_clk_get_value(uint16_t idx)
{
	if (idx >= soc_clock_value_count) {
		return 0;
	}
	return soc_clock_values[idx];
}

#endif
