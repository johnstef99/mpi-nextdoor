#
# Created by:   github.com/johnstef99
# Last updated: 2022-11-12
#

SRC_DIR  = ./src
OBJ_DIR  = ./obj
LIB_DIR  = ./libs

CC       = xcrun /opt/opencilk/bin/clang
CFILES   = $(wildcard $(SRC_DIR)/*.c)
LIBFILES   = $(wildcard $(LIB_DIR)/*.c)
OBJFILES = $(CFILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) $(LIBFILES:$(LIB_DIR)/%.c=$(OBJ_DIR)/%.o)
OUT      = ./bin/mpi_nextdoor

CFLAGS  = -Wall -fopencilk
LDFLAGS = -framework Accelerate -fopencilk

all: CFLAGS  += -O3 -g
all: $(OUT)

cilkscan: CFLAGS  += -Og -g -fsanitize=cilk
cilkscan: LDFLAGS += -fsanitize=cilk
cilkscan: clean $(OUT) run


$(OUT): $(OBJFILES)
	$(CC) $(LDFLAGS) -o $@ $^ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean test run
run:
	./bin/mpi_nextdoor

clean:
	rm -f $(OBJFILES) $(OUT)

test:
	echo $(OBJFILES)
