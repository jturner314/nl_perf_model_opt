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
#include "bt_model.h"

/*
 * Represents the bounds of the design variables for initial population
 * generation and the standard deviations for Gaussian mutation.
 */
typedef struct {
    design_var_t lower_bounds[DESIGN_VAR_COUNT];
    design_var_t upper_bounds[DESIGN_VAR_COUNT];
    design_var_t stdevs[DESIGN_VAR_COUNT];
} bt_design_bounds_t;

/*
 * Reads the design variables info file at the specified path.
 *
 * The resulting pointer must be freed with `bt_bounds_free`.
 */
bt_design_bounds_t *bt_bounds_load(const char *path, const bt_data_t *bt_data);

/*
 * Writes the design variable info to the given stream.
 */
void bt_bounds_write(FILE *stream, const bt_design_bounds_t *bounds);

/*
 * Frees a pointer allocated by `bt_bounds_load`.
 */
void bt_bounds_free(bt_design_bounds_t *bounds);
