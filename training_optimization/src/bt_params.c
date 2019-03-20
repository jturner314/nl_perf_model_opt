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


/**
 * Maximum length of any design variable name, excluding the terminating null
 * byte.
 */
#define MAX_DESIGN_VAR_NAME_LENGTH 5


#define str(x) #x
#define expand(x) str(x)


static int bt_params_parse_line(const char *line, bt_params_t *parameters)
{
    char *name = calloc(MAX_DESIGN_VAR_NAME_LENGTH + 1, 1);;
    param_t value = 0;

    if (sscanf(line, "%"expand(MAX_DESIGN_VAR_NAME_LENGTH)"s %lf",
               name, &value) != 2) {
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
    // Open file
    FILE *file;
    if ((file = fopen(path, "r")) == NULL)
        return NULL;
    char *line = NULL;
    size_t length = 0;

    // Skip header line
    if (getline(&line, &length, file) == -1) {
        free(line);
        fclose(file);
        return NULL;
    }

    // Allocate memory
    bt_params_t *parameters = calloc(1, sizeof(bt_params_t));

    // Read data
    while (getline(&line, &length, file) != -1) {
        if (bt_params_parse_line(line, parameters) != 0) {
            fprintf(stderr, "Error: Unable to parse line '%s'.\n", line);
            free(line);
            fclose(file);
            bt_params_free(parameters);
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
