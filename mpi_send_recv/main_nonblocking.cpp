#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <cuda.h>
#include <cuda_runtime.h>

void check_mpi_status(int status) {
    if (status!=MPI_SUCCESS) {
        fprintf(stderr, "error: MPI was unhappy about something or other\n");
        fflush(stderr);
        exit(1);
    }
}

void check_cuda_status(cudaError_t status) {
    if(status != cudaSuccess) {
        fprintf(stderr, "error: CUDA API call : %s\n", cudaGetErrorString(status));
        fflush(stderr);
        exit(1);
    }
}

// Pearson hash to generate input data that is interesting enough to verify contents of buffers.
const uint8_t T[] = {175, 165, 241, 29, 112, 78, 157, 15, 92, 131, 212, 243, 152, 169, 168, 224, 162, 194, 218, 155, 48, 61, 56, 187, 46, 150, 164, 233, 156, 9, 242, 176, 251, 174, 254, 172, 54, 102, 173, 20, 8, 93, 57, 142, 149, 35, 116, 12, 10, 223, 77, 26, 146, 139, 103, 236, 192, 216, 74, 113, 167, 129, 14, 1, 228, 23, 68, 245, 188, 19, 81, 45, 141, 40, 71, 86, 248, 67, 145, 140, 105, 210, 126, 123, 41, 76, 24, 36, 222, 201, 43, 197, 249, 121, 135, 32, 133, 160, 22, 3, 94, 72, 90, 111, 255, 253, 153, 91, 138, 62, 51, 39, 27, 247, 154, 30, 84, 250, 118, 44, 7, 237, 59, 130, 80, 178, 190, 193, 166, 108, 219, 60, 75, 235, 88, 52, 69, 184, 246, 106, 64, 124, 170, 100, 28, 114, 189, 230, 214, 115, 182, 209, 234, 66, 37, 143, 122, 232, 186, 252, 58, 227, 213, 82, 211, 180, 134, 238, 240, 181, 0, 191, 239, 177, 18, 110, 185, 206, 33, 207, 107, 21, 204, 85, 183, 97, 17, 5, 208, 53, 49, 127, 117, 198, 63, 161, 79, 125, 158, 195, 99, 163, 89, 226, 132, 215, 148, 98, 128, 203, 229, 16, 55, 4, 50, 104, 179, 220, 231, 221, 120, 31, 196, 171, 244, 199, 25, 159, 11, 13, 34, 136, 47, 144, 42, 137, 225, 6, 95, 205, 151, 200, 38, 73, 147, 2, 83, 217, 70, 87, 119, 202, 96, 65, 101, 109};

uint8_t hash(uint8_t h, int i) {
    return T[h ^ (i%256)];
}

void print_usage_and_exit(int rank) {
    if (!rank) {
        fprintf(stderr, "usage: ./a.out N SRC DST\n"
                        "      N is size of message in bytes\n"
                        "      SRC is send buffer location (H or D)\n"
                        "      DST is receive buffer location (H or D)\n"
                        "Requires exactly 2 MPI ranks\n");
        fflush(stderr);
    }
    exit(1);
}

int main(int argc, char** argv) {
    int rank, size;

    check_mpi_status(MPI_Init(&argc, &argv));
    check_mpi_status(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    check_mpi_status(MPI_Comm_size(MPI_COMM_WORLD, &size));

    if (size!=2) print_usage_and_exit(rank);

    // command line arguments
    if (argc!=4) print_usage_and_exit(rank);
    const int N = atoi(argv[1]);
    if (N<1) print_usage_and_exit(rank);
    if (strcmp(argv[2], "D") && strcmp(argv[2], "H")) print_usage_and_exit(rank);
    if (strcmp(argv[3], "D") && strcmp(argv[3], "H")) print_usage_and_exit(rank);
    const char snd_loc = *argv[2];
    const char rcv_loc = *argv[3];

    if (!rank) {
        printf("Test sending %d bytes from %s to %s\n", N, (snd_loc=='H'? "host": "device"), (rcv_loc=='H'? "host": "device"));
        fflush(stdout);
    }

    const int tag = 42;
    uint8_t* hbuffer = (uint8_t*)malloc(N);
    uint8_t* dbuffer;
    check_cuda_status(cudaMalloc(&dbuffer, N));
    printf("rank %d -- host / device buffers: %p / %p\n", rank, (void*)hbuffer, (void*)dbuffer);
    fflush(stdout);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank==0) {
        uint8_t h = 0;
        for (int i=0; i<N; ++i) {
            hbuffer[i] = h = hash(h, i);
        }
        MPI_Request request;
        if (snd_loc=='H') {
            check_mpi_status(
                MPI_Isend(hbuffer, N, MPI_BYTE, 1, tag, MPI_COMM_WORLD, &request));
        }
        else {
            check_cuda_status(
                cudaMemcpy(dbuffer, hbuffer, N, cudaMemcpyHostToDevice));
            check_mpi_status(
                MPI_Isend(dbuffer, N, MPI_BYTE, 1, tag, MPI_COMM_WORLD, &request));
            MPI_Wait(&request, MPI_STATUSES_IGNORE);
        }
    } else {
        for (int i = 0; i < N; ++i)
            hbuffer[i] = 0;
        MPI_Request request;
        MPI_Status status_;
        if (rcv_loc == 'H') {
            check_mpi_status(MPI_Irecv(hbuffer, N, MPI_BYTE, 0, tag,
                                       MPI_COMM_WORLD, &request));
        } else {
            check_mpi_status(MPI_Irecv(dbuffer, N, MPI_BYTE, 0, tag,
                                       MPI_COMM_WORLD, &request));
            MPI_Wait(&request, MPI_STATUSES_IGNORE);
            check_cuda_status(
                cudaMemcpy(hbuffer, dbuffer, N, cudaMemcpyDeviceToHost));
        }
    }

    // check results
    if (rank) {
        uint8_t h = 0;
        for (int i=0; i<N; ++i) {
            h = hash(h, i);
            if (hbuffer[i]!=h) {
                fprintf(stderr, "error: invalid value at location %d in receive buffer\n", i);
                fflush(stderr);
                exit(1);
            }
        }
        printf("SUCCESS\n");
    }

    free(hbuffer);
    check_cuda_status(cudaFree(dbuffer));

    MPI_Finalize();
    return 0;
}
