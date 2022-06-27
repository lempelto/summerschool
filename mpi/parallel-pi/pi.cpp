#include <cstdio>
#include <cmath>
#include <mpi.h>

constexpr int n = 840;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int rank;
    int nproc;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    if (rank == 0){
        printf("Computing approximation to pi with N=%d\n", n);
    }
    
    // Set 'istart' and 'istop' according to rank
    int chunksize = n / nproc;
    int istart = rank * chunksize + 1;
    int istop = (rank + 1) * chunksize;

    if (istop > n) { istop = n; }

    double pi_partial = 0.0;
    for (int i=istart; i <= istop; i++) {
        double x = (i - 0.5) / n;
        pi_partial += 1.0 / (1.0 + x*x);
    }

    
    if (rank != 0) {
        MPI_Send(&pi_partial, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    } else if (rank == 0) {
        double pi = pi_partial; // rank 0 initialises pi to its own value

        // Gather 'pi_partials' into 'pi' as a sum
        for (int i = 1; i < nproc; i++)
        {
            double ip;
            MPI_Recv(&ip, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            pi += ip;
        }
        
        pi *= 4.0 / n;
        printf("Approximate pi=%18.16f (exact pi=%10.8f)\n", pi, M_PI);
    }
}
   

