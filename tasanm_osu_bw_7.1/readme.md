<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [osu_bw from osu-micro-benchmarks 7.1](#osu_bw-from-osu-micro-benchmarks-71)
    - [ldd/libtree](#lddlibtree)
    - [gdb backtrace](#gdb-backtrace)
    - [COS and libfabric version](#cos-and-libfabric-version)

<!-- markdown-toc end -->


# osu_bw from osu-micro-benchmarks 7.1 #

Here are the results from the tests with osu-micro-benchmakrs v7.1 from December 7 on tasnam.

We have run the following sbatch script:

```bash
module load PrgEnv-gnu
module swap  gcc/12.2.0 gcc/10.3.0
module load cudatoolkit/23.3_11.8
module load craype-accel-nvidia80
module load libfabric

export MPICH_GPU_MANAGED_SUPPORT_ENABLED=1
export MPICH_GPU_SUPPORT_ENABLED=1
export MPICH_RDMA_ENABLED_CUDA=1
export MPICH_VERSION_DISPLAY=1
export FI_LOG_LEVEL=debug
srun -N 2 -n 2 --ntasks-per-node=1 -u ./numactl.sh c/mpi/pt2pt/standard/osu_bw -d cuda D D 2>&1
```

The standard output is attached (duplicated error msg have been removed, it was
190MB large): [stdout.out](./stdout.out)

It hangs, the following lines are repeated infinitely:
```
libfabric:100855:1701967917::cxi:mr:cxip_do_map():90<warn> nid001097: cxil_map() failed: -22
libfabric:100855:1701967917::cxi:mr:cxip_map_cache():313<warn> nid001097: Failed to acquire mapping (0x1531aa800000, 4194304): -22
libfabric:100855:1701967917::cxi:ep_data:cxip_recv_req_alloc():211<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to map recv buffer: -22
libfabric:100855:1701967917::cxi:ep_data:cxip_recv_common():3434<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to allocate recv request
libfabric:100855:1701967917::cxi:mr:cxip_do_map():90<warn> nid001097: cxil_map() failed: -22
libfabric:100855:1701967917::cxi:mr:cxip_map_cache():313<warn> nid001097: Failed to acquire mapping (0x1531aa800000, 4194304): -22
libfabric:100855:1701967917::cxi:ep_data:cxip_recv_req_alloc():211<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to map recv buffer: -22
libfabric:100855:1701967917::cxi:ep_data:cxip_recv_common():3434<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to allocate recv request
libfabric:100855:1701967917::cxi:mr:cxip_do_map():90<warn> nid001097: cxil_map() failed: -22
```

## ldd/libtree ##

```bash
libtree -p c/mpi/pt2pt/standard/osu_bw
c/mpi/pt2pt/standard/osu_bw 
├── /opt/nvidia/hpc_sdk/Linux_x86_64/23.3/cuda/11.8/lib64/libcudart.so.11.0 [rpath]
│   └── /lib64/librt.so.1 [default path]
├── /opt/cray/pe/lib64/libmpi_gnu_91.so.12 [ld.so.conf]
│   ├── /opt/cray/pe/gcc-libs/libatomic.so.1 [rpath of 1]
│   ├── /opt/cray/pe/gcc-libs/libquadmath.so.0 [rpath of 1]
│   ├── /opt/cray/pe/gcc-libs/libgfortran.so.5 [rpath of 1]
│   │   └── /opt/cray/pe/gcc/10.3.0/snos/lib64/libquadmath.so.0 [LD_LIBRARY_PATH]
│   ├── /opt/cray/libfabric/1.15.2.0/lib64/libfabric.so.1 [LD_LIBRARY_PATH]
│   │   ├── /opt/cray/pe/gcc/10.3.0/snos/lib64/libatomic.so.1 [LD_LIBRARY_PATH]
│   │   ├── /usr/lib64/libcxi.so.1 [runpath]
│   │   ├── /usr/lib64/libcurl.so.4 [runpath]
│   │   │   ├── /lib64/libz.so.1 [default path]
│   │   │   ├── /usr/lib64/libnghttp2.so.14 [default path]
│   │   │   ├── /usr/lib64/libbrotlidec.so.1 [default path]
│   │   │   │   └── /usr/lib64/libbrotlicommon.so.1 [default path]
│   │   │   ├── /usr/lib64/libzstd.so.1 [default path]
│   │   │   ├── /usr/lib64/liblber-2.4.so.2 [default path]
│   │   │   ├── /usr/lib64/libldap_r-2.4.so.2 [default path]
│   │   │   │   ├── /lib64/libresolv.so.2 [default path]
│   │   │   │   ├── /usr/lib64/liblber-2.4.so.2 [default path]
│   │   │   │   ├── /usr/lib64/libssl.so.1.1 [default path]
│   │   │   │   │   └── /usr/lib64/libcrypto.so.1.1 [default path]
│   │   │   │   │       ├── /lib64/libz.so.1 [default path]
│   │   │   │   │       └── /usr/lib64/libjitterentropy.so.3 [default path]
│   │   │   │   ├── /usr/lib64/libsasl2.so.3 [default path]
│   │   │   │   └── /usr/lib64/libcrypto.so.1.1 [default path]
│   │   │   ├── /usr/lib64/libgssapi_krb5.so.2 [default path]
│   │   │   │   ├── /lib64/libcom_err.so.2 [default path]
│   │   │   │   ├── /usr/lib64/libkrb5.so.3 [default path]
│   │   │   │   │   ├── /lib64/libcom_err.so.2 [default path]
│   │   │   │   │   ├── /lib64/libresolv.so.2 [default path]
│   │   │   │   │   ├── /usr/lib64/libk5crypto.so.3 [default path]
│   │   │   │   │   │   ├── /usr/lib64/libkrb5support.so.0 [default path]
│   │   │   │   │   │   │   └── /lib64/libselinux.so.1 [default path]
│   │   │   │   │   │   │       └── /usr/lib64/libpcre.so.1 [default path]
│   │   │   │   │   │   └── /usr/lib64/libcrypto.so.1.1 [default path]
│   │   │   │   │   ├── /usr/lib64/libkrb5support.so.0 [default path]
│   │   │   │   │   └── /usr/lib64/libkeyutils.so.1 [default path]
│   │   │   │   ├── /usr/lib64/libkrb5support.so.0 [default path]
│   │   │   │   └── /usr/lib64/libk5crypto.so.3 [default path]
│   │   │   ├── /usr/lib64/libcrypto.so.1.1 [default path]
│   │   │   ├── /usr/lib64/libssl.so.1.1 [default path]
│   │   │   ├── /usr/lib64/libpsl.so.5 [default path]
│   │   │   │   ├── /usr/lib64/libidn2.so.0 [default path]
│   │   │   │   │   └── /usr/lib64/libunistring.so.2 [default path]
│   │   │   │   └── /usr/lib64/libunistring.so.2 [default path]
│   │   │   ├── /usr/lib64/libssh.so.4 [default path]
│   │   │   │   ├── /lib64/libz.so.1 [default path]
│   │   │   │   ├── /usr/lib64/libcrypto.so.1.1 [default path]
│   │   │   │   └── /usr/lib64/libgssapi_krb5.so.2 [default path]
│   │   │   └── /usr/lib64/libidn2.so.0 [default path]
│   │   ├── /usr/lib64/libjson-c.so.3 [runpath]
│   │   └── /lib64/librt.so.1 [default path]
│   ├── /opt/cray/pe/lib64/libpmi2.so.0 [ld.so.conf]
│   │   └── /opt/cray/pe/lib64/libpals.so.0 [ld.so.conf]
│   │       └── /usr/lib64/libjansson.so.4 [default path]
│   ├── /opt/cray/pe/lib64/libpmi.so.0 [ld.so.conf]
│   │   └── /opt/cray/pe/lib64/libpmi2.so.0 [ld.so.conf]
│   └── /lib64/librt.so.1 [default path]
├── /opt/cray/pe/lib64/libmpi_gtl_cuda.so.0 [ld.so.conf]
│   ├── /opt/nvidia/hpc_sdk/Linux_x86_64/23.3/cuda/11.8/lib64/libcudart.so.11.0 [rpath of 1]
│   └── /usr/lib64/libcuda.so.1 [default path]
│       └── /lib64/librt.so.1 [default path]
└── /usr/lib64/libcuda.so.1 [default path]
```



## gdb backtrace ##

Here are the backtraces from both sides of `osu_bw`.

nid001096
```
#0  0x0000145af4e3db27 in write () from /lib64/libc.so.6
#1  0x0000145af4dc9dad in _IO_file_write@@GLIBC_2.2.5 () from /lib64/libc.so.6
#2  0x0000145af4dc900f in new_do_write () from /lib64/libc.so.6
#3  0x0000145af4dca59e in __GI__IO_file_xsputn () from /lib64/libc.so.6
#4  0x0000145af4dbd037 in fputs () from /lib64/libc.so.6
#5  0x0000145af4631b1f in fprintf (__fmt=<synthetic pointer>, __stream=<optimized out>) at /usr/include/bits/stdio2.h:100
#6  fi_log_ (prov=prov@entry=0x145af4906440 <cxip_prov>, level=level@entry=FI_LOG_WARN, subsys=subsys@entry=FI_LOG_EP_DATA, func=func@entry=0x145af46d2cf0 <__func__.37690> "cxip_recv_req_alloc",
    line=line@entry=211, fmt=fmt@entry=0x145af46d25a8 "%s: RXC (%#x:%u:%u) PtlTE %u: Failed to map recv buffer: %d\n") at src/log.c:191
#7  0x0000145af46858e1 in cxip_recv_req_alloc (len=<optimized out>, buf=<optimized out>, rxc=0xe04cb0) at prov/cxi/src/cxip_msg.c:211
#8  cxip_recv_common (rxc=0xe04cb0, buf=<optimized out>, len=<optimized out>, desc=<optimized out>, src_addr=<optimized out>, tag=<optimized out>, ignore=246290604621824, context=0x1163840,
    flags=16777216, tagged=true, comp_cntr=0x0) at prov/cxi/src/cxip_msg.c:3432
#9  0x0000145af4685b8f in cxip_trecv (ep=<optimized out>, buf=<optimized out>, len=<optimized out>, desc=<optimized out>, src_addr=<optimized out>, tag=<optimized out>, ignore=246290604621824,
    context=0x1163840) at prov/cxi/src/cxip_msg.c:4734
#10 0x0000145af66506c0 in PMPI_Recv () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#11 0x000000000040301b in main (argc=<optimized out>, argv=<optimized out>) at osu_bw.c:240
#+end_example
```

nid001097
```
#0  0x0000150284145b27 in write () from /lib64/libc.so.6
#1  0x00001502840d1dad in _IO_file_write@@GLIBC_2.2.5 () from /lib64/libc.so.6
#2  0x00001502840d100f in new_do_write () from /lib64/libc.so.6
#3  0x00001502840d259e in __GI__IO_file_xsputn () from /lib64/libc.so.6
#4  0x00001502840c5037 in fputs () from /lib64/libc.so.6
#5  0x0000150283939b1f in fprintf (__fmt=<synthetic pointer>, __stream=<optimized out>) at /usr/include/bits/stdio2.h:100
#6  fi_log_ (prov=<optimized out>, level=level@entry=FI_LOG_WARN, subsys=subsys@entry=FI_LOG_MR, func=func@entry=0x1502839dc218 <__func__.37596> "cxip_do_map", line=90,
    fmt=0x1502839dbeeb "%s: cxil_map() failed: %d\n") at src/log.c:191
#7  0x00001502839959d8 in cxip_do_map (cache=0xdfa330, entry=<optimized out>) at prov/cxi/src/cxip_iomm.c:90
#8  0x0000150283960c02 in util_mr_cache_create (entry=0x7ffdd4318a40, info=0x7ffdd43189a0, cache=0xdfa330) at prov/util/src/util_mr_cache.c:277
#9  ofi_mr_cache_search (cache=cache@entry=0xdfa330, attr=attr@entry=0x7ffdd4318a80, entry=entry@entry=0x7ffdd4318a40) at prov/util/src/util_mr_cache.c:381
#10 0x000015028399617a in cxip_map_cache (md=0x7ffdd4318b30, attr=0x7ffdd4318a80, dom=0xdf81d0) at prov/cxi/src/cxip_iomm.c:310
#11 cxip_map (dom=dom@entry=0xdf81d0, buf=buf@entry=0x150252800000, len=len@entry=1, flags=flags@entry=0, md=md@entry=0x7ffdd4318b30) at prov/cxi/src/cxip_iomm.c:586
#12 0x000015028398cfb7 in cxip_recv_req_alloc (len=1, buf=0x150252800000, rxc=0xe04c50) at prov/cxi/src/cxip_msg.c:209
#13 cxip_recv_common (rxc=0xe04c50, buf=0x150252800000, len=1, desc=<optimized out>, src_addr=<optimized out>, tag=100, ignore=246290604621824, context=0x150287c747e8 <MPIR_Request_direct+776>,
    flags=16777216, tagged=true, comp_cntr=0x0) at prov/cxi/src/cxip_msg.c:3432
#14 0x000015028398db8f in cxip_trecv (ep=<optimized out>, buf=<optimized out>, len=<optimized out>, desc=<optimized out>, src_addr=<optimized out>, tag=<optimized out>, ignore=246290604621824,
    context=0x150287c747e8 <MPIR_Request_direct+776>) at prov/cxi/src/cxip_msg.c:4734
#15 0x00001502857e6d76 in PMPI_Irecv () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#16 0x000000000040328d in main (argc=<optimized out>, argv=<optimized out>) at osu_bw.c:280
```

## COS and libfabric version ##
```
PRODUCT="HPE Cray OS"
OS=SLES15SP4
ARCH=x86_64
VERSION=2.5
DATE=20230714124224
libfabric-devel-1.15.2.0-SSHOT2.0.2_20230429001018_0155162561c7.x86_64
```

```
[tasna][simonpi@nid001096 osu-micro-benchmarks-7.1]$ rpm -qi libfabric
Name        : libfabric
Version     : 1.15.2.0
Release     : SSHOT2.0.2_20230429001018_0155162561c7
Architecture: x86_64
Install Date: Tue 05 Dec 2023 02:01:10 PM CET
Group       : System Environment/Libraries
Size        : 8174735
License     : GPLv2 or BSD
Signature   : RSA/SHA256, Sat 29 Apr 2023 02:30:34 AM CEST, Key ID d4dae1e39da39f44
Source RPM  : libfabric-1.15.2.0-SSHOT2.0.2_20230429001018_0155162561c7.src.rpm
Build Date  : Sat 29 Apr 2023 02:22:11 AM CEST
Build Host  : 27813ee50131
Relocations : (not relocatable)
Packager    : https://www.suse.com/
URL         : https://github.hpe.com/hpe/hpc-shs-libfabric-netc.git
Summary     : User-space RDMA Fabric Interfaces
Description :
Git Repository: hpc-shs-libfabric-netc
Git Branch: release/slingshot-2.0
Git Commit Revision: 0155162561c7
Git Commit Timestamp: Wed Apr 26 12:50:00 2023 -0500

libfabric provides a user-space API to access high-performance fabric
services, such as RDMA.
Distribution: SUSE Linux Enterprise Server 15 SP4
```

