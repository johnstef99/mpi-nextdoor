.\" config
.nr PS 12
.nr FM 0.5i
.nr GROWPS 2
.nr PSINCR 3p
.nr FL \n[LL]
.nr FGPS \n[PS]-2

.OH '-\En[%]-''\E*[TITLE]'
.EH '\E*[TITLE]''-\En[%]-'

.de PT
.ie \\n%=1 .if \\n[pg*P1] .tl \\*[pg*OH]
.el \{\
.	ie o .tl \\*[pg*OH]
.	el .tl \\*[pg*EH]
.\}
.sp -1
\l'6i\[ul]'
..

.de BL
.IP \(bu 2
..

.nr FigCount 1

.de figure
. ps \n[FGPS]
. nop Figure \\n[FigCount]: \\$^
. ps \n[PS]
.nr FigCount +1
..

.EQ
delim $$
define forall '\[fa]'
define memof  '\[mo]'
define odot   '"\s-2\[ci]\s+2"'
.EN

.R1
discard BXYZ
no-accumulate
.R2


.ds TITLE MPI brute-force all-kNN search

.\" cover
.TL
\*[TITLE]
.AU
Ioannis Stefanidis - 9587
.AI
Aristotel University of Thessaloniki
.sp 6p
.C
8 January 2023
.AB no
For this assignment we were asked implement in MPI a distributed brute-force
all-KNN search algorithm for the $k$ nearest neighbors (kNN) of each point in a
dataset.
.AE

.\" beginning of document
.SH 1
Communication in a Ring
.PP
Each MPI process $P sub i$ is reading a part of the provided dataset and it
calculates the distance of its own points from all (other) points and record the
distances and global indices of the $k$ nearest for each of its own points.
While calculating the distances, each process sends it's own points to the next
process and receives the points from the previous process. By doing that
asynchronous we hide the communication costs for these data transfers.
.LP
Having $NP$ number of processes means that after $NP$ iterations of sending and
receiving all processes would have checked their points against the whole
dataset. At this points the host machine (process #0) writes it's own results to
a file and one by one receives and appends the results from all other processes.

.SH 1
kNN Search Algorithm
.SH 2
Finding the distances
.PP
To find the $k$ nearest neighbors for each point we calculate the Euclidean
Distance Matrix $D$, then sort each row (in parallel with \f[CW]opencilk\fR)
using quickselect to get the $k$ smallest distances and finally sort each row
from 0 to $k$ using quicksort. To calculate the matrix $D$ the \f[CW]openblas\fR
library is used to multiply the matrix X with Y, where X are the process's
points and Y the points that are cycling through the ring. After that for every
point $d sub ij memof ^ D$ the addition $d sub ij = d sub ij + sum from 0 to d
{x sub id sup 2 + y sub jd sup 2}$ is performed. So the final matrix $D$ can be
expressed as (the $-2$ multiplication is performed by \f[CW]openblas\fR):
.EQ
define XT 'X sup T'
define YT 'Y sup T'
D = (X odot X) 1 sub {d times n} - 2 X YT + (Y odot Y) 1 sub {d times m}
.EN
.SH 2
Keeping track of global indices
.PP
When the matrix $D$ is created we also create a matrix $D sub ind$ to keep track
of the indices. While sorting the $D$ with quickselect and quicksort the same
swaps are preformed on the $D_ind$ matrix, this way we can keep track of the
global indices.

.SH 1
Testing
.PP
To check the correctness of the program regular Cartesian 2D,3D, and 4D grids
were used as inputs. For these grids the first $3 sup d$ neighbors are known for
each point, so the program output for $k=3 sup d$ was compered with the known
neighbors and found equal.

.SH 1
Benchmarking
.PP
To measure the performance and speedup we used the following datasets: 
.BL
MNIST $60"K" times 784$
.BL
2D grid $1"M" times 2$
.BL
3D grid $1"M" times 3$
.BL
4D grid $810"K" times 4$
\
.LP
These grids were generated using the \f[CW]grid\fR executable\*[*] which is
provided with the source code.
.FS
e.g. \s-2\f[CW]./grid 3 100 100 100\fR\s+2 outputs the 3D grid.
.FE
.SH 2
Finding the best config
.PP
The AUTH HPC\*[*] provided us maximum of 1024 cpus. Each node in the \fIrome\fR
partition has 128 cpus so to fully take advantage of the Aristotel cluster we
need to use 8 nodes. So we ran 4 tests for each dataset, in each test we
assigned a different amount of cpus to every proccess. The best results of these
tests are shown at the following table (Fig. \n[FigCount]), where $N$ is the
number of nodes, $ppn$ is the number of proccess per node and $cpp$ is the
number of cores per proccess ($ppn = 128/cpp$).
.FS
HPC website: https://hpc.auth.gr/
.FE
.DS C
.TS
tab(|);
|l|c|c|c|c|c|c|
|l|r|r|r|r|r|r|.
_
Dataset  |    m |   d |  k | $cpp$ | $ppn$ | time(s)
_
_
2D-Grid  |   1M |   2 |  9 |     4 |    32 |     402
3D-Grid  |   1M |   3 | 27 |     4 |    32 |     117
4D-Grid  | 810K |   4 | 81 |     2 |    64 |      62
12D-Grid | 531K |  12 | 10 |     2 |    64 |      13
MNIST    |  60K | 784 |  3 |     8 |    16 |    5.65
_
.TE
.figure Table of the best configs for each dataset
.DE

.G1
frame invis ht 2 wid 5 left solid bot solid
#ticks bottom off
label left "$log(t)$ (ms)" left 0.2
label bottom "Number of cores per proccess"

draw    d3 dashed color    "red"
draw    d4 dashed color   "blue"
draw mnist dashed color  "green"
draw    d2 dashed color "purple"
draw   d12 dashed color   "cyan"

i = 1

copy "data/data.d" thru {
  if i > 1 then {
    if i < 7 then {
      next d3 at $3, log($4)
      grid bottom at $3
    }
  }
  if i > 6 then {
    if i < 12 then {
      next d4 at $3, log($4)
    }
  }
  if i > 11 then {
    if i < 17 then {
      next mnist at $3, log($4)
    }
  }
  if i > 16 then {
    if i < 22 then {
      next d2 at $3, log($4)
    }
  }
  if i > 21 then {
    if i < 27 then {
      next d12 at $3, log($4)
    }
  }
  i = i+1
}

# legend
lx  = 26   # legend x
ly  = 4.95    # legend y
lyg = 0.2  # legend gap
lxg = 1    # legend gap

copy until "DONE" thru {
  line color "$3" $2 from lx, $1 to lx+lxg, $1
  " $4" size -2 ljust at lx+lxg, $1
}
      ly solid purple  2D-Grid
  ly-lyg solid    red  3D-Grid
ly-lyg*2 solid   blue  4D-Grid
ly-lyg*3 solid   cyan 12D-Grid
ly-lyg*4 solid  green    MNIST
DONE
.G2


.B1
.CD
The source code for this assignment is available at:
.br
.I "https://github.com/johnstef99/mpi_nextdoor"
.DE
.B2
