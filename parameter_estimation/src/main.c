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

#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bt_bounds.h"
#include "bt_data.h"
#include "bt_trials.h"
#include "bt_model.h"
#include "ga.h"
#include "randomkit.h"
#include "stats.h"


#define MAX_PATH_LENGTH 1000


struct arguments {
    char *bounds_path;
    char *data_path;
    char *trials_path;
    char *output_path;
    size_t num_iterations;
    size_t max_generations;
    size_t population_size;
    size_t cull_keep;
    double mutate_probability;
    double blx_alpha;
    char *output_integration;
    char *output_population;
    char *output_convergence;
    bool debug;
};


void fail(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}


static void usage(const char *program_name)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s [OPTION...] BOUNDS_PATH DATA_PATH TRIALS_PATH OUTPUT_PATH\n", program_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Positional arguments:\n");
    fprintf(stderr, "  BOUNDS_PATH  Path to file with bounds and stdevs for model parameters.\n");
    fprintf(stderr, "  DATA_PATH    Path to file with training test data.\n");
    fprintf(stderr, "  TRIALS_PATH  Path to file with the indices of the performance trials. If a\n");
    fprintf(stderr, "                 '%%' char is in the string, then it is treated as a pattern\n");
    fprintf(stderr, "                 where the input is the iteration number.\n");
    fprintf(stderr, "  OUTPUT_PATH  Path to output file for writing optimal designs.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -nCOUNT, --num-iterations=COUNT     Number of iterations of the genetic\n");
    fprintf(stderr, "                                         algorithm.\n");
    fprintf(stderr, "  -gCOUNT, --max-generations=COUNT    Maximum number of generations.\n");
    fprintf(stderr, "  -pCOUNT, --population-size=COUNT    Number of individuals in each generation.\n");
    fprintf(stderr, "  -kCOUNT, --cull-keep=COUNT          Number of individuals from the previous\n");
    fprintf(stderr, "                                         generation to keep when culling.\n");
    fprintf(stderr, "  -mFLOAT, --mutate-probability=FLOAT Probability of mutating each design\n");
    fprintf(stderr, "                                         variable.\n");
    fprintf(stderr, "  -aFLOAT, --blx-alpha=FLOAT          Alpha to use for BLX-alpha crossover.\n");
    fprintf(stderr, "  -i[PATTERN], --output-integration[=PATTERN]\n");
    fprintf(stderr, "                                      Output the integration of the best design\n");
    fprintf(stderr, "                                        from each iteration. PATTERN specifies\n");
    fprintf(stderr, "                                        the names of the files, where %%zd is\n");
    fprintf(stderr, "                                        replaced by the iteration number.\n");
    fprintf(stderr, "  -w[PATTERN], --output-population[=PATTERN]\n");
    fprintf(stderr, "                                      Output the final population from each\n");
    fprintf(stderr, "                                        iteration. PATTERN specifies the names\n");
    fprintf(stderr, "                                        of the files, where %%zd is replaced by\n");
    fprintf(stderr, "                                        the iteration number.\n");
    fprintf(stderr, "  -c[PATTERN], --output-convergence[=PATTERN]\n");
    fprintf(stderr, "                                      Output the fitness quartiles of each\n");
    fprintf(stderr, "                                        generation from each iteration. PATTERN\n");
    fprintf(stderr, "                                        specifies the names of the files, where\n");
    fprintf(stderr, "                                        %%zd is replaced by the iteration\n");
    fprintf(stderr, "                                        number.\n");
    fprintf(stderr, "  -d, --debug                         Show debug output.\n");
    fprintf(stderr, "  -h, --help                          Show this message.\n");
    exit(EXIT_FAILURE);
}


