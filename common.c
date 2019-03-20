/* 
 * common.c: Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)
 * Date: 18-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based on imavmp.c developed by Fabio Lopez Pires, as follows:
 * common.c: Virtual Machine Placement Problem - Common Functions
 * Author: Fabio Lopez Pires (flopezpires@gmail.com)
 * Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters
 * Available at: https://github.com/flopezpires
*/

	
/* include common header */
#include "common.h"

/* get_h_size: returns the number of physical machines
 * parameter: path to the datacenter file
 * returns: number of physical machines
*/
int get_h_size(char path_to_file[])
{
	/* datacenter file to read from */
	FILE *datacenter_file;
	/* line readed from file */
	char input_line[TAM_BUFFER];
	/* number of physical machines */
	int h_size = 0;
	/* 1 if reading the physical machines block in the file */
	int reading_physical = 0;
 	/* open the file for reading */ 
	datacenter_file = fopen(path_to_file,"r");
    /* if it is opened ok, we continue */    
	if (datacenter_file != NULL) 
	{ 
		/* read until the end */
		while(!feof(datacenter_file)) 
		{
			/* get line per line */
			fgets(input_line, TAM_BUFFER, datacenter_file);
			/* if the line is equal to H_HEADER, we begin the physical machines block in the file */
			if (strstr(input_line,H_HEADER) != NULL)
			{
				reading_physical = 1;
			}
			/* if the line is equal to V_HEADER, we end the physical machines block in the file */
			if (strstr(input_line,V_HEADER) != NULL) 
			{
				reading_physical = 0;
				break;
			}
			/* if it is the correct block in the file, it is not the header and it is not a blank line, we count */			
			if (reading_physical == 1 && strstr(input_line,H_HEADER) == NULL && strcmp(input_line, "\n") != 0)
			{
				h_size++;
			}
		}
	}
	/* close the file */
	fclose(datacenter_file);
	/* return the value */
	return h_size;
}

/* get_v_size: returns the number of virtual machines
 * parameter: path to the datacenter file
 * returns: number of virtual machines
*/
int get_v_size(char path_to_file[])
{
	/* datacenter file to read from */
	FILE *datacenter_file;
	/* line readed from file */
	char input_line[TAM_BUFFER];
	/* number of virtual machines */
	int v_size = 0;
	/* 1 if reading the virtual machines block in the file */
	int reading_virtual = 0;
 	/* open the file for reading */
	datacenter_file = fopen(path_to_file,"r");
    /* if it is opened ok, we continue */
	if (datacenter_file != NULL) 
	{ 
		/* read until the end */
		while(!feof(datacenter_file)) 
		{
			/* get line per line */
			fgets(input_line, TAM_BUFFER, datacenter_file);
			/* if the line is equal to V_HEADER, we begin the virtual machines block in the file */
			if (strstr(input_line,V_HEADER) != NULL)
			{
				reading_virtual = 1;
			}

			/* if we are in the correct block in the file, it is not the header and it is not a blank line, we count */
			if (reading_virtual == 1 && strstr(input_line,V_HEADER) == NULL && strcmp(input_line, "\n") != 0)
			{
				v_size++;
			}
		}
	}
	/* close the file */
	fclose(datacenter_file);
	/* return the value */
	return v_size;
}

/* print_int_matrix: prints on screen a integer matrix
 * parameter: matrix to print
 * parameter: number of individuals
 * parameter: number of virtual machines
 * returns: nothing, it's void
*/
void print_int_matrix(int **matrix, int rows, int columns)
{
	/* iterators */
	int iterator_row;
	int iterator_column;
	/* iterate on rows */	
	for (iterator_row=0; iterator_row < rows; iterator_row++)
	{
		printf("ROW %d:\t",iterator_row);	
		/* iterate on columns */
		for (iterator_column = 0; iterator_column < columns; iterator_column++)
		{
			printf("%d ",matrix[iterator_row][iterator_column]);	
		}
		printf("\n");
	}
}

