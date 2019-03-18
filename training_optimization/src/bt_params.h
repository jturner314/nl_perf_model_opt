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
 * @file bt_params.h
 *
 * Parameters and initial conditions for the nonlinear model and related
 * functions.
 */

#pragma once

/**
 * Type of a parameter or initial condition.
 *
 * This is primarily useful from a documentation perspective for clarifying the
 * desired inputs/outputs of functions.
 */
typedef double param_t;

/**
 * The set of parameters and initial conditions for the nonlinear model.
 */
typedef struct bt_params_t {
    param_t tau1;
    param_t tau2;
    param_t alpha;
    param_t beta;
    param_t k1;
    param_t k2;
    param_t p0;
    param_t f0;
    param_t u0;
} bt_params_t;

/**
 * Loads parameters from the file located at the given path.
 *
 * The returned pointer must be freed with bt_params_free().
 *
 * @param[in] path Path where the input file is located.
 * @returns A pointer to the parameters, or `NULL` on failure.
 */
bt_params_t *bt_params_load(const char *path);

/**
 * Frees a set of parameters allocated by bt_params_load().
 *
 * @param[in] parameters Set of parameters to free.
 */
void bt_params_free(bt_params_t *parameters);