static void parse_arguments(const int argc, char * const argv[], struct arguments *args)
{
    // Set defaults
    args->bounds_path = NULL;
    args->data_path = NULL;
    args->trials_path = NULL;
    args->output_path = NULL;
    args->num_iterations = 1;
    args->max_generations = 100;
    args->population_size = 100;
    args->cull_keep = 10;
    args->mutate_probability = 0.1;
    args->blx_alpha = 0.5;
    args->output_integration = NULL;
    args->output_population = NULL;
    args->output_convergence = NULL;
    args->debug = false;

    // Options
    static const struct option long_options[] = {
        {"num-iterations", 1, NULL, 'n'},
        {"max-generations", 1, NULL, 'g'},
        {"population-size", 1, NULL, 'p'},
        {"cull-keep", 1, NULL, 'k'},
        {"mutate-probability", 1, NULL, 'm'},
        {"blx-alpha", 1, NULL, 'a'},
        {"output-integration", 2, NULL, 'i'},
        {"output-population", 2, NULL, 'w'},
        {"debug", 0, NULL, 'd'},
        {"help", 0, NULL, 'h'},
        {NULL}
    };

    // Parse options
    int c;
    while ((c = getopt_long(argc, argv, "n:g:p:k:m:a:i::w::c::dh", long_options, NULL)) != -1) {
        switch (c) {
        case 'n':
            if (sscanf(optarg, "%zd", &args->num_iterations) != 1)
                usage(argv[0]);
            break;
        case 'g':
            if (sscanf(optarg, "%zd", &args->max_generations) != 1)
                usage(argv[0]);
            break;
        case 'p':
            if (sscanf(optarg, "%zd", &args->population_size) != 1)
                usage(argv[0]);
            break;
        case 'k':
            if (sscanf(optarg, "%zd", &args->cull_keep) != 1)
                usage(argv[0]);
            break;
        case 'm':
            if (sscanf(optarg, "%lf", &args->mutate_probability) != 1)
                usage(argv[0]);
            break;
        case 'a':
            if (sscanf(optarg, "%lf", &args->blx_alpha) != 1)
                usage(argv[0]);
            break;
        case 'i':
            if (optarg)
                args->output_integration = optarg;
            else
                args->output_integration = "integration%04zd.tsv";
            break;
        case 'w':
            if (optarg)
                args->output_population = optarg;
            else
                args->output_population = "population%04zd.tsv";
            break;
        case 'c':
            if (optarg)
                args->output_convergence = optarg;
            else
                args->output_convergence = "convergence%04zd.tsv";
            break;
        case 'd':
            args->debug = true;
            break;
        case 'h':
        case '?':
            usage(argv[0]);
            break;
        default:
            fail("Error: getopt returned character code 0%o\n", c);
        }
    }

    // Parse positional args
    static const int num_required_positional_args = 4;
    if (argc - optind == num_required_positional_args) {
        args->bounds_path = argv[optind++];
        args->data_path = argv[optind++];
        args->trials_path = argv[optind++];
        args->output_path = argv[optind++];
    } else if (argc - optind < num_required_positional_args) {
        fprintf(stderr, "%s: missing required positional arguments\n", argv[0]);
        usage(argv[0]);
    } else {
        fprintf(stderr, "%s: too many positional arguments\n", argv[0]);
        usage(argv[0]);
    }
}


void fprintf_arguments(FILE *stream, struct arguments *args)
{
    fprintf(stream, "BOUNDS_PATH = %s\n", args->bounds_path);
    fprintf(stream, "DATA_PATH = %s\n", args->data_path);
    fprintf(stream, "TRIALS_PATH = %s\n", args->trials_path);
    fprintf(stream, "OUTPUT_PATH = %s\n", args->output_path);
    fprintf(stream, "num-iterations = %zd\n", args->num_iterations);
    fprintf(stream, "max-generations = %zd\n", args->max_generations);
    fprintf(stream, "population-size = %zd\n", args->population_size);
    fprintf(stream, "cull-keep = %zd\n", args->cull_keep);
    fprintf(stream, "mutate-probability = %lf\n", args->mutate_probability);
    fprintf(stream, "blx-alpha = %lf\n", args->blx_alpha);
    fprintf(stream, "output-integration = %s\n", args->output_integration);
    fprintf(stream, "output-population = %s\n", args->output_population);
    fprintf(stream, "output-convergence = %s\n", args->output_convergence);
    fprintf(stream, "debug = %d\n", args->debug);
}


