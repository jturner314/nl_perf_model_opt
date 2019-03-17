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

#include "randomkit.h"

/*
 * Type of design variable values.
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double design_var_t;

/*
 * Type of fitness values.
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double fitness_t;

/*
 * Generates a random population of designs, where the design variable
 * values are within the specified bounds.
 */
void init_random_population(const size_t nmemb, const size_t design_var_count,
                            design_var_t designs[][design_var_count],
                            const design_var_t lower_bounds[],
                            const design_var_t upper_bounds[],
                            rk_state *rng);

/*
 * Determines the suitable indices of parents by tournament selection.
 *
 * The `fitnesses` of the population must be correct.
 */
void ga_tournament_select(const size_t nmemb, const fitness_t fitnesses[],
                          const size_t num_winners, size_t winner_indices[],
                          rk_state *rng);

/*
 * Creates children by combining (crossover) the parents by BLX-alpha.
 *
 * The lengths of `population`, `parent_indices`, and `children` must be `nmemb`.
 * `alpha` is the parameter for BLX-alpha.
 *
 * `children` does not need to be initialized ahead-of-time. If you're
 * keeping track of fitnesses, you may want to update the fitnesses of
 * `children` after this.
 */
void ga_blx_alpha(const size_t nmemb, const size_t design_var_count,
                  const design_var_t population[][design_var_count],
                  const size_t parent_indices[],
                  design_var_t children[][design_var_count],
                  const double alpha,
                  rk_state *rng);

/*
 * Mutates the population (randomly changes the design variables).
 *
 * `design_var_stdevs` specifies the standard deviations of the design
 * variables for Gaussian mutation, and `mutate_probability` specifies the
 * probability that any particular design variable in a design will be mutated.
 *
 * You probably want to update the fitnesses of the population after this.
 */
void ga_mutate(const size_t nmemb, const size_t design_var_count,
               design_var_t population[][design_var_count],
               const design_var_t design_var_stdevs[],
               const double mutate_probability, rk_state *rng);

/*
 * Combines the parent and children populations, keeping `num_keep` of the best
 * parents. The results are written to `designs` and `fitnesses`.
 *
 * The fitnesses of the parents (`fitnesses`) and children (`child_fitnesses`)
 * must be correct on entry.
 */
void ga_cull(const size_t nmemb,
             const size_t design_var_count,
             design_var_t designs[][design_var_count],
             fitness_t fitnesses[],
             const size_t num_keep,
             const design_var_t child_designs[][design_var_count],
             fitness_t child_fitnesses[]);

/*
 * Writes a summary of the fitnesses (min, median, and max) to the provided stream.
 */
void fprintf_fitness_summary(FILE *stream,
                             const size_t nmemb, const fitness_t fitnesses[]);

/*
 * Writes a description of the quartiles of the fitnesses to the provided stream.
 */
void fprintf_fitness_quartiles(FILE *stream,
                               const size_t nmemb, const fitness_t fitnesses[]);
