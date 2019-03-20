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
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

bool approx_eq(const double a, const double b, const double eps)
{
    return fabs(a - b) < eps;
}

void test_stats_sort()
{
    double a[] = {-0.188, 0.262, 0.648, -0.241, 0.213, -0.145, 0.604, 0.721};
    const size_t len = 8;
    assert(sizeof(a) / sizeof(double) == len);

    const double correct[] = {-0.241, -0.188, -0.145, 0.213, 0.262, 0.604, 0.648, 0.721};
    assert(sizeof(correct) / sizeof(double) == len);

    stats_sort(a, len);
    for (size_t i = 0; i < len; i++) {
        assert(a[i] == correct[i]);
    }
}

void test_stats_sort_index()
{
    const double a[] = {-0.188, 0.262, 0.648, -0.241, 0.213, -0.145, 0.604, 0.721};
    const size_t len = 8;
    assert(sizeof(a) / sizeof(double) == len);

    const size_t correct[] = {3, 0, 5, 4, 1, 6, 2, 7};
    assert(sizeof(correct) / sizeof(size_t) == len);

    size_t indices[len];
    stats_sort_index(indices, a, len);
    for (size_t i = 0; i < len; i++) {
        assert(indices[i] == correct[i]);
    }
}

void test_stats_median_from_sorted()
{
    const double a[] = {-0.595, -0.505, -0.464, -0.332, 0.248, 0.353, 0.802, 0.876};
    const size_t len = 8;
    assert(sizeof(a) / sizeof(double) == len);

    const double even = stats_median_from_sorted(a, len);
    assert(approx_eq(even, -0.042, 1e-9));

    const double odd = stats_median_from_sorted(a, len - 1);
    assert(approx_eq(odd, -0.332, 1e-9));

    const double empty[] = {};
    assert(isnan(stats_median_from_sorted(empty, 0)));
}

void test_stats_quantile_from_sorted()
{
    const double a[] = {-0.595, -0.505, -0.464, -0.332, 0.248, 0.353, 0.802, 0.876};
    const size_t len = 8;
    assert(sizeof(a) / sizeof(double) == len);

    const double qs[] = {    0.0,    0.1,     0.25,    0.5,    0.8,   1.0};
    const double out[] = {-0.595, -0.532, -0.47425, -0.042, 0.6224, 0.876};
    for (size_t i = 0; i < sizeof(qs) / sizeof(double); i++) {
        const double quantile = stats_quantile_from_sorted(a, len, qs[i]);
        assert(approx_eq(quantile, out[i], 1e-9));
    }

    const double empty[] = {};
    assert(isnan(stats_quantile_from_sorted(empty, 0, 0.5)));
}

void test_stats_min_index()
{
    double a[] = {-0.188, 0.262, 0.648, -0.241, 0.213, -0.145, 0.604, 0.721};
    const size_t len = 8;
    assert(sizeof(a) / sizeof(double) == len);

    assert(stats_min_index(a, len) == 3);

    a[4] = NAN;
    a[6] = NAN;

    assert(stats_min_index(a, len) == 4);

    const double empty[] = {};
    assert(stats_min_index(empty, 0) == 0);
}

void test_stats_max_index()
{
    double a[] = {-0.188, 0.262, 0.648, -0.241, 0.213, 0.721, -0.145, 0.604};
    const size_t len = 8;
    assert(sizeof(a) / sizeof(double) == len);

    assert(stats_max_index(a, len) == 5);

    a[4] = NAN;
    a[6] = NAN;

    assert(stats_max_index(a, len) == 4);

    const double empty[] = {};
    assert(stats_max_index(empty, 0) == 0);
}

void test_stats_min_max()
{
    double a[] = {-0.188, 0.262, 0.648, -0.241, 0.213, -0.145, 0.604, 0.721};
    const size_t len = 8;
    assert(sizeof(a) / sizeof(double) == len);

    double min, max;

    stats_min_max(&min, &max, a, len);
    assert(min == -0.241);
    assert(max == 0.721);

    a[4] = NAN;
    a[6] = NAN;

    stats_min_max(&min, &max, a, len);
    assert(isnan(min));
    assert(isnan(max));

    const double empty[] = {};
    double min2, max2;
    stats_min_max(&min2, &max2, empty, 0);

    assert(isnan(min2));
    assert(isnan(max2));
}

int main(int argc, char *argv[])
{
    test_stats_sort();
    test_stats_sort_index();
    test_stats_median_from_sorted();
    test_stats_quantile_from_sorted();
    test_stats_min_index();
    test_stats_max_index();
    test_stats_min_max();

    printf("Success!\n");
}
