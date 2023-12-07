<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [MPI Send/Receive Device to Device](#mpi-sendreceive-device-to-device)
    - [Blocking](#blocking)
        - [16 bytes -> hangs](#16-bytes---hangs)
        - [256 bytes -> crashes](#256-bytes---crashes)
    - [Non-Blocking](#non-blocking)
        - [16 bytes -> hangs](#16-bytes---hangs-1)
        - [256 bytes -> crashes](#256-bytes---crashes-1)

<!-- markdown-toc end -->

# MPI Send/Receive Device to Device #

## Blocking ##

[main.cpp](./main.cpp) contains `MPI_send|recv` accepting the number of bytes as cmd input.

Observation:
- hangs when msg size is 16 bytes (curiously the backtrace is different to the one found in osu_bw)
- crashes when msg size is 256 bytes

```
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

nbytes=16

srun -N 2 -n 2 --ntasks-per-node=1 -u ./cuda-direct 16 D D 2>&1
```

### 16 bytes -> hangs ###

node1
```
(gdb) bt
#0  0x00001498016c7539 in cxip_cq_eq_progress (eq=0x4e8fa0, cq=0x4e8e80) at prov/cxi/src/cxip_cq.c:508
#1  cxip_cq_progress (cq=0x4e8e80) at prov/cxi/src/cxip_cq.c:550
#2  0x00001498016c7c79 in cxip_util_cq_progress (util_cq=0x4e8e80) at prov/cxi/src/cxip_cq.c:563
#3  0x00001498016a3111 in ofi_cq_readfrom (cq_fid=0x4e8e80, buf=<optimized out>, count=8, src_addr=0x0)
    at prov/util/src/util_cq.c:232
#4  0x0000149803c28807 in MPIR_Wait_impl.part.0 () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#5  0x0000149804a1f726 in MPIC_Wait () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#6  0x0000149804a330ec in MPIC_Sendrecv () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#7  0x0000149804934a2c in MPIR_Allreduce_intra_recursive_doubling () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#8  0x0000149802ea6811 in MPIR_Allreduce_intra_auto () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#9  0x0000149802ea69f5 in MPIR_Allreduce_impl () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#10 0x0000149804c2a7b7 in MPIR_CRAY_Allreduce () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#11 0x00001498051d44b2 in _cxi_global_counters_report () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#12 0x00001498051d501f in MPIDI_CRAY_ofi_gather_cxi_counters () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#13 0x00001498052768f6 in MPIDI_CRAY_finalize () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#14 0x0000149804dbc3f0 in MPID_Finalize () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#15 0x0000149803443a0e in PMPI_Finalize () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#16 0x000000000040126a in main ()
```

node2
```
(gdb) bt
#0  0x000014cf5593bb27 in write () from /lib64/libc.so.6
#1  0x000014cf558c7dad in _IO_file_write@@GLIBC_2.2.5 () from /lib64/libc.so.6
#2  0x000014cf558c700f in new_do_write () from /lib64/libc.so.6
#3  0x000014cf558c859e in __GI__IO_file_xsputn () from /lib64/libc.so.6
#4  0x000014cf558bb037 in fputs () from /lib64/libc.so.6
#5  0x000014cf5512fb1f in fprintf (__fmt=<synthetic pointer>, __stream=<optimized out>) at /usr/include/bits/stdio2.h:100
#6  fi_log_ (prov=prov@entry=0x14cf55404440 <cxip_prov>, level=level@entry=FI_LOG_WARN, subsys=subsys@entry=FI_LOG_EP_DATA,
    func=func@entry=0x14cf551d0cf0 <__func__.37690> "cxip_recv_req_alloc", line=line@entry=211,
    fmt=fmt@entry=0x14cf551d05a8 "%s: RXC (%#x:%u:%u) PtlTE %u: Failed to map recv buffer: %d\n") at src/log.c:191
#7  0x000014cf551838e1 in cxip_recv_req_alloc (len=<optimized out>, buf=<optimized out>, rxc=0x4f3770)
    at prov/cxi/src/cxip_msg.c:211
#8  cxip_recv_common (rxc=0x4f3770, buf=<optimized out>, len=<optimized out>, desc=<optimized out>, src_addr=<optimized out>,
    tag=<optimized out>, ignore=246290604621824, context=0x14cf5946a7e8 <MPIR_Request_direct+776>, flags=16777216,
    tagged=true, comp_cntr=0x0) at prov/cxi/src/cxip_msg.c:3432
#9  0x000014cf55183b8f in cxip_trecv (ep=<optimized out>, buf=<optimized out>, len=<optimized out>, desc=<optimized out>,
    src_addr=<optimized out>, tag=<optimized out>, ignore=246290604621824, context=0x14cf5946a7e8 <MPIR_Request_direct+776>)
    at prov/cxi/src/cxip_msg.c:4734
#10 0x000014cf5714e6c0 in PMPI_Recv () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#11 0x000000000040118a in main ()
```

### 256 bytes -> crashes ###

It shows the same cxi warnings, but crashes after a few seconds:
```
libfabric:11521:1701985650::cxi:ep_data:cxip_recv_req_alloc():211<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to map recv buffer: -22
libfabric:11521:1701985650::cxi:ep_data:cxip_recv_common():3434<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to allocate recv request
libfabric:11521:1701985650::cxi:mr:cxip_do_map():90<warn> nid001097: cxil_map() failed: -22
libfabric:11521:1701985650::cxi:mr:cxip_map_cache():313<warn> nid001097: Failed to acquire mapping (0x151242400000, 256): -22
libfabric:11521:1701985650::cxi:ep_data:cxip_recv_req_alloc():211<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to map recv buffer: -22
libfabric:11521:1701985650::cxi:ep_data:cxip_recv_common():3434<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to allocate recv request
libfabric:11521:1701985650::cxi:mr:cxip_do_map():90<warn> nid001097: cxil_map() failed: -22
srun: error: nid001097: task 1: Terminated
srun: Force Terminated StepId=8442.0
```


## Non-Blocking ##

[main_nonblockin.cpp](./main_nonblocking.cpp) contains `MPI_send|recv` accepting the number of bytes as cmd input.

### 16 bytes -> hangs ###

node1
```
#0  0x00001483d973db27 in write () from /lib64/libc.so.6
#1  0x00001483d96c9dad in _IO_file_write@@GLIBC_2.2.5 () from /lib64/libc.so.6
#2  0x00001483d96c900f in new_do_write () from /lib64/libc.so.6
#3  0x00001483d96ca59e in __GI__IO_file_xsputn () from /lib64/libc.so.6
#4  0x00001483d96bd037 in fputs () from /lib64/libc.so.6
#5  0x00001483d8f31b1f in fprintf (__fmt=<synthetic pointer>, __stream=<optimized out>) at /usr/include/bits/stdio2.h:100
#6  fi_log_ (prov=prov@entry=0x1483d9206440 <cxip_prov>, level=level@entry=FI_LOG_WARN,
    subsys=subsys@entry=FI_LOG_EP_DATA, func=func@entry=0x1483d8fd2cf0 <__func__.37690> "cxip_recv_req_alloc",
    line=line@entry=211, fmt=fmt@entry=0x1483d8fd25a8 "%s: RXC (%#x:%u:%u) PtlTE %u: Failed to map recv buffer: %d\n")
    at src/log.c:191
#7  0x00001483d8f858e1 in cxip_recv_req_alloc (len=<optimized out>, buf=<optimized out>, rxc=0x4f3810)
    at prov/cxi/src/cxip_msg.c:211
#8  cxip_recv_common (rxc=0x4f3810, buf=<optimized out>, len=<optimized out>, desc=<optimized out>,
    src_addr=<optimized out>, tag=<optimized out>, ignore=246290604621824,
    context=0x1483dd26c7e8 <MPIR_Request_direct+776>, flags=16777216, tagged=true, comp_cntr=0x0)
    at prov/cxi/src/cxip_msg.c:3432
#9  0x00001483d8f85b8f in cxip_trecv (ep=<optimized out>, buf=<optimized out>, len=<optimized out>, desc=<optimized out>,
    src_addr=<optimized out>, tag=<optimized out>, ignore=246290604621824,
    context=0x1483dd26c7e8 <MPIR_Request_direct+776>) at prov/cxi/src/cxip_msg.c:4734
#10 0x00001483dadded76 in PMPI_Irecv () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#11 0x0000000000401205 in main ()
```

node2
```
(gdb) bt
#0  ofi_genlock_lock (lock=0x4e8ef0) at ./include/ofi_lock.h:359
#1  ofi_cq_readfrom (cq_fid=0x4e8e80, buf=0x7ffed8bc0330, count=8, src_addr=0x0) at prov/util/src/util_cq.c:229
#2  0x0000152713130807 in MPIR_Wait_impl.part.0 () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#3  0x0000152713f27726 in MPIC_Wait () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#4  0x0000152713f3b0ec in MPIC_Sendrecv () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#5  0x0000152713e3ca2c in MPIR_Allreduce_intra_recursive_doubling () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#6  0x00001527123ae811 in MPIR_Allreduce_intra_auto () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#7  0x00001527123ae9f5 in MPIR_Allreduce_impl () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#8  0x00001527141327b7 in MPIR_CRAY_Allreduce () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#9  0x00001527146dc4b2 in _cxi_global_counters_report () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#10 0x00001527146dd01f in MPIDI_CRAY_ofi_gather_cxi_counters () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#11 0x000015271477e8f6 in MPIDI_CRAY_finalize () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#12 0x00001527142c43f0 in MPID_Finalize () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#13 0x000015271294ba0e in PMPI_Finalize () from /opt/cray/pe/lib64/libmpi_gnu_91.so.12
#14 0x00000000004012f6 in main ()
```

### 256 bytes -> crashes ###

It crashes, the following error messages are shown:
```
libfabric:16567:1701987363::cxi:ep_data:cxip_recv_common():3434<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to allocate recv request
libfabric:16567:1701987363::cxi:mr:cxip_do_map():90<warn> nid001097: cxil_map() failed: -22
libfabric:16567:1701987363::cxi:mr:cxip_map_cache():313<warn> nid001097: Failed to acquire mapping (0x150cb2400000, 256): -22
libfabric:16567:1701987363::cxi:ep_data:cxip_recv_req_alloc():211<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to map recv buffer: -22
libfabric:16567:1701987363::cxi:ep_data:cxip_recv_common():3434<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to allocate recv request
libfabric:16567:1701987363::cxi:mr:cxip_do_map():90<warn> nid001097: cxil_map() failed: -22
libfabric:16567:1701987363::cxi:mr:cxip_map_cache():313<warn> nid001097: Failed to acquire mapping (0x150cb2400000, 256): -22
libfabric:16567:1701987363::cxi:ep_data:cxip_recv_req_alloc():211<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to map recv buffer: -22
libfabric:16567:1701987363::cxi:ep_data:cxip_recv_common():3434<warn> nid001097: RXC (0xbf3:0:0) PtlTE 12: Failed to allocate recv request
libfabric:16567:1701987363::cxi:mr:cxip_do_map():90<warn> nid001097: cxil_map() failed: -22
srun: error: nid001097: task 1: Terminated
srun: Force Terminated StepId=8597.0
```
