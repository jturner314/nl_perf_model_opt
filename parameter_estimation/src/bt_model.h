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
 * @file bt_model.h
 *
 * Nonlinear model and objective function.
 */

#pragma once

#include "bt_data.h"
#include "bt_trials.h"
#include "ga.h"

/**
 * Count of design variables.
 */
#define DESIGN_VAR_COUNT 9

/**
 * Maximum length of any design variable name, excluding the terminating null
 * byte.
 */
#define MAX_DESIGN_VAR_NAME_LENGTH 5

/**
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

/**
 * Array of the names of the design variables.
 */
extern const char *bt_design_var_names[DESIGN_VAR_COUNT];

/**
 * Finds the index corresponding to the design variable with the given @p name
 * (ignoring case).
 *
 * @param[in] name The name of the design variable to find.
 * @returns The index corresponding to the design variable, or -1 if the name
 *   is invalid.
 */
int bt_model_design_var_name_to_index(const char *name);

/**
 * Writes the designs in tab-separated-value format to the stream.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] nmemb The number of designs.
 * @param[in] designs The array of designs.
 * @param[in] mean_abs_residuals (Optional) An array of the mean absolute
 *   residuals. If this is not `NULL`, the residuals are written as the last
 *   column in the file.
 */
void bt_model_fprint_designs(FILE *stream, const size_t nmemb,
                             const design_var_t designs[][DESIGN_VAR_COUNT],
                             const fitness_t mean_abs_residuals[]);

/**
 * Integrates the nonlinear model, writing the performance values to @p data.
 *
 * @param[in] design Initial conditions and parameters.
 * @param[in,out] data Time and training stress inputs and performance output.
 */
void bt_model_integrate(const design_var_t design[DESIGN_VAR_COUNT],
                        bt_data_t *data);

/**
 * Calculates the total absolute residual between the data and the model at the
 * specified trial indices.
 *
 * @param[in] design Initial conditions and parameters for the model.
 * @param[in] data Training data.
 * @param[in] trials Indices in the training data to compute the residual
 *   between the model and the data.
 * @returns The total absolute residual.
 */
fitness_t bt_model_calculate_error(const design_var_t design[DESIGN_VAR_COUNT],
                                   const bt_data_t *data, const bt_trials_t *trials);

/**
 * Updates the objective function values and mean absolute residuals
 * corresponding to the designs.
 *
 * @param[in] nmemb The number of designs.
 * @param[in] designs The array of designs.
 * @param[out] fitnesses (Optional) The array to write the objective function
 *   values. If this is `NULL`, it is ignored.
 * @param[out] mean_abs_residuals (Optional) The array to write the mean
 *   absolute residuals. If this is `NULL`, it is ignored.
 * @param[in] data Training data.
 * @param[in] trials Indices in the training data to compute the residual
 *   between the model and the data.
 */
void bt_model_update_fitnesses(const size_t nmemb,
                               const design_var_t designs[][DESIGN_VAR_COUNT],
                               fitness_t fitnesses[],
                               fitness_t mean_abs_residuals[],
                               const bt_data_t *data,
                               const bt_trials_t *trials);
