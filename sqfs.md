Here are the steps to load and run the reproducer using the squashfs image.

```bash
# start a shell with the image mounted
squashfs-mount /scratch/mch/simonpi/osu-mpich-8.1.21.sqfs:/user-environment -- bash

# check that something has been mounted
ls /user-environment

# use modules provided in the image
module use /user-environment/modules/
module load osu-micro-benchmarks/7.1-1

# this will work - both devices are on the same node
srun -n2 -N1 ./test-osu.sh
# this will hang
srun -n2 -N2 ./test-osu.sh
```

the `test-osu.sh` script is in this repo.
