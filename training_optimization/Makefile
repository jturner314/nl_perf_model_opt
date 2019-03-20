# Copyright 2015-2019 Duke University
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License Version 2 as published by the
# Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License Version 2
# along with this program. If not, see
# <https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.

CFLAGS ?= -Wall -std=c99 -fopenmp -D_GNU_SOURCE -D__USE_MINGW_ANSI_STDIO -g -O3
LDFLAGS ?= -lm -lgomp
MKDIR ?= mkdir
MAKEFLAGS ?= --warn-undefined-variables

SRC = src
BIN = bin
CONSTRAINTS_SRC = src/constraints
CONSTRAINTS_BIN = bin/constraints
HEADERS = $(wildcard $(SRC)/*.h)
SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SOURCES))
CONSTRAINT_SOURCES = $(wildcard $(CONSTRAINTS_SRC)/*.c)
CONSTRAINT_OBJECTS = $(patsubst $(CONSTRAINTS_SRC)/%.c, $(CONSTRAINTS_BIN)/%.o, $(CONSTRAINT_SOURCES))
TARGETS = $(patsubst $(CONSTRAINTS_SRC)/%.c, $(BIN)/bt_ga_%, $(CONSTRAINT_SOURCES))
RESULTS_DIRS = $(patsubst $(CONSTRAINTS_SRC)/%.c, results_%, $(CONSTRAINT_SOURCES))
RESULTS = $(patsubst $(CONSTRAINTS_SRC)/%.c, results_%/results.tsv, $(CONSTRAINT_SOURCES))

.PRECIOUS: $(TARGETS) $(OBJECTS) $(CONSTRAINT_OBJECTS)

.PHONY: default
default: $(TARGETS)

.PHONY: all
all: default $(RESULTS)

print-%: ; @echo $* = $($*)

$(OBJECTS): $(BIN)/%.o: $(SRC)/%.c $(HEADERS) $(SOURCES)
	$(MKDIR) -p $(BIN)
	$(CC) $(CFLAGS) -c $< -o $@

$(CONSTRAINT_OBJECTS): $(CONSTRAINTS_BIN)/%.o: $(CONSTRAINTS_SRC)/%.c $(HEADERS) $(SOURCES)
	$(MKDIR) -p $(CONSTRAINTS_BIN)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGETS): $(BIN)/bt_ga_%: $(CONSTRAINTS_BIN)/%.o $(OBJECTS)
	$(MKDIR) -p $(BIN)
	$(CC) $< $(OBJECTS) -Wall $(LDFLAGS) -o $@

results_%/results.tsv: bin/bt_ga_% params.tsv
	$(RM) -r $(dir $@)
	$(MKDIR) -p $(dir $@)
	$< --output-integration=$(dir $@)/integration%zd.tsv --output-population=$(dir $@)/population%zd.tsv --output-convergence=$(dir $@)/convergence%zd.tsv -n5 params.tsv $(dir $@)/results.tsv

results_fitness_fatigue_ratio/results.tsv: bin/bt_ga_fitness_fatigue_ratio params.tsv
	$(RM) -r $(dir $@)
	$(MKDIR) -p $(dir $@)
	$< --output-integration=$(dir $@)/integration%zd.tsv --output-population=$(dir $@)/population%zd.tsv --output-convergence=$(dir $@)/convergence%zd.tsv -n5 -g5000 --mutate-change-rate=0.9999 --max-roughness-factor=60 params.tsv $(dir $@)/results.tsv

results_fitness_max_stress_fatigue_max_stress_fitness_fatigue_ratio/results.tsv: bin/bt_ga_fitness_max_stress_fatigue_max_stress_fitness_fatigue_ratio params.tsv
	$(RM) -r $(dir $@)
	$(MKDIR) -p $(dir $@)
	$< --output-integration=$(dir $@)/integration%zd.tsv --output-population=$(dir $@)/population%zd.tsv --output-convergence=$(dir $@)/convergence%zd.tsv -n5 -g5000 --mutate-change-rate=0.9999 --max-roughness-factor=70 params.tsv $(dir $@)/results.tsv

.PHONY: doc
doc:
	doxygen

.PHONY: clean
clean:
	git clean -fXdq
