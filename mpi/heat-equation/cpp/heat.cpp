#include "heat.hpp"
#include "matrix.hpp"
#include <iostream>
#include <mpi.h>

void Field::setup(int nx_in, int ny_in, ParallelData parallel) 
{
    nx_full = nx_in;
    ny_full = ny_in;

    ny = ny_full / parallel.dimensions[1];
    nx = nx_full / parallel.dimensions[0];
    if (nx * parallel.dimensions[1] * ny * parallel.dimensions[0] != nx_full * ny_full) {
        std::cout << "Cannot divide grid evenly to processors" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, -2);
    }

   // matrix includes also ghost layers
   temperature = Matrix<double> (nx + 2, ny + 2);
}

void Field::generate(ParallelData parallel) {

    // Radius of the source disc 
    auto radius = nx_full / 6.0;
    for (int i = 0; i < nx + 2; i++) {
        for (int j = 0; j < ny + 2; j++) {
            // Distance of point i, j from the origin 
            auto dx = i + parallel.coords[0] * nx - nx_full / 2 + 1;
            auto dy = j + parallel.coords[1] * ny - nx_full / 2 + 1;
            if (dx * dx + dy * dy < radius * radius) {
                temperature(i, j) = 5.0;
            } else {
                temperature(i, j) = 65.0;
            }
        }
    }

    for (int j = 0; j < ny; j++) {
        if (parallel.coords[0] == 0) {
            temperature(0, j+1) = 20.0; //    left
        } else if (parallel.coords[0] == parallel.dimensions[0]-1) {
            temperature(nx+1, j+1) = 70.0; // right
        }
    }
    
    if (parallel.coords[1] == 0) {
        for (int i = 0; i < nx; i++) {
            temperature(i+1, 0) = 85.0; //    top
        }
    } else if (parallel.coords[1] == parallel.dimensions[1]-1) {
        for (int i = 0; i < nx; i++) {
            temperature(i+1, ny+1) = 5.0; //  bottom
        }
    }
}
