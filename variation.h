/*
 * variation.h: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * variation.h: Virtual Machine Placement Problem - Genetic Operators Functions Header
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
int *non_dominated_sorting(float ** solutions, int number_of_individuals);
int is_dominated(float ** solutions, int a, int b);
int selection(int *fronts, int number_of_individuals, float percent);
void crossover(int **population, int position_parent1, int position_parent2, int v_size);
void mutation(int **population, int **V, int number_of_individuals, int h_size, int v_size);
void population_evolution(int **P, int **Q, float **objectives_functions_P, float **objectives_functions_Q, int *fronts_P, int number_of_individuals, int v_size);
long double pareto_size();
void update_pareto_set(int **P, float **objectives_functions_P, int *fronts_P, int number_of_individuals, int v_size);
