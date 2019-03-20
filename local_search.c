/* 
 * local_search.c: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * local_search.c: Virtual Machine Placement Problem - Local Search Stage
 * Author: Fabio Lopez Pires (flopezpires@gmail.com)
 * Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters
 * Available at: https://github.com/flopezpires
*/


/* include local search stage header */
#include "local_search.h"
#include "commitment.h"
#include "common.h"

/* local_search: local optimization of the population
 * parameter: population matrix
 * parameter: utilization matrix of the physical machines of all the individuals
 * parameter: commitment matrix
 * parameter: physical machines matrix
 * parameter: virtual machines matrix
 * parameter: number of individuals
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * returns: optimized population matrix
*/
void local_search(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int CRITICAL_SERVICES)
{
	/* iterators */
	int iterator_individual = 0;
	int physical_position = 0;
	int physical_position2 = 0;
	int iterator_position = 0;
	int iterator_virtual = 0;
	int iterator_virtual2 = 0;
	int iterator_physical = 0;

	float available_cpus = 0.0, available_memory = 0.0;
	int allocated_vm, physical_turned_off = 0;

	/*  try to place the virtual machines turned off in the physical machines turned on */

	/* iterate on individuals */
	for (iterator_individual = 0 ; iterator_individual < number_of_individuals ; iterator_individual++)
	{
		/* iterate on virtual machines */
		for (iterator_virtual = 0 ; iterator_virtual < v_size ; iterator_virtual++)
		{

			/* get the position of the physical machine */
			physical_position = population[iterator_individual][iterator_virtual];

			if (physical_position == 0)
			{
				allocated_vm = 0;
				
				/* iterate on virtual machines */
				for (iterator_virtual2 = 0 ; iterator_virtual2 < v_size ; iterator_virtual2++)
				{
					/* get the position of the physical machine */
					physical_position2 = population[iterator_individual][iterator_virtual2];

					if (physical_position2 > 0)
					{
						if (cm[iterator_individual][physical_position2-1] > 1.0)
						{
							available_cpus = (float) H[physical_position2-1][0] * cm[iterator_individual][physical_position2-1];
							available_memory = (float) H[physical_position2-1][1] * cm[iterator_individual][physical_position2-1];
						}
						else
						{
							available_cpus = (float) H[physical_position2-1][0];
							available_memory = (float) H[physical_position2-1][1];
						}
						
						/* If the use of the VM not exceeds the capacity of the physical machine performs the migration */
						if ( (float) utilization[iterator_individual][physical_position2-1][0] + (float) V[iterator_virtual][0] <= available_cpus && 
							 (float) utilization[iterator_individual][physical_position2-1][1] + (float) V[iterator_virtual][1] <= available_memory && 
							 (float) utilization[iterator_individual][physical_position2-1][2] + (float) V[iterator_virtual][2] <= (float) H[physical_position2-1][2])
						{
							/* refresh the utilization */
							utilization[iterator_individual][physical_position2-1][0] += V[iterator_virtual][0];
							utilization[iterator_individual][physical_position2-1][1] += V[iterator_virtual][1];
							utilization[iterator_individual][physical_position2-1][2] += V[iterator_virtual][2];
							/* refresh the population */
							population[iterator_individual][iterator_virtual] = population[iterator_individual][iterator_virtual2];

							/* updte the commitment matrix */
							update_commitment_line (population, cm, V, iterator_individual, h_size, v_size, CRITICAL_SERVICES);

							/* The vm was aloccated */
							allocated_vm = 1;
							break;
						}
					}
				}

				/* The turned off VM was not alocatted. Try to find Physical Machine turned off */
				if (!allocated_vm)
				{
					if ( global_h_sizes[iterator_individual] < h_size )
						global_h_sizes[iterator_individual]++;

					for (physical_turned_off = 0; physical_turned_off < global_h_sizes[iterator_individual]; physical_turned_off++)
					{
						if (cm[iterator_individual][physical_turned_off] == 0.0)
						{
							/* refresh the utilization */
							utilization[iterator_individual][physical_turned_off][0] += V[iterator_virtual][0];
							utilization[iterator_individual][physical_turned_off][1] += V[iterator_virtual][1];
							utilization[iterator_individual][physical_turned_off][2] += V[iterator_virtual][2];
							/* refresh the population */
							population[iterator_individual][iterator_virtual] = physical_turned_off + 1;

							/* updte the commitment matrix */
							update_commitment_line (population, cm, V, iterator_individual, h_size, v_size, CRITICAL_SERVICES);
							break;
						}
					}
				}
			}
		}
	}

	/* comes of shut down physical machines to the migrate in other virtual machines */

	/* Concentrating VMs WITH NO Critical Services */
	/* iterate on individuals */
	for (iterator_individual = 0 ; iterator_individual < number_of_individuals ; iterator_individual++)
	{
		/* iterate on virtual machines */
		for (iterator_virtual = 0 ; iterator_virtual < v_size ; iterator_virtual++)
		{
			if (V[iterator_virtual][3] != CRITICAL_SERVICES)
			{
				/* get the position of the physical machine */
				physical_position = population[iterator_individual][iterator_virtual];

				if (physical_position > 0)
				{
					if (cm[iterator_individual][physical_position-1] > 1.0)
					{
						available_cpus = (float) H[physical_position-1][0] * cm[iterator_individual][physical_position-1];
						available_memory = (float) H[physical_position-1][1] * cm[iterator_individual][physical_position-1];
					}
					else
					{
						available_cpus = (float) H[physical_position-1][0];
						available_memory = (float) H[physical_position-1][1];
					}

					/* iterate on virtual machines */
					for (iterator_virtual2 = 0 ; iterator_virtual2 < v_size ; iterator_virtual2++)
					{
						if (V[iterator_virtual2][3] != CRITICAL_SERVICES)
						{
							/* get the position of the physical machine */
							physical_position2 = population[iterator_individual][iterator_virtual2];

							if (physical_position != physical_position2 && physical_position2 > 0)
							{
								/* If the use of the VM not exceeds the capacity of the physical machine performs the migration */
								if ((float) utilization[iterator_individual][physical_position-1][0] + (float) V[iterator_virtual2][0] <= available_cpus && 
									(float) utilization[iterator_individual][physical_position-1][1] + (float) V[iterator_virtual2][1] <= available_memory && 
									(float) utilization[iterator_individual][physical_position-1][2] + (float) V[iterator_virtual2][2] <= (float) H[physical_position-1][2])
								{
									/* refresh the utilization */
									utilization[iterator_individual][physical_position2-1][0] -= V[iterator_virtual2][0];
									utilization[iterator_individual][physical_position2-1][1] -= V[iterator_virtual2][1];
									utilization[iterator_individual][physical_position2-1][2] -= V[iterator_virtual2][2];

									utilization[iterator_individual][physical_position-1][0] += V[iterator_virtual2][0];
									utilization[iterator_individual][physical_position-1][1] += V[iterator_virtual2][1];
									utilization[iterator_individual][physical_position-1][2] += V[iterator_virtual2][2];
									/* refresh the population */
									population[iterator_individual][iterator_virtual2] = population[iterator_individual][iterator_virtual];

									/* updte the commitment matrix */
									update_commitment_line (population, cm, V, iterator_individual, h_size, v_size, CRITICAL_SERVICES);
								}
							}
						}
					}
				}
			}
		}
	}

	/* Concentrating VMs WITH Critical Services */
	/* iterate on individuals */
	for (iterator_individual = 0 ; iterator_individual < number_of_individuals ; iterator_individual++)
	{
		/* iterate on virtual machines */
		for (iterator_virtual = 0 ; iterator_virtual < v_size ; iterator_virtual++)
		{
			if (V[iterator_virtual][3] == CRITICAL_SERVICES)
			{
				/* get the position of the physical machine */
				physical_position = population[iterator_individual][iterator_virtual];

				if (physical_position > 0)
				{
					available_cpus = (float) H[physical_position-1][0];
					available_memory = (float) H[physical_position-1][1];

					/* iterate on virtual machines */
					for (iterator_virtual2 = 0 ; iterator_virtual2 < v_size ; iterator_virtual2++)
					{
						if (V[iterator_virtual2][3] == CRITICAL_SERVICES)
						{
							/* get the position of the physical machine */
							physical_position2 = population[iterator_individual][iterator_virtual2];

							if (physical_position != physical_position2)
							{
								/* If the use of the VM not exceeds the capacity of the physical machine performs the migration */
								if ( (float) utilization[iterator_individual][physical_position-1][0] + (float) V[iterator_virtual2][0] <= available_cpus && 
									 (float) utilization[iterator_individual][physical_position-1][1] + (float) V[iterator_virtual2][1] <= available_memory && 
									 (float) utilization[iterator_individual][physical_position-1][2] + (float) V[iterator_virtual2][2] <= (float) H[physical_position-1][2])
								{
									/* refresh the utilization */
									utilization[iterator_individual][physical_position2-1][0] -= V[iterator_virtual2][0];
									utilization[iterator_individual][physical_position2-1][1] -= V[iterator_virtual2][1];
									utilization[iterator_individual][physical_position2-1][2] -= V[iterator_virtual2][2];

									utilization[iterator_individual][physical_position-1][0] += V[iterator_virtual2][0];
									utilization[iterator_individual][physical_position-1][1] += V[iterator_virtual2][1];
									utilization[iterator_individual][physical_position-1][2] += V[iterator_virtual2][2];
									/* refresh the population */
									population[iterator_individual][iterator_virtual2] = population[iterator_individual][iterator_virtual];

									/* updte the commitment matrix */
									update_commitment_line (population, cm, V, iterator_individual, h_size, v_size, CRITICAL_SERVICES);
								}
							}
						}
					}
				}
			}
		}
	}

	int m;

	/* Now, we try to migrate VMs from PMs with just one or few VMs to another PM to shutdown one more PM */

	//* iterate on individuals
	for (iterator_individual = 0 ; iterator_individual < number_of_individuals ; iterator_individual++)
	{
		for (physical_position = 0; physical_position < global_h_sizes[iterator_individual]; physical_position++)
		{
			allocated_vm = 0;

			/* Verifying if the utilization is less than 10% and bigger than zero */
			if ( (float) utilization[iterator_individual][physical_position][0] <= ((float) H[physical_position][0] / 10.0) && ( utilization[iterator_individual][physical_position][0] > 0 ) )
			{
				for (iterator_virtual = 0 ; iterator_virtual < v_size ; iterator_virtual++)
				{
					if ( (V[iterator_virtual][3] == CRITICAL_SERVICES) && (population[iterator_individual][iterator_virtual]-1 == physical_position) )
					{
						for (physical_position2 = 0 ; physical_position2 < global_h_sizes[iterator_individual] ; physical_position2++)
						{
							available_cpus = (float) H[physical_position2][0];
							available_memory = (float) H[physical_position2][1];
							
							if (physical_position2 != physical_position && cm[iterator_individual][physical_position2] == cl_cs )
							{
								/* If the use of the VM not exceeds the capacity of the physical machine performs the migration */
								if ( (float) utilization[iterator_individual][physical_position2][0] + (float) V[iterator_virtual][0] <= available_cpus && 
									 (float) utilization[iterator_individual][physical_position2][1] + (float) V[iterator_virtual][1] <= available_memory && 
									 (float) utilization[iterator_individual][physical_position2][2] + (float) V[iterator_virtual][2] <= (float) H[physical_position2][2])
								{
									/* refresh the utilization */
									utilization[iterator_individual][physical_position][0] -= V[iterator_virtual][0];
									utilization[iterator_individual][physical_position][1] -= V[iterator_virtual][1];
									utilization[iterator_individual][physical_position][2] -= V[iterator_virtual][2];

									utilization[iterator_individual][physical_position2][0] += V[iterator_virtual][0];
									utilization[iterator_individual][physical_position2][1] += V[iterator_virtual][1];
									utilization[iterator_individual][physical_position2][2] += V[iterator_virtual][2];
									
									/* refresh the population */
									population[iterator_individual][iterator_virtual] = physical_position2+1;

									/* updte the commitment matrix */
									update_commitment_line (population, cm, V, iterator_individual, h_size, v_size, CRITICAL_SERVICES);

									allocated_vm = 1;
									break;
								}
							}
						}
					}
					else
					{
						if ( (V[iterator_virtual][3] != CRITICAL_SERVICES) && (population[iterator_individual][iterator_virtual]-1 == physical_position) )
						{
							for (physical_position2 = 0 ; physical_position2 < global_h_sizes[iterator_individual] ; physical_position2++)
							{	
								if (physical_position2 != physical_position && cm[iterator_individual][physical_position2] == cl_nc )
								{
									available_cpus = (float) H[physical_position2][0] * cm[iterator_individual][physical_position2];
									available_memory = (float) H[physical_position2][1] * cm[iterator_individual][physical_position2];

									/* If the use of the VM not exceeds the capacity of the physical machine performs the migration */
									if ( (float) utilization[iterator_individual][physical_position2][0] + (float) V[iterator_virtual][0] <= available_cpus && 
										 (float) utilization[iterator_individual][physical_position2][1] + (float) V[iterator_virtual][1] <= available_memory && 
										 (float) utilization[iterator_individual][physical_position2][2] + (float) V[iterator_virtual][2] <= (float) H[physical_position2][2])
									{
										/* refresh the utilization */
										utilization[iterator_individual][physical_position][0] -= V[iterator_virtual][0];
										utilization[iterator_individual][physical_position][1] -= V[iterator_virtual][1];
										utilization[iterator_individual][physical_position][2] -= V[iterator_virtual][2];

										utilization[iterator_individual][physical_position2][0] += V[iterator_virtual][0];
										utilization[iterator_individual][physical_position2][1] += V[iterator_virtual][1];
										utilization[iterator_individual][physical_position2][2] += V[iterator_virtual][2];
										
										/* refresh the population */
										population[iterator_individual][iterator_virtual] = physical_position2+1;

										/* updte the commitment matrix */
										update_commitment_line (population, cm, V, iterator_individual, h_size, v_size, CRITICAL_SERVICES);

										allocated_vm = 1;
										break;
									}
								}
							}
						}
					}
				}
			}
			/* if was allocated, stay in the same PM to migrate all VMs */
			if (allocated_vm)
				physical_position--;
		}
	}
}
