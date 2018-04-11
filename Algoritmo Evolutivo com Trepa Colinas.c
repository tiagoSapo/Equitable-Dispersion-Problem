#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Trabalho Pratico de IIA
// Ultima alteracao: 20 Dez 2016
// Compilado em Visual Studio 2015, formato x86 (32 bits)

/*
Versoes e historial
V1.0 -> 20 Dez 2016 - Trepa Colinas + Evolutivo concluido
V1.1 -> 23 Dez 2016 - Revisao de bugs
*/

// Activa a rotina de geracao de numero aleatorios
void inicia_aleatorios() {
	srand((unsigned)time(NULL));
}

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

int **gera_populacao(int *n) {

	int **pop;
	int i;

	pop = (int **)malloc(sizeof(int *) * (*n));
	for (i = 0; i < (*n); i++)
		pop[i] = (int *)malloc(sizeof(int) * (*n));

	if (pop == NULL) {
		printf("Nao existe memoria!\n");
		exit(1);
	}

	for (i = 0; i < (*n); i++)
		pop[i] = gera_solucao_inicial(n);

	return pop;
}

int flip(int *valor) {
	if (*valor == 1)
		return 0;
	else
		return 1;
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
		//printf("%d %d %lf\n", x, y, valor);
		tabela[x - 1][y - 1] = valor;
		tabela[y - 1][x - 1] = valor;
	}


	//for (i = 0; i < n; i++) {
	//printf("%d| ", i + 1);
	//for (int j = 0; j < n; j++)
	//printf("%.2lf ", tabela[i][j]);
	//printf("\n");
	//}
	fclose(f);
	return tabela;
}

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

	if (solucao_a_verificar == NULL)
		exit(1);

	//printf("TOTAL = %.2lf, m = %d, = %.2lf", total, m, total / m);
	return total / m;
}

int ** seleccao(double ***tabela, int **pop, int *n) {

	int i, j, k;
	double *qualidades;
	double total_qualidade;
	int **temp;
	double valor;
	double maior_negativo;
	int flag;

	// calcula fitness
	qualidades = (double *)malloc(sizeof(double) * (*n));

	// se encontrar fitness negativas (adiciona o modulo do maior negativo a todos)
	flag = 0;
	for (i = 0; i < (*n); i++) {
		double fit = fitness(*tabela, (*n), pop[i]);
		if (fit < 0)
			flag = 1;
		qualidades[i] = fit;
	}
	maior_negativo = 0.0;
	if (flag != 0) {
		for (i = 0; i < (*n); i++) {
			if (qualidades[i] < maior_negativo)
				maior_negativo = qualidades[i];
		}

		for (i = 0; i < (*n); i++) {
			qualidades[i] += (-maior_negativo);
		}
	}


	total_qualidade = 0.0;
	for (i = 0; i < (*n); i++)
		total_qualidade += qualidades[i];

	for (i = 0; i < (*n); i++)
		qualidades[i] = ((qualidades[i] / total_qualidade) * 100);


	// copiar os melhores
	temp = (int **)malloc(sizeof(int) * (*n));
	for (i = 0; i < (*n); i++)
		temp[i] = (int *)malloc(sizeof(int) * (*n));

	for (i = 0; i < (*n); i++)
		for (j = 0; j < (*n); j++)
			temp[i][j] = pop[i][j];

	for (i = 0; i < (*n); i++)
		if (i != 0)
			qualidades[i] += qualidades[i - 1];

	// preenche pop
	for (i = 0; i < (*n); i++) {
		valor = rand() % 100;
		for (j = 0; j < (*n); j++) {

			if (valor <= qualidades[j]) {
				for (k = 0; k < (*n); k++) {
					pop[i][k] = temp[j][k];
				}
				break;
			}
		}
	}
	free(qualidades);
	for (i = 0; i < (*n); i++)
		free(temp[i]);
	free(temp);
	return pop;
}

