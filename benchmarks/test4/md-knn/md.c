/*
Implemenataion based on:
A. Danalis, G. Marin, C. McCurdy, J. S. Meredith, P. C. Roth, K. Spafford, V. Tipparaju, and J. S. Vetter.
The scalable heterogeneous computing (shoc) benchmark suite.
In Proceedings of the 3rd Workshop on General-Purpose Computation on Graphics Processing Units, 2010.
*/

#include <stdlib.h>
#include <stdio.h>

#define TYPE int

// Problem Constants
#define nAtoms        32
#define maxNeighbors  32
// LJ coefficients
#define lj1           1//1.5
#define lj2           2//2.0

TYPE force_x[nAtoms];
TYPE force_y[nAtoms];
TYPE force_z[nAtoms];
TYPE position_x[nAtoms];
TYPE position_y[nAtoms];
TYPE position_z[nAtoms];
int NL[nAtoms * maxNeighbors];

void md_kernel()
{
    TYPE delx, dely, delz, r2inv;
    TYPE r6inv, potential, force, j_x, j_y, j_z;
    TYPE i_x, i_y, i_z, fx, fy, fz;

    int i, j, jidx;

loop_i:
    for (i = 0; i < nAtoms; i++)
    {
        i_x = position_x[i];
        i_y = position_y[i];
        i_z = position_z[i];
        fx = 0;
        fy = 0;
        fz = 0;
    loop_j:
        for (j = 0; j < maxNeighbors; j++)
        {
            please_map_me();
            // Get neighbor
            jidx = NL[i * maxNeighbors + j];
            // Look up x,y,z positions
            j_x = position_x[jidx];
            j_y = position_y[jidx];
            j_z = position_z[jidx];
            // Calc distance
            delx = i_x - j_x;
            dely = i_y - j_y;
            delz = i_z - j_z;
            r2inv = 10 / (delx * delx + dely * dely + delz * delz); // r2inv = 1.0/( delx*delx + dely*dely + delz*delz );
            // Assume no cutoff and aways account for all nodes in area
            r6inv = r2inv * r2inv * r2inv;
            potential = r6inv * (lj1 * r6inv - lj2 * 1000);
            // Sum changes in force
            force = r2inv * potential;
            /*potential = r6inv/100000000*(lj1*r6inv - lj2*1000000000);
            // Sum changes in force
            force = r2inv*potential/10000000000;*/
            fx += delx * force;
            fy += dely * force;
            fz += delz * force;
        }
        // Update forces after all neighbors accounted for.
        force_x[i] = fx;
        force_y[i] = fy;
        force_z[i] = fz;
        // printf("dF=%lf,%lf,%lf\n", fx, fy, fz);
    }
}
