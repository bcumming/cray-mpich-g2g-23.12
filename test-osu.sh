#!/bin/bash
export LOCAL_RANK=$SLURM_LOCALID
export GLOBAL_RANK=$SLURM_PROCID

# affinity for devices indexed by numa node
export GPUS=(3 2 1 0)

# bind devices to mpi rank
export NUMA_NODE=$LOCAL_RANK
export CUDA_VISIBLE_DEVICES=${GPUS[$NUMA_NODE]}
export MPICH_GPU_SUPPORT_ENABLED=1

echo "rank $GLOBAL_RANK:$LOCAL_RANK on $(hostname) gpu $CUDA_VISIBLE_DEVICES numa-node $NUMA_NODE"

export LD_LIBRARY_PATH=/user-environment/linux-sles15-zen3/gcc-11.3.0/cuda-11.8.0-mys3s4mctqbnkjalzmpivuika767vo62/lib64

export FI_LOG_LEVEL=1
numactl --cpunodebind=$NUMA_NODE --membind=$NUMA_NODE osu_bw  --accelerator=cuda D D

#if [ $GLOBAL_RANK -eq 0 ]
#then
    #strace="strace -f"
#fi
#numactl --cpunodebind=$NUMA_NODE --membind=$NUMA_NODE $strace osu_bw  --accelerator=cuda D D |& tee strace.tasnam.withcuda

