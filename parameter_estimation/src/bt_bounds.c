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

#include <stdio.h>
#include <stdlib.h>
#include "bt_bounds.h"

#define str(x) #x
#define expand(x) str(x)


static int bt_bounds_parse_line(const char *line, bt_design_bounds_t *bounds)
{
    char *var_name = calloc(MAX_DESIGN_VAR_NAME_LENGTH + 1, 1);
    design_var_t lower_bound = 0;
    design_var_t upper_bound = 0;
    design_var_t stdev = 0;

    if (sscanf(line, "%"expand(MAX_DESIGN_VAR_NAME_LENGTH)"s %lf %lf %lf",
               var_name, &lower_bound, &upper_bound, &stdev) != 4) {
        free(var_name);
        return 1;
    }

    const size_t index = bt_model_design_var_name_to_index(var_name);
    free(var_name);
    if (index == -1) {
        return 1;
    }

    bounds->lower_bounds[index] = lower_bound;
    bounds->upper_bounds[index] = upper_bound;
    bounds->stdevs[index] = stdev;

    return 0;
}


bt_design_bounds_t *bt_bounds_load(const char *path)
{
    bt_design_bounds_t *bounds = calloc(1, sizeof(bt_design_bounds_t));

    // Open file
    FILE *file = fopen(path, "r");
    char *line = NULL;
    size_t length = 0;

    // Skip header line
    getline(&line, &length, file);

    // Read data
    while (getline(&line, &length, file) != -1) {
        if (bt_bounds_parse_line(line, bounds) != 0) {
            fprintf(stderr, "Error: Unable to parse line '%s'.\n", line);
            return NULL;
        }
    }

    // Free resources.
    free(line);
    fclose(file);

    return bounds;
}

void bt_bounds_write(FILE *stream, const bt_design_bounds_t *bounds)
{
    fprintf(stream, "design_variable\tlower_bound\tupper_bound\tstdev\n");
    for (int i = 0; i < DESIGN_VAR_COUNT; i++) {
        design_var_t lower_bound = bounds->lower_bounds[i];
        design_var_t upper_bound = bounds->upper_bounds[i];
        fprintf(stream, "%s\t%lf\t%lf\t%lf\n", bt_design_var_names[i],
                lower_bound, upper_bound, bounds->stdevs[i]);
    }
}

void bt_bounds_free(bt_design_bounds_t *bounds)
{
    free(bounds);
}
