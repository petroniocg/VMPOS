/* 
 * pareto.c: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * pareto.c: Virtual Machine Placement Problem - Pareto Functions
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

/* include headers */
#include "pareto.h"
#include "common.h"
#include "variation.h"

/* structure of an element */
struct pareto_element
{
	int *solution;
	float *costs;
	struct pareto_element *prev;
	struct pareto_element *next;
};

struct pareto_element * pareto_create(int v_size, int *individual, float *objectives_functions)
{
	struct pareto_element *pareto_current = (struct pareto_element *) malloc (sizeof(struct pareto_element));
	pareto_current->solution = (int *) malloc (v_size *sizeof(int));
	pareto_current->costs = (float *) malloc (3 *sizeof(float));
	pareto_current->solution = individual;
	pareto_current->costs = objectives_functions;
	pareto_current->prev = NULL;
	pareto_current->next = NULL;
	return pareto_current;
}

struct pareto_element * pareto_insert(struct pareto_element *pareto_head, int v_size, int *individual, float *objectives_functions)
{
	struct pareto_element *pareto_current = pareto_create(v_size, individual, objectives_functions);
	struct pareto_element *ptr1;
	ptr1 = pareto_head;

	int iterator_virtual;
	int duplicate = 1;

	if (pareto_head == NULL)
	{
	  pareto_head = pareto_current;
		return pareto_head;
	}
	else
	{	
		duplicate = 1;
		/* is it duplicate? */
		while(ptr1 != NULL)
		{
			for (iterator_virtual = 0 ; iterator_virtual < v_size ; iterator_virtual++)
			{
		   	if(ptr1->solution[iterator_virtual] != pareto_current->solution[iterator_virtual])
		   	{
					duplicate = 0;
					break;
				}
			}
			if(duplicate == 1)
			{
				return pareto_head;
			}
			duplicate = 1;
			ptr1 = ptr1->next;
		}
		/* is it NOT duplicate */
		pareto_head->prev = pareto_current;
		pareto_current->next = pareto_head;
		pareto_head = pareto_current;
		return pareto_head;
	}
}

/* reference: http://www.geeksforgeeks.org/remove-duplicates-from-an-unsorted-linked-list/ */
void pareto_remove_duplicates(struct pareto_element *pareto_head, int v_size)
{
	struct pareto_element *ptr1, *ptr2, *dup;
	ptr1 = pareto_head;
	int iterator_virtual;
	int duplicate = 1;
	/* Pick elements one by one */
	while(ptr1 != NULL && ptr1->next != NULL)
	{
		ptr2 = ptr1;
		/* Compare the picked element with rest of the elements */
    while(ptr2->next != NULL)
    {
			for (iterator_virtual = 0 ; iterator_virtual < v_size ; iterator_virtual++)
			{
				if(ptr1->solution[iterator_virtual] != ptr2->next->solution[iterator_virtual])
				{
					duplicate = 0;
					ptr2 = ptr2->next;
					break;
				}
			}
			if(duplicate == 1)
			{
				dup = ptr2->next;
				ptr2->next = ptr2->next->next;
				free(dup);
			}
			duplicate = 1;
		}
		ptr1 = ptr1->next;
	}
}

/* get_min_cost: calculate the min value of an objective function in a population
 * parameter: array of the values of the objective functions
 * parameter: objective functions id for evaluation
 * parameter: number of individuals
 * parameter: non-dominated sorting fronts
 * returns: min cost of the identified objetive function
*/
float get_min_cost(struct pareto_element *pareto_head, int objective)
{
	float min_cost = BIG_COST;
	struct pareto_element *ptr1;
	ptr1 = pareto_head;
	/* Pick elements one by one */
	while(ptr1 != NULL)
	{
		if(ptr1->costs[objective] < min_cost)
		{
			min_cost = ptr1->costs[objective];
		}
		ptr1 = ptr1->next;
	}
	return min_cost;
}

