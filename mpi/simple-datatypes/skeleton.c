#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int rank;
    int array[8][8];
    //TODO: Declare a variable storing the MPI datatype
    MPI_Datatype taip;

    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialize arrays
    if (rank == 0) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                array[i][j] = (i + 1) * 10 + j + 1;
            }
        }
    } else {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                array[i][j] = 0;
            }
        }
    }

    if (rank == 0) {
        printf("Data in rank 0\n");
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                printf("%3d", array[i][j]);
            }
            printf("\n");
        }
    }

    //TODO: Create datatype
    const int size[2] = {8,8};
    const int subsize[2] = {4,4};
    const int offset[2] = {2,2};
    MPI_Type_create_subarray(2, size, subsize, offset, MPI_ORDER_C, MPI_INT, &taip);
    MPI_Type_commit(&taip);

    //TODO: Send data
    if (rank == 0)
    {
        MPI_Send(array, 1, taip, 1, 1, MPI_COMM_WORLD);
        
    } else {
        MPI_Recv(array, 1, taip, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    

    //TODO: Free datatype
    MPI_Type_free(&taip);

    // Print out the result on rank 1
    if (rank == 1) {
        printf("Received data\n");
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                printf("%3d", array[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();

    return 0;
}