/* print_float_matrix: prints on screen a float matrix
 * parameter: matrix to print
 * parameter: number of individuals
 * parameter: number of virtual machines
 * returns: nothing, it's void
*/
void print_float_matrix(float **matrix, int rows, int columns)
{
	/* iterators */
	int iterator_row;
	int iterator_column;
	/* iterate on rows */	
	for (iterator_row=0; iterator_row < rows; iterator_row++)
	{
		printf("ROW %d:\t",iterator_row);	
		/* iterate on columns */
		for (iterator_column = 0; iterator_column < columns; iterator_column++)
		{
			printf("%g\t",matrix[iterator_row][iterator_column]);
		}
		printf("\n");
	}
}

/* print_int_array: prints on screen a int array
 * parameter: array to print
 * parameter: number of virtual machines
 * returns: nothing, it's void
*/
void print_int_array(int *array, int columns)
{
	/* iterators */
	int iterator_column;
	/* iterate on columns */
	for (iterator_column = 0; iterator_column < columns; iterator_column++)
	{
		printf("[%d]: %d\t",iterator_column,array[iterator_column]);	
	}
	printf("\n");
}


void print_int_array_line(int *array, int columns)
{
	/* iterators */
	int iterator_column;
	/* iterate on columns */
	for (iterator_column = 0; iterator_column < columns; iterator_column++)
	{
		printf("%d ", array[iterator_column]);	
	}
	printf("\n");
}


/* print_float_array: prints on screen a float array
 * parameter: array to print
 * parameter: number of columns
 * returns: nothing, it's void
*/
void print_float_array(float *array, int columns)
{
	/* iterators */
	int iterator_column;
	/* iterate on columns */
	for (iterator_column = 0; iterator_column < columns; iterator_column++)
	{
		printf("[DEBUG] [%d]: %g\n",iterator_column,array[iterator_column]);	
	}
}

/* load_H: load the values of H
 * parameter: number of physical machines
 * parameter: path to the datacenter file
 * returns: H matrix
*/
int** load_H(int h_size, char path_to_file[])
{
	/* datacenter file to read from */
	FILE *datacenter_file;
	/* line readed from file */
	char input_line[TAM_BUFFER];
	/* iterator */
	int iterator = 0;
	/* 1 if is reading the physical machines block in the file */
	int reading_physical = 0;
	/* memory reserve for h_size physical machines */
	int **H = (int **) malloc (h_size *sizeof (int *));
 	/* open the file for reading */ 
	datacenter_file = fopen(path_to_file,"r");
    /* if it is opened ok, we continue */    
	if (datacenter_file != NULL) 
	{ 
		/* read until the end */
		while(!feof(datacenter_file)) 
		{
			/* get line per line */
			fgets(input_line, TAM_BUFFER, datacenter_file);
			/* if the line is equal to H_HEADER, we begin the physical machines block in the file */
			if (strstr(input_line,H_HEADER) != NULL)
			{
				reading_physical = 1;
			}
			/* if the line is equal to V_HEADER, we end the physical machines block in the file */
			if (strstr(input_line,V_HEADER) != NULL) 
			{
				reading_physical = 0;
				break;
			}
			/* if it's the correct block in the file, it is not the header and it is not a blank line, we count */			
			if (reading_physical == 1 && strstr(input_line,H_HEADER) == NULL && strcmp(input_line, "\n") != 0)
			{
				/* reserve 4 columns for Processor, Memory, Storage and Power Consumption */
				H[iterator] = (int *) malloc (4 *sizeof (int));
				/* load on the matrix and increment iterator */
				sscanf(input_line,"%d %d %d %d\n",&H[iterator][0],&H[iterator][1],&H[iterator][2],&H[iterator][3]);
				iterator++;
			}
		}
	}
	fclose(datacenter_file);
	return H;
}

