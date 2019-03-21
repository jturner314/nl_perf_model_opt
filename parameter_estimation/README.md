<!-- Copyright 2015-2019 Duke University

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License Version 2 as published by the Free
Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License Version 2
along with this program. If not, see
<https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>. -->

# Parameter Estimation Genetic Algorithm for Nonlinear Performance Model

This is an implementation of a genetic algorithm to find initial conditions and
parameters of the nonlinear performance model described in [Turner et al. 2017]
that fit an individual's training data. This corresponds to the "Human
performance data" section of the paper.

## Building

To build the program, you need GNU Make and a C99 compiler. OpenMP is required
if you want multithreading for better performance. Build using

* Linux with glibc and OpenMP:

  ```sh
  make
  ```

* Linux with glibc and without OpenMP:

  ```sh
  make CFLAGS='-Wall -std=c99 -D_GNU_SOURCE -g -O3' LDFLAGS='-lm'
  ```

* macOS with OpenMP (untested):

  ```sh
  make CFLAGS='-Wall -std=c99 -fopenmp -D_GNU_SOURCE -g -O3 -DMAC_OSX'
  ```

* macOS without OpenMP:

  ```sh
  make CFLAGS='-Wall -std=c99 -D_GNU_SOURCE -g -O3 -DMAC_OSX' LDFLAGS='-lm'
  ```

## Usage

After building the program, run

```sh
bin/bt_ga --help
```

to see help for the command line interface. See the `data` directory in this
project for some sample input data files. To run the GA for the example data,
you can run

```sh
make all
```

## Reproducibility

For a specific version of this project, the results should be the same for the
same inputs because the pseudorandom number generator (PRNG) is seeded from
constant values. However, the results may change from version-to-version as a
result of small changes to the PRNG, sorting algorithms (due to sorting
instability), etc. The results will not be numerically identical to those from
[Turner et al. 2017] because of small changes to the PRNG and sorting
algorithms for licensing reasons; however, they should have the same
distribution.

## Documentation

Run

```sh
make doc
```

to generate Doxygen documentation of the implementation.

## License

See the `COPYING` file in this directory.

[Turner et al. 2017]: http://dx.doi.org/10.1515/bhk-2017-0013