/* get_max_cost: calculate the max value of an objective function in a population
 * parameter: array of the values of the objective functions
 * parameter: objective functions id for evaluation
 * parameter: number of individuals
 * parameter: non-dominated sorting fronts
 * returns: max cost of the identified objetive function
*/
float get_max_cost(struct pareto_element *pareto_head, int objective)
{
	float max_cost = -1.0;
	struct pareto_element *ptr1;
	ptr1 = pareto_head;
	/* Pick elements one by one */
	while(ptr1 != NULL)
	{
		if(ptr1->costs[objective] > max_cost)
		{
			max_cost = ptr1->costs[objective];
		}
		ptr1 = ptr1->next;
	}
	return max_cost;
}

void print_pareto_set(struct pareto_element *pareto_head, int v_size)
{
	int iterator_virtual;
	while (pareto_head != NULL)
	{
		/* iterate on columns */
		for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
		{
			printf("%d\t",pareto_head->solution[iterator_virtual]);	
		}
		printf("\n");
		pareto_head = pareto_head->next;
	}
}

void print_pareto_front(struct pareto_element *pareto_head, int v_size)
{
	while (pareto_head != NULL)
	{
		print_int_array_line (pareto_head->solution, v_size);

		printf("E: %g, D: %g, M: %g\n",pareto_head->costs[0],pareto_head->costs[1],pareto_head->costs[2]);	
		printf("\n");
		pareto_head = pareto_head->next;
	}
}

int load_pareto_size(struct pareto_element *pareto_head)
{
	struct pareto_element *ptr1 = pareto_head;
	int size = 0;
	while(ptr1 != NULL)
	{
		size++;
		ptr1 = ptr1->next;
	}
	return size;
}