/* load_V: load the values of V
 * parameter: number of virtual machines
 * parameter: path to the datacenter file
 * returns: V matrix
*/
int** load_V(int v_size, char path_to_file[], int CRITICAL_SERVICES)
{
	/* datacenter file to read from */
	FILE *datacenter_file;
	FILE *pareto_result;
	/* line readed from file */
	char input_line[TAM_BUFFER];
	/* iterator */
	int iterator = 0;
	/* 1 if is reading the virtual machines block in the file */
	int reading_virtual = 0;
	/* memory reserve for v_size virtual machines */
	int **V = (int **) malloc (v_size *sizeof (int *));
 	/* open the file for reading */ 
	datacenter_file = fopen(path_to_file,"r");
    /* if it is opened ok, we continue */    
	if (datacenter_file != NULL) 
	{ 
		/* read until the end */
		while(!feof(datacenter_file)) 
		{
			/* get line per line */
			fgets(input_line, TAM_BUFFER, datacenter_file);
			/* if the line is equal to V_HEADER, we begin the virtual machines block in the file */
			if (strstr(input_line,V_HEADER) != NULL)
			{
				reading_virtual = 1;
			}

			/* if it's the correct block in the file, it is not the header and it is not a blank line, we count */			
			if (reading_virtual == 1 && strstr(input_line,V_HEADER) == NULL && strcmp(input_line, "\n") != 0)
			{
				/* reserve 4 columns for Processor, Memory, Storage and Flag for Critical Service */
				V[iterator] = (int *) malloc (4 *sizeof (int));
				sscanf(input_line,"%d %d %d %d\n",&V[iterator][0],&V[iterator][1],&V[iterator][2],&V[iterator][3]);

				//Petronio: Nivel de Dispersao
				if (V[iterator][3] == CRITICAL_SERVICES)
					number_vms_sc++;

				iterator++;
			}
		}
	}
	fclose(datacenter_file);

	pareto_result = fopen("results/pareto_result","a");
	fprintf(pareto_result,"\nNumber of CRITICAL VMs: %d (%.1f%%)\n\n", number_vms_sc, ((float)number_vms_sc/(float)v_size)*100.0);
	fclose(pareto_result);

	return V;
}


/* load_dc_config: load the values of DC configuration
 * parameter: path to the datacenter file
 * returns: nothing, it's void
*/
void load_dc_config()
{
	/* datacenter file to read from */
	FILE *datacenter_cfg_file;
	/* line readed from file */
	char input_line[TAM_BUFFER];

	/* 1 if is reading the physical machines block in the file */
	int reading_population = 0;
	int reading_objectives = 0;
	int reading_commitments = 0;
	int reading_vms_config = 0;

 	/* open the file for reading */ 
	datacenter_cfg_file = fopen("vmpos_config.vmp","r");

    /* if it is opened ok, we continue */    
	if (datacenter_cfg_file != NULL)
	{ 
		/* read until the end */
		while(!feof(datacenter_cfg_file))
		{
			/* get line per line */
			fgets(input_line, TAM_BUFFER, datacenter_cfg_file);

			/* if the line is equal to POPULATION, we begin the population configuration block */
			if (strstr(input_line,"POPULATION") != NULL)
			{
				reading_population = 1;
			}

			/* if the line is equal to OBJECTIVES WEIGHT, we end the POPULATION block and we begin the objectives configuration block */
			if (strstr(input_line,"OBJECTIVES WEIGHT") != NULL) 
			{
				reading_population = 0;
				reading_objectives = 1;
				reading_commitments = 0;
				reading_vms_config = 0;
			}
			/* if the line is equal to COMMITMENT LEVEL, we end the OBJECTIVES WEIGHT block and we begin the commitment configuration block */
			if (strstr(input_line,"COMMITMENT LEVEL") != NULL) 
			{
				reading_population = 0;
				reading_objectives = 0;
				reading_commitments = 1;
				reading_vms_config = 0;
			}
			/* if the line is equal to MIGRATION COSTS, we end the COMMITMENT LEVEL block and we begin the VMs costs of migration configuration block */
			if (strstr(input_line,"MIGRATION COSTS") != NULL) 
			{
				reading_population = 0;
				reading_objectives = 0;
				reading_commitments = 0;
				reading_vms_config = 1;
			}

			/* if it's the correct block in the file, it is not the header and it is not a blank line, we read */			
			if (reading_population == 1 && strstr(input_line,"POPULATION") == NULL && strcmp(input_line, "\n") != 0)
			{
				/* load on the population and increment iterator */
				sscanf(input_line,"%d %d",&total_of_individuals, &total_of_generations);
			}

			if (reading_objectives == 1 && strstr(input_line,"OBJECTIVES WEIGHT") == NULL && strcmp(input_line, "\n") != 0)
			{
				/* load objectives values */
				sscanf(input_line,"%f %f %f",&energy_objective_weight, &dispersion_objective_weight, &migration_objective_weight);
			}

			if (reading_commitments == 1 && strstr(input_line,"COMMITMENT LEVEL") == NULL && strcmp(input_line, "\n") != 0)
			{
				/* load commitment parameters values */
				sscanf(input_line,"%f %f", &cl_cs, &cl_nc);
			}
			if (reading_vms_config == 1 && strstr(input_line,"MIGRATION COSTS") == NULL && strcmp(input_line, "\n") != 0)
			{
				/* load VMs costs parameters values */
				sscanf(input_line,"%f %f", &mc_cs, &mc_ncs);
				break;
			}
		}
	}

	fclose(datacenter_cfg_file);
}


