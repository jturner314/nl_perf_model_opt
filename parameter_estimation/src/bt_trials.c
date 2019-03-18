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
#include "bt_trials.h"


static int bt_trials_parse_line(const char *line, bt_trials_t *trials) {
    return sscanf(line, "%zd", trials->trial_indices + trials->size) == 1 ? 0 : 1;
}


bt_trials_t *bt_trials_load(const char *path)
{
    // Allocate memory
    size_t capacity = 1000;
    bt_trials_t *trials = calloc(1, sizeof(bt_trials_t));
    trials->trial_indices = malloc(capacity * sizeof(size_t));

    // Open file
    FILE *file;
    if ((file = fopen(path, "r")) == NULL)
        return NULL;
    char *line = NULL;
    size_t length = 0;

    // Skip header line
    getline(&line, &length, file);

    // Read trials
    while (getline(&line, &length, file) != -1) {
        // Enlarge arrays if necessary
        if (trials->size >= capacity) {
            capacity <<= 1;
            trials->trial_indices = realloc(trials->trial_indices, capacity * sizeof(size_t));
        }

        // Parse line
        if (bt_trials_parse_line(line, trials) != 0) {
            fprintf(stderr, "Unable to parse line '%s'.\n", line);
            return NULL;
        }
        trials->size++;
    }

    // Free resources.
    free(line);
    fclose(file);

    // Resize arrays to the number of values they contain
    trials->trial_indices = realloc(trials->trial_indices, trials->size * sizeof(size_t));

    return trials;
}


void bt_trials_free(bt_trials_t *trials)
{
    if (trials == NULL)
        return;

    free(trials->trial_indices);
    free(trials);
}
