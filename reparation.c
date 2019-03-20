/*
 * reparation.c: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * reparation.c: Virtual Machine Placement Problem - Reparation Stage
 * Author: Fabio Lopez Pires (flopezpires@gmail.com)
 * Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters
 * Available at: https://github.com/flopezpires
*/

/* include reparation stage header */
#include "reparation.h"
#include "commitment.h"
#include "common.h"

/* reparation: reparates the population
 * parameter: population matrix
 * parameter: utilization tridimentional matrix
 * parameter: commitment matrix
 * parameter: physical machines matrix
 * parameter: virtual machines matrix
 * parameter: number of individuals
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * returns: reparated population matrix
*/
int **reparation(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int CRITICAL_SERVICES)
{
	/* repairs population from not factible individuals */
	repair_population(population, utilization, cm, H, V, number_of_individuals, h_size, v_size, CRITICAL_SERVICES);
	return population;
}

/* reparation: reparates the population
 * parameter: population matrix
 * parameter: utilization of the physical machines matrix
 * parameter: commitment matrix
 * parameter: physical machines matrix
 * parameter: virtual machines matrix
 * parameter: number of individuals
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * returns: reparated population matrix
*/
void repair_population(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int CRITICAL_SERVICES)
{
	/* iterators */
	int iterator_individual = 0;
	int iterator_virtual = 0;
	int iterator_physical = 0;
	int factibility = 1;

	/* iterate on individuals */
	for (iterator_individual = 0; iterator_individual < number_of_individuals ; iterator_individual++)
	{
		/* every individual is feasible until it's probed other thing */		
		factibility = 1;
		/* constraint: Provision of VMs with Critical Services. Virtual machines with CRITICAL SERVICES have to be placed mandatorily */
		for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
		{
			if (V[iterator_virtual][3] == CRITICAL_SERVICES && population[iterator_individual][iterator_virtual] == 0)
			{
				factibility = 0;
				break;
			}
		}
		/* constraints: Resource capacity of physical machines. Iterate on physical machines */
		for (iterator_physical = 0; iterator_physical < h_size ; iterator_physical++)
		{
			// Checks whether the physical machine is being used
			if (cm[iterator_individual][iterator_physical] > 0.0)
				/* if any physical machine is overloaded on any resource, the individual is not factible */
				if (is_overloaded(H, utilization, cm, iterator_individual, iterator_physical))
				{
					factibility = 0;
					break;
				}
		}

		/* if the individual is not factible */
		if (factibility == 0)
		{
			repair_individual(population, utilization, cm, H, V, number_of_individuals, h_size, v_size, CRITICAL_SERVICES, iterator_individual);
		}
	}
}