int read_base_solution(int lenght)
{
	int i = 0;

	/* Base Individual file to read from */
	FILE *base_file;

 	/* open the file for reading */ 
	base_file = fopen("vmpos_base_individual.vmp","r");

	base_solution = (int*) malloc( lenght * sizeof(int) );
	
	if (base_file == NULL)
	{
		printf("\n[WARNING]: Could not open the file with the base individual. Or, there is no such file.\n");
		return(0);
	}

	while( ( fscanf(base_file,"%d", &base_solution[i]) ) != EOF )
		if (base_solution[i] > 0)
			i++;

	return i;
}



/* load_utilization: loads the utilization of the physical machines of all the individuals
 * parameter: population matrix
 * parameter: physical machines matrix
 * parameter: virtual machines matrix
 * parameter: number of individuals
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * returns: utilization tridimentional matrix
*/
int ***load_utilization(int **population, int **H, int **V, int number_of_individuals, int h_size, int v_size)
{
	/* iterators */
	int iterator_individual;
	int iterator_virtual;
	int iterator_physical;
	/* utilization holds the physical machines utilization of Processor, Memory and Storage of every individual */
	int ***utilization = (int ***) malloc (number_of_individuals *sizeof (int **));
	/* iterate on individuals */	
	for (iterator_individual=0; iterator_individual < number_of_individuals; iterator_individual++)
	{
		/* requirements matrix, holds the sum of virtual machines requirements for each physical machine */
		int **requirements = (int **) malloc (h_size *sizeof (int *));
		/* utilization holds the physical machines utilization of Processor, Memory and Storage of every individual */
		utilization[iterator_individual] = (int **) malloc (h_size *sizeof (int *));
		for (iterator_physical=0; iterator_physical < h_size; iterator_physical++)
		{
			/* virtual machine requirements in Processor, Memory and Storage. Initialized to 0 */
			requirements[iterator_physical] = (int *) malloc (3 *sizeof (int));
 			requirements[iterator_physical][0] = requirements[iterator_physical][1] = requirements[iterator_physical][2] = 0;
			/* physical machine utilization of Processor, Memory and Storage. Initialized to 0 */
			utilization[iterator_individual][iterator_physical] = (int *) malloc (3 *sizeof (int));
			utilization[iterator_individual][iterator_physical][0] = utilization[iterator_individual][iterator_physical][1] = 
			utilization[iterator_individual][iterator_physical][2] = 0;
		}

		/* iterate on positions of an individual */
		for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
		{
			/* if the virtual machine has a placement assigned */
			if (population[iterator_individual][iterator_virtual] != 0)
			{
				/* increment the requirements of the assigned physical machine with the virtual machine requirements
				of Processor, Memory and Storage */
				requirements[population[iterator_individual][iterator_virtual]-1][0] += V[iterator_virtual][0];
				requirements[population[iterator_individual][iterator_virtual]-1][1] += V[iterator_virtual][1];
				requirements[population[iterator_individual][iterator_virtual]-1][2] += V[iterator_virtual][2];
			}
		}

		/* iterate on positions of an individual */
		for (iterator_physical=0; iterator_physical < h_size; iterator_physical++)
		{
			/* virtual machine requirements in Processor, Memory and Storage. Initialized to 0 */
			utilization[iterator_individual][iterator_physical][0] = requirements[iterator_physical][0];
			utilization[iterator_individual][iterator_physical][1] = requirements[iterator_physical][1];
			utilization[iterator_individual][iterator_physical][2] = requirements[iterator_physical][2];
		}
	}
	return utilization;
}

