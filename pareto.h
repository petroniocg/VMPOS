/* 
 * pareto.h: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * pareto.h: Virtual Machine Placement Problem - Pareto Functions Header
 * Author: Fabio Lopez Pires (flopezpires@gmail.com)
 * Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters
 * Available at: https://github.com/flopezpires
*/


//* The global variable to control the evolution of the best solution
extern int generations_without_improvment;
extern float global_best_individual_cost;
extern int *global_best_individual;
extern float *global_best_objective_functions;
extern int generation;
extern int best_generation;

extern float energy_objective_weight;
extern float dispersion_objective_weight;
extern float migration_objective_weight;

/* include functions */
struct pareto_element * pareto_create(int v_size, int *individual, float *objectives_functions);
struct pareto_element * pareto_insert(struct pareto_element *pareto_head, int v_size, int *individual, float *objectives_functions);
void pareto_remove_duplicates(struct pareto_element *pareto_head, int v_size);
float get_min_cost(struct pareto_element *pareto_head, int objective);
float get_max_cost(struct pareto_element *pareto_head, int objective);
void print_pareto_set(struct pareto_element *pareto_head, int v_size);
void print_pareto_front(struct pareto_element *pareto_head, int v_size);
int load_pareto_size(struct pareto_element *pareto_head);
void report_best_population(struct pareto_element *pareto_head, int **H, int **V, int v_size, int h_size);
int is_different (int *array1, int *array2, int columns);
void free_pareto_front(struct pareto_element *pareto_head);
