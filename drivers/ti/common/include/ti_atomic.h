/*
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * TI Atomic Operations
 *
 * Type-safe wrappers around compiler atomic intrinsics for thread-safe
 * operations on shared data structures.
 */

#ifndef TI_ATOMIC_H
#define TI_ATOMIC_H

#include <stdint.h>

/**
 * \brief Atomically load an 8-bit value with acquire semantics
 *
 * \param ptr Pointer to the value to load
 * \return The loaded value
 */
static inline uint8_t atomic_load_u8(const uint8_t *ptr)
{
	return __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
}

/**
 * \brief Atomically increment an 8-bit counter
 *
 * \param ptr Pointer to the counter
 */
static inline void atomic_inc_u8(uint8_t *ptr)
{
	(void)__atomic_fetch_add(ptr, 1, __ATOMIC_ACQ_REL);
}

/**
 * \brief Atomically decrement an 8-bit counter and return new value
 *
 * \param ptr Pointer to the counter
 * \return The value after decrement
 */
static inline uint8_t atomic_dec_u8(uint8_t *ptr)
{
	return __atomic_sub_fetch(ptr, 1, __ATOMIC_ACQ_REL);
}

/**
 * \brief Atomically set bits in an 8-bit flags field
 *
 * \param ptr Pointer to the flags field
 * \param bits Bits to set
 */
static inline void atomic_set_bits_u8(uint8_t *ptr, uint8_t bits)
{
	(void)__atomic_or_fetch(ptr, bits, __ATOMIC_ACQ_REL);
}

/**
 * \brief Atomically clear bits in an 8-bit flags field
 *
 * \param ptr Pointer to the flags field
 * \param bits Bits to clear
 */
static inline void atomic_clear_bits_u8(uint8_t *ptr, uint8_t bits)
{
	(void)__atomic_and_fetch(ptr, (uint8_t)~bits, __ATOMIC_ACQ_REL);
}

#endif /* TI_ATOMIC_H */
