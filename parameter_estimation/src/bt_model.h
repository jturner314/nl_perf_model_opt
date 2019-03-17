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

#pragma once

#include "bt_data.h"
#include "bt_trials.h"
#include "ga.h"

#define DESIGN_VAR_COUNT 9

/*
 * Enum mapping design variable names to indices.
 */
enum bt_design_var_index {
    VAR_TAU1 = 0,
    VAR_TAU2 = 1,
    VAR_ALPHA = 2,
    VAR_BETA = 3,
    VAR_K1 = 4,
    VAR_K2 = 5,
    VAR_P0 = 6,
    VAR_F0 = 7,
    VAR_U0 = 8
};

/*
 * Array of the names of the design variables.
 */
extern const char *bt_design_var_names[DESIGN_VAR_COUNT];

/*
 * Returns the index corresponding to the design variable with the specified
 * name (ignoring case).
 *
 * Returns -1 if the name is invalid.
 */
int bt_model_design_var_name_to_index(const char *name);

/*
 * Writes the designs in tab-separated-value format to the stream.
 *
 * If `mean_abs_residuals` is not a null pointer, writes that data as the last
 * column.
 */
void bt_model_fprint_designs(FILE *stream, const size_t nmemb,
                             const design_var_t designs[][DESIGN_VAR_COUNT],
                             const fitness_t mean_abs_residuals[]);

/*
 * Integrates the model, using the `design` (initial conditions and parameters)
 * in `design`, and writes the results to `data`.
 */
void bt_model_integrate(const design_var_t design[DESIGN_VAR_COUNT],
                        bt_data_t *data);

/*
 * Calculates the total absolute error between the data and the model at the
 * specified trial indices, using the specified `design` (initial conditions
 * and parameters).
 */
fitness_t bt_model_calculate_error(const design_var_t design[DESIGN_VAR_COUNT],
                                   const bt_data_t *data, const bt_trials_t *trials);

/*
 * Updates the fitnesses and mean absolute residuals corresponding to the designs.
 *
 * If `fitnesses` is a null pointer, it is ignored. If `mean_abs_residuals` is
 * a null pointer, it is ignored.
 */
void bt_model_update_fitnesses(const size_t nmemb,
                               const design_var_t designs[][DESIGN_VAR_COUNT],
                               fitness_t fitnesses[],
                               fitness_t mean_abs_residuals[],
                               const bt_data_t *data,
                               const bt_trials_t *trials);
