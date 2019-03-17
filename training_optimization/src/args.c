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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "bt_model.h"
#include "args.h"


void usage(const char *program_name)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s [OPTION...] PARAMS_PATH OUTPUT_PATH\n", program_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Positional arguments:\n");
    fprintf(stderr, "  PARAMS_PATH  Path to file with the values of the model parameters.\n");
    fprintf(stderr, "  OUTPUT_PATH  Path to output file for writing optimal designs.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Objective function:\n");
    fprintf(stderr, "  -fCOUNT, --num-days=COUNT               Number of training days.\n");
    fprintf(stderr, "  -yFLOAT, --max-daily-stress=FLOAT       Maximum stress per day.\n");
    fprintf(stderr, "  -rFLOAT, --init-penalty-factor=FLOAT    Initial penalty factor.\n");
    fprintf(stderr, "  -tFLOAT, --penalty-factor-rate=FLOAT    Rate of exponential increase in\n");
    fprintf(stderr, "                                            penalty factor for each generation.\n");
    fprintf(stderr, "  -oFLOAT, --max-roughness-factor=FLOAT   Maximum roughness penalty factor.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Genetic algorithm:\n");
    fprintf(stderr, "  -nCOUNT, --num-iterations=COUNT     Number of iterations of the genetic\n");
    fprintf(stderr, "                                         algorithm.\n");
    fprintf(stderr, "  -gCOUNT, --max-generations=COUNT    Maximum number of generations.\n");
    fprintf(stderr, "  -zCOUNT, --population-size=COUNT    Number of individuals in each generation.\n");
    fprintf(stderr, "  -kCOUNT, --cull-keep=COUNT          Number of individuals from the previous\n");
    fprintf(stderr, "                                        generation to keep when culling.\n");
    fprintf(stderr, "  -aFLOAT, --init-blx-alpha=FLOAT     Initial alpha to use for BLX-alpha\n");
    fprintf(stderr, "                                        crossover.\n");
    fprintf(stderr, "  -sFLOAT, --blx-alpha-change-rate=FLOAT\n");
    fprintf(stderr, "                                      Rate of exponential change in alpha for\n");
    fprintf(stderr, "                                        each generation.\n");
    fprintf(stderr, "  -mFLOAT, --init-mutate-stdev=FLOAT  Initial standard deviation to use for\n");
    fprintf(stderr, "                                        mutation of stress values.\n");
    fprintf(stderr, "  -lFLOAT, --init-mutate-probability=FLOAT\n");
    fprintf(stderr, "                                      Initial probability of mutating any\n");
    fprintf(stderr, "                                        particular stress value.\n");
    fprintf(stderr, "  -wFLOAT, --mutate-change-rate=FLOAT Rate of exponential change in\n");
    fprintf(stderr, "                                        mutation parameters for each generation.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Extra output:\n");
    fprintf(stderr, "  -i[PATTERN], --output-integration[=PATTERN]\n");
    fprintf(stderr, "                                      Output the integration of the best design\n");
    fprintf(stderr, "                                        from each iteration. PATTERN specifies\n");
    fprintf(stderr, "                                        the names of the files, where %%zd is\n");
    fprintf(stderr, "                                        replaced by the iteration number.\n");
    fprintf(stderr, "  -p[PATTERN], --output-population[=PATTERN]\n");
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
    fprintf(stderr, "\n");
    fprintf(stderr, "Help:\n");
    fprintf(stderr, "  -d, --debug                         Show debug output.\n");
    fprintf(stderr, "  -h, --help                          Show this message.\n");
    exit(EXIT_FAILURE);
}


void help(const char *program_name) {
    fprintf(stderr, "This program designs a training routine that maximizes performance at the end of\n");
    fprintf(stderr, "the last training day (equivalently, the beginning of the day after the last\n");
    fprintf(stderr, "training day).");
    fprintf(stderr, "\n");
    usage(program_name);
}


