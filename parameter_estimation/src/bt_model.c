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
#include <math.h>
#include <string.h>
#include <strings.h>


const char *bt_design_var_names[DESIGN_VAR_COUNT] = {
    "tau1",
    "tau2",
    "alpha",
    "beta",
    "k1",
    "k2",
    "p0",
    "f0",
    "u0"
};


int bt_model_design_var_name_to_index(const char *name) {
    for (size_t i = 0; i < DESIGN_VAR_COUNT; i++)
        if (strcasecmp(bt_design_var_names[i], name) == 0)
            return i;
    return -1;
}


void bt_model_fprint_designs(FILE *stream, const size_t nmemb,
                             design_var_t (*const designs)[DESIGN_VAR_COUNT],
                             const fitness_t mean_abs_residuals[])
{
    // Header
    for (int i = 0; i < DESIGN_VAR_COUNT; i++) {
        if (i > 0)
            fprintf(stream, "\t");
        fprintf(stream, "%s", bt_design_var_names[i]);
    }
    if (mean_abs_residuals)
        fprintf(stream, "\tmean_abs_residual");
    fprintf(stream, "\n");

    // Data
    for (int i = 0; i < nmemb; i++) {
        for (int j = 0; j < DESIGN_VAR_COUNT; j++) {
            if (j > 0)
                fprintf(stream, "\t");
            fprintf(stream, "%lf", designs[i][j]);
        }
        if (mean_abs_residuals)
            fprintf(stream, "\t%lf", mean_abs_residuals[i]);
        fprintf(stream, "\n");
    }
}


static design_var_t bt_model_calc_fitness_d(const design_var_t fitness, const design_var_t training_stress,
                                     const design_var_t design[DESIGN_VAR_COUNT])
{
    return -1/design[VAR_TAU1] * pow(fitness, design[VAR_ALPHA]) + design[VAR_K1] * training_stress;
}


static design_var_t bt_model_calc_fatigue_d(const design_var_t fatigue, const design_var_t training_stress,
                                     const design_var_t design[DESIGN_VAR_COUNT])
{
    return -1/design[VAR_TAU2] * pow(fatigue, design[VAR_BETA]) + design[VAR_K2] * training_stress;
}


static design_var_t bt_model_euler_step(
    design_var_t (*y_d)(const design_var_t y, const design_var_t training_stress, const design_var_t design[DESIGN_VAR_COUNT]),
    const design_var_t y, const design_var_t dt, const design_var_t training_stress, const design_var_t design[DESIGN_VAR_COUNT])
{
    return y + dt * y_d(y, training_stress, design);
}


void bt_model_performance_integrate_interval(
    design_var_t *performance, design_var_t *fitness, design_var_t *fatigue,
    const design_var_t training_stress, const design_var_t interval_duration,
    const design_var_t design[DESIGN_VAR_COUNT])
{
    // Integrate during the step.
    *fitness = bt_model_euler_step(bt_model_calc_fitness_d, *fitness, interval_duration, training_stress, design);
    *fatigue = bt_model_euler_step(bt_model_calc_fatigue_d, *fatigue, interval_duration, training_stress, design);
    // Update the performance.
    *performance = design[VAR_P0] + *fitness - *fatigue;
}


void bt_model_integrate(const design_var_t design[DESIGN_VAR_COUNT], bt_data_t *data)
{
    design_var_t fitness = design[VAR_F0];
    design_var_t fatigue = design[VAR_U0];
    design_var_t performance = design[VAR_P0] + design[VAR_F0] - design[VAR_U0];
    data->performance[0] = performance;
    for (size_t interval = 0; interval < data->size - 1; interval++) {
        bt_model_performance_integrate_interval(
            &performance, &fitness, &fatigue, data->training_stress[interval],
            data->time[interval+1] - data->time[interval], design);
        data->performance[interval+1] = performance;
    }
}


/* TODO: this unnecessarily caluclates the TSS of the data every time */
fitness_t bt_model_calculate_error(const design_var_t design[DESIGN_VAR_COUNT],
                                   const bt_data_t *data, const bt_trials_t *trials)
{
    // Check parameter constraints first
    if (design[VAR_TAU1] < 0 || design[VAR_TAU2] < 0 || design[VAR_K1] < 0 || design[VAR_K2] < 0 || design[VAR_ALPHA] < 1 || design[VAR_BETA] > 1)
        return NAN;

    // Initialize total error
    design_var_t total_error = 0;

    // Perform integration
    design_var_t fitness = design[VAR_F0];
    design_var_t fatigue = design[VAR_U0];
    design_var_t performance = design[VAR_P0] + design[VAR_F0] - design[VAR_U0];
    size_t prev_trial_index = 0;
    for (size_t trial = 0; trial < trials->size; trial++) {
        size_t trial_index = trials->trial_indices[trial];
        for (size_t interval = prev_trial_index; interval < trial_index; interval++) {
            bt_model_performance_integrate_interval(
                &performance, &fitness, &fatigue, data->training_stress[interval],
                data->time[interval+1] - data->time[interval], design);
        }
        design_var_t residual = data->performance[trial_index] - performance;
        total_error += fabs(residual);
        prev_trial_index = trial_index;
    }

    return total_error;
}


void bt_model_update_fitnesses(const size_t nmemb,
                               design_var_t (*const designs)[DESIGN_VAR_COUNT],
                               fitness_t fitnesses[],
                               fitness_t mean_abs_residuals[],
                               const bt_data_t *data,
                               const bt_trials_t *trials)
{
    #pragma omp parallel for
    for (size_t i = 0; i < nmemb; i++) {
        fitness_t error = bt_model_calculate_error(designs[i], data, trials);
        fitness_t mean_abs_residual = error / trials->size;
        if (!isnan(error)) {
            if (fitnesses)
                fitnesses[i] = -error;
            if (mean_abs_residuals)
                mean_abs_residuals[i] = mean_abs_residual;
        } else {
            if (fitnesses)
                fitnesses[i] = -INFINITY;
            if (mean_abs_residuals)
                mean_abs_residuals[i] = NAN;
        }
    }
}
