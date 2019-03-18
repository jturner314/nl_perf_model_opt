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
 * @file bt_bounds.h
 *
 * Design variable initial bounds and standard deviations, and related
 * functions.
 */

#pragma once

#include "bt_model.h"

/**
 * The bounds of the design variables for initial population generation and the
 * standard deviations for Gaussian mutation.
 */
typedef struct {
    /**
     * Lower bounds of the design variables for generating initial population.
     */
    design_var_t lower_bounds[DESIGN_VAR_COUNT];
    /**
     * Upper bounds of the design variables for generating initial population.
     */
    design_var_t upper_bounds[DESIGN_VAR_COUNT];
    /**
     * Standard deviations for Gaussian mutation of the design variables.
     */
    design_var_t stdevs[DESIGN_VAR_COUNT];
} bt_design_bounds_t;

/**
 * Reads the design variables info file at the specified path.
 *
 * The resulting pointer must be freed with bt_bounds_free().
 *
 * @param[in] path Path where the file is located.
 * @returns A pointer to the design variables info, or `NULL` on failure.
 */
bt_design_bounds_t *bt_bounds_load(const char *path);

/**
 * Writes the design variable info to the given stream.
 *
 * @param[in,out] stream Stream to write to.
 * @param[in] bounds The design variable info to write.
 */
void bt_bounds_write(FILE *stream, const bt_design_bounds_t *bounds);

/**
 * Frees a pointer allocated by bt_bounds_load().
 *
 * @param[in] bounds Design variable info to free.
 */
void bt_bounds_free(bt_design_bounds_t *bounds);
