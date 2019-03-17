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
 * @file bt_ga.h
 *
 * Steps of the genetic algorithm.
 */

#pragma once

#include "bt_population.h"
#include "randomkit.h"

/**
 * Randomly generates initial training stress values.
 *
 * @param[in] nmemb The number of designs in the population.
 * @param[in] num_days The number of training stresses in each design.
 * @param[in] max_daily_stress The maximum possible training stress.
 * @param[out] stresses The population of training stresses to write.
 * @param[in,out] rng The state of the PRNG.
 */
void ga_init_stresses(const size_t nmemb, const size_t num_days,
                      const stress_t max_daily_stress, stress_t **stresses,
                      rk_state *rng);

/**
 * Selects indices of suitable parents for generating children via
 * tournament selection.
 *
 * @param[in] nmemb The number of designs in the population.
 * @param[in] fitnesses The array of penalized objective function values of the
 *   designs.
 * @param[in] num_winners The number of designs to select.
 * @param[out] winner_indices The indices of the selected designs.
 * @param[in,out] rng The state of the PRNG.
 */
void ga_tournament_select(const size_t nmemb, const fitness_t fitnesses[],
                          const size_t num_winners, size_t winner_indices[],
                          rk_state *rng);

/**
 * Generates new children by crossing the parents.
 *
 * You may want to update the objective function and penalty values of @p
 * children after this.
 *
 * @param[in] nmemb The number of designs in each population.
 * @param[in] design_var_count The number of elements in each design.
 * @param[in] population The population of parent designs.
 * @param[in] parent_indices Indices within @p population to use as parents.
 * @param[out] children The population of children to generate.
 * @param[in] alpha The alpha parameter to use for BLX-alpha crossover.
 * @param[in] min The lower bound for any design variable value (values are
 *   clipped to this).
 * @param[in] max The upper bound for any design variable value (values are
 *   clipped to this).
 * @param[in,out] rng The state of the PRNG.
 */
void ga_blx_alpha(const size_t nmemb, const size_t design_var_count,
                  stress_t **population,
                  const size_t parent_indices[],
                  double **children,
                  const double alpha,
                  const double min, const double max,
                  rk_state *rng);

/**
 * Mutates the given population using Gaussian mutation.
 *
 * You probably want to update the objective function and penalty values of @p
 * population after this.
 *
 * @param[in] nmemb The number of designs in each population.
 * @param[in] design_var_count The number of elements in each design.
 * @param[in,out] population The population of designs to mutate.
 * @param[in] stdev Standard deviation for Gaussian mutation.
 * @param[in] min The lower bound for any design variable value (values are
 *   clipped to this).
 * @param[in] max The upper bound for any design variable value (values are
 *   clipped to this).
 * @param[in] mutate_probability Probability that any individual design
 *   variable value will be mutated.
 * @param[in,out] rng The state of the PRNG.
 */
void ga_mutate(const size_t nmemb, const size_t design_var_count,
               double **population,
               const double stdev, const double min, const double max,
               const double mutate_probability, rk_state *rng);

/**
 * Combines the two populations, keeping the best designs.
 *
 * @param[in,out] parents The population of parents.
 * @param[in] children The population of children.
 * @param[in] num_keep Number of the best parents to keep. The rest of the
 *   parents are replaced with the best children.
 */
void ga_cull(bt_population_t *parents, const bt_population_t *children, const size_t num_keep);
