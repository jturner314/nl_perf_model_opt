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
#include "bt_data.h"


static int bt_data_parse_line(const char *line, bt_data_t *data) {
    return sscanf(line, "%lf %lf %lf",
                  data->time + data->size,
                  data->performance + data->size,
                  data->training_stress + data->size) == 3 ? 0 : 1;
}


bt_data_t *bt_data_load(const char *path)
{
    // Allocate memory
    size_t capacity = 1000;
    bt_data_t *data = calloc(1, sizeof(bt_data_t));
    data->time = malloc(capacity * sizeof(double));
    data->performance = malloc(capacity * sizeof(double));
    data->training_stress = malloc(capacity * sizeof(double));

    // Open file
    FILE *file = fopen(path, "r");
    char *line = NULL;
    size_t length = 0;

    // Skip header line
    getline(&line, &length, file);

    // Read data
    while (getline(&line, &length, file) != -1) {
        // Enlarge arrays if necessary
        if (data->size >= capacity) {
            capacity <<= 1;
            data->time = realloc(data->time, capacity * sizeof(double));
            data->performance = realloc(data->performance, capacity * sizeof(double));
            data->training_stress = realloc(data->training_stress, capacity * sizeof(double));
        }

        // Parse line
        if (bt_data_parse_line(line, data) != 0) {
            fprintf(stderr, "Unable to parse line '%s'.\n", line);
            return NULL;
        }
        data->size++;
    }

    // Free resources.
    free(line);
    fclose(file);

    // Resize arrays to the number of values they contain
    data->time = realloc(data->time, data->size * sizeof(double));
    data->performance = realloc(data->performance, data->size * sizeof(double));
    data->training_stress = realloc(data->training_stress, data->size * sizeof(double));

    return data;
}


bt_data_t *bt_data_copy(const bt_data_t *data)
{
    bt_data_t *copy = calloc(1, sizeof(bt_data_t));
    copy->size = data->size;
    copy->time = malloc(data->size * sizeof(double));
    memcpy(copy->time, data->time, data->size * sizeof(double));
    copy->performance = malloc(data->size * sizeof(double));
    memcpy(copy->performance, data->performance, data->size * sizeof(double));
    copy->training_stress = malloc(data->size * sizeof(double));
    memcpy(copy->training_stress, data->training_stress, data->size * sizeof(double));
    return copy;
}


void bt_data_write(FILE *stream, const bt_data_t *data)
{
    fprintf(stream, "day\tperformance\ttraining_stress\n");
    for (int j = 0; j < data->size; j++) {
        fprintf(stream, "%lf\t%lf\t%lf\n",
                data->time[j], data->performance[j], data->training_stress[j]);
    }
}


void bt_data_free(bt_data_t *data)
{
    free(data->time);
    free(data->performance);
    free(data->training_stress);
    free(data);
}