/* repair_individual: repairs not factible individuals
 * parameter: population matrix
 * parameter: utilization of the physical machines matrix
 * parameter: commitment matrix
 * parameter: physical machines matrix
 * parameter: virtual machines matrix
 * parameter: number of individuals
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * parameter: identificator of the not factible individual to repair
 * returns: nothing, it's void()
*/
void repair_individual(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int CRITICAL_SERVICES, int individual)
{
	int iterator_virtual = 0;
	int iterator_virtual2 = 0;
	int iterator_virtual_again=0;
	int iterator_physical = 0;	
	/* every individual is not feasible until it's probed other thing */		
	int factibility = 0;
	/* id of a candidate physical machine for migration */
	int candidate = 0;
	/* a migration flag for overloaded physical machines indicating that a virtual machine was or not migrated yet */
	int migration = 0;

	float available_cpus = 0.0, available_memory = 0.0;

	/* iterate on each virtual machine to search for overloaded physical machines */
	for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
	{
		/* if the virtual machine was placed */
		if (population[individual][iterator_virtual] != 0)
		{
			migration = 0;
			/* verify is the physical machine assigned is overloaded in any physical resource */
			if ( is_overloaded(H, utilization, cm, individual, ( population[individual][iterator_virtual] - 1 )) )
			{
				/* we search for a correct candidate for VM "migration" (it is not really a migration, only a physical machine change) */
				candidate = rand() % global_h_sizes[individual];

				for (iterator_physical=0; iterator_physical < global_h_sizes[individual]; iterator_physical++)
				{
					if (cm[individual][candidate] > 1.0)
					{
						available_cpus = (float) H[candidate][0] * cm[individual][candidate];
						available_memory = (float) H[candidate][1] * cm[individual][candidate];
					}
					else
					{
						available_cpus = (float) H[candidate][0];
						available_memory = (float) H[candidate][1];
					}
					if ( (float) utilization[individual][candidate][0] + (float) V[iterator_virtual][0] <= available_cpus &&
	   			   	 	 (float) utilization[individual][candidate][1] + (float) V[iterator_virtual][1] <= available_memory && 
						 (float) utilization[individual][candidate][2] + (float) V[iterator_virtual][2] <= (float) H[candidate][2] )
					{
						/* delete requirements from physical machine migration source */
						utilization[individual][population[individual][iterator_virtual]-1][0] -= V[iterator_virtual][0];
						utilization[individual][population[individual][iterator_virtual]-1][1] -= V[iterator_virtual][1];
						utilization[individual][population[individual][iterator_virtual]-1][2] -= V[iterator_virtual][2];

						/* add requirements from physical machine migration destination */
						utilization[individual][candidate][0] += V[iterator_virtual][0];
						utilization[individual][candidate][1] += V[iterator_virtual][1];
						utilization[individual][candidate][2] += V[iterator_virtual][2];

						/* refresh the population */
						population[individual][iterator_virtual] = candidate + 1;

						// updte the commitment matrix
						update_commitment_line (population, cm, V, individual, h_size, v_size, CRITICAL_SERVICES);

						/* virtual machine correctly "migrated" */
						migration = 1;
						break;
					}

					if (candidate < global_h_sizes[individual] - 1) //** Aqui estava com: candidate < h_size, e ele incrementava o candidato, mas não tem MF igual a h_size, tem que ser menor
					{
						candidate++;
					}
					else
					{
						candidate = 0;
					}
				}
				if (!migration)
				{
					if (V[iterator_virtual][3]!=CRITICAL_SERVICES)
					{
						utilization[individual][population[individual][iterator_virtual]-1][0] -= V[iterator_virtual][0];
						utilization[individual][population[individual][iterator_virtual]-1][1] -= V[iterator_virtual][1];
						utilization[individual][population[individual][iterator_virtual]-1][2] -= V[iterator_virtual][2];
						
						/* refresh the population */
						population[individual][iterator_virtual] = 0;

						// updte the commitment matrix
						update_commitment_line (population, cm, V, individual, h_size, v_size, CRITICAL_SERVICES);

						/* virtual machine correctly "deleted" */
						migration = 1;
					}
				}
				if(!migration)
				{
					// Looking for no critial VMs in the same PM to turn off
					for (iterator_virtual2 = 0; iterator_virtual2 < v_size; iterator_virtual2++)
					{
						if ( population[individual][iterator_virtual2] == population[individual][iterator_virtual] &&
							 V[iterator_virtual2][3] != CRITICAL_SERVICES )
						{
							/* delete requirements from physical machine migration source */
							utilization[individual][population[individual][iterator_virtual]-1][0] -= V[iterator_virtual2][0];
							utilization[individual][population[individual][iterator_virtual]-1][1] -= V[iterator_virtual2][1];
							utilization[individual][population[individual][iterator_virtual]-1][2] -= V[iterator_virtual2][2];
							
							/* refresh the population */
							population[individual][iterator_virtual2] = 0;

							// updte the commitment matrix
							update_commitment_line (population, cm, V, individual, h_size, v_size, CRITICAL_SERVICES);

							/* virtual machine correctly "deleted" */
							migration = 1;

							if ( !is_overloaded(H, utilization, cm, individual, ( population[individual][iterator_virtual] - 1 )) )
								break;
						}
					}		
				}
			}
		}
	}
}


/* is_overloaded: verifies if the PM is overloaded
 * parameter: physical machines matrix
 * parameter: utilization of the physical machines matrix
 * parameter: commitment matrix
 * parameter: the individual to verify
 * parameter: the physical machines identificaion
 * returns: 1 if is overloaded, otherwise returns 0
*/
int is_overloaded(int **H, int ***utilization, float **cm, int individual, int physical)
{
	// If the use of the VM exceeds the capacity of the physical machine returns 1, otherwise returns 0
	if ( ( (float) utilization[individual][physical][0] > (float) H[physical][0] * cm[individual][physical])
	||   ( (float) utilization[individual][physical][1] > (float) H[physical][1] * cm[individual][physical])
	||   ( (float) utilization[individual][physical][2] > (float) H[physical][2] ) )
	{
		return 1;
	}
	return 0;
}
