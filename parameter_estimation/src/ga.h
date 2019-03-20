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
 * @file ga.h
 *
 * Steps of the genetic algorithm.
 */

#pragma once

#include "randomkit.h"

/**
 * Type of design variable values.
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double design_var_t;

/**
 * Type of fitness values.
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double fitness_t;

/**
 * Generates a random population of designs, where the design variable
 * values are within the specified bounds.
 *
 * @param[in] nmemb The number of designs in the population.
 * @param[in] design_var_count The number of design variables in each design.
 * @param[out] designs The population of designs to write.
 * @param[in] lower_bounds The lower bounds for the design variables.
 * @param[in] upper_bounds The upper bounds for the design variables.
 * @param[in,out] rng The state of the PRNG.
 */
void init_random_population(const size_t nmemb, const size_t design_var_count,
                            design_var_t designs[][design_var_count],
                            const design_var_t lower_bounds[],
                            const design_var_t upper_bounds[],
                            rk_state *rng);

/**
 * Selects indices of suitable parents by tournament selection.
 *
 * @param[in] nmemb The number of designs in the population.
 * @param[in] fitnesses The objective function values of the designs.
 * @param[in] num_winners The number of designs to select.
 * @param[out] winner_indices The indices of the selected designs.
 * @param[in,out] rng The state of the PRNG.
 */
void ga_tournament_select(const size_t nmemb, const fitness_t fitnesses[],
                          const size_t num_winners, size_t winner_indices[],
                          rk_state *rng);

/**
 * Creates children by combining (crossover) the parents by BLX-alpha.
 *
 * You may want to update the objective function values of @p children after
 * this.
 *
 * @param[in] nmemb The number of designs in each population. Also, the number
 *   of parent indices.
 * @param[in] design_var_count The number of variables in each design.
 * @param[in] population The population of parent designs.
 * @param[in] parent_indices Indices within @p population to use as parents.
 * @param[out] children The population of children to generate.
 * @param[in] alpha The alpha parameter to use for BLX-alpha crossover.
 * @param[in,out] rng The state of the PRNG.
 *
 * @note Ideally, @p population would be defined as `const design_var_t (*const
 * population)[design_var_count]`, but due to limitations in the C standard,
 * that would require callers to make an explicit cast. [See RATIONALE for more
 * information.](http://pubs.opengroup.org/onlinepubs/9699919799/functions/exec.html)
 */
void ga_blx_alpha(const size_t nmemb, const size_t design_var_count,
                  design_var_t (*const population)[design_var_count],
                  const size_t parent_indices[],
                  design_var_t (*children)[design_var_count],
                  const double alpha,
                  rk_state *rng);

/**
 * Mutates the population (randomly changes the design variables) using
 * Gaussian mutation.
 *
 * You probably want to update the objective function values of @p population
 * after this.
 *
 * @param[in] nmemb The number of designs in each population.
 * @param[in] design_var_count The number of variables in each design.
 * @param[in,out] population The population of designs to mutate.
 * @param[in] design_var_stdevs Standard deviations for Gaussian mutation.
 * @param[in] mutate_probability Probability that any individual design
 *   variable value will be mutated.
 * @param[in,out] rng The state of the PRNG.
 */
void ga_mutate(const size_t nmemb, const size_t design_var_count,
               design_var_t population[][design_var_count],
               const design_var_t design_var_stdevs[],
               const double mutate_probability, rk_state *rng);

/**
 * Combines the two populations, keeping @p num_keep of the best parents. The
 * results are written to @p designs and @p fitnesses.
 *
 * The fitnesses of the parents (@p fitnesses) and children (@p
 * child_fitnesses) must be correct on entry.
 *
 * @param[in] nmemb The number of designs in each population.
 * @param[in] design_var_count The number of variables in each design.
 * @param[in,out] designs The parent population (and the output population).
 * @param[in,out] fitnesses The parent objective function values (and the
 *   output objective function values).
 * @param[in] num_keep Number of the best parents to keep. The rest of the
 *   parents are replaced with the best children.
 * @param[in] child_designs The population of children.
 * @param[in] child_fitnesses The objective function values of the child
 *   population.
 *
 * @note Ideally, @p child_designs would be defined as `const design_var_t
 * (*const child_designs)[design_var_count]`, but due to limitations in the C
 * standard, that would require callers to make an explicit cast. [See
 * RATIONALE for more information.](http://pubs.opengroup.org/onlinepubs/9699919799/functions/exec.html)
 */
void ga_cull(const size_t nmemb,
             const size_t design_var_count,
             design_var_t (*designs)[design_var_count],
             fitness_t fitnesses[],
             const size_t num_keep,
             design_var_t (*const child_designs)[design_var_count],
             fitness_t child_fitnesses[]);

/**
 * Writes a summary (min/median/max) of the objective function values to the
 * given stream.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] nmemb Number of objective function values.
 * @param[in] fitnesses Array of objective function values.
 */
void fprintf_fitness_summary(FILE *stream,
                             const size_t nmemb, const fitness_t fitnesses[]);

/**
 * Writes a summary (min/q1/median/q3/max) of the objective function values to
 * the given stream.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] nmemb Number of objective function values.
 * @param[in] fitnesses Array of objective function values.
 */
void fprintf_fitness_quartiles(FILE *stream,
                               const size_t nmemb, const fitness_t fitnesses[]);
