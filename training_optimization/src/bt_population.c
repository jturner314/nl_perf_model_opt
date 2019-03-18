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

#include <stdlib.h>
#include <string.h>
#include "bt_population.h"
#include "stats.h"


bt_population_t *bt_population_alloc(const size_t nmemb, const size_t num_days)
{
    bt_population_t *population = malloc(sizeof(bt_population_t));
    population->nmemb = nmemb;
    population->num_days = num_days;
    population->stresses = malloc(nmemb * sizeof(size_t *));
    population->stresses[0] = malloc(nmemb * num_days * sizeof(stress_t));
    for (size_t i = 1; i < nmemb; i++)
        population->stresses[i] = population->stresses[0] + i * num_days;
    population->final_performances = malloc(nmemb * sizeof(performance_t));
    population->penalties = malloc(nmemb * sizeof(penalty_t));
    population->roughnesses = malloc(nmemb * sizeof(penalty_t));
    population->fitnesses = malloc(nmemb * sizeof(fitness_t));
    return population;
}


void bt_population_write(FILE *stream, const bt_population_t *population)
{
    // Header
    for (size_t i = 0; i < population->num_days; i++) {
        if (i != 0)
            fprintf(stream, "\t");
        fprintf(stream, "day%03zd", i);
    }
    if (population->final_performances != NULL)
        fprintf(stream, "\tfinal_performance");
    if (population->penalties != NULL)
        fprintf(stream, "\tpenalty");
    if (population->roughnesses != NULL)
        fprintf(stream, "\troughness");
    if (population->fitnesses != NULL)
        fprintf(stream, "\tfitness");
    fprintf(stream, "\n");

    // Data
    for (size_t memb_index = 0; memb_index < population->nmemb; memb_index++) {
        for (size_t day = 0; day < population->num_days; day++) {
            if (day > 0)
                fprintf(stream, "\t");
            fprintf(stream, "%lf", population->stresses[memb_index][day]);
        }
        if (population->final_performances != NULL)
            fprintf(stream, "\t%lf", population->final_performances[memb_index]);
        if (population->penalties != NULL)
            fprintf(stream, "\t%lf", population->penalties[memb_index]);
        if (population->roughnesses != NULL)
            fprintf(stream, "\t%lf", population->roughnesses[memb_index]);
        if (population->fitnesses != NULL)
            fprintf(stream, "\t%lf", population->fitnesses[memb_index]);
        fprintf(stream, "\n");
    }
}


void bt_population_free(bt_population_t *population)
{
    if (population == NULL)
        return;

    free(population->stresses[0]);
    free(population->stresses);
    free(population->final_performances);
    free(population->penalties);
    free(population->roughnesses);
    free(population->fitnesses);
    free(population);
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
