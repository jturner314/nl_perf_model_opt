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
 * Represents the data from a trial indices file.
 *
 * This specifies the indices of the performance values within the training
 * data file that should be fit during optimization.
 */
typedef struct bt_trials_t {
    size_t size;
    size_t *trial_indices;
} bt_trials_t;

/*
 * Reads the trial indices from the specified path.
 *
 * The resulting pointer must be freed with `bt_trials_free`.
 */
bt_trials_t *bt_trials_load(const char *path);

/*
 * Frees a pointer allocated by `bt_trials_load`.
 */
void bt_trials_free(bt_trials_t *data);
