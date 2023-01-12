# MPI-Nextdoor
kNN search algorithm implementation using MPI
___


### Building from source

The makefile was tested on MacOS and on the HPC (Aristotel cluster).

#### MacOS
Make sure you have **open-mpi and openblas** installed, you can install these
with
```sh
brew install open-mpi openblas
```
You also need [OpenCilk](https://www.opencilk.org/), if you don't have OpenCilk
in `/opt/opencilk/` you would need to change the `OMPI_CC` variable in the
Makefile.

#### HPC 
To build this project in HPC you first need to load the required modules by
running
```sh
module load OpenCilk gcc/9.4.0-eewq4j6 openmpi openblas/0.3.20-653pmjy
```
After successfully loading the modules you simply run `make` to build the
mpi-nextdoor, or `make grid` to build the grid generator.

### How to use

The executable needs 7 arguments to run 
```
mpi_nextdoor [filename] [max_line_size] [columns_to_skip] [m] [d] [k] [output_file]
```
- `filename`: the dataset csv file (comma separated, see [test.csv](./assets/test.csv))
- `max_line_size`: the max number of characters that a line may contain in the csv file
- `columns_to_skip`: how many columns to skip before start reading `d` items (for example the MNIST dataset has the labels as the first column)
- `m`: number of points in the dataset
- `d`: the dimensions of each point
- `k`: how many neighbors to find for each point (including the point it self with 0 distance)
- `output_file`: the prefix of the output file

You must have a directory named `results` in you working directory when calling
`mpi_nextdoor`. Inside `results` for each run 3 files are going to be created
`output_file_dist.csv`, `output_file_idx.csv` and `output_file_time.txt` which
contain the distances, indexes and the execution time respectively.