void run_ga(design_var_t best_design[], fitness_t *best_mean_abs_residual,
            const size_t max_generations, const size_t population_size,
            const size_t cull_keep, const double mutate_probability,
            const double blx_alpha, const bt_design_bounds_t *bt_design_bounds,
            const bt_data_t *bt_data, const bt_trials_t *bt_trials,
            const unsigned long random_seed, const char *output_integration,
            const char *output_population, const char *output_convergence,
            const bool debug)
{
    // Allocate objects
    design_var_t (*designs)[DESIGN_VAR_COUNT] = malloc(population_size * DESIGN_VAR_COUNT * sizeof(design_var_t));
    fitness_t *fitnesses = malloc(population_size * sizeof(fitness_t));
    rk_state *rng = malloc(sizeof(rk_state));

    // Temporary variables for the GA
    size_t *winners = malloc(population_size * sizeof(size_t));
    design_var_t (*children)[DESIGN_VAR_COUNT] = malloc(population_size * DESIGN_VAR_COUNT * sizeof(design_var_t));
    fitness_t *child_fitnesses = malloc(population_size * sizeof(fitness_t));

    // Initialize objects
    rk_seed(random_seed, rng);
    init_random_population(population_size, DESIGN_VAR_COUNT, designs,
                           bt_design_bounds->lower_bounds, bt_design_bounds->upper_bounds, rng);
    bt_model_update_fitnesses(population_size, designs, fitnesses, NULL, bt_data, bt_trials);

    // Open convergence file
    FILE *conv_file = NULL;
    if (output_convergence) {
        char conv_path[MAX_PATH_LENGTH];
        snprintf(conv_path, MAX_PATH_LENGTH, output_convergence, random_seed);
        if ((conv_file = fopen(conv_path, "w")) == NULL)
            fail("Unable to open convergence file: %s.\n", conv_path);
        fprintf(conv_file, "generation\tmin\tq1\tmedian\tq3\tmax\n");
    }

    // Run the GA
    for (ssize_t i = 0; i < max_generations; i++) {
        if (debug) {
            fprintf(stderr, "Seed %lu, Generation %zd:\t", random_seed, i+1);
            fprintf_fitness_summary(stderr, population_size, fitnesses);
            fprintf(stderr, "\n");
        }
        if (output_convergence) {
            fprintf(conv_file, "%zd\t", i+1);
            fprintf_fitness_quartiles(conv_file, population_size, fitnesses);
            fprintf(conv_file, "\n");
        }
        ga_tournament_select(population_size, fitnesses,
                             population_size, winners, rng);
        ga_blx_alpha(population_size, DESIGN_VAR_COUNT, designs, winners,
                     children, blx_alpha, rng);
        ga_mutate(population_size, DESIGN_VAR_COUNT, children,
                  bt_design_bounds->stdevs, mutate_probability, rng);
        bt_model_update_fitnesses(population_size, children, child_fitnesses, NULL, bt_data, bt_trials);
        ga_cull(population_size, DESIGN_VAR_COUNT,
                designs, fitnesses, cull_keep,
                children, child_fitnesses);
    }

    // Close convergence file
    if (output_convergence) {
        fclose(conv_file);
    }

    // Copy the best design to the output variables
    size_t best_index = stats_max_index(fitnesses, population_size);
    design_var_t min_error = bt_model_calculate_error(designs[best_index], bt_data, bt_trials);
    memcpy(best_design, designs[best_index], sizeof(design_var_t[DESIGN_VAR_COUNT]));
    *best_mean_abs_residual = min_error / bt_trials->size;

    // Write final population.
    if (output_population) {
        char pop_path[MAX_PATH_LENGTH];
        snprintf(pop_path, MAX_PATH_LENGTH, output_population, random_seed);
        FILE *pop_file = fopen(pop_path, "w");
        fitness_t mean_abs_residuals[population_size];
        bt_model_update_fitnesses(population_size, designs, NULL,
                                  mean_abs_residuals, bt_data, bt_trials);
        bt_model_fprint_designs(pop_file, population_size, designs, mean_abs_residuals);
        fclose(pop_file);
    }

    // Write integration of best design.
    if (output_integration) {
        bt_data_t *integ_data = bt_data_copy(bt_data);
        bt_model_integrate(designs[best_index], integ_data);
        char integ_path[MAX_PATH_LENGTH];
        snprintf(integ_path, MAX_PATH_LENGTH, output_integration, random_seed);
        FILE *integ_file = fopen(integ_path, "w");
        bt_data_write(integ_file, integ_data);
        fclose(integ_file);
    }

    // Free objects
    free(child_fitnesses);
    free(children);
    free(winners);
    free(rng);
    free(fitnesses);
    free(designs);
}


