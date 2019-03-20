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
 * @file bt_constraints.h
 *
 * Interface for constraint functions.
 *
 * A constraint should implement the functions in this header, and
 * then to use that constraint, it should be linked into the binary.
 * This might seem like a weird approach, but it keeps the
 * implementation simple.
 */

#pragma once

#include "bt_population.h"
#include <stdio.h>

/**
 * Updates the penalty value.
 *
 * @param[in] penalty The starting penalty value.
 * @param[in] performance The performance prediction from the nonlinear model.
 * @param[in] fitness The fitness prediction from the nonlinear model.
 * @param[in] fatigue The fatigue prediction from the nonlinear model.
 * @param[in] training_stress The training stress for the whole day.
 * @param[in] max_daily_stress The maximum daily training stress.
 * @returns The new penalty value.
 */
penalty_t bt_constraints_penalty_step(
    const penalty_t penalty, const performance_t performance,
    const performance_t fitness, const performance_t fatigue,
    const stress_t training_stress, const stress_t max_daily_stress);

/**
 * Writes the portion of a TSV file header corresponding to this constraint function.
 *
 * @param[in,out] stream The stream to write to.
 */
void bt_constraints_print_header(FILE *stream);

/**
 * Writes the portion of a TSV file body corresponding to this constraint function.
 *
 * @param[in,out] stream The stream to write to.
 * @param[in] performance The performance prediction from the nonlinear model.
 * @param[in] fitness The fitness prediction from the nonlinear model.
 * @param[in] fatigue The fatigue prediction from the nonlinear model.
 * @param[in] max_daily_stress The maximum daily training stress.
 */
void bt_constraints_print_value(
    FILE *stream, const performance_t performance, const performance_t fitness,
    const performance_t fatigue, const stress_t max_daily_stress);
