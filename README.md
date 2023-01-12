### MPI-Nextdoor
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
