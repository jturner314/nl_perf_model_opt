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
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "bt_model.h"
#include "bt_ga.h"
#include "stats.h"


void ga_init_stresses(const size_t nmemb, const size_t num_days,
                      const stress_t max_daily_stress, stress_t **stresses,
                      rk_state *rng)
{
    for (size_t i = 0; i < nmemb; i++)
        for (size_t j = 0; j < num_days; j++)
            stresses[i][j] = rk_double(rng) * max_daily_stress;
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
                  stress_t **population,
                  const size_t parent_indices[],
                  double **children,
                  const double alpha,
                  const double min, const double max,
                  rk_state *rng)
{
    for (size_t i = 0; i < nmemb-1; i += 2) {
        const double *p1 = population[parent_indices[i]];  // Select first parent
        const double *p2 = population[parent_indices[i+1]];  // Select second parent
        for (size_t j = 0; j < design_var_count; j++) {
            double cmin = p1[j] <= p2[j] ? p1[j] : p2[j];  // Select min value
            double cmax = p1[j] > p2[j] ? p1[j] : p2[j];  // Select max value
            double range = cmax - cmin;  // Select range
            double a = cmin - range * alpha;  // Select lower bound
            double b = cmax + range * alpha;  // Select upper bound
            children[i][j] = fmin(fmax(a + (b - a) * rk_double(rng), min), max); // Set child design
            children[i+1][j] = fmin(fmax(a + (b - a) * rk_double(rng), min), max); // Set child design
        }
    }
}


void ga_mutate(const size_t nmemb, const size_t design_var_count,
               double **population,
               const double stdev, const double min, const double max,
               const double mutate_probability, rk_state *rng)
{
    for (size_t i = 0; i < nmemb; i++) {
        for (size_t j = 0; j < design_var_count; j++) {
            if (rk_double(rng) < mutate_probability) {
                population[i][j] += stdev * rk_gauss(rng);
                population[i][j] = fmin(fmax(population[i][j], min), max);
            }
        }
    }
}


static int compare_size_t(const void *first, const void *second)
{
    return (int)((long long)(*((size_t *) first)) - (long long)(*((size_t *) second)));
}


void ga_cull(bt_population_t *parents, const bt_population_t *children, const size_t num_keep)
{
    const size_t nmemb = parents->nmemb;
    assert(children->nmemb == nmemb);
    assert(num_keep <= nmemb);

    // Copy best parents to the start of the arrays
    size_t sorted_parent_indices[nmemb];
    stats_sort_index(sorted_parent_indices, parents->fitnesses, nmemb);
    qsort(sorted_parent_indices + (nmemb - num_keep), num_keep, sizeof(size_t), compare_size_t);
    for (size_t i = 0; i < num_keep; i++) {
        const size_t s = sorted_parent_indices[i + nmemb - num_keep];
        memmove(parents->stresses[i], parents->stresses[s], parents->num_days * sizeof(stress_t));
        parents->final_performances[i] = parents->final_performances[s];
        parents->penalties[i] = parents->penalties[s];
        parents->fitnesses[i] = parents->fitnesses[s];
    }

    // Copy best children to the rest of the arrays
    size_t sorted_child_indices[nmemb];
    stats_sort_index(sorted_child_indices, children->fitnesses, nmemb);
    for (size_t i = num_keep; i < nmemb; i++) {
        const size_t s = sorted_child_indices[i];
        memcpy(parents->stresses[i], children->stresses[s], children->num_days * sizeof(stress_t));
        parents->final_performances[i] = children->final_performances[s];
        parents->penalties[i] = children->penalties[s];
        parents->fitnesses[i] = children->fitnesses[s];
    }
}