/* load_objectives: calculate the cost of each objective of each solution
 * parameter: population matrix
 * parameter: utilization tridimentional matrix
 * parameter: commitment matrix
 * parameter: physical machines matrix
 * parameter: virtual machines matrix
 * parameter: number of individuals
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * parameter: base_solution individual
 * returns: cost of each objetive matrix
*/
float **load_objectives(int **population, int ***utilization, float **cm, int **H, int **V, int number_of_individuals, int h_size, int v_size, int* base_solution, int CRITICAL_SERVICES)
{
	/* iterators */
	int iterator_individual;
	//int iterator_virtual2;
	int iterator_virtual;
	int physical_position;
	int iterator_physical;

	float power_consumption;

	/* utility of a physical machine */
	float utilidad, total_used_cpu, total_used_memory;
	float total_cpu, total_memory;

	int found_critical, found_no_critical;
	int total_pm_CS, total_pm_NCS, total_pm_ON;

	/* value solution holds the cost of each solution */
	float **value_solution = (float **) malloc (number_of_individuals *sizeof (float *));

	for (iterator_individual = 0 ; iterator_individual < number_of_individuals; iterator_individual++)
		value_solution[iterator_individual] = (float *) malloc (3 *sizeof (float));

	/* iterate on individuals */
	for (iterator_individual = 0; iterator_individual < number_of_individuals; iterator_individual++)
	{
		power_consumption = 0.0;
		total_cpu = total_used_cpu = 0.0;
		total_memory = total_used_memory = 0.0;
		value_solution[iterator_individual][0] = value_solution[iterator_individual][1] = value_solution[iterator_individual][2] = 0.0;

		total_pm_CS = total_pm_NCS = total_pm_ON = 0;

		/* (OF1 - Objective Function 1) calculate energy consumption of each solution*/
		/* iterate on physical machines */
		for (iterator_physical = 0 ; iterator_physical < h_size ; iterator_physical++)
		{
			if (utilization[iterator_individual][iterator_physical][0] > 0)
			{
				/* calculates utility of a physical machine */
				utilidad = (float) utilization[iterator_individual][iterator_physical][0] / ( (float) H[iterator_physical][0] * cm[iterator_individual][iterator_physical] );
				
				/* calculates energy consumption of a physical machine */
				power_consumption += ( (float) H[iterator_physical][3] - ( (float) H[iterator_physical][3] * 0.6) ) * utilidad + 
				( (float) H[iterator_physical][3] * 0.6 );

				found_critical = found_no_critical = 0;
				total_pm_ON++;

				if (cl_cs == cl_nc)
				{
					for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
					{
						if (population[iterator_individual][iterator_virtual]-1 == iterator_physical)
						{
							if ( V[iterator_virtual][3] == CRITICAL_SERVICES )
								found_critical++;
							else
								found_no_critical++;
						}
						if (found_critical > 0 && found_no_critical > 0)
							break;
					}

					if( found_critical > 0 )
						total_pm_CS++;
					if( found_no_critical > 0 )
						total_pm_NCS++;
				}
				else
				{
					if (cm[iterator_individual][iterator_physical] == cl_cs)
					{
						total_pm_CS++;

						for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
							/* verify if there is a no critical VM on PM */
							if ( (population[iterator_individual][iterator_virtual]-1 == iterator_physical) && V[iterator_virtual][3] != CRITICAL_SERVICES)
							{
								total_pm_NCS++;
								break;
							}
					}
					else
						total_pm_NCS++;
				}			
			}
		}

		/* loads energy consumption of each solution */
		value_solution[iterator_individual][0] = power_consumption;

		/* (OF2) calculate the Dispersion Level of each solution */
		if(total_pm_CS != 0 && total_pm_NCS != 0)
		{
			value_solution[iterator_individual][1] = ( (float)total_pm_CS / ( ((float)number_vms_sc/(float)total_pm_CS) + ( ((float)v_size - (float)number_vms_sc) / total_pm_NCS ) ) );
		}
		else
		{
			if(total_pm_CS == 0)
				value_solution[iterator_individual][1] = ((float)(v_size - (float)number_vms_sc) / (float)total_pm_NCS);
			else
				value_solution[iterator_individual][1] = ((float) number_vms_sc / (float)total_pm_CS);
		}

		/* (OF3) calculate migration cost of each solution */
		value_solution[iterator_individual][2] = migration_cost (V, base_solution, population[iterator_individual], v_size, CRITICAL_SERVICES);
	}

	return value_solution;
}


