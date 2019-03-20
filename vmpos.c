/* 
 * vmpos.c: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * imavmp.c: Interactive Memetic Algorithm for Virtual Machine Placement (IMAVMP)
 * Author: Fabio Lopez Pires (flopezpires@gmail.com)
 * Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters
 * Available at: https://github.com/flopezpires
*/


/* structure of a pareto element */
struct pareto_element{
	int *solution;
	float *costs;
	struct pareto_element *prev;
	struct pareto_element *next;
};

/* include libraries */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* include own headers */
#include "common.h"
#include "initialization.h"
#include "commitment.h"
#include "reparation.h"
#include "local_search.h"
#include "variation.h"
#include "pareto.h"

#define SELECTION_PERCENT 0.5
#define CRITICAL_SERVICES 1 //Indicates if the VM run Critical Services

#define NUMBER_OF_PARAMETERS_PM 4
#define NUMBER_OF_PARAMETERS_VM 4

// The global variable to read informations from datacenter configuration file.
int total_of_individuals = 0;
int total_of_generations = 0;

float mc_cs; //Migration Cost of a VM with Critical Services
float mc_ncs; //Migration Cost of a VM with No Critical Services

float cl_cs = 0.0; // Commitment Level (CL) for PM hosting at least one VM with Critical Services (CS)
float cl_nc = 0.0; // Commitment Level (CL) for PM hosting only VMs with NO Critical services (NC)

float energy_objective_weight = 0.0;
float dispersion_objective_weight = 0.0;
float migration_objective_weight = 0.0;

//* The global variable to control the evolution of the best solution
int generations_without_improvment = -1;  //When calculate the first best_individual, goes to zero
float global_best_individual_cost = BIG_COST;

int best_generation = 0;

int *global_best_individual;
float *global_best_objective_functions;

int *global_h_sizes;

int *base_solution;
int readed_solution = 0;

//Petronio: Nivel de Dispersao
int number_vms_sc = 0;

int generation;

/* main: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription
 * parameter: path to the datacenter infrastructure file
 * returns: exit state
 */
