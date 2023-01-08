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
CFLAGS  = -Wall -fopencilk
LDFLAGS = -fopencilk

ifeq ($(shell uname),Darwin)
	# config compile on macos
	OMPI_CC  = xcrun\ /opt/opencilk/bin/clang
	CFLAGS  += -DMACOS
	LDFLAGS += -framework Accelerate
else
	# config compile on HPC AUTH
	OMPI_CC  = $(OPENCILK)/build/bin/clang
	CFLAGS  += -I$(OPENBLAS_ROOT)/include
	LDFLAGS += -L$(OPENBLAS_ROOT)/lib -lopenblas
endif
  
# default values for testing
NP            = 2
FILE          = assets/test.csv
MAX_LINE_SIZE = 100
COL_TO_SKIP   = 0
M             = 20
D             = 2
K             = 3
OUTPUT_FILE   = test

all: CFLAGS += -O3
all: $(OUT)

cilkscan: CFLAGS  += -Og -g -fsanitize=cilk
cilkscan: LDFLAGS += -fsanitize=cilk
cilkscan: clean $(OUT) test

$(OUT): $(OBJFILES)
	OMPI_CC=$(OMPI_CC) $(CC) $(LDFLAGS) -o $@ $^ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	OMPI_CC=$(OMPI_CC) $(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean test debug

ifeq ($(shell uname),Darwin)
test:
	mpirun -np $(NP) ./bin/mpi_nextdoor $(FILE) $(MAX_LINE_SIZE) $(COL_TO_SKIP) $(M) $(D) $(K) $(OUTPUT_FILE)
else
# each node in rome partition in HPC has 128 cpu cores
test:
	srun -p rome -J mpi_nextdoor -N 4 --ntasks-per-node=1 -c 4 ./bin/mpi_nextdoor $(FILE) $(MAX_LINE_SIZE) $(COL_TO_SKIP) $(M) $(D) $(K) $(OUTPUT_FILE)
endif

debug: 
	@DEBUG=1 make run

clean:
	rm -f $(OBJFILES) $(OUT)

grid:
	clang -O3 -Wall ./grid_generator/grid.c -o grid
