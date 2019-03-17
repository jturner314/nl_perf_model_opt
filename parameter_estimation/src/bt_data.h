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

/*
 * Represents the information from a training data file.
 */
typedef struct bt_data_t {
    size_t size;
    double *time;
    double *performance;
    double *training_stress;
} bt_data_t;

/*
 * Reads the training data file at the specified path.
 *
 * The resulting pointer must be freed with `bt_data_free`.
 */
bt_data_t *bt_data_load(const char *path);

/*
 * Copies the given trainig data.
 *
 * The resulting pointer must be freed with `bt_data_free`.
 */
bt_data_t *bt_data_copy(const bt_data_t *data);

/*
 * Writes the training data to the given file.
 */
void bt_data_write(FILE *stream, const bt_data_t *data);

/*
 * Frees a pointer allocated by `bt_data_load` or `bt_data_copy`.
 */
void bt_data_free(bt_data_t *data);