int main (int argc, char *argv[]) {

    int m, iterator_virtual, iterator_physical;
	int pm_used_in_base_individual = 0;

	FILE *pareto_result;
	FILE *pareto_data;

    /* parameters verification */
	if (argc == 1)
	{
		/* wrong parameters */
		pareto_result = fopen("results/pareto_result","a");

		fprintf(pareto_result,"\n[ERROR] Usage: %s path_to_datacenter_file\n[ERROR] File not found. Check the writing, please.\n\n", argv[0]);

		fclose(pareto_result);
		/* finish him */
		return 1;
	}

    /* good parameters */	
	else
	{
		/* Interactive Memetic Algorithm previous stuff */

		/* number of generation, for iterative reference of generations */
		generation = 0;

		/* get the number of physical machines and virtual machines from the datacenter infrastructure file (argv[1]) */
		int h_size = get_h_size(argv[1]); // ** Num of PMs **
		int v_size = get_v_size(argv[1]); // ** Num of VMs **

		pareto_result = fopen("results/pareto_result","a");
		fprintf(pareto_result,"\nDATACENTER CONFIGURATION:\nNum of PMs: h_size = %d,\nNum of VMs: v_size = %d",h_size,v_size);
		fclose(pareto_result);

		global_best_individual = (int *) malloc ( v_size * sizeof (int) );

		for (iterator_virtual=0; iterator_virtual < v_size; iterator_virtual++)
			global_best_individual[iterator_virtual] = 0;

		global_best_objective_functions = (float *) malloc (3 *sizeof (float *));

		/* load physical machines resources and virtual machines requirements from the datacenter infrastructure file */
		int **H = load_H(h_size, argv[1]);
		int **V = load_V(v_size, argv[1], CRITICAL_SERVICES);

		/* load the configurations of the datacenter from vmpos_config_file */
		load_dc_config();

		/* Looks for a base solution in file */
		if ( (readed_solution = read_base_solution(v_size) ) > 0 )
		{
			printf("\nBase Individual already exist and was readed, with %d VMs.\n", readed_solution);

			for (iterator_physical = 0; iterator_physical < h_size; iterator_physical++)
			{
				for (iterator_virtual=0; iterator_virtual < v_size; iterator_virtual++)
				{
					if (base_solution[iterator_virtual] == iterator_physical + 1)
					{
						pm_used_in_base_individual++;
						break;
					}
				}
			}

			global_h_sizes = (int*) malloc( total_of_individuals * sizeof(int) );

			for (iterator_physical = 0; iterator_physical < total_of_individuals; iterator_physical++)
			{
				global_h_sizes[iterator_physical] = pm_used_in_base_individual;
			}
		}

		pareto_result = fopen("results/pareto_result","a");
		fprintf(pareto_result,"\nVMPOS CONFIGURATION PARAMETERS:\n");
		fprintf(pareto_result,"Number of Individuals: %d\nNumber of Generations: %d\n",total_of_individuals, total_of_generations);
		fprintf(pareto_result,"Energy Weight: %.3f\nDispersion Weight: %.3f\nMigration Weight: %.3f\n", energy_objective_weight, dispersion_objective_weight, migration_objective_weight);
		fprintf(pareto_result,"Commitment Level for PM with VMs with Critical Services: %.1f\nCommitment Level for PM with VMs with NO Critical Services: %.1f\n", cl_cs, cl_nc);
		fprintf(pareto_result,"Costs for VMs migrations: mc_cs = %.2f and mc_ncs = %.2f\n",mc_cs, mc_ncs);
		if ( readed_solution )
			fprintf(pareto_result,"Base Individual with %d VMs allocated in %d PM:\n", readed_solution, pm_used_in_base_individual);
		fclose(pareto_result);

		/* seed for rand() */
		srand((unsigned int) time(NULL));
		/* randon value of 0-1 */
		srand48(time(NULL));

		/* Interactive Memetic Algorithm with Over Subscription starts here */

		/*******************************************/
		/*    Initialize population P_0 	       */
		/*******************************************/
		int **P;

		/* CM: Commitment Matrix. Has the level of commitment for each PM in each individal */
		float **CM;
		
		/* Additional task: load the utilization of physical machines of all individuals/solutions */
		int ***utilization_P;

		if ( !readed_solution )
		{
			// The last individuo (the +1 in first parameter) is the base solution that will be used to compare 
			// with other solution in migration cost objective
			P = initialization(total_of_individuals + 1, h_size, v_size, V, CRITICAL_SERVICES);

			base_solution = generate_base_solution(P, v_size, total_of_individuals);

			for (iterator_physical = 0; iterator_physical < h_size; iterator_physical++)
			{
				for (iterator_virtual=0; iterator_virtual < v_size; iterator_virtual++)
				{
					if (base_solution[iterator_virtual] == iterator_physical + 1)
					{
						pm_used_in_base_individual++;
						break;
					}
				}
			}

			global_h_sizes = (int*) malloc( (total_of_individuals+1) * sizeof(int) );

			for (iterator_physical = 0; iterator_physical < total_of_individuals+1; iterator_physical++)
			{
				global_h_sizes[iterator_physical] = pm_used_in_base_individual;
			}

			/* CM: Commitment Matrix. Has the level of commitment for each PM in each individal */
			CM = calculates_commitment(P, total_of_individuals + 1, h_size, v_size, V, CRITICAL_SERVICES);

			utilization_P = load_utilization(P, H, V, total_of_individuals + 1, h_size, v_size);

			repair_population(P, utilization_P, CM, H, V, total_of_individuals + 1, h_size, v_size, CRITICAL_SERVICES);
		}
		else
		{
			P = initialization(total_of_individuals, pm_used_in_base_individual, v_size, V, CRITICAL_SERVICES);

			/* CM: Commitment Matrix. Has the level of commitment for each PM in each individal */
			CM = calculates_commitment(P, total_of_individuals, h_size, v_size, V, CRITICAL_SERVICES);

			/* Additional task: load the utilization of physical machines of all individuals/solutions */
			utilization_P = load_utilization(P, H, V, total_of_individuals, h_size, v_size);

			/************************************************/
			/*     P0’ = repair infeasible solutions of P_0 */
			/************************************************/
			repair_population(P, utilization_P, CM, H, V, total_of_individuals,  pm_used_in_base_individual, v_size, CRITICAL_SERVICES);
		}

		/******************************************************/
		/*     P0’’ = apply local search to solutions of P_0’ */
		/******************************************************/
		local_search(P, utilization_P, CM, H, V, total_of_individuals, h_size, v_size, CRITICAL_SERVICES);
		
		/* Additional task: calculate the cost of each objective function for each solution */
		float **objectives_functions_P = load_objectives(P, utilization_P, CM, H, V, total_of_individuals, h_size, v_size, base_solution, CRITICAL_SERVICES);

		/* Additional task: calculate the non-dominated fronts according to NSGA-II */
		int *fronts_P = non_dominated_sorting(objectives_functions_P, total_of_individuals);

		/**********************************************************/
		/*    Update set of nondominated solutions Pc from P_0’’  */
		/**********************************************************/
		struct pareto_element *pareto_head = NULL;
		int iterator_individual;

		/* considering that the P_c is empty at first population, each non-dominated solution from first front is added */
		for (iterator_individual = 0 ; iterator_individual < total_of_individuals ; iterator_individual++)
		{
		 	if(fronts_P[iterator_individual] == 1)
		 		pareto_head = (struct pareto_element *) pareto_insert(pareto_head,v_size,P[iterator_individual],objectives_functions_P[iterator_individual]);
		}
		
		report_best_population(pareto_head, H, V, v_size, h_size);
		
		/* Additional task: identificators for the crossover parents */
		int father, mother;

		/* Additional task: structures for Q and PQ */
		int **Q;
		int ***utilization_Q;
		float **objectives_functions_Q;
		int *fronts_Q;

		/* CM_Q: Commitment Matrix of Q. Has the level of commitment for each PM in each individal */
		float **CM_Q;

		/* While (stopping criterion is not met), do */
		while (generation < total_of_generations )
		{
			/* this is a new generation! */
			generation++;

		 	/* Additional task: Q is a random generated population, lets initialize it */
			Q = initialization(total_of_individuals, pm_used_in_base_individual, v_size, V, CRITICAL_SERVICES);

			CM_Q = calculates_commitment(Q, total_of_individuals, h_size, v_size, V, CRITICAL_SERVICES);

			/* Q_t = selection of solutions from P_t ∪ P_c */
			father = selection(fronts_P, total_of_individuals, SELECTION_PERCENT);
			mother = selection(fronts_P, total_of_individuals, SELECTION_PERCENT);

			while (father == mother)
			{
				mother = selection(fronts_P, total_of_individuals, SELECTION_PERCENT);
			}

			/* Q_t’ = crossover of solutions of Q_t */
			crossover(Q, father, mother, v_size);

			/* Q_t’ = mutation of solutions of Q_t */
			mutation(Q,V,total_of_individuals,h_size,v_size);

			/* Additional task: load the utilization of physical machines of all individuals/solutions */
			utilization_Q = load_utilization(Q, H, V, total_of_individuals, h_size, v_size);

			CM_Q = calculates_commitment(Q, total_of_individuals, h_size, v_size, V, CRITICAL_SERVICES);

			/* Q_t’’ = repair infeasible solutions of Q_t’ */
			Q = reparation(Q, utilization_Q, CM_Q, H, V, total_of_individuals, pm_used_in_base_individual, v_size, CRITICAL_SERVICES);

			/* Q_t’’’ = apply local search to solutions of Q_t’’ */
			local_search(Q, utilization_Q, CM_Q, H, V, total_of_individuals, h_size, v_size, CRITICAL_SERVICES);

			/* Additional task: calculate the cost of each objective function for each solution */
			objectives_functions_Q = load_objectives(Q, utilization_Q, CM_Q, H, V, total_of_individuals, h_size, v_size, base_solution, CRITICAL_SERVICES);

			/* Additional task: calculate the non-dominated fronts according to NSGA-II */
			fronts_Q = non_dominated_sorting(objectives_functions_Q,total_of_individuals);

			/* Update set of nondominated solutions Pc from Qt’’’ */
			for (iterator_individual = 0 ; iterator_individual < total_of_individuals ; iterator_individual++)
		    {
			 	if(fronts_Q[iterator_individual] == 1)
			 		pareto_head = (struct pareto_element *) pareto_insert(pareto_head,v_size,Q[iterator_individual],objectives_functions_Q[iterator_individual]);
			}
			
			/* Pt = fitness selection from Pt ∪ Qt’’’ */
			population_evolution(P, Q, objectives_functions_P, objectives_functions_Q, fronts_P, total_of_individuals, v_size);

			report_best_population(pareto_head, H, V, v_size, h_size);
		}

		pareto_result = fopen("results/pareto_result","a");
		fprintf(pareto_result,"\nTotal of Generations to Generate: %d\nActual Generation: %d\nWithout Enhance the Best: %d\n\n", total_of_generations, generation, generations_without_improvment);
		fclose(pareto_result);

		free (global_best_individual);
		free (global_best_objective_functions);
		free (global_h_sizes);
		free_pareto_front (pareto_head);

		/* finish him */
		return 0;
	}
}
