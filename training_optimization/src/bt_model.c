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

#include "bt_model.h"
#include "bt_constraints.h"
#include <math.h>


#define DAY_LENGTH 1


static inline performance_t bt_model_calc_fitness_d(const performance_t fitness,
                                                    const stress_t training_stress,
                                                    const bt_params_t *parameters)
{
    return -1/parameters->tau1 * pow(fitness, parameters->alpha) + parameters->k1 * training_stress;
}


static inline performance_t bt_model_calc_fatigue_d(const performance_t fatigue,
                                                    const stress_t training_stress,
                                                    const bt_params_t *parameters)
{
    return -1/parameters->tau2 * pow(fatigue, parameters->beta) + parameters->k2 * training_stress;
}


static inline performance_t bt_model_euler_step(
    performance_t (*y_d)(const performance_t y, const stress_t training_stress, const bt_params_t *parameters),
    const performance_t y, const double dt, const stress_t training_stress, const bt_params_t *parameters)
{
    return y + dt * y_d(y, training_stress, parameters);
}


static void bt_model_integrate_interval(
    performance_t *performance, performance_t *fitness, performance_t *fatigue, penalty_t *penalty,
    const stress_t training_stress, const param_t interval_duration, const stress_t max_daily_stress,
    const bt_params_t *parameters)
{
    *penalty = bt_constraints_penalty_step(*penalty, *performance, *fitness, *fatigue, training_stress, max_daily_stress);
    *fitness = bt_model_euler_step(bt_model_calc_fitness_d, *fitness, interval_duration, training_stress, parameters);
    *fatigue = bt_model_euler_step(bt_model_calc_fatigue_d, *fatigue, interval_duration, training_stress, parameters);
    *performance = parameters->p0 + *fitness - *fatigue;
    *penalty = bt_constraints_penalty_step(*penalty, *performance, *fitness, *fatigue, training_stress, max_daily_stress);
}


void bt_model_fprint_integrate(
    FILE *stream,
    const size_t num_days, const stress_t *stresses,
    const stress_t max_daily_stress, const bt_params_t *parameters)
{
    fprintf(stream, "day\tstress\tfitness\tfatigue\tperformance");
    bt_constraints_print_header(stream);
    fprintf(stream, "\n");
    performance_t fitness = parameters->f0;
    performance_t fatigue = parameters->u0;
    performance_t performance = parameters->p0 + parameters->f0 - parameters->u0;
    penalty_t penalty = 0;
    size_t day;
    for (day = 0; day < num_days; day++) {
        fprintf(stream, "%zd\t%lf\t%lf\t%lf\t%lf", day, stresses[day], fitness, fatigue, performance);
        bt_constraints_print_value(stream, performance, fitness, fatigue, max_daily_stress);
        fprintf(stream, "\n");
        bt_model_integrate_interval(
            &performance, &fitness, &fatigue, &penalty, stresses[day],
            DAY_LENGTH, max_daily_stress, parameters);
    }
    fprintf(stream, "%zd\t%lf\t%lf\t%lf\t%lf", day, 0., fitness, fatigue, performance);
    bt_constraints_print_value(stream, performance, fitness, fatigue, max_daily_stress);
    fprintf(stream, "\n");
}


/* this calculates the performance at the start of the last day */
static void bt_model_calculate_final_performance_and_penalty(
    const size_t num_days, const stress_t *stresses, const stress_t max_daily_stress,
    const bt_params_t *parameters,
    performance_t *final_performance, penalty_t *penalty)
{
    // Perform integration
    performance_t fitness = parameters->f0;
    performance_t fatigue = parameters->u0;
    performance_t performance = parameters->p0 + parameters->f0 - parameters->u0;
    *penalty = 0;
    for (size_t day = 0; day < num_days; day++) {
        bt_model_integrate_interval(
            &performance, &fitness, &fatigue, penalty, stresses[day],
            DAY_LENGTH, max_daily_stress, parameters);
    }
    *final_performance = performance;
}


static penalty_t bt_model_calculate_roughness(
    const size_t num_days, const stress_t *stresses, const size_t roughness_days)
{
    penalty_t roughness = 0;
    for (size_t day = 0; day < num_days; day++) {
        if (day >= roughness_days) {
            for (size_t old_day = day - roughness_days; old_day < day; old_day++)
                roughness += fabs(stresses[old_day] - stresses[old_day+1]);
            roughness -= fabs(stresses[day-roughness_days] - stresses[day]);
        }
    }
    return roughness;
}


static inline fitness_t bt_model_calculate_objective_function(
    const performance_t final_performance, const penalty_t penalty, const fitness_t penalty_factor,
    const penalty_t roughness, const fitness_t roughness_factor)
{
    return final_performance - penalty_factor * penalty - roughness_factor * roughness;
}


void bt_model_update_penalty_factors(const fitness_t penalty_factor, const fitness_t roughness_factor,
                                     const size_t roughness_days, bt_population_t *population)
{
    #pragma omp parallel for
    for (size_t i = 0; i < population->nmemb; i++) {
        if (roughness_factor > 0) {
            population->roughnesses[i] = bt_model_calculate_roughness(
                population->num_days, population->stresses[i], roughness_days);
        }
        population->fitnesses[i] = bt_model_calculate_objective_function(
            population->final_performances[i], population->penalties[i], penalty_factor,
            population->roughnesses[i], roughness_factor);
    }
}


void bt_model_update_obj_func(
    const bt_params_t *parameters, const size_t roughness_days,
    const fitness_t penalty_factor, const fitness_t roughness_factor,
    const stress_t max_daily_stress, bt_population_t *population)
{
    #pragma omp parallel for
    for (size_t i = 0; i < population->nmemb; i++) {

        // Calculate roughness.
        penalty_t roughness  = 0;
        if (roughness_factor > 0) {
            roughness = bt_model_calculate_roughness(
                population->num_days, population->stresses[i], roughness_days);
        }

        // Calculate final performance and penalty.
        performance_t final_performance;
        penalty_t penalty;
        bt_model_calculate_final_performance_and_penalty(
            population->num_days, population->stresses[i], max_daily_stress,
            parameters, &final_performance, &penalty);

        // Calculate overall fitness.
        fitness_t fitness = bt_model_calculate_objective_function(
            final_performance, penalty, penalty_factor, roughness, roughness_factor);

        // Handle any numerical problems.
        if (!isnan(fitness)) {
            population->final_performances[i] = final_performance;
            population->penalties[i] = penalty;
            population->roughnesses[i] = roughness;
            population->fitnesses[i] = fitness;
        } else {
            population->final_performances[i] = -INFINITY;
            population->penalties[i] = INFINITY;
            population->roughnesses[i] = INFINITY;
            population->fitnesses[i] = -INFINITY;
        }
    }
}
