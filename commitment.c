/* 
 * commitment.c: Virtual Machine Placement With Over Subscription - Initialization of The Commitment Matrix
 * Date: 27-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 */

/* include libraries */
#include <stdio.h>
#include <stdlib.h>

/* include commitment header */
#include "commitment.h"

/* calculates_commitment: calculates the Commitment Matrix
 * parameter: population matrix
 * parameter: number of individuals
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * parameter: virtual machines requirements matrix
 * returns: commitment matrix
*/
float **calculates_commitment(int **population, int number_of_individuals, int h_size, int v_size, int **V, int CRITICAL_SERVICES)
{
	// iterators 
	int iterator_individual;
	int iterator_virtual;
	int iterator_physical;

    //* commitment: initial Commitment Matrix 
	float **commitment = (float **) malloc (number_of_individuals *sizeof (float *));

 	// iterate on individuals
	for (iterator_individual=0; iterator_individual < number_of_individuals; iterator_individual++)
	{
		commitment[iterator_individual] = (float *) malloc (h_size *sizeof (float));

		for (iterator_physical=0; iterator_physical < h_size; iterator_physical++)
		{
			//* Commitment Level of PMs initialized to 0.0
			commitment[iterator_individual][iterator_physical] = 0.0;
		}
		
		// iterate on positions of an individual
		for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
		{
			// VM with no CRITICAL_SERVICES
			if (V[iterator_virtual][3] != CRITICAL_SERVICES)
			{
				if (population[iterator_individual][iterator_virtual] != 0)
				{
					// Only change the Commitment Level if the PM was not set with Critical Services
					if (commitment[iterator_individual][(population[iterator_individual][iterator_virtual])-1] != cl_cs)
						commitment[iterator_individual][(population[iterator_individual][iterator_virtual])-1] = cl_nc;
				}
			}
			// VM with CRITICAL_SERVICES
			else
			{
				if (population[iterator_individual][iterator_virtual] != 0)
					commitment[iterator_individual][(population[iterator_individual][iterator_virtual])-1] = cl_cs;
			}
		}
	}

	return commitment;
}


/* update_commitment_line: updates one line of the Commitment Matrix
 * parameter: population matrix
 * parameter: commitment matrix
 * parameter: virtual machines requirements matrix
 * parameter: the individual overloaded
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * returns: nothing, it's void
*/
void update_commitment_line (int **population, float **cm, int **V, int individual_overloaded, int h_size, int v_size, int CRITICAL_SERVICES)
{
	int iterator_virtual;
	int iterator_physical;
	
	for (iterator_physical=0; iterator_physical < h_size; iterator_physical++)
	{
		//* Commitment Level of PMs initialized to 0 
		cm[individual_overloaded][iterator_physical] = 0.0;
	}
	
	for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
	{
		// VM with no CRITICAL_SERVICES
		if (V[iterator_virtual][3] != CRITICAL_SERVICES)
		{
			if (population[individual_overloaded][iterator_virtual] != 0)
			{
				// Only change the Commitment Level if the PM was not set with Critical Services
				if (cm[individual_overloaded][(population[individual_overloaded][iterator_virtual])-1] != cl_cs)
					cm[individual_overloaded][(population[individual_overloaded][iterator_virtual])-1] = cl_nc;
			}
		}
		// VM with CRITICAL_SERVICES
		else
		{
			if (population[individual_overloaded][iterator_virtual] != 0) // Vai ser gerado valor diferente de zero. Pensar se vai ser em todas as situações
				cm[individual_overloaded][(population[individual_overloaded][iterator_virtual])-1] = cl_cs;
		}
	}
}


/* calculates_commitment_line: verify if one VM can migrate to a PM
 * parameter: one individual
 * parameter: the VM to be updated
 * parameter: the PM to be updated
 * parameter: commitment matrix
 * parameter: PM that will receive the VM
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * parameter: virtual machines requirements matrix
 * returns: 1 if can migrate the VM or 0 if can't
*/
int calculates_commitment_line(int *actual_population_line, int vm_to_update, int h_to_update, float **actual_cm, int individual_adjustment, int h_size, int v_size, int **V, int CRITICAL_SERVICES)
{
	int iterator_virtual;
	int iterator_physical;

	int *population_line_after = (int *) malloc (v_size *sizeof (int));
	float *cm_line_after = (float *) malloc (h_size *sizeof (float));


	//* Commitment Level of PMs initialized to 0 
	for (iterator_physical=0; iterator_physical < h_size; iterator_physical++)
		cm_line_after[iterator_physical] = 0.0;
	
	for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
	{
		if (iterator_virtual != vm_to_update)
			population_line_after[iterator_virtual] = actual_population_line[iterator_virtual];
		else
			population_line_after[iterator_virtual] = h_to_update;
	}

	for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
	{
		// VM with no CRITICAL_SERVICES
		if (V[iterator_virtual][3] != CRITICAL_SERVICES)
		{
			if (population_line_after[iterator_virtual] != 0)
			{
				// Only change the Commitment Level if the PM was not set with Critical Services
				if (cm_line_after[(population_line_after[iterator_virtual])-1] != cl_cs)
					cm_line_after[population_line_after[iterator_virtual]-1] = cl_nc;
			}
		}
		// VM with CRITICAL_SERVICES
		else
		{
			if (population_line_after[iterator_virtual] != 0)
				cm_line_after[ (population_line_after[iterator_virtual]) - 1 ] = cl_cs;
		}
	}

	/* 	If the future commitment calculation found to the PM that will receive the VM is bigger or equal to the current,
		that means can migrate (returns 1), if not, means that the commitment that will be applied will be less than the
		current, so you are trying to migrate a critical VM to a PM with only no critical VMs (returns 0).
	*/

	if (cm_line_after[h_to_update-1] >= actual_cm[individual_adjustment][h_to_update-1])
		return 1;
	else
		return 0;
}