void parse_arguments(const int argc, char * const argv[], arguments_t *args)
{
    // Set defaults
    args->params_path = NULL;
    args->output_path = NULL;
    args->num_days = 84;
    args->max_daily_stress = 300;
    args->init_penalty_factor = 6e-7;
    args->penalty_factor_rate = 1.02;
    args->max_roughness_factor = 0;
    args->num_iterations = 1;
    args->max_generations = 2000;
    args->population_size = 500;
    args->cull_keep = args->population_size / 10;
    args->init_blx_alpha = 0.5;
    args->blx_alpha_change_rate = 0.9999;
    args->init_mutate_stdev = 10;
    args->init_mutate_probability = 0.1;
    args->mutate_change_rate = 0.999;
    args->output_integration = NULL;
    args->output_population = NULL;
    args->output_convergence = NULL;
    args->debug = false;

    // Options
    static const struct option long_options[] = {
        {"num-days", 1, NULL, 'f'},
        {"max-daily-stress", 1, NULL, 'y'},
        {"init-penalty-factor", 1, NULL, 'r'},
        {"penalty-factor-rate", 1, NULL, 't'},
        {"max-roughness-factor", 1, NULL, 'o'},
        {"num-iterations", 1, NULL, 'n'},
        {"max-generations", 1, NULL, 'g'},
        {"population-size", 1, NULL, 'z'},
        {"cull-keep", 1, NULL, 'k'},
        {"init-blx-alpha", 1, NULL, 'a'},
        {"blx-alpha-change-rate", 1, NULL, 's'},
        {"init-mutate-stdev", 1, NULL, 'm'},
        {"init-mutate-probability", 1, NULL, 'l'},
        {"mutate-change-rate", 1, NULL, 'w'},
        {"output-integration", 2, NULL, 'i'},
        {"output-population", 2, NULL, 'p'},
        {"output-convergence", 2, NULL, 'c'},
        {"debug", 0, NULL, 'd'},
        {"help", 0, NULL, 'h'},
        {NULL}
    };

    // Parse options
    int c;
    while ((c = getopt_long(argc, argv, "f:y:r:t:o:n:g:z:k:a:m:l:w:i::p::c::dh", long_options, NULL)) != -1) {
        switch (c) {
        case 'f':
            if (sscanf(optarg, "%zd", &args->num_days) != 1)
                usage(argv[0]);
            break;
        case 'y':
            if (sscanf(optarg, "%lf", &args->max_daily_stress) != 1)
                usage(argv[0]);
            break;
        case 'r':
            if (sscanf(optarg, "%lf", &args->init_penalty_factor) != 1)
                usage(argv[0]);
            break;
        case 't':
            if (sscanf(optarg, "%lf", &args->penalty_factor_rate) != 1)
                usage(argv[0]);
            break;
        case 'o':
            if (sscanf(optarg, "%lf", &args->max_roughness_factor) != 1)
                usage(argv[0]);
            break;
        case 'n':
            if (sscanf(optarg, "%zd", &args->num_iterations) != 1)
                usage(argv[0]);
            break;
        case 'g':
            if (sscanf(optarg, "%zd", &args->max_generations) != 1)
                usage(argv[0]);
            break;
        case 'z':
            if (sscanf(optarg, "%zd", &args->population_size) != 1)
                usage(argv[0]);
            break;
        case 'k':
            if (sscanf(optarg, "%zd", &args->cull_keep) != 1)
                usage(argv[0]);
            break;
        case 'a':
            if (sscanf(optarg, "%lf", &args->init_blx_alpha) != 1)
                usage(argv[0]);
            break;
        case 's':
            if (sscanf(optarg, "%lf", &args->blx_alpha_change_rate) != 1)
                usage(argv[0]);
            break;
        case 'm':
            if (sscanf(optarg, "%lf", &args->init_mutate_stdev) != 1)
                usage(argv[0]);
            break;
        case 'l':
            if (sscanf(optarg, "%lf", &args->init_mutate_probability) != 1)
                usage(argv[0]);
            break;
        case 'w':
            if (sscanf(optarg, "%lf", &args->mutate_change_rate) != 1)
                usage(argv[0]);
            break;
        case 'i':
            if (optarg)
                args->output_integration = optarg;
            else
                args->output_integration = "integration%04zd.tsv";
            break;
        case 'p':
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
            fprintf(stderr, "Error: getopt returned character code 0%o\n", c);
            exit(EXIT_FAILURE);
        }
    }

    // Parse positional args
    static const int num_required_positional_args = 2;
    if (argc - optind == num_required_positional_args) {
        args->params_path = argv[optind++];
        args->output_path = argv[optind++];
    } else if (argc - optind < num_required_positional_args) {
        fprintf(stderr, "%s: missing required positional arguments\n", argv[0]);
        usage(argv[0]);
    } else {
        fprintf(stderr, "%s: too many positional arguments\n", argv[0]);
        usage(argv[0]);
    }
}


void fprintf_arguments(FILE *stream, const arguments_t *args)
{
    fprintf(stream, "PARAMS_PATH = %s\n", args->params_path);
    fprintf(stream, "OUTPUT_PATH = %s\n", args->output_path);
    fprintf(stream, "num-days = %zd\n", args->num_days);
    fprintf(stream, "max-daily-stress = %lf\n", args->max_daily_stress);
    fprintf(stream, "init-penalty-factor = %lf\n", args->init_penalty_factor);
    fprintf(stream, "penalty-factor-rate = %lf\n", args->penalty_factor_rate);
    fprintf(stream, "max-roughness-factor = %lf\n", args->max_roughness_factor);
    fprintf(stream, "num-iterations = %zd\n", args->num_iterations);
    fprintf(stream, "max-generations = %zd\n", args->max_generations);
    fprintf(stream, "population-size = %zd\n", args->population_size);
    fprintf(stream, "cull-keep = %zd\n", args->cull_keep);
    fprintf(stream, "init-blx-alpha = %lf\n", args->init_blx_alpha);
    fprintf(stream, "blx-alpha-change-rate = %lf\n", args->blx_alpha_change_rate);
    fprintf(stream, "init-mutate-stdev = %lf\n", args->init_mutate_stdev);
    fprintf(stream, "init-mutate-probability = %lf\n", args->init_mutate_probability);
    fprintf(stream, "mutate-change-rate = %lf\n", args->mutate_change_rate);
    fprintf(stream, "output-integration = %s\n", args->output_integration);
    fprintf(stream, "output-population = %s\n", args->output_population);
    fprintf(stream, "output-convergence = %s\n", args->output_convergence);
    fprintf(stream, "debug = %d\n", args->debug);
}
