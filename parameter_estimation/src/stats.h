/*
 * Copyright 2015-2019 Duke University
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License Version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License Version 2
 * along with this program. If not, see
 * <https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 */

/**
 * @file stats.h
 *
 * Statistics and sorting routines.
 */

#pragma once

#include <stddef.h>

/**
 * Sorts the data in increasing order.
 *
 * The ordering of `NAN` values is undefined. The sort is not stable.
 *
 * @param[in,out] data Data array.
 * @param[in] n Number of elements in @p data.
 */
void stats_sort(double data[], const size_t n);

/**
 * Writes the indices to @p indices that would sort the data in increasing
 * order.
 *
 * For example, `indices[0]` will be the index of the minimum value in @p data.
 * @p indices does not need to be initialized ahead-of-time.
 *
 * The ordering of NAN values is undefined. The sort is not stable.
 *
 * @param[out] indices Indices that would sort the data in increasing order.
 * @param[in] data Data array.
 * @param[in] n Number of elements in @p data.
 */
void stats_sort_index(size_t indices[], const double data[], const size_t n);

/**
 * Returns the median of the @p data containing @p n elements.
 *
 * If @p n is even, the result is the midpoint of the two middle values. The
 * data must already be sorted for the result to be correct.
 *
 * @param[in] data Data array.
 * @param[in] n Number of elements in @p array.
 * @returns The median of the @p data.
 */
double stats_median_from_sorted(const double data[], const size_t n);

/**
 * Returns the @p q th quantile of the @p data containing @p n elements.
 *
 * For example, if @p q is 0.5, this is the median. Uses linear interpolation
 * between the two values on either side of the quantile. The data must already
 * be sorted for the result to be correct.
 *
 * @param[in] data Data array.
 * @param[in] n Number of elements in @p data.
 * @param[in] q Quantile to compute; must be between 0 and 1, inclusive.
 * @returns The @p q th quantile of the @p data.
 */
double stats_quantile_from_sorted(const double data[], const size_t n,
                                  const double q);

/**
 * Returns the index of the minimum value in the @p data.
 *
 * If any elements are `NAN`, the index to the first `NAN` element is returned.
 *
 * @param[in] data Data array.
 * @param[in] n Number of elements in @p data.
 * @returns The index of the minimum value in the @p data.
 */
size_t stats_min_index(const double data[], const size_t n);

/**
 * Returns the index of the maximum value in the @p data.
 *
 * If any elements are `NAN`, the index to the first `NAN` element is returned.
 *
 * @param[in] data Data array.
 * @param[in] n Number of elements in @p data.
 * @returns The index of the maximum value in the @p data.
 */
size_t stats_max_index(const double data[], const size_t n);

/**
 * Writes the minimum and maximum value of the data to @p min and @p max.
 *
 * If any elements are `NAN`, @p min and @p max are set to `NAN`.
 *
 * @param[out] min Minimum value of @p data.
 * @param[out] max Maximum value of @p data.
 * @param[in] data Data array.
 * @param[in] n Number of elements in @p data.
 */
void stats_min_max(double *min, double *max,
                   const double data[], const size_t n);
