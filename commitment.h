/* 
 * commitment.h: Virtual Machine Placement with OverSubscription Problem - Commitment Functions Header
 * Date: 27-04-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
*/

extern float cl_cs;
extern float cl_nc;

float **calculates_commitment(int **population, int number_of_individuals, int h_size, int v_size, int **V, int CRITICAL_SERVICES);
void update_commitment_line (int **population, float **cm, int **V, int individual_overloaded, int h_size, int v_size, int CRITICAL_SERVICES);
