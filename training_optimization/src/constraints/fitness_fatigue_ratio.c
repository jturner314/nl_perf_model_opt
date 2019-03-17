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
 * @file fitness_fatigue_ratio.c
 *
 * Person-specific fatigue/fitness constraint.
 *
 * Penalizes fatigue/fitness ratio values greater than 0.8.
 */

#include <math.h>
#include "../bt_constraints.h"

const performance_t MAX_FATIGUE_FITNESS_RATIO = 0.8;

penalty_t bt_constraints_penalty_step(
    const penalty_t penalty, const performance_t performance,
    const performance_t fitness, const performance_t fatigue,
    const stress_t training_stress, const stress_t max_daily_stress)
{
    penalty_t new_penalty = penalty;

    performance_t fatigue_fitness_ratio = fatigue / fitness;
    new_penalty += fmax(fatigue_fitness_ratio, MAX_FATIGUE_FITNESS_RATIO) - MAX_FATIGUE_FITNESS_RATIO;

    return new_penalty;
}

void bt_constraints_print_header(FILE *stream)
{
    fprintf(stream, "\tmax_fatigue_fitness_ratio");
}

void bt_constraints_print_value(
    FILE *stream, const performance_t performance, const performance_t fitness,
    const performance_t fatigue, const stress_t max_daily_stress)
{
    fprintf(stream, "\t%lf", MAX_FATIGUE_FITNESS_RATIO);
}
