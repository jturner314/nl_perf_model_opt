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
 * @file fitness_max_stress_fatigue_max_stress.c
 *
 * Combination of training progression constraint and person-specific fatigue
 * constraint.
 *
 * Penalizes training stresses greater than
 * @f$\sigma_\mathrm{max}\left(1-0.9\mathrm{e}^{-f/150}\right)@f$ and training
 * stresses greater than
 * @f$\sigma_\mathrm{max}\left(0.1+0.9\mathrm{e}^{-u/800}\right)@f$ where
 * @f$\sigma_\mathrm{max}@f$ is `max_daily_stress`, @f$f@f$ is the predicted
 * fitness, and @f$u@f$ is the predicted fatigue.
 */

#include <math.h>
#include "../bt_constraints.h"

static inline stress_t bt_constraints_calc_max_stress_fitness(
    const stress_t max_daily_stress, const performance_t fitness)
{
    return max_daily_stress * (1 - 0.9 * exp(-fitness / 150));
}

static inline stress_t bt_constraints_calc_max_stress_fatigue(
    const stress_t max_daily_stress, const performance_t fatigue)
{
    return max_daily_stress * (0.1 + 0.9 * exp(-fatigue / 800));
}

penalty_t bt_constraints_penalty_step(
    const penalty_t penalty, const performance_t performance,
    const performance_t fitness, const performance_t fatigue,
    const stress_t training_stress, const stress_t max_daily_stress)
{
    penalty_t new_penalty = penalty;

    stress_t max_stress_fitness = bt_constraints_calc_max_stress_fitness(max_daily_stress, fitness);
    new_penalty += fmax(training_stress, max_stress_fitness) - max_stress_fitness;

    stress_t max_stress_fatigue = bt_constraints_calc_max_stress_fatigue(max_daily_stress, fatigue);
    new_penalty += fmax(training_stress, max_stress_fatigue) - max_stress_fatigue;

    return new_penalty;
}

void bt_constraints_print_header(FILE *stream)
{
    fprintf(stream, "\tfitness_max_stress\tfatigue_max_stress");
}

void bt_constraints_print_value(
    FILE *stream, const performance_t performance, const performance_t fitness,
    const performance_t fatigue, const stress_t max_daily_stress)
{
    const stress_t max_stress_fitness = bt_constraints_calc_max_stress_fitness(max_daily_stress, fitness);
    const stress_t max_stress_fatigue = bt_constraints_calc_max_stress_fatigue(max_daily_stress, fatigue);

    fprintf(stream, "\t%lf\t%lf", max_stress_fitness, max_stress_fatigue);
}
