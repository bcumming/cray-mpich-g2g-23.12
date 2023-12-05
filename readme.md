Hello,

we are experience a hang on tasnam [1] in inter-node cuda-aware MPI_Send, when
using cray-mpich installed as a binary package in the stackinator squashfs-image
(spack) [3]. Intra-node GPU direct, and host-to-host communcation is working.
GPU Direct works as expected using the same image on a different machine with older CrayOS 
(clariden [2]).

The stackinator image is essentially a spack install tree stored in a squashfs
file. It is self-contained, meaning the executable is bundled with the
cray-mpich, pmi, pals libraries, where only `libfabric.so` is used from the
system. The cray-mpich and dependent libraries are copied to the spack install
tree and RPATHs are set accordingly.


As a test we have used osu-micro-benchmarks (osu_bw -d cuda D D), which gets stuck in `MPI_Send`.

```
$ FI_LOG_LEVEL=debug LD_LIBRARY_PATH=/path/to/libcudart.so srun -N2 -n2 --ntasks-per-node osu_bw -d cuda D D
```

```
# OSU MPI-CUDA Bandwidth Test v7.1
# Send Buffer on DEVICE (D) and Receive Buffer on DEVICE (D)
# Size      Bandwidth (MB/s)
# Datatype: MPI_CHAR.
libfabric:54799:1701770603::cxi:mr:cxip_do_map():90<warn> nid001001: cxil_map() failed: -22
libfabric:54799:1701770603::cxi:mr:cxip_map_cache():313<warn> nid001001: Failed to acquire mapping (0x14b11a800000, 4194304): -22
libfabric:54799:1701770603::cxi:ep_data:cxip_recv_req_alloc():211<warn> nid001001: RXC (0x8f3:0:0) PtlTE 12: Failed to map recv buffer: -22
libfabric:54799:1701770603::cxi:ep_data:cxip_recv_common():3434<warn> nid001001: RXC (0x8f3:0:0) PtlTE 12: Failed to allocate recv request
libfabric:54799:1701770603::cxi:mr:cxip_do_map():90<warn> nid001001: cxil_map() failed: -22
libfabric:54799:1701770603::cxi:mr:cxip_map_cache():313<warn> nid001001: Failed to acquire mapping (0x14b11a800000, 4194304): -22
libfabric:54799:1701770603::cxi:ep_data:cxip_recv_req_alloc():211<warn> nid001001: RXC (0x8f3:0:0) PtlTE 12: Failed to map recv buffer: -22
```

The full stdout of `srun osu_bw` is attached. During initialization there are
errors regarding dlopen of `libze_loader.so` and `libhsa-runtime.so`. When
`LD_LIBRARY_PATH` isn't set, there is a dlopen error for `libcudart.so`. The
same behavior is observed on clariden, when `LD_LIBRARY_PATH` doesn't point to
libcudart, it hangs and `FI_LOG_LEVEL=debug` reports the same errors (cxi)
listed above.

One difference I could spot is that on tsanam, it always tries to dlopen libze and libhsa. 



[1] tasnsam is a virtual cluster running
```
PRODUCT="HPE Cray OS"
OS=SLES15SP4
ARCH=x86_64
VERSION=2.5
DATE=20230714124224
libfabric-devel-1.15.2.0-SSHOT2.0.2_20230429001018_0155162561c7.x86_64
```

[2] clariden is a virtual cluster running
```
PRODUCT="HPE Cray OS"
OS=SLES15SP3
ARCH=x86_64
VERSION=2.3
DATE=20221020174525
libfabric-devel-1.15.2.0-SSHOT2.0.1_20230104144000_a898af7f4.x86_64
```

[3] https://eth-cscs.github.io/stackinator/ 

[4] cray-mpich versions

| cray-pals  | 1.2.12 |                     |
| cray-pmi   | 6.1.11 |                     |
| cray-gtl   | 8.1.26 |                     |
| cray-mpich | 8.1.26 |                     |
| cuda       | 11.8   | installed via spack |

