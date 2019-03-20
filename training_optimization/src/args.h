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
 * @file args.h
 *
 * Command line argument parsing and formatting.
 */

#pragma once

#include <stdbool.h>
#include <stdio.h>

/**
 * Command line arguments.
 */
typedef struct arguments_t {
    // Positional arguments
    char *params_path;
    char *output_path;

    // Objective function
    size_t num_days;
    double max_daily_stress;
    double init_penalty_factor;
    double penalty_factor_rate;
    double max_roughness_factor;

    // Genetic algorithm
    size_t num_iterations;
    size_t max_generations;
    size_t population_size;
    size_t cull_keep;
    double init_blx_alpha;
    double blx_alpha_change_rate;
    double init_mutate_stdev;
    double init_mutate_probability;
    double mutate_change_rate;

    // Extra output
    char *output_integration;
    char *output_population;
    char *output_convergence;

    // Debug
    bool debug;
} arguments_t;

/**
 * Prints the usage to `stderr` and exit.
 *
 * @param[in] program_name Name of this program to print in usage.
 */
void usage(const char *program_name);

/**
 * Parses the command line arguments.
 *
 * @param[in] argc Count of command line arguments (i.e. length of @p argv).
 * @param[in] argv Array of command line arguments.
 * @param[out] args Parsed command line arguments.
 */
void parse_arguments(const int argc, char * const argv[], arguments_t *args);

/**
 * Writes the command line arguments to the given stream.
 *
 * @param[in,out] stream Stream to write to.
 * @param[in] args Arguments to write.
 */
void fprintf_arguments(FILE *stream, const arguments_t *args);
