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
#include <string.h>
#include "bt_params.h"


static int bt_params_parse_line(const char *line, bt_params_t *parameters)
{
    char *name = NULL;
    param_t value = 0;

    if (sscanf(line, "%ms %lf", &name, &value) != 2) {
        free(name);
        return 1;
    }

    if (strcmp("tau1", name) == 0) {
        parameters->tau1 = value;
    } else if (strcmp("tau2", name) == 0) {
        parameters->tau2 = value;
    } else if (strcmp("alpha", name) == 0) {
        parameters->alpha = value;
    } else if (strcmp("beta", name) == 0) {
        parameters->beta = value;
    } else if (strcmp("k1", name) == 0) {
        parameters->k1 = value;
    } else if (strcmp("k2", name) == 0) {
        parameters->k2 = value;
    } else if (strcmp("p0", name) == 0) {
        parameters->p0 = value;
    } else if (strcmp("f0", name) == 0) {
        parameters->f0 = value;
    } else if (strcmp("u0", name) == 0) {
        parameters->u0 = value;
    } else {
        free(name);
        return 1;
    }
    free(name);

    return 0;
}


bt_params_t *bt_params_load(const char *path)
{
    bt_params_t *parameters = calloc(1, sizeof(bt_params_t));

    // Open file
    FILE *file = fopen(path, "r");
    char *line = NULL;
    size_t length = 0;

    // Skip header line
    getline(&line, &length, file);

    // Read data
    while (getline(&line, &length, file) != -1) {
        if (bt_params_parse_line(line, parameters) != 0) {
            fprintf(stderr, "Error: Unable to parse line '%s'.\n", line);
            return NULL;
        }
    }

    // Free resources.
    free(line);
    fclose(file);

    return parameters;
}


void bt_params_free(bt_params_t *parameters)
{
    free(parameters);
}
