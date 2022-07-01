#pragma once
#include <string>
#include "matrix.hpp"
#include <mpi.h>

// Class for basic parallelization information
struct ParallelData {
    int size;            // Number of MPI tasks
    int rank;
    int coords[2];
    int dimensions[2] = {0,0};
    int nup, ndown, nleft, nright;      // Ranks of neighbouring MPI tasks
    MPI_Comm communicator;

    ParallelData() {      // Constructor

      // TODO start: query number of MPI tasks and store it in
      // the size attribute of the class
      MPI_Comm_size(MPI_COMM_WORLD, &size);

      // Create cartesian communicator
      int ndims = 2;
      int periodt[ndims] = {0,0};
      MPI_Dims_create(size, ndims, dimensions);
      MPI_Cart_create(MPI_COMM_WORLD, ndims, dimensions, periodt, 0, &communicator);

      // Query MPI rank of this task and store it in the rank attribute
      // Determine also up and down neighbours of this domain and store
      // them in nup and ndown attributes, remember to cope with
      // boundary domains appropriatly
      MPI_Comm_rank(communicator, &rank);
      MPI_Cart_coords(communicator, rank, 2, coords);

      MPI_Cart_shift(communicator, 1, 1, &nup, &ndown);
      MPI_Cart_shift(communicator, 0, 1, &nleft, &nright);

      // TODO end

    };

};

// Class for temperature field
struct Field {
    // nx and ny are the true dimensions of the field. The temperature matrix
    // contains also ghost layers, so it will have dimensions nx+2 x ny+2 
    int nx;                     // Local dimensions of the field
    int ny;
    int nx_full;                // Global dimensions of the field
    int ny_full;                // Global dimensions of the field
    double dx = 0.01;           // Grid spacing
    double dy = 0.01;

    Matrix<double> temperature;

    void setup(int nx_in, int ny_in, ParallelData parallel);

    void generate(ParallelData parallel);

    // standard (i,j) syntax for setting elements
    double& operator()(int i, int j) {return temperature(i, j);}

    // standard (i,j) syntax for getting elements
    const double& operator()(int i, int j) const {return temperature(i, j);}

};

// Function declarations
void initialize(int argc, char *argv[], Field& current,
                Field& previous, int& nsteps, ParallelData parallel);

void exchange(Field& field, const ParallelData parallel);

void evolve(Field& curr, const Field& prev, const double a, const double dt);

void write_field(const Field& field, const int iter, const ParallelData parallel);

void read_field(Field& field, std::string filename,
                const ParallelData parallel);

double average(const Field& field, const ParallelData parallel);
