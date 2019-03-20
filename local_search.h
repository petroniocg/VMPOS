/* 
 * local_search.h: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * local_search.h: Virtual Machine Placement Problem - Local Search Stage Header
 * Author: Fabio Lopez Pires (flopezpires@gmail.com)
 * Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters
 * Available at: https://github.com/flopezpires
*/

/* include libraries */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

extern float cl_cs;
extern float cl_nc;

extern int *global_h_sizes;

/* function headers definitions */
void local_search(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int CRITICAL_SERVICES);