int main(int argc, char *argv[])
{
    // Parse the arguments.
    struct arguments args;
    parse_arguments(argc, argv, &args);
    if (args.debug) {
        fprintf(stderr, "Using arguments:\n");
        fprintf_arguments(stderr, &args);
        fprintf(stderr, "\n");
    }

    // Load the input files.
    bt_data_t *bt_data;
    if ((bt_data = bt_data_load(args.data_path)) == NULL)
        fail("Unable to parse data file.\n");
    bt_trials_t *bt_trials[args.num_iterations];
    bool trials_is_pattern = strchr(args.trials_path, '%');
    if (trials_is_pattern) {
        // Treat the path as a pattern.
        for (size_t i = 0; i < args.num_iterations; i++) {
            char formatted_path[MAX_PATH_LENGTH];
            snprintf(formatted_path, MAX_PATH_LENGTH, args.trials_path, i+1);
            if ((bt_trials[i] = bt_trials_load(formatted_path)) == NULL)
                fail("Unable to parse trials file: %s.\n", formatted_path);
        }
    } else {
        // It's just a path, so we only need to load it once.
        if ((bt_trials[0] = bt_trials_load(args.trials_path)) == NULL)
            fail("Unable to parse trials file.\n");
        for (size_t i = 1; i < args.num_iterations; i++)
            bt_trials[i] = bt_trials[0];
    }
    bt_design_bounds_t *bt_design_bounds;
    if ((bt_design_bounds = bt_bounds_load(args.bounds_path, bt_data)) == NULL)
        fail("Unable to parse bounds file.\n");
    if (args.debug) {
        fprintf(stderr, "Using bounds:\n");
        bt_bounds_write(stderr, bt_design_bounds);
        fprintf(stderr, "\n");
    }

    // Create the output arrays.
    design_var_t best_designs[args.num_iterations][DESIGN_VAR_COUNT];
    fitness_t best_mean_abs_residuals[args.num_iterations];

    // Run the GA.
    for (size_t i = 0; i < args.num_iterations; i++) {
        fprintf(stderr, "Iteration %zd\n", i+1);
        fflush(stderr);
        run_ga(best_designs[i],
               &best_mean_abs_residuals[i],
               args.max_generations,
               args.population_size,
               args.cull_keep,
               args.mutate_probability,
               args.blx_alpha,
               bt_design_bounds,
               bt_data,
               bt_trials[i],
               i + 1,
               args.output_integration,
               args.output_population,
               args.output_convergence,
               args.debug);
    }

    // Write the output file.
    FILE *output_file = fopen(args.output_path, "w");
    bt_model_fprint_designs(output_file, args.num_iterations,
                            best_designs, best_mean_abs_residuals);
    fclose(output_file);

    // Cleanup the input data.
    bt_data_free(bt_data);
    if (trials_is_pattern)
        for (size_t i = 0; i < args.num_iterations; i++)
            bt_trials_free(bt_trials[i]);
    else
        bt_trials_free(bt_trials[0]);
    bt_bounds_free(bt_design_bounds);

    return EXIT_SUCCESS;
}
