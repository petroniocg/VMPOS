/*
 * reparation.h: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * reparation.h: Virtual Machine Placement Problem - Reparation Stage Header
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

extern int *global_h_sizes;

/* function headers definitions */
int **reparation(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int CS);
void repair_population(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int CS);
void repair_individual(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int CS, int individual);
int is_overloaded(int **H, int ***utilization, float **cm, int individual, int physical);
