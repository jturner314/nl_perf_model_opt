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

#include "args.h"
#include "bt_model.h"
#include "bt_params.h"
#include "bt_population.h"
#include "bt_ga.h"
#include "stats.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_PATH_LENGTH 1000
#define MAX_ROUGHNESS_DAYS 14


void run_ga(const size_t num_days,
            const size_t max_generations, const size_t population_size,
            const stress_t max_daily_stress, const double init_penalty_factor,
            const double penalty_factor_rate, const double max_roughness_factor,
            const size_t cull_keep, const double init_blx_alpha, const double blx_alpha_change_rate,
            const double init_mutate_stdev, const double init_mutate_probability,
            const double mutate_change_rate,
            const bt_params_t *parameters, const unsigned long random_seed,
            const char *output_integration, const char *output_population,
            const char *output_convergence, const bool debug,
            stress_t best_stresses[],
            performance_t *best_final_performance, penalty_t *best_penalty, fitness_t *best_fitness)
{
    // Allocate objects
    bt_population_t *designs = bt_population_alloc(population_size, num_days);
    rk_state *rng = malloc(sizeof(rk_state));

    // Temporary variables for the GA
    double penalty_factor = init_penalty_factor;
    double roughness_factor = 0;
    size_t roughness_days = MAX_ROUGHNESS_DAYS;
    double blx_alpha = init_blx_alpha;
    double mutate_stdev = init_mutate_stdev;
    double mutate_probability = init_mutate_probability;
    size_t *winners = malloc(population_size * sizeof(size_t));
    bt_population_t *children = bt_population_alloc(population_size, num_days);

    // Initialize objects
    rk_seed(random_seed, rng);
    ga_init_stresses(population_size, num_days, max_daily_stress, designs->stresses, rng);
    bt_model_update_obj_func(parameters, roughness_days, penalty_factor, roughness_factor,
                             max_daily_stress, designs);

    // Open convergence file
    FILE *conv_file = NULL;
    if (output_convergence) {
        char conv_path[MAX_PATH_LENGTH];
        snprintf(conv_path, MAX_PATH_LENGTH, output_convergence, random_seed);
        if ((conv_file = fopen(conv_path, "w")) == NULL) {
            fprintf(stderr, "Unable to open convergence file: %s.\n", conv_path);
            exit(EXIT_FAILURE);
        }
        fprintf(conv_file, "generation\tmin\tq1\tmedian\tq3\tmax\n");
    }

    // Run the GA.
    for (ssize_t i = 0; i < max_generations; i++) {

        // Update roughness_factor and roughness_days.
        ssize_t min_roughness_generation = max_generations / 5.;
        ssize_t max_roughness_generation = 2 * max_generations / 3.;
        if (min_roughness_generation < i && i < max_roughness_generation) {
            roughness_factor = max_roughness_factor;
            roughness_days = MAX_ROUGHNESS_DAYS * (max_roughness_generation - i) /
                (max_roughness_generation - min_roughness_generation);
            roughness_days = roughness_days > 1 ? roughness_days : 1;
        } else {
            roughness_factor = 0;
        }

        // Update calculated fitnesses with the new penalty and roughness values.
        bt_model_update_penalty_factors(penalty_factor, roughness_factor, roughness_days, designs);

        // Debug output.
        if (debug) {
            fprintf(stderr, "Seed %lu, Generation %zd:\t", random_seed, i+1);
            fprintf_fitness_summary(stderr, population_size, designs->fitnesses);
            fprintf(stderr, "\n");
        }

        // Convergence file output.
        if (output_convergence) {
            fprintf(conv_file, "%zd\t", i+1);
            fprintf_fitness_quartiles(conv_file, population_size, designs->fitnesses);
            fprintf(conv_file, "\n");
        }

        // Run steps of the GA.
        ga_tournament_select(population_size, designs->fitnesses,
                             population_size, winners, rng);
        ga_blx_alpha(population_size, num_days, designs->stresses, winners,
                     children->stresses, blx_alpha, 0., max_daily_stress, rng);
        ga_mutate(population_size, num_days, children->stresses,
                  mutate_stdev, 0., max_daily_stress, mutate_probability, rng);
        bt_model_update_obj_func(parameters, roughness_days, penalty_factor, roughness_factor,
                                 max_daily_stress, children);
        ga_cull(designs, children, cull_keep);

        // Update penalty factor and GA parameters.
        penalty_factor *= penalty_factor_rate;
        blx_alpha *= blx_alpha_change_rate;
        mutate_stdev *= mutate_change_rate;
        mutate_probability *= mutate_change_rate;
    }

    // Close convergence file
    if (output_convergence) {
        fclose(conv_file);
    }

    // Copy the best design to the output variables
    size_t best_index = stats_max_index(designs->fitnesses, population_size);
    memcpy(best_stresses, designs->stresses[best_index], num_days * sizeof(stress_t));
    *best_final_performance = designs->final_performances[best_index];
    *best_penalty = designs->penalties[best_index];
    *best_fitness = designs->fitnesses[best_index];

    // Write final population.
    if (output_population) {
        char pop_path[MAX_PATH_LENGTH];
        snprintf(pop_path, MAX_PATH_LENGTH, output_population, random_seed);
        FILE *pop_file = fopen(pop_path, "w");
        bt_population_write(pop_file, designs);
        fclose(pop_file);
    }

    // Write integration of best design.
    if (output_integration) {
        char integ_path[MAX_PATH_LENGTH];
        snprintf(integ_path, MAX_PATH_LENGTH, output_integration, random_seed);
        FILE *integ_file = fopen(integ_path, "w");
        bt_model_fprint_integrate(integ_file, num_days, best_stresses, max_daily_stress, parameters);
        fclose(integ_file);
    }

    // Free objects
    bt_population_free(children);
    bt_population_free(designs);
    free(winners);
    free(rng);
}


int main(int argc, char *argv[])
{
    // Parse the arguments.
    arguments_t args;
    parse_arguments(argc, argv, &args);
    if (args.debug) {
        fprintf(stderr, "Using arguments:\n");
        fprintf_arguments(stderr, &args);
        fprintf(stderr, "\n");
    }

    // Load the input files.
    bt_params_t *parameters;
    if ((parameters = bt_params_load(args.params_path)) == NULL) {
        fprintf(stderr, "Unable to parse paramaters file.\n");
        exit(EXIT_FAILURE);
    }

    // Create the output population.
    bt_population_t *best_designs = bt_population_alloc(
        args.num_iterations, args.num_days);

    // Run the GA.
    for (size_t i = 0; i < args.num_iterations; i++) {
        fprintf(stderr, "Iteration %zd\n", i+1);
        fflush(stderr);
        run_ga(args.num_days,
               args.max_generations,
               args.population_size,
               args.max_daily_stress,
               args.init_penalty_factor,
               args.penalty_factor_rate,
               args.max_roughness_factor,
               args.cull_keep,
               args.init_blx_alpha,
               args.blx_alpha_change_rate,
               args.init_mutate_stdev,
               args.init_mutate_probability,
               args.mutate_change_rate,
               parameters,
               i + 1,
               args.output_integration,
               args.output_population,
               args.output_convergence,
               args.debug,
               best_designs->stresses[i],
               &best_designs->final_performances[i],
               &best_designs->penalties[i],
               &best_designs->fitnesses[i]);
    }

    // Write the output file.
    FILE *output_file = fopen(args.output_path, "w");
    bt_population_write(output_file, best_designs);
    fclose(output_file);

    // Cleanup the input data.
    bt_params_free(parameters);

    return EXIT_SUCCESS;
}
