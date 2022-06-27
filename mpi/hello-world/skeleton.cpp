#include <cstdio>
#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char *argv[])
{
    // TODO: say hello! in parallel
    MPI_Init(&argc,&argv);
    auto comm = MPI_COMM_WORLD;
    int rank;
    MPI_Comm_rank(comm,&rank);
    /*char procname[MPI_MAX_PROCESSOR_NAME];
    int procname_len;
    MPI_Get_processor_name(procname,&procname_len);*/
    cout << "Hi from rank " << rank << "!\n";
    MPI_Finalize();
}
