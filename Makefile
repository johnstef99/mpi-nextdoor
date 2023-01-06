#
# Created by:   github.com/johnstef99
# Last updated: 2022-11-12
#

SRC_DIR  = src
OBJ_DIR  = obj
LIB_DIR  = libs

CFILES   = $(wildcard $(SRC_DIR)/*.c)
LIBFILES = $(wildcard $(LIB_DIR)/*.c)
OBJFILES = $(CFILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) $(LIBFILES:$(LIB_DIR)/%.c=$(OBJ_DIR)/%.o)
OUT      = bin/mpi_nextdoor

CC      = mpicc
OMPI_CC = /opt/opencilk/bin/clang
CFLAGS  = -Wall -fopencilk
LDFLAGS = -fopencilk -lblas

# default values for testing
NP            = 2
FILE          = assets/test.csv
MAX_LINE_SIZE = 100
COL_TO_SKIP   = 0
M             = 20
D             = 2
K             = 3
OUTPUT_FILE   = test

all: CFLAGS += -O3 -g
all: $(OUT)

cilkscan: CFLAGS  += -Og -g -fsanitize=cilk
cilkscan: LDFLAGS += -fsanitize=cilk
cilkscan: K=3
cilkscan: clean $(OUT) run

$(OUT): $(OBJFILES)
	OMPI_CC=$(OMPI_CC) $(CC) $(LDFLAGS) -o $@ $^ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	OMPI_CC=$(OMPI_CC) $(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean test run debug
run:
	mpirun -np $(NP) ./bin/mpi_nextdoor $(FILE) $(MAX_LINE_SIZE) $(COL_TO_SKIP) $(M) $(D) $(K) $(OUTPUT_FILE)

debug: 
	@DEBUG=1 make run

clean:
	rm -f $(OBJFILES) $(OUT)

