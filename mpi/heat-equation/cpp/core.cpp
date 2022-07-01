// Main solver routines for heat equation solver

#include <mpi.h>

#include "heat.hpp"

// Exchange the boundary values
void exchange(Field& field, const ParallelData parallel)
{
    double* sbuf = field.temperature.data();
    
    // Create data type for matrix column (and row, maybe)
    MPI_Datatype matcol, matrow;
    MPI_Type_contiguous(field.ny, MPI_DOUBLE, &matcol);
    MPI_Type_vector(field.nx, 1, field.ny+2, MPI_DOUBLE, &matrow);
    MPI_Type_commit(&matcol);
    MPI_Type_commit(&matrow);
    
    
    // Find relative addresses of column/row starting points
    MPI_Aint origin, setop, sbtm, sleft, sright, rtop, rbtm, rleft, rright;

    MPI_Get_address(field.temperature.data(), &origin);

    MPI_Get_address(field.temperature.data(1,1), &setop);
    MPI_Get_address(field.temperature.data(1,field.ny), &sbtm);
    MPI_Get_address(field.temperature.data(1,1), &sleft);
    MPI_Get_address(field.temperature.data(field.nx,1), &sright);

    MPI_Get_address(field.temperature.data(1,0), &rtop);
    MPI_Get_address(field.temperature.data(1,field.ny+1), &rbtm);
    MPI_Get_address(field.temperature.data(0,1), &rleft);
    MPI_Get_address(field.temperature.data(field.nx+1,1), &rright);

    // Get relative addresses
    setop -= origin;    sbtm -= origin;    sleft -= origin;    sright -= origin;
    rtop -= origin;     rbtm -= origin;    rleft -= origin;    rright -= origin;
    
    // Send all to all using the Cartesian (hopefully) communicator of 'parallel'
    int scounts[4] = {1, 1, 1, 1};
    MPI_Aint sdispls[4] = {sleft, sright, setop, sbtm};
    MPI_Datatype stypes[4] = {matcol, matcol, matrow, matrow};
    int rcounts[4] = {1, 1, 1, 1};
    //MPI_Aint rdispls[4] = {0, sbtm, sleft, sright};
    MPI_Aint rdispls[4] = {rleft, rright, rtop, rbtm};
    MPI_Datatype rtypes[4] = {matcol, matcol, matrow, matrow};

    MPI_Neighbor_alltoallw(sbuf, scounts, sdispls, stypes, sbuf, rcounts, rdispls, rtypes, parallel.communicator);
        
    MPI_Type_free(&matcol);
    MPI_Type_free(&matrow);

    // TODO end
}

// Update the temperature values using five-point stencil */
void evolve(Field& curr, const Field& prev, const double a, const double dt)
{

  // Compilers do not necessarily optimize division to multiplication, so make it explicit
  auto inv_dx2 = 1.0 / (prev.dx * prev.dx);
  auto inv_dy2 = 1.0 / (prev.dy * prev.dy);

  // Determine the temperature field at next time step
  // As we have fixed boundary conditions, the outermost gridpoints
  // are not updated.
  for (int i = 1; i < curr.nx + 1; i++) {
    for (int j = 1; j < curr.ny + 1; j++) {
            curr(i, j) = prev(i, j) + a * dt * (
	       ( prev(i + 1, j) - 2.0 * prev(i, j) + prev(i - 1, j) ) * inv_dx2 +
	       ( prev(i, j + 1) - 2.0 * prev(i, j) + prev(i, j - 1) ) * inv_dy2
               );
    }
  }

}
