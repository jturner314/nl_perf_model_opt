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

CC = gcc
CFLAGS = -Wall -std=c99 -fopenmp -D_GNU_SOURCE -D__USE_MINGW_ANSI_STDIO -g -O3
LDFLAGS = -lm -lgomp
PYTHON = python
CP = cp
MKDIR = mkdir

SRC = src
BIN = bin
MAIN_BIN = $(BIN)/bt_ga
TEST_BIN = $(BIN)/test
SOURCES = $(wildcard $(SRC)/*.c)
HEADERS = $(wildcard $(SRC)/*.h)
ALL_OBJECTS = $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SOURCES))
INCL_OBJECTS = $(patsubst $(SRC)/%.h, $(BIN)/%.o, $(HEADERS))
MAIN_OBJECT = $(BIN)/main.o
TEST_OBJECT = $(BIN)/test.o
RESULTS = results/results.tsv

.PRECIOUS: $(MAIN_BIN) $(TEST_BIN) $(OBJECTS)

.PHONY: default
default: $(MAIN_BIN)

.PHONY: all
all: $(MAIN_BIN) $(RESULTS)

$(BIN)/%.o: $(SRC)/%.c $(HEADERS) $(SOURCES)
	$(MKDIR) -p $(BIN)
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN_BIN): $(MAIN_OBJECT) $(INCL_OBJECTS) $(BIN)
	$(MKDIR) -p $(BIN)
	$(CC) $(MAIN_OBJECT) $(INCL_OBJECTS) -Wall $(LDFLAGS) -o $@

$(TEST_BIN): $(TEST_OBJECT) $(INCL_OBJECTS) $(BIN)
	$(MKDIR) -p $(BIN)
	$(CC) $(TEST_OBJECT) $(INCL_OBJECTS) -Wall $(LDFLAGS) -o $@

results/results.tsv: $(MAIN_BIN) data/dv_bounds.tsv data/training_data.tsv data/trial_indices.tsv
	$(MKDIR) -p results
	$(MAIN_BIN) -n10 -g5000 -iresults/integration%zd.tsv -wresults/population%zd.tsv -cresults/convergence%zd.tsv data/dv_bounds.tsv data/training_data.tsv data/trial_indices.tsv $@

.PHONY: test
test: $(TEST_BIN)
	$(TEST_BIN)

.PHONY: clean
clean:
	$(RM) -r $(BIN)