int ** recombinacao_metade(int **pop, int *n) {

	int i, j;
	int *temp;
	int ponto_de_corte;

	ponto_de_corte = ((*n) - (*n) / 2);

	temp = (int *)malloc(sizeof(int) * (*n));

	for (i = 0; i < (int)(*n) / 2; i = i + 2) {
		for (j = 0; j < (*n); j++) {
			temp[j] = pop[i][j];
		}

		for (j = ponto_de_corte - 1; j < (*n); j++) {
			pop[i][j] = pop[i + 1][j];
		}


		for (j = ponto_de_corte - 1; j < (*n); j++) {
			pop[i + 1][j] = temp[j];
		}
	}

	free(temp);
	return pop;
}
int ** recombinacao(int **pop, int *n) {

	int i, j;
	int *temp;
	int ponto_de_corte;

	ponto_de_corte = 0;

	temp = (int *)malloc(sizeof(int) * (*n));

	for (i = 0; i < (*n); i += 2) {
		for (j = 0; j < (*n); j++) {
			temp[j] = pop[i][j];
		}

		for (j = ponto_de_corte - 1; j < ((*n) / 4); j++) {
			pop[i][j] = pop[i + 1][j];
		}

		for (j = ponto_de_corte - 1; j < ((*n) / 4); j++) {
			pop[i + 1][j] = temp[j];
		}
	}

	ponto_de_corte = ((*n) / 2);

	for (i = 0; i < (int)(*n); i = i + 2) {
		for (j = 0; j < (*n); j++) {
			temp[j] = pop[i][j];
		}

		for (j = ponto_de_corte - 1; j < ((*n) / 2) + ((*n) / 4); j++) {
			pop[i][j] = pop[i + 1][j];
		}


		for (j = ponto_de_corte - 1; j < ((*n) / 2) + ((*n) / 4); j++) {
			pop[i + 1][j] = temp[j];
		}
	}

	free(temp);
	return pop;
}
int ** mutacao(int **pop, int *n) {

	int i, valor_aleatorio;
	int probabilidade_mutacao;

	probabilidade_mutacao = 40;

	for (i = 0; i < (*n); i++) {
		if (probabilidade_mutacao > rand() % 100) {
			valor_aleatorio = rand() % (*n);
			pop[i][valor_aleatorio] = flip(&pop[i][valor_aleatorio]);
		}
	}

	return pop;
}
int ** mutacao_melhorado(int **pop, int *n) {

	int i, j, valor_aleatorio;
	int probabilidade_mutacao, n_variaveis;

	probabilidade_mutacao = 30;

	for (i = 0; i < (*n); i++) {
		if (probabilidade_mutacao > rand() % 100) {
			n_variaveis = rand() % (*n - 1);
			if (n_variaveis == 0) {
				n_variaveis++;
			}
			for (j = 0; j < n_variaveis; j++) {
				valor_aleatorio = rand() % (*n);
				pop[i][valor_aleatorio] = flip(&pop[i][valor_aleatorio]);
			}
		}
	}

	return pop;
}


double media(double ***tabela, int **pop, int n) {

	double media = 0.0;

	for (int i = 0; i < n; i++)
		media += fitness(*tabela, n, pop[i]);

	return media;
}

int *encontra_melhor_solucao(double ***tabela, int **pop, int n, double *melhor_fitness) {

	int i;
	int posicao;
	double melhor_qualidade;

	melhor_qualidade = -1000.0;
	posicao = 0;
	for (i = 0; i < n; i++)
		if (fitness(*tabela, n, pop[i]) >= melhor_qualidade) {
			melhor_qualidade = fitness(*tabela, n, pop[i]);
			posicao = i;
		}

	*melhor_fitness = melhor_qualidade;

	int *copia = malloc(sizeof(int) * n);
	for (i = 0; i < n; i++)
		copia[i] = pop[posicao][i];
	return copia;
}

void mostra_pop(double ***tabela, int **pop, int *n) {

	int i, j;


	for (i = 0; i < (*n); i++) {
		printf("S%d, q = %.2lf: ", i, fitness(*tabela, *n, pop[i]));
		for (j = 0; j < (*n); j++) {
			printf("%d ", pop[i][j]);
		}
		printf("\n");
	}


}
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


