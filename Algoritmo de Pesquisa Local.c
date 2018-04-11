#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Trabalho Pratico de IIA
// Ultima alteracao: 18 Dez 2016
// Compilado em Visual Studio 2015, formato x86 (32 bits)

/*
Versoes e historial
V1.0 -> 13 Dez 2016 - Compatibilidade apenas com o exemplo do enunciado
V1.1 -> 16 Dez 2016 - Melhoramento da funcao fitness + a possibilidade de inserir param por linha de comandos
V2.0 -> 17 Dez 2016 - Compatibilidade para todos os exemplos ("Trepa_Colinas Normal" concluido)
V2.1 -> 18 Dez 2016 - Novas funcionalidades: "Aceitar solucoes iguais" e a "Recristalizacao Simulada"
*/

// Activa a rotina de geracao de numero aleatorios
void inicia_aleatorios() {
	srand((unsigned)time(NULL));
}

// Preenche uma tabela/matriz com os dados obtidos a partir do ficheiro
// Devolve um pointeiro para o inicio dessa tabela/matriz
double **preenche_dados(char fich[], int n) {

	FILE *f;
	int i, x, y;
	double valor;
	double **tabela;

	f = fopen(fich, "rt");
	if (f == NULL) {
		printf("O ficheiro '%s' nao existe.\n", fich);
		exit(1);
	}

	tabela = (double **)malloc(n * sizeof(double *));
	for (i = 0; i < n; i++) {
		tabela[i] = (double *)malloc(n * sizeof(double));
	}

	if (tabela == NULL) {
		printf("Nao existe memoria para alocar tabela.\n");
		exit(1);
	}

	for (i = 0; i < n; i++)
		tabela[i][i] = 0;

	for (i = 0; i < n *(n - 1) / 2; i++) {
		fscanf(f, " %d %d %lf", &x, &y, &valor);
		/*printf("%d %d %lf\n", x, y, valor);*/
		tabela[x - 1][y - 1] = valor;
		tabela[y - 1][x - 1] = valor;
	}


	/*for (i = 0; i < n; i++) {
	printf("%d| ", i + 1);
	for (int j = 0; j < n; j++)
	printf("%.2lf ", tabela[i][j]);
	printf("\n");
	}*/

	return tabela;
}

// Calcula uma solucao_inicial
// Devolve a solucao_inicial
int *gera_solucao_inicial(int *n) {

	int *sol;
	int i;
	int num_uns;

	sol = malloc((*n) * sizeof(int));
	if (sol == NULL) {
		printf("Nao existe memoria para criar sol\n");
		exit(1);
	}

	while (1) {
		num_uns = 0;
		for (i = 0; i < (*n); i++)
			sol[i] = rand() % 2;
		for (i = 0; i < (*n); i++)
			if (sol[i] == 1)
				num_uns++;
		if (num_uns > 1)
			break;
	}

	return sol;
}

// Calcula a fitness da solucao
// Devolve a fitness
double fitness(double **tabela, int n, int *solucao_a_verificar) {

	int i, j, m, flag_todos_a_zero;
	double total;

	total = 0.0;
	m = 0;

	flag_todos_a_zero = 1;
	for (i = 0; i < n; i++)
		if (solucao_a_verificar[i] == 1)
			flag_todos_a_zero = 0;

	if (flag_todos_a_zero == 1)
		return total;

	for (i = 0; i < n - 1; i++) {
		if (solucao_a_verificar[i] == 1) {
			for (j = i + 1; j < n; j++) {
				if (solucao_a_verificar[j] == 1) {
					total += tabela[i][j];
				}
			}
		}
	}


	for (i = 0; i < n; i++)
		if (solucao_a_verificar[i] == 1)
			m++;

	return total / m;
}

// Mostra a solucao e a sua fitness
void mostra_solucao(double **tabela, int *solucao, int *n) {

	int i;

	printf("\nSolucao: ");
	for (i = 0; i < (*n); i++)
		printf("%d ", solucao[i]);
	printf(", q = %.2lf\n", fitness(tabela, (*n), solucao));

	/*
	for (i = 0; i < (*n); i++)
	if (solucao[i] == 1)
	printf("%d ", i + 1);

	printf("\n");
	*/
}

