/*
 * Copyright 2019 Duke University
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
 *
 *
 * Random kit 1.3 with modifications
 *
 * The original version of this file was obtained from
 * https://github.com/numpy/numpy/blob/master/numpy/random/mtrand/randomkit.h
 *
 * The NumPy-specific types and macros have been replaced with
 * standard types from C99.
 *
 * The NumPy license is as follows:
 *
 *     Copyright (c) 2005-2019, NumPy Developers.
 *     All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions are
 *     met:
 *
 *         * Redistributions of source code must retain the above copyright
 *            notice, this list of conditions and the following disclaimer.
 *
 *         * Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in
 *            the documentation and/or other materials provided with the
 *            distribution.
 *
 *         * Neither the name of the NumPy Developers nor the names of any
 *            contributors may be used to endorse or promote products derived
 *            from this software without specific prior written permission.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *     IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *     TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *     PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 *     OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *     EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *     PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *     PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *     LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *     NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The original version of this file carried the following copyright notice in
 * its header:
 *
 *     Copyright (c) 2003-2005, Jean-Sebastien Roy (js@jeannot.org)
 *
 *     Permission is hereby granted, free of charge, to any person obtaining a
 *     copy of this software and associated documentation files (the
 *     "Software"), to deal in the Software without restriction, including
 *     without limitation the rights to use, copy, modify, merge, publish,
 *     distribute, sublicense, and/or sell copies of the Software, and to
 *     permit persons to whom the Software is furnished to do so, subject to
 *     the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included
 *     in all copies or substantial portions of the Software.
 *
 *     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *     OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *     SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* @(#) $Jeannot: randomkit.h,v 1.24 2005/07/21 22:14:09 js Exp $ */

/*
 * Typical use:
 *
 * {
 *  rk_state state;
 *  unsigned long seed = 1, random_value;
 *
 *  rk_seed(seed, &state); // Initialize the RNG
 *  ...
 *  random_value = rk_random(&state); // Generate random values in [0..RK_MAX]
 * }
 *
 * Instead of rk_seed, you can use rk_randomseed which will get a random seed
 * from /dev/urandom (or the clock, if /dev/urandom is unavailable):
 *
 * {
 *  rk_state state;
 *  unsigned long random_value;
 *
 *  rk_randomseed(&state); // Initialize the RNG with a random seed
 *  ...
 *  random_value = rk_random(&state); // Generate random values in [0..RK_MAX]
 * }
 */

/*
 * Useful macro:
 *  RK_DEV_RANDOM: the device used for random seeding.
 *                 defaults to "/dev/urandom"
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define RK_STATE_LEN 624

typedef struct rk_state_
{
    unsigned long key[RK_STATE_LEN];
    int pos;
    int has_gauss; /* !=0: gauss contains a gaussian deviate */
    double gauss;

    /* The rk_state structure has been extended to store the following
     * information for the binomial generator. If the input values of n or p
     * are different than nsave and psave, then the other parameters will be
     * recomputed. RTK 2005-09-02 */

    int has_binomial; /* !=0: following parameters initialized for
                              binomial */
    double psave;
    long nsave;
    double r;
    double q;
    double fm;
    long m;
    double p1;
    double xm;
    double xl;
    double xr;
    double c;
    double laml;
    double lamr;
    double p2;
    double p3;
    double p4;

}
rk_state;

typedef enum {
    RK_NOERR = 0, /* no error */
    RK_ENODEV = 1, /* no RK_DEV_RANDOM device */
    RK_ERR_MAX = 2
} rk_error;

/* error strings */
extern char *rk_strerror[RK_ERR_MAX];

/* Maximum generated random value */
#define RK_MAX 0xFFFFFFFFUL

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialize the RNG state using the given seed.
 */
extern void rk_seed(unsigned long seed, rk_state *state);

/*
 * Initialize the RNG state using a random seed.
 * Uses /dev/random or, when unavailable, the clock (see randomkit.c).
 * Returns RK_NOERR when no errors occurs.
 * Returns RK_ENODEV when the use of RK_DEV_RANDOM failed (for example because
 * there is no such device). In this case, the RNG was initialized using the
 * clock.
 */
extern rk_error rk_randomseed(rk_state *state);

/*
 * Returns a random unsigned long between 0 and RK_MAX inclusive
 */
extern unsigned long rk_random(rk_state *state);

/*
 * Returns a random long between 0 and LONG_MAX inclusive
 */
extern long rk_long(rk_state *state);

/*
 * Returns a random unsigned long between 0 and ULONG_MAX inclusive
 */
extern unsigned long rk_ulong(rk_state *state);

/*
 * Returns a random unsigned long between 0 and max inclusive.
 */
extern unsigned long rk_interval(unsigned long max, rk_state *state);

/*
 * Fills an array with cnt random uint64_t between off and off + rng
 * inclusive. The numbers wrap if rng is sufficiently large.
 */
extern void rk_random_uint64(uint64_t off, uint64_t rng, intptr_t cnt,
                             uint64_t *out, rk_state *state);

/*
 * Fills an array with cnt random uint32_t between off and off + rng
 * inclusive. The numbers wrap if rng is sufficiently large.
 */
extern void rk_random_uint32(uint32_t off, uint32_t rng, intptr_t cnt,
                             uint32_t *out, rk_state *state);

/*
 * Fills an array with cnt random uint16_t between off and off + rng
 * inclusive. The numbers wrap if rng is sufficiently large.
 */
extern void rk_random_uint16(uint16_t off, uint16_t rng, intptr_t cnt,
                             uint16_t *out, rk_state *state);

/*
 * Fills an array with cnt random uint8_t between off and off + rng
 * inclusive. The numbers wrap if rng is sufficiently large.
 */
extern void rk_random_uint8(uint8_t off, uint8_t rng, intptr_t cnt,
                            uint8_t *out, rk_state *state);

/*
 * Fills an array with cnt random bool between off and off + rng
 * inclusive. It is assumed tha bool as the same size as uint8_t.
 */
extern void rk_random_bool(bool off, bool rng, intptr_t cnt,
                           bool *out, rk_state *state);

/*
 * Returns a random double between 0.0 and 1.0, 1.0 excluded.
 */
extern double rk_double(rk_state *state);

/*
 * fill the buffer with size random bytes
 */
extern void rk_fill(void *buffer, size_t size, rk_state *state);

/*
 * fill the buffer with randombytes from the random device
 * Returns RK_ENODEV if the device is unavailable, or RK_NOERR if it is
 * On Unix, if strong is defined, RK_DEV_RANDOM is used. If not, RK_DEV_URANDOM
 * is used instead. This parameter has no effect on Windows.
 * Warning: on most unixes RK_DEV_RANDOM will wait for enough entropy to answer
 * which can take a very long time on quiet systems.
 */
extern rk_error rk_devfill(void *buffer, size_t size, int strong);

/*
 * fill the buffer using rk_devfill if the random device is available and using
 * rk_fill if it is not
 * parameters have the same meaning as rk_fill and rk_devfill
 * Returns RK_ENODEV if the device is unavailable, or RK_NOERR if it is
 */
extern rk_error rk_altfill(void *buffer, size_t size, int strong,
                            rk_state *state);

/*
 * return a random gaussian deviate with variance unity and zero mean.
 */
extern double rk_gauss(rk_state *state);

#ifdef __cplusplus
}
#endif