int * trepa_colinas(double **tabela, int *n, int *pop_solucao) {

	int j, i;
	int **pop_temp;

	pop_temp = (int **)malloc(sizeof(int *) * (*n));
	for (i = 0; i < (*n); i++) {
		pop_temp[i] = (int *)malloc(sizeof(int) * (*n));
	}
	
	for (i = 0; i < (*n); i++) {
		for (j = 0; j < (*n); j++) {
			pop_temp[i][j] = pop_solucao[j];
		}
	}
	
	for (i = 0; i < (*n); i++) {
		pop_temp[i][i] = flip(&pop_temp[i][i]);
	}
	
	pop_temp = seleccao(&tabela, pop_temp, n);
	pop_temp = recombinacao_metade(pop_temp, n);
	pop_temp = mutacao(pop_temp, n);
	double f;

	int *vizinho = encontra_melhor_solucao(&tabela, pop_temp, *n, &f);
	
	for (i = 0; i < *n; i++) {
		free(pop_temp[i]);
	}
	free(pop_temp);
	return vizinho;
}


// Parametros se executado por linha de comandos -> prog.exe(argv[0]) conjuntos_n(argv[1]) nome_ficheiro.txt(argv[2]) numero_de_geracoes(argv[3])
int main(int argc, char *argv[]) {

	int n, runs, geracoes, max_geracoes;
	char ficheiro[100];
	double **tabela, fit_solucao;
	int **pop, *solucao_final;

	printf("\n\t\t**********************************************************************\n");
	printf("\t\t*************** Algoritmo Evolutivo + Trepa Colinas ******************\n");
	printf("\t\t**********************************************************************\n\n");

	if (argc == 4) {
		if ((n = atoi(argv[1])) > 100) {
			printf("AVISO: As solucoes nao serao mostradas, pois o valor 'n' e' elevado!\n\n");
		}
		else if (n <= 1) {
			printf("O valor de 'n' nao e' valido!\n");
			return 1;
		}
		strcpy(ficheiro, argv[2]);
		max_geracoes = atoi(argv[3]);
	}
	else {
		do {
			printf("Numero de conjuntos n (valor >= 5):\n> ");
			scanf("%d", &n);
			if (n < 5) {
				printf("ERRO: valor invalido!\n\n");
			}
			if (n >= 150) {
				printf("AVISO: N e' um valor elevado!\nAVISO: A execucao deste algoritmo podera' levar algum tempo a ser concluido.\n\n");
			}
		} while (n < 5);

		do {
			printf("Numero de Geracoes (valor >= 100):\n> ");
			scanf("%d", &max_geracoes);
			if (max_geracoes < 100) {
				printf("ERRO: valor invalido!\n\n");
			}
		} while (max_geracoes < 100);

		printf("Ficheiro:\n> ");
		scanf("%s", &ficheiro);
	}

	inicia_aleatorios();
	tabela = preenche_dados(ficheiro, n);

	pop = gera_populacao(&n);

	double fitness_pop_runs, solucao_actual_fit;
	int *solucao_actual;

	
	printf("\nPor favor, aguarde ");
	for (runs = 0; runs < 5; runs++) {
		printf(". ");

		for (geracoes = 0; geracoes < max_geracoes; geracoes++) {

			if (geracoes == 0) {
				fitness_pop_runs = fitness(tabela, n, pop[runs]);
			}
			
			solucao_actual = trepa_colinas(tabela, &n, pop[runs]);
			solucao_actual_fit = fitness(tabela, n, solucao_actual);

			if (solucao_actual_fit > fitness_pop_runs) {
				free(pop[runs]);
				pop[runs] = solucao_actual;
				fitness_pop_runs = solucao_actual_fit;
			}
			else {
				free(solucao_actual);
			}
			//printf("SO_AC = %.2lf, m = %.2lf\n", solucao_actual_fit, fitness_pop_runs);
			//mostra_pop(&tabela, pop, &n);
			//printf("IT%d - G%d\n", runs, geracoes);
		}
		
	}

	if (n < 150) {
		printf("\n\n");
		mostra_pop(&tabela, pop, &n);
	}
	else {
		printf("\n");
	}
	
	solucao_final = encontra_melhor_solucao(&tabela, pop, n, &fit_solucao);
	printf("\nQualidade da solucao final: %.2lf, %d itens seleccionados\n", fit_solucao, numero_de_itens(solucao_final, &n));
	free(solucao_final);

	printf("\nPressionar ENTER para sair... ");
	getchar();
	getchar();

	return 0;
}