// Cria a vizinhanca a partir da solucao_actual
// Devolve a melhor_solucao da vizinhanca da solucao_actual
int *cria_e_mostra_vizinhos(double **tabela, int *solucao_actual, int n) {

	int **solucoes;
	double *qualidades;
	double melhor_qualidade;
	int i, j, posicao;

	solucoes = (int **)malloc(n * sizeof(int *));
	for (i = 0; i < n; i++)
		solucoes[i] = (int *)malloc(n * sizeof(int));

	qualidades = (double *)malloc(n * sizeof(double));

	if (solucoes == NULL || qualidades == NULL) {
		printf("Nao existe memoria!\n");
		exit(1);
	}

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			solucoes[i][j] = solucao_actual[j];

	for (i = 0; i < n; i++) {
		if (solucoes[i][i] == 1)
			solucoes[i][i] = 0;
		else
			solucoes[i][i] = 1;
	}

	for (i = 0; i < n; i++)
		qualidades[i] = fitness(tabela, n, solucoes[i]);

	melhor_qualidade = -5000.0; // numero muito grande negativo (ATENCAO nao pode ser zero, porque pode haver distancias negativas)
	posicao = 0;
	for (i = 0; i < n; i++) {
		if (qualidades[i] > melhor_qualidade) {
			melhor_qualidade = qualidades[i];
			posicao = i;
		}
	}


	for (i = 0; i < n; i++) {
		printf("S%d, q = %.2lf: ", i + 1, fitness(tabela, n, solucoes[i]));
		if (n < 100) {
			for (j = 0; j < n; j++) {
				printf("%d ", solucoes[i][j]);
			}
		}
		printf("\n");
	}

	free(qualidades);
	for (i = 0; i < n; i++)
		if (i != posicao)
			free(solucoes[i]);

	return solucoes[posicao];
}

// Encontra o numero de itens de uma solucao
int numero_de_itens(int *solucao, int *n) {

	int i;
	int uns;

	uns = 0;

	for (i = 0; i < (*n); i++) {
		if (solucao[i] == 1)
			uns++;
	}

	return uns;
}

// Parametros se executado por linha de comandos -> prog.exe(argv[0]) conjuntos_n(argv[1]) nome_ficheiro.txt(argv[2]) opcao(argv[3])
int main(int argc, char *argv[]) {

	int n, i, opcao, *solucao_actual, *nova_solucao;
	double mbf, delta, **tabela;
	char fich[100];

	printf("\n\t\t**********************************************************************\n");
	printf("\t\t***************    Algoritmos de Pesquisa Local     ******************\n");
	printf("\t\t**********************************************************************\n\n");
	if (argc == 4) {
		if ((n = atoi(argv[1])) > 100) {
			printf("AVISO: As solucoes nao serao mostradas, apenas serao mostradas as fitness, porque o 'n' e' um valor muito elevado!\n\n");
		}
		else if (n <= 1) {
			printf("O valor de 'n' nao e' valido!\n");
			return 1;
		}
		strcpy(fich, argv[2]);
		opcao = atoi(argv[3]);
	}
	else {

		printf("Numero de conjuntos 'n':\n> ");
		scanf("%d", &n);

		if (n >= 100) {
			printf("AVISO: As solucoes nao serao mostradas, apenas serao mostradas as fitness, porque o 'n' e' um valor muito elevado!\n\n");
		}
		else if (n <= 1) {
			printf("O valor 'n' nao e' valido!\n");
			return 1;
		}

		printf("Ficheiro:\n> ");
		scanf("%s", &fich);

		printf("TP normal(0), TP aceita iguais(1), Recristalizacao Simulada(2):\n> ");
		scanf("%d", &opcao);
	}

	inicia_aleatorios();
	tabela = preenche_dados(fich, n);
	solucao_actual = gera_solucao_inicial(&n);
	mostra_solucao(tabela, solucao_actual, &n);

	i = 1;
	mbf = 0.0;

	while (1) {

		printf("\n\tRepeticao %d\n", i);
		nova_solucao = cria_e_mostra_vizinhos(tabela, solucao_actual, n);

		if (opcao == 0) {
			if (fitness(tabela, n, nova_solucao) > fitness(tabela, n, solucao_actual)) {
				mbf += fitness(tabela, n, nova_solucao);
				mostra_solucao(tabela, nova_solucao, &n);
				free(solucao_actual);
				solucao_actual = nova_solucao;
			}
			else
				break;
		}
		else if (opcao == 1) {
			if (fitness(tabela, n, nova_solucao) >= fitness(tabela, n, solucao_actual)) {
				mbf += fitness(tabela, n, nova_solucao);
				mostra_solucao(tabela, nova_solucao, &n);
				free(solucao_actual);
				solucao_actual = nova_solucao;
			}
			else
				break;
		}
		else {

			delta = fitness(tabela, n, nova_solucao) - fitness(tabela, n, solucao_actual);

			if (delta > 0) {
				mbf += fitness(tabela, n, nova_solucao);
				mostra_solucao(tabela, nova_solucao, &n);
				free(solucao_actual);
				solucao_actual = nova_solucao;
			}
			else {
				if ((exp(delta / i) * 100) < (double)(rand() % 100)) {
					mbf += fitness(tabela, n, nova_solucao);
					mostra_solucao(tabela, nova_solucao, &n);
					free(solucao_actual);
					solucao_actual = nova_solucao;
				}
				else {
					break;
				}
			}

		}

		i++;
	}

	printf("\n\n\tMelhor Solucao\n");
	printf("MBF = %.2lf", mbf / i);
	mostra_solucao(tabela, solucao_actual, &n);
	printf("Itens seleccionados: %d\n", numero_de_itens(solucao_actual, &n));

	getchar();
	printf("Pressionar uma tecla para sair... ");
	getchar();

	free(solucao_actual);
	free(nova_solucao);
	free(tabela);
	return 0;
}