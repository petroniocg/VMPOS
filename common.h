/* 
 * common.h: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * common.h: Virtual Machine Placement Problem - Common Functions Header
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

/* definitions */
#define H_HEADER "PHYSICAL MACHINES"
#define V_HEADER "VIRTUAL MACHINES"
#define TAM_BUFFER BUFSIZ
#define CONSTANT 10000
#define BIG_COST 1000000

extern float mc_cs;
extern float mc_ncs;

extern int number_vms_sc;
extern int *global_best_individual;
extern int *base_solution;
extern int *global_h_sizes;

extern int total_of_individuals;
extern int total_of_generations;
extern float cl_cs;
extern float cl_nc;
extern float energy_objective_weight;
extern float dispersion_objective_weight;
extern float migration_objective_weight;

void load_dc_config();
int read_base_solution(int lenght);

/* get the number of physical and virtual machines */
int get_h_size(char path_to_file[]);
int get_v_size(char path_to_file[]);

/* print matrix and array functions */
void print_int_matrix(int ** matrix, int rows, int columns);
void print_float_matrix(float ** matrix, int rows, int columns);
void print_int_array(int * array, int columns);
void print_int_array_line(int *array, int columns);
void print_float_array(float * array, int columns);

/* load datacenter physical resources and virtual requirements */
int** load_H(int h_size, char path_to_file[]);
int** load_V(int v_size, char path_to_file[], int CRITICAL_SERVICES);

/* load utilization of physical resources and costs of the considered objective functions */
int*** load_utilization(int **population, int **H, int **V, int number_of_individuals, int h_size, int v_size);
float** load_objectives(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int* base_solution, int CRITICAL_SERVICES);
float migration_cost (int **vm, int *b_solution, int *individ, int v_size, int CRITICAL_SERVICES);
int* generate_base_solution(int **population, int v_size, int number_of_individuals);
void update_pm_used_in_individual (int individual, int h_size, int v_size);
