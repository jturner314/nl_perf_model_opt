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

#include "stats.h"
#include <math.h>
#include <stdlib.h>

static int compare_doubles(const void *first, const void *second)
{
    const double x1 = *((double *) first);
    const double x2 = *((double *) second);
    if (x1 < x2) {
        return -1;
    } else if (x1 > x2) {
        return 1;
    } else {
        return 0;
    }
}

void stats_sort(double data[], const size_t n)
{
    qsort(data, n, sizeof(double), compare_doubles);
}

static int compare_indices_by_data(
#ifdef MAC_OSX
    void *data, const void *first, const void *second
#else
    const void *first, const void *second, void *data
#endif
)
{
    // Cast types and dereference indices.
    const size_t i1 = *((size_t *) first);
    const size_t i2 = *((size_t *) second);
    const double *xs = (double *) data;

    // Perform comparison.
    const double x1 = xs[i1];
    const double x2 = xs[i2];
    if (x1 < x2) {
        return -1;
    } else if (x1 > x2) {
        return 1;
    } else {
        return 0;
    }
}

void stats_sort_index(size_t indices[], const double data[], const size_t n)
{
    for (size_t i = 0; i < n; i++) {
        indices[i] = i;
    }
    // Strip const qualifier because `qsort_r` doesn't take the pointer as
    // const. This is safe because `qsort_r` doesn't modify the data in this
    // case (since `compare_indices_by_data` doesn't modify the data).
    double *non_const_data = (double *) data;
    qsort_r(indices, n, sizeof(size_t),
#ifdef MAC_OSX
            non_const_data, compare_indices_by_data
#else
            compare_indices_by_data, non_const_data
#endif
    );
}

double stats_median_from_sorted(const double data[], const size_t n)
{
    if (n == 0) {
        return NAN;
    }

    const size_t lower = (n - 1) / 2;
    const size_t upper = n / 2;
    if (lower != upper) {
        return (data[lower] + data[upper]) / 2;
    } else {
        return data[lower];
    }
}

double stats_quantile_from_sorted(const double data[], const size_t n,
                                  const double q)
{
    if (n == 0) {
        return NAN;
    }

    const double prod = q * (n - 1);
    const size_t lower = prod;
    if (lower == n - 1) {
        return data[lower];
    } else {
        const size_t upper = lower + 1;
        const double delta = prod - lower;
        return fma(data[upper] - data[lower], delta, data[lower]);
    }
}

size_t stats_min_index(const double data[], const size_t n)
{
    double min = NAN;
    size_t min_index = 0;
    for (size_t i = 0; i < n; i++) {
        if (isnan(data[i])) {
            return i;
        }
        if (!(data[i] >= min)) {
            min = data[i];
            min_index = i;
        }
    }
    return min_index;
}

size_t stats_max_index(const double data[], const size_t n)
{
    double max = NAN;
    size_t max_index = 0;
    for (size_t i = 0; i < n; i++) {
        if (isnan(data[i])) {
            return i;
        }
        if (!(data[i] <= max)) {
            max = data[i];
            max_index = i;
        }
    }
    return max_index;
}

void stats_min_max(double *min, double *max,
                   const double data[], const size_t n)
{
    *min = NAN;
    *max = NAN;
    for (size_t i = 0; i < n; i++) {
        if (isnan(data[i])) {
            *min = NAN;
            *max = NAN;
            return;
        }
        if (!(data[i] >= *min)) {
            *min = data[i];
        }
        if (!(data[i] <= *max)) {
            *max = data[i];
        }
    }
}