/* migration_cost: calculates the migration cost of a solution
 * parameter: virtual machines matrix
 * parameter: base_solution individual
 * parameter: population individual vector
 * parameter: number of virtual machines
 * returns: cost of migration to change VMs positions from base_individual to individ
*/
float migration_cost (int **vm, int *b_solution, int *individ, int v_size, int CRITICAL_SERVICES)
{
	int iter_virt, total_critical_vms = 0, total_no_critical_vms = 0;
	float sum = 0.0;

	for (iter_virt=0; iter_virt < v_size; iter_virt++)
	{
		if (b_solution[iter_virt] != individ[iter_virt])
		{
			if (vm[iter_virt][3] == CRITICAL_SERVICES)
				total_critical_vms++;
			else
				total_no_critical_vms++;
		}
	}

	sum = (float)(total_critical_vms * mc_cs) + (float)(total_no_critical_vms * mc_ncs);	

	return sum;
}


/* generate_base_solution: copy the random base_solution generated in initial population, to be used in migration cost objective
 * parameter: population matrix
 * parameter: number of individuals
 * parameter: number of virtual machines
 * returns: the base_individual
*/
int *generate_base_solution(int **population, int v_size, int number_of_individuals)
{
	/* iterators */
	int iterator_virtual;	
	
	int* base_s = (int *) malloc ( v_size * sizeof (int) );

	for (iterator_virtual = 0 ; iterator_virtual < v_size ; iterator_virtual++)
		/* Copy the extra individual generated in population to base individual */
		base_s[iterator_virtual] = population[number_of_individuals][iterator_virtual];

	return base_s;
}


/* update_pm_used_in_individual: counts the number of PM used in an individual
 * parameter: the individual
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * returns: nothing, it's void
*/
void update_pm_used_in_individual (int individual, int h_size, int v_size)
{
	int iterator_virtual, iterator_physical;
	
	int pm_used_in_individual = 0;

	for (iterator_physical = 0; iterator_physical < h_size; iterator_physical++)
	{
		for (iterator_virtual=0; iterator_virtual < v_size; iterator_virtual++)
		{
			if (global_best_individual[iterator_virtual] == iterator_physical + 1)
			{
				pm_used_in_individual++;
				break;
			}
		}
	}

	global_h_sizes[individual] = pm_used_in_individual;
}
