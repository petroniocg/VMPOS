# VMPOS
Virtual Machine Placement with OverSubscription (VMPOS)


	COMO FUNCIONA O GERENCIADOR VMPOS:
	=============================================================


	Para executar o programa VMPOS é necessário executar a seguinte linha de comando:


	$ ./vmpos <caminho_arquivo_datacenter>

	Por exemplo,

	$ ./vmpos ~/VMPOS/input/4x8.vmp


	Lembrando que tem que editar o arquivo de configurações dos parâmetros do modelo. As orientações estão mais a frente neste arquivo.


	Detalhes do arquivo de datacenter:
	==================================

	1) As configurações de capacidades das MFs, assim como dos requisitos das VMs, devem ser colocadas em um arquivo do datacenter, que pode ficar dentro da pasta /input. O nome dado a este arquivo será utilizado na hora da execução do programa, como entrada.

	O arquivo de datacenter com 4 máquinas físicas e oito VMs, tem a seguinte estrutura (a parte com texto, que separa MFs de VMs, é fixa, e tem que existir):

		PHYSICAL MACHINES

		12	512	10000	1000	
		16	1024	10000	1300	
		10	512	10000	1000	
		8	1024	10000	1300

		VIRTUAL MACHINES

		4	15	80	1
		8	30	160	0
		2	4	32	1
		4	8	80	0
		8	15	160	1
		4	30	320	0
		2	60	640	1
		2	15	32	0


	As 4 colunas existentes na parte de MFs indicam respectivamente os valores possíveis de: Número de Processadores (no exemplo, 12), Quantidade de Memória (no exemplo, 512), Capacidade de Disco (no exemplo, 1000) e Consumo Energético (no exemplo, 1000).

	As 4 linhas colunas existentes na parte de VMs indicam respectivamente os valores possíveis de: Número de Processadores Virtuais (no exemplo, 4), Quantidade de Memória (no exemplo, 15), Capacidade de Disco (no exemplo, 80) e se a VM é crítica (1) ou não (0).

	O programa irá gerar um arquivo na pasta /results com o nome: pareto_result, com as informações da rodada, e o melhor indivíduo selecionado segundo as configurações do arquivo de configuração. Além de um arquivo com o nome pareto_result_data apenas com os dados gerados como saída em cada execução do modelo. Além dos dados dos três objetivos, este arquivo também traz o valor da função de fitness, e o tempo para chegar a solução.



	Detalhes do arquivo de configuração dos parâmetros do Modelo:
	=============================================================

	O arquivo de configuração tem o nome de vmpos_config.vmp e fica na pasta principal do VMPOS. Ele tem a seguinte estrutura:

		POPULATION

		5	200

		OBJECTIVES WEIGHT

		0.333	0.333	0.333

		COMMITMENT LEVEL

		1.0	1.5


	Abaixo da linha POPULATION tem dois valores separados por tabulação. O primeiro indica a quantidade de indivíduos (5) e o número de gerações (200) que a população deve evoluir.

	Abaixo da linha OBJECTIVES WEIGHT tem três valores separados por tabulação. Eles indicam os pesos que devem ser aplicados para os objetivos: Consumo de Energia, Dispersão e Custo de Migração, respectivamente. Utilizou-se sempre valores com soma dos três sendo igual a 1 (um). Se o desejo for favorecer a redução do consumo de energia, por exemplo, uma distribuição de pesos utilizada era: 0.6   0.2   0.2. Caso não se deseje favorecer nenhum objetivo, a configuração seria como no exemplo (0.333  0.333  0.333).

	Abaixo da linha COMMITMENT LEVEL tem dois valores separados por tabulação. O primeiro indica o nível de comprometimento desejado para as MFs que abrigam pelo menos uma VM crítica (1.0), e o segundo valor o associado a VMs não críticas (1.5). Isto quer dizer que, com esta configuração, o nível de comprometimento das MFs que abrigam pelo menos uma VM com serviços críticos é 1.0, ou seja, não se aplicará oversubscription. E o das MFs que abrigam APENAS VMs com serviços NÂO críticos é 1.5, ou seja, será aplicardo oversubscription de uma vez e meia aos recursos desta MF.


	Individuo Base:
	===============

	É possível passar um arquivo com uma configuração prévia de posicionamento de VMs nas MFs existentes. Mesmo que a alocação passada não esteja usando todas as MFs do datacenter, é importante colocar todas as existentes no arquivo que descreve o datacenter pois, pode ser que o vmpos precise de mais MFs para realizar a alocação de todas as VMs.
	O arquivo de individuo base tem o nome de vmpos_base_individual.vmp e fica na pasta principal do VMPOS.
	Se este arquivo não existir, o vmpos irá gerar um indivíduo base aleatório para calcular o objetivo de custo de migração.

