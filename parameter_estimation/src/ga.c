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

#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ga.h"
#include "randomkit.h"
#include "stats.h"


void init_random_population(const size_t nmemb, const size_t design_var_count,
                            design_var_t designs[][design_var_count],
                            const design_var_t lower_bounds[],
                            const design_var_t upper_bounds[],
                            rk_state *rng)
{
    for (size_t i = 0; i < nmemb; i++) {
        for (size_t j = 0; j < design_var_count; j++) {
            designs[i][j] = lower_bounds[j] +
                rk_double(rng) * (upper_bounds[j] - lower_bounds[j]);
        }
    }
}

void ga_tournament_select(const size_t nmemb, const fitness_t fitnesses[],
                          const size_t num_winners, size_t winner_indices[],
                          rk_state *rng)
{
    for (size_t i = 0; i < num_winners; i++) {
        size_t comp1 = rk_interval(nmemb - 1, rng);
        size_t comp2 = rk_interval(nmemb - 1, rng);
        winner_indices[i] = fitnesses[comp1] >= fitnesses[comp2] ? comp1 : comp2;
    }
}

void ga_blx_alpha(const size_t nmemb, const size_t design_var_count,
                  const design_var_t population[][design_var_count],
                  const size_t parent_indices[],
                  design_var_t children[][design_var_count],
                  const double alpha,
                  rk_state *rng)
{
    for (size_t i = 0; i < nmemb-1; i += 2) {
        const design_var_t *p1 = population[parent_indices[i]];  // Select first parent
        const design_var_t *p2 = population[parent_indices[i+1]];  // Select second parent
        for (size_t j = 0; j < design_var_count; j++) {
            design_var_t cmin = p1[j] <= p2[j] ? p1[j] : p2[j];  // Select min value
            design_var_t cmax = p1[j] > p2[j] ? p1[j] : p2[j];  // Select max value
            design_var_t range = cmax - cmin;  // Select range
            design_var_t a = cmin - range * alpha;  // Select lower bound
            design_var_t b = cmax + range * alpha;  // Select upper bound
            children[i][j] = a + (b - a) * rk_double(rng); // Set child design
            children[i+1][j] = a + (b - a) * rk_double(rng); // Set child design
        }
    }
}

void ga_mutate(const size_t nmemb, const size_t design_var_count,
               design_var_t population[][design_var_count],
               const design_var_t design_var_stdevs[],
               const double mutate_probability, rk_state *rng)
{
    for (size_t i = 0; i < nmemb; i++)
        for (size_t j = 0; j < design_var_count; j++)
            if (rk_double(rng) < mutate_probability)
                population[i][j] += design_var_stdevs[j] * rk_gauss(rng);
}

static int compare_size_t(const void *first, const void *second)
{
    return (int)((long long)(*((size_t *) first)) - (long long)(*((size_t *) second)));
}

void ga_cull(const size_t nmemb,
             const size_t design_var_count,
             design_var_t designs[][design_var_count],
             fitness_t fitnesses[],
             const size_t num_keep,
             const design_var_t child_designs[][design_var_count],
             fitness_t child_fitnesses[])
{
    assert(num_keep <= nmemb);

    // Copy best parents to the start of the arrays.
    size_t sorted_indices[nmemb];
    stats_sort_index(sorted_indices, fitnesses, nmemb);
    qsort(sorted_indices + (nmemb - num_keep), num_keep, sizeof(size_t), compare_size_t);
    for (size_t i = 0; i < num_keep; i++) {
        memmove(designs[i], designs[sorted_indices[i + (nmemb - num_keep)]],
                design_var_count * sizeof(design_var_t));
        fitnesses[i] = fitnesses[sorted_indices[i + (nmemb - num_keep)]];
    }

    // Copy best children to the rest of the arrays.
    size_t sorted_child_indices[nmemb];
    stats_sort_index(sorted_child_indices, child_fitnesses, nmemb);
    for (size_t i = num_keep; i < nmemb; i++) {
        memcpy(designs[i], child_designs[sorted_child_indices[i]], design_var_count * sizeof(design_var_t));
        fitnesses[i] = child_fitnesses[sorted_child_indices[i]];
    }
}

void fprintf_fitness_summary(FILE *stream, const size_t nmemb, const fitness_t fitnesses[])
{
    fitness_t min_fitness, max_fitness, median_fitness;
    fitness_t fitnesses_copy[nmemb];
    memcpy(fitnesses_copy, fitnesses, nmemb * sizeof(fitness_t));
    stats_sort(fitnesses_copy, nmemb);
    min_fitness = stats_quantile_from_sorted(fitnesses_copy, nmemb, 0.0);
    max_fitness = stats_quantile_from_sorted(fitnesses_copy, nmemb, 1.0);
    median_fitness = stats_median_from_sorted(fitnesses_copy, nmemb);
    fprintf(stream, "Min: %lf\tMedian: %lf\t Max: %lf",
            min_fitness, median_fitness, max_fitness);
}

void fprintf_fitness_quartiles(FILE *stream, const size_t nmemb, const fitness_t fitnesses[])
{
    fitness_t min_fitness, q1_fitness, median_fitness, q3_fitness, max_fitness;
    fitness_t fitnesses_copy[nmemb];
    memcpy(fitnesses_copy, fitnesses, nmemb * sizeof(fitness_t));
    stats_sort(fitnesses_copy, nmemb);
    min_fitness = stats_quantile_from_sorted(fitnesses_copy, nmemb, 0.0);
    max_fitness = stats_quantile_from_sorted(fitnesses_copy, nmemb, 1.0);
    median_fitness = stats_median_from_sorted(fitnesses_copy, nmemb);
    q1_fitness = stats_quantile_from_sorted(fitnesses_copy, nmemb, 0.25);
    q3_fitness = stats_quantile_from_sorted(fitnesses_copy, nmemb, 0.75);
    fprintf(stream, "%lf\t%lf\t%lf\t%lf\t%lf",
            min_fitness, q1_fitness, median_fitness, q3_fitness, max_fitness);
}
