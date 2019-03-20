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
 * @file bt_population.h
 *
 * GA population types and related functions.
 */

#pragma once

#include <stdio.h>

/**
 * Type of training stress values.
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double stress_t;

/**
 * Type of training performance values.
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double performance_t;

/**
 * Type of penalty function values.
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double penalty_t;

/**
 * Type of penalized objective function values.
 *
 * Note that this is not the "fitness" portion of the nonlinear physiological
 * model; instead, it describes values of the penalized objective function
 * (i.e. how good a design is).
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double fitness_t;

/**
 * A population of designs and associated objective function and penalty
 * values.
 *
 * A design consists of a sequence of training stresses for a given number of
 * days.
 */
typedef struct bt_population_t {
    /**
     * Number of member designs in the population.
     */
    size_t nmemb;
    /**
     * Number of days of training stresses for each design.
     */
    size_t num_days;
    /**
     * 2-D array of training stresses.
     *
     * The first index is the member, and the second index is the day.
     */
    stress_t **stresses;
    /**
     * Predicted performances at the end of all the training stresses.
     */
    performance_t *final_performances;
    /**
     * Penalties corresponding to the constraint function.
     */
    penalty_t *penalties;
    /**
     * Penalties corresponding to the "roughnesses" of the sequences of
     * training stresses.
     */
    penalty_t *roughnesses;
    /**
     * Penalized objective function values.
     */
    fitness_t *fitnesses;
} bt_population_t;

/**
 * Allocates a new population.
 *
 * The returned pointer must be freed with bt_population_free().
 *
 * @param[in] nmemb Number of members (designs) in the population.
 * @param[in] num_days Number of training stresses in each design.
 * @returns A pointer to the population.
 */
bt_population_t *bt_population_alloc(const size_t nmemb, const size_t num_days);

/**
 * Writes the population data to the given stream.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] population The population to write.
 */
void bt_population_write(FILE *stream, const bt_population_t *population);

/**
 * Frees a population allocated with bt_population_alloc().
 *
 * @param[in] population The population to free.
 */
void bt_population_free(bt_population_t *population);

/**
 * Writes a summary (min/median/max) of the penalized objective function values
 * to the given stream.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] nmemb Number of penalized objective function values.
 * @param[in] fitnesses Array of penalized objective function values.
 */
void fprintf_fitness_summary(FILE *stream,
                             const size_t nmemb, const fitness_t fitnesses[]);

/**
 * Writes a summary (min/q1/median/q3/max) of the penalized objective function
 * values to the given stream.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] nmemb Number of penalized objective function values.
 * @param[in] fitnesses Array of penalized objective function values.
 */
void fprintf_fitness_quartiles(FILE *stream,
                               const size_t nmemb, const fitness_t fitnesses[]);
