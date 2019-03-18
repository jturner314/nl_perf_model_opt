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
 * @file bt_data.h
 *
 * Training data and related functions.
 */


#pragma once

/**
 * Represents the training data.
 *
 * The times should be in increasing order.
 */
typedef struct bt_data_t {
    /**
     * Number of rows in the training data (i.e the length of each array).
     */
    size_t size;
    /**
     * Array of the time instants.
     */
    double *time;
    /**
     * Array of the performance values.
     */
    double *performance;
    /**
     * Array of the training stresses.
     */
    double *training_stress;
} bt_data_t;

/**
 * Reads the training data file at the specified path.
 *
 * The resulting pointer must be freed with bt_data_free().
 *
 * @param[in] path Path where the file is located.
 * @returns A pointer to the training data, or `NULL` on failure.
 */
bt_data_t *bt_data_load(const char *path);

/**
 * Copies the given trainig data.
 *
 * The resulting pointer must be freed with bt_data_free().
 *
 * @param[in] data The data to copy.
 * @returns A pointer to the copy.
 */
bt_data_t *bt_data_copy(const bt_data_t *data);

/**
 * Writes the training data to the given stream.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] data The data to write.
 */
void bt_data_write(FILE *stream, const bt_data_t *data);

/**
 * Frees a pointer allocated by bt_data_load() or bt_data_copy().
 *
 * @param[in] data The data to free.
 */
void bt_data_free(bt_data_t *data);
