# The Interactive Memetic Algorithm for Virtual Machine Placement with Over Subscription (VMPOS)

This software makes the Virtual Machines (VM) placement considering the oversubscription of the Physical Machines (PM), following an evolutionary algorithm. 

This code is based on Interactive Memetic Algorithm for Virtual Machine Placement (IMAVMP) developed by Fabio Lopez Pires [1].

The VMPOS approach considers three objectives that must be met simultaneously: the reduction of Energy Consuption, the reduction of Dispersion Level and the reduction of Migration costs. The solutions are evolved seeking the best trade-off between the objectives and applying the theory of Pareto Frontier [2].

The VMPOS receives a Datacenter (DC) text file as input with the resources of the machines (physical and virtual) and generates a positioning of all VMs in PMs, applying different levels of commitment (oversubscription), following the population evolution of a genetic algorithm. 

In addition, two different types of VMs are considered, both critical and non-critical. Migration costs are applied in an attempt to reduce the number of VM migrations with critical services.

The VMPOS receives an input file (*vmpos_config.vmp*) with some settings to apply, such as the number of individuals and generations to be used in evolution. Another input file (optional) can be provided to the VMPOS, which may contain a base positioning that must be evolved through VMPOS. This file is called *vmpos_base_individual.vmp*. Both files are in same path of VMPOS executable.

At the end of processing the VMPOS returns an output file with the positioning of the VMs in the PMs.

## Operation:

The configuration and initial positioning files are read during program execution. The datacenter file must be passed.

**_a)_ Configuration:**<br>
The *vmpos_config.vmp* file, with settings of the VMPOS must be configured and is in same path of VMPOS executable. It has the following structure:

    POPULATION

    5	50

    OBJECTIVES WEIGHT

    1.0	1.0	1.0

    COMMITMENT LEVEL

    1.0	2.0

    MIGRATION COSTS

    2.0	1.0


Below POPULATION line has two tab-separated values. The first is number of individuals (5) and second is the number of generations to evolve (50).

Below OBJECTIVES WEIGHT line has three tab-separated values. They indicate the weights that should be applied to the objectives: Energy Consumption, Dispersion Level and Migration Cost, respectively. If the wish is to favor the reduction of the energy consumption, for example, a distribution of weights used was: 0.6 0.2 0.2 (with sum of the three being 1 (one)). If you do not want to favor any objective, the configuration would be as in the example (1.0 1.0 1.0). 

Below the COMMITMENT LEVEL line has two tab-separated values. The first indicates the level of commitment desired for PMs that host at least one critical VM (1.0), and the second value associated with non-critical VMs (2.0). 
In code, the oversubscription is applied on PMs through two constants, CL_CS (*Commitment Level for Critical Services*) and CL_NC (*Commitment Level for No Critical Services*) that receive these values from *vmpos_config.vmp* file. With the data CL_CS = 1.0 and CL_NC = 2.0 means that the resources available in the PMs will be multiplied by 1.0 (CL_CS) when it is hosting **at least one VM configured as with critical services**, and that the resources in the PMs will be multiplied by 2.0 (CL_NC) when is hosting **only VMs with non-critical services**.

Below the MIGRATION COSTS line has two tab-separated values. The first indicates the cost applied to VMs with critical services (2.0), and the second value is the cost applied to VMs with non-critical services (1.0). This means that a bigger cost associated to VMs with critical services tends to reduce the chance of choice a solution with more migrations of critical VMs. A lower cost of VMs with non-critical services tends to impact less on the migration cost objective calculations, favoring solutions that deliver results with more migrations from non-critical VMs and less from critical VMs.<br><br>

**_b)_ Base Individual File:**<br>
It is possible to pass a file with a previous configuration of VMs positioning in the existing PMs. Even if the allocation is not using all datacenter PMs, it is important to put all the existing ones in the file describing the datacenter because it may be that VMPOS needs more PMs to perform the allocation of all VMs. The base individual file is named *vmpos_base_individual*.vmp and stays in the main VMPOS folder. If this file does not exist, the vmpos will generate a base individual randomly to calculate the migration cost objective.<br><br>


**_c)_ Datacenter File:**<br>
The datacenter file, with settings of the PMs, as well as the requirements of the VMs, must be passed (here we have two examples in **/input** folder) as input to the VMPOS through the command line.

The datacenter file must have the following structure (the text part, which separates PMs from VMs, is fixed and must exist):

    PHYSICAL MACHINES

    12	512	10000	1000
    16	1024	10000	1300
    10      512     10000   1000
    8       1024	10000	1300

    VIRTUAL MACHINES

    4	15	80	1
    8	30	160	0
    2	4	32	1
    4	8	80	0
    8	15	160	1
    4	30	320	0
    2	60	640	1
    2	15	32	0

Let's call this datacenter file of **4x8.vmp** that is a datacenter with 4 PMs and 8 VMs. The 4 existing lines in the PMs part indicate the resources values for each of the PMs in datacenter. And the sequence in each line represents, respectively: Number of Processors (in the example of the first line, 12), Memory Quantity (in the example of the first line, 512), Disk Capacity (in the example of the first line, 10000) and Energy Consumption (in the example of the first line, 1000). Each row is associated with the resources available from one PM.

The 4 columns in the lines of the VMs part indicate the possible values for required resources by each of VMs in the datacenter. The sequence of these 4 columns represents, respectively: Number of Virtual Processors (in the example of the first line, 4), Memory Amount (in the example of the first line, 15), Disk Capacity (in the example of the first line, 80) and the flag that indicates if the VM run critical services (1) or not (0).

If you want to try another datacenters configurations try to use the SIMULATOR software, available at https://github.com/petroniocg/simulator, that generates random datacenters with the number of PM and VMs you want.<br><br>

**_d)_ Running:**<br>
With the files DC and configuration ready, and optionally one file with a base individual, the VMPOS is executed as follows:


```sh
$ ./vmpos input/4x8.vmp
```

where, **4x8.vmp** was the name given to the file with the datacenter configuration.<br><br>

**_e)_ Results:**<br>
The program will generate two files in the folder **/results**, the main has name **pareto_result** which has many informations about the results and, of course, the placement indication of VMs. The second file called **pareto_result_data** has the following sequence in columns of results: Energy Consumption, Dispersion Level, Migration Cost, Fitness, Generation that found best individual and Duration of the execution. 

With this example, it was used the following base positioning file (*vmpos_base_individual.vmp*):
 
    1 2 2 2 3 1 4 4

And the best placement result was like this:

    =========================================================
                    THE BEST POSITIONING IS:
    2 1 2 1 2 1 2 1

In this result, the numbers 1 and 2 are the PMs used to allocate the eight VMs. The PMs 3 and 4 were not used in this placement solution so, they could be turned off. From the 4 PMs available the VMPOS placed the VMs in 2 PMs. Note that the placement in base individual VMs were occupying the 4 PMs.

The position of the number in the solution, from left to right, indicates which VM is allocated in which PM. So, the VMs 1, 3, 5 and 7 are in PM 2. The VMs 2, 4, 6 and 8 are in PM 1.

## Modifications:

If you want to adapt the code to your needs, the compilation command to be used is:

```sh
$ gcc -o vmpos vmpos.c common.c initialization.c reparation.c local_search.c commitment.c pareto.c variation.c -g -lm
```


## References

[1] Fabio Lopez Pires. Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters. The code is available at: https://github.com/flopezpires

[2] Pareto Frontier. Available at https://en.wikipedia.org/wiki/Pareto_efficiency