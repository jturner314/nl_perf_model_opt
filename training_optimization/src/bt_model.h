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
 * Nonlinear model, objective function, and penalties.
 */

#pragma once

#include "bt_params.h"
#include "bt_population.h"

/**
 * Writes the result of integrating the nonlinear model.
 *
 * Note that the penalty values are written only at the beginnings of
 * days, not at the ends of days, even though the penalized objective
 * function value includes penalties both at the beginnings and ends
 * of days.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] num_days The number of days of training stresses.
 * @param[in] stresses The array of training stresses.
 * @param[in] max_daily_stress The maximum allowable daily stress (for
 *   calculating penalties).
 * @param[in] parameters Parameters and initial conditions for the nonlinear
 *   model.
 */
void bt_model_fprint_integrate(
    FILE *stream,
    const size_t num_days, const stress_t *stresses,
    const stress_t max_daily_stress, const bt_params_t *parameters);

/**
 * Updates the penalized objective function values according to new
 * penalty and roughness factors.
 *
 * Call this function if the `penalty_factor` or `roughness_factor`
 * have changed but the designs have not since the last call to
 * bt_model_update_obj_func(). It avoids re-integrating the nonlinear
 * model.
 *
 * @param[in] penalty_factor Coefficient of penalty function.
 * @param[in] roughness_factor Coefficient of roughness value.
 * @param[in] roughness_days Number of days used for calculating roughness
 *   value.
 * @param[in,out] population Population to update.
 */
void bt_model_update_penalty_factors(const fitness_t penalty_factor, const fitness_t roughness_factor,
                                     const size_t roughness_days, bt_population_t *population);

/**
 * Updates the objective function values, penalties, and penalized
 * objective function values.
 *
 * Call this function if the designs have changed, because it will
 * integrate the nonlinear model using the new designs. If the designs
 * have not changed since the last call to this function but the
 * penalty/roughness factors have, call
 * bt_model_update_penalty_factors() instead to avoid the expense of
 * re-integrating the nonlinear model.
 *
 * @param[in] parameters Parameters and initial conditions for the nonlinear
 *   model.
 * @param[in] roughness_days Number of days used for calculating roughness
 *   value.
 * @param[in] penalty_factor Coefficient of penalty function.
 * @param[in] roughness_factor Coefficient of roughness value.
 * @param[in] max_daily_stress The maximum allowable daily stress (for
 *   calculating penalties).
 * @param[in,out] population Population to update.
 */
void bt_model_update_obj_func(
    const bt_params_t *parameters, const size_t roughness_days,
    const fitness_t penalty_factor, const fitness_t roughness_factor,
    const stress_t max_daily_stress, bt_population_t *population);
