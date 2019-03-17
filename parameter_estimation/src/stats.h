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

#pragma once

#include <stddef.h>

/*
 * Sorts the data in increasing order.
 *
 * The ordering of NAN values is undefined. The sort is not stable.
 */
void stats_sort(double data[], const size_t n);

/*
 * Writes the indices to `indices` that would sort the data in increasing order.
 *
 * For example, `indices[0]` will be the index of the minimum value in `data`.
 * `indices` does not need to be initialized ahead-of-time.
 *
 * The ordering of NAN values is undefined. The sort is not stable.
 */
void stats_sort_index(size_t indices[], const double data[], const size_t n);

/*
 * Returns the median of the `data` containing `n` elements.
 *
 * If `n` is even, the result is the midpoint of the two middle values. The
 * data must already be sorted for the result to be correct.
 */
double stats_median_from_sorted(const double data[], const size_t n);

/*
 * Returns the `q`th quantile of the `data` containing `n` elements.
 *
 * For example, if `q` is 0.5, this is the median. Uses linear interpolation
 * between the two values on either side of the quantile. The data must already
 * be sorted for the result to be correct.
 *
 * `q` must be between 0 and 1, inclusive.
 */
double stats_quantile_from_sorted(const double data[], const size_t n,
                                  const double q);

/*
 * Returns the index of the minimum value in the data.
 *
 * If any elements are `NAN`, the index to the first `NAN` element is returned.
 */
size_t stats_min_index(const double data[], const size_t n);

/*
 * Returns the index of the maximum value in the data.
 *
 * If any elements are `NAN`, the index to the first `NAN` element is returned.
 */
size_t stats_max_index(const double data[], const size_t n);

/*
 * Writes the minimum and maximum value of the data to `min` and `max`.
 *
 * If any elements are `NAN`, `min` and `max` are set to `NAN`.
 */
void stats_min_max(double *min, double *max,
                   const double data[], const size_t n);
