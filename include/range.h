#pragma once

#include "random.h"

/**
 * @brief Struct that has a float range.
 */
typedef struct {
	/// Start of the range
	float start;
	/// End of the range
	float end;
} RangeFloat;

/**
 * @brief Struct that has a float range.
 */
typedef struct {
	/// Start of the range
	int start;
	/// End of the range
	int end;
} RangeInt;

/**
 * @brief Get a random value from the range.
 *
 * @param range
 *        Range to get start and end from.
 */
inline float range_get_from_float(RangeFloat *range) {
	return RANDRF(range->start, range->end);
}

/**
 * @brief Get a random value from the range.
 *
 * @param range
 *        Range to get start and end from.
 */
inline float range_get_from_int(RangeInt *range) {
	return RANDR(range->start, range->end);
}