void report_best_population(struct pareto_element *pareto_head, int **H, int **V, int v_size, int h_size)
{
	int pareto_size = load_pareto_size(pareto_head);
	int **best_P = (int **) malloc (pareto_size *sizeof (int *));
	float **objective_functions_best_P = (float **) malloc (pareto_size *sizeof (float *));

	int iterator_individual;
	int iterator_virtual;
	int iterator_physical;

	int best_indice = 0;

	float max_energy_cost = 0.0;
	float max_dispersion_cost = 0.0;
	float max_migration_cost = 0.0;
	float best_positioning = 0.0;
	float actual_positioning_value = 0.0;

	struct pareto_element *ptr2 = pareto_head;

	FILE *pareto_result;
	FILE *pareto_data;

	/* copying pareto individuals */	
	for (iterator_individual=0; iterator_individual < pareto_size; iterator_individual++)
	{
		best_P[iterator_individual] = (int *) malloc (v_size *sizeof (int));
		objective_functions_best_P[iterator_individual] = (float *) malloc (3 *sizeof (float));
		best_P[iterator_individual] = pareto_head->solution;
		objective_functions_best_P[iterator_individual] = pareto_head->costs;
		pareto_head = pareto_head->next;
	}

	/* once the best population is on a matrix, non-dominated sorting is performed */
	int *fronts_best_P = (int *) non_dominated_sorting(objective_functions_best_P, pareto_size);
	int ***utilization_best_P = load_utilization(best_P, H, V, pareto_size, h_size, v_size);


	/* save the Pareto set and the Pareto front  */
	/* for each individual */
	for (iterator_individual=0; iterator_individual < pareto_size; iterator_individual++)
	{
		//* only the first pareto front
		if(fronts_best_P[iterator_individual]==1)
		{
			if (max_energy_cost < objective_functions_best_P[iterator_individual][0])
				max_energy_cost = objective_functions_best_P[iterator_individual][0];
			if (max_dispersion_cost < objective_functions_best_P[iterator_individual][1])
				max_dispersion_cost = objective_functions_best_P[iterator_individual][1];
			if (max_migration_cost < objective_functions_best_P[iterator_individual][2])
				max_migration_cost = objective_functions_best_P[iterator_individual][2];
		}
	}

	/* Initialization to the first individal to compare with others */
	if(max_migration_cost != 0)
		best_positioning = energy_objective_weight*(objective_functions_best_P[0][0]/max_energy_cost) + dispersion_objective_weight*(objective_functions_best_P[0][1]/max_dispersion_cost) + 
										migration_objective_weight*(objective_functions_best_P[0][2]/max_migration_cost);
	else
		best_positioning = energy_objective_weight*(objective_functions_best_P[0][0]/max_energy_cost) + dispersion_objective_weight*(objective_functions_best_P[0][1]/max_dispersion_cost) + 
										migration_objective_weight*(objective_functions_best_P[0][2]);

	for (iterator_individual=0; iterator_individual < pareto_size; iterator_individual++)
	{
		//* only the first pareto front
		if(fronts_best_P[iterator_individual]==1)
		{
			if(max_migration_cost != 0)			
				actual_positioning_value = energy_objective_weight*(objective_functions_best_P[iterator_individual][0]/max_energy_cost) + 
											dispersion_objective_weight*(objective_functions_best_P[iterator_individual][1]/max_dispersion_cost) + 
											migration_objective_weight*(objective_functions_best_P[iterator_individual][2]/max_migration_cost);
			else
				actual_positioning_value = energy_objective_weight*(objective_functions_best_P[iterator_individual][0]/max_energy_cost) + 
											dispersion_objective_weight*(objective_functions_best_P[iterator_individual][1]/max_dispersion_cost) + 
											migration_objective_weight*(objective_functions_best_P[iterator_individual][2]);

			if (best_positioning > actual_positioning_value )
			{
				best_positioning = actual_positioning_value;
				best_indice = iterator_individual;
			}
		}
	}

	if ( best_positioning < global_best_individual_cost && is_different (best_P[best_indice ], global_best_individual, v_size))
	{
		pareto_result = fopen("results/pareto_result","a");

		fprintf(pareto_result,"\n *** GENERATION = %d ***", generation);

		best_generation = generation;
		global_best_individual_cost = best_positioning;
		generations_without_improvment = 0;

		fprintf(pareto_result,"\n=========================================================\n");
		fprintf(pareto_result,"\t\tTHE BEST POSITIONING IS:\n");

		// Copy the best individual found to global_best_individual
		for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
		{
			global_best_individual[iterator_virtual] = best_P[best_indice][iterator_virtual];
			fprintf(pareto_result,"%d ", global_best_individual[iterator_virtual]);
		}

		global_best_objective_functions[0] = objective_functions_best_P[best_indice][0];
		global_best_objective_functions[1] = objective_functions_best_P[best_indice][1];
		global_best_objective_functions[2] = objective_functions_best_P[best_indice][2];

		update_pm_used_in_individual (best_indice, h_size, v_size);

		fprintf(pareto_result,"\n");
		fprintf(pareto_result,"\nCOSTS:\tEnergy Cons.: %g\tDispersion Level.: %g\tMigr.: %g\t", global_best_objective_functions[0], global_best_objective_functions[1],global_best_objective_functions[2]);
		fprintf(pareto_result,"Allocated in [%d] PM.\n", global_h_sizes[best_indice]);
		fprintf(pareto_result,"=========================================================\n\n");
		
		fclose(pareto_result);
	}
	else
	{
		generations_without_improvment++;
	}
	
	if (generation == total_of_generations)
	{
		pareto_data = fopen("results/pareto_result_data","a");
		fprintf(pareto_data,"%g\t%g\t%g\t%f\t%d\t", global_best_objective_functions[0], global_best_objective_functions[1],global_best_objective_functions[2], global_best_individual_cost, best_generation);
		fclose(pareto_data);
	}
}


int is_different (int *array1, int *array2, int columns)
{
	int i;

	for (i = 0; i < columns; i++)
	{
		if (array1[i] != array2[i])
			return 1;	// The individuals are differents
	}
	return 0;	// The individuals have the same distribution
}




void free_pareto_front(struct pareto_element *pareto_head)
{
	struct pareto_element *ptr1, *ptr2;
	ptr1 = pareto_head;

	int iterator;
	int pareto_size = load_pareto_size(pareto_head);

	for (iterator = 0 ; iterator < pareto_size ; iterator++)
	{
		ptr2 = ptr1->next;
		free( ptr1 -> solution );
		free( ptr1 -> costs);
		ptr1 = ptr2;
	}
	free(pareto_head);
}
