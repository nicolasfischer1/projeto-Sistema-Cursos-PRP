#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "Manipulacao_Turmas.h"

//  ProtÃ³tipos das funÃ§Ãµes
void cadastrar_aluno_espera(void);		 // cadastrar aluno na lista de espera
void cadastrar_aluno(void);				 // cadastrar aluno
void imprime_lista_espera(void);		 // ver lista de espera
void imprime_alunos_turma(Turma aux);	 // ver os alunos de uma turma
void imprimir_todos_alunos(void);		 // ver todos os alunos
Aluno buscar_aluno(int matricula);		 // buscar aluno pela matrÃ­cula
void editar_aluno(int matricula);		 // editar dados de um aluno
void desmatricular_aluno(int matricula); // desmatricular aluno
int qtd_alunos_cadastrados(void);		 // ver quantidade de alunos cadastrados
int entrada_dados_aluno(Aluno *novo_aluno);
void imprime_aluno(Aluno aluno_aux);
void atualizar_lista_espera(void); // atualizar a lista de espera

Aluno buscar_aluno(int matricula)
{

	FILE *arquivo_alunos = fopen("Alunos.bin", "rb"); //  Abre o arquivo 'Alunos.bin' "rb"(abertura para leitura de dados)

	if (arquivo_alunos != NULL) //  Se for possÃ­vel abrir/criar o arquivo 'Alunos.bin'
	{
		Aluno buscar;
		while (fread(&buscar, sizeof(Aluno), 1, arquivo_alunos) == 1)
		{
			if (matricula == buscar.matricula)
				return buscar;
		}
		buscar.matricula = -2;
		fclose(arquivo_alunos);
		return buscar;
	}
	else
		printf("Problema no arquivo 'Alunos.bin'\n");
}

void cadastrar_aluno_espera(void) // manipular o arquivo turmas e adicionar acrÃ©scimo do aluno na lista de espera!!!
{

	FILE *arquivo_alunos = fopen("Alunos.bin", "a+b"); //  Abre/Cria o arquivo 'Alunos.bin' e posiciona o ponteiro no final dele (abertura para gravaÃ§Ã£o de dados)

	if (arquivo_alunos != NULL) //  Se for possÃ­vel abrir/criar o arquivo 'Alunos.bin'
	{
		Aluno aluno_espera; //  Criando registro temporÃ¡rio (visÃ­vel apenas dentro desta condicional)
		int validador = entrada_dados_aluno(&aluno_espera);
		aluno_espera.aux = aluno_espera.turma;
		aluno_espera.turma = -1;

		if (validador != 0)
		{
			fwrite(&aluno_espera, sizeof(Aluno), 1, arquivo_alunos); //  "Escreve" o registro dentro do arquivo

			FILE *arquivo_turmas = fopen("Turmas.bin", "r+b");
			if (arquivo_turmas)
			{
				Turma aux;
				while (fread(&aux, sizeof(Turma), 1, arquivo_turmas))
				{
					if (aux.codigo == -1)
					{
						fseek(arquivo_turmas, (sizeof(Turma) * -1), SEEK_CUR);
						aux.qtd_alunos++;
						fwrite(&aux, sizeof(Turma), 1, arquivo_turmas);
						printf("\nAluno cadastrado com sucesso na lista de espera!\n");
						break;
					}
				}
				fclose(arquivo_turmas);
			}
			else
				printf("Problema no arquivo 'Turmas.bin'\n");
		}
	}
	else
		printf("Problema no arquivo 'Alunos.bin'\n");

	fclose(arquivo_alunos); //  Salva as alteraÃ§Ãµes, limpando o buffer e fechando o arquivo
}

int entrada_dados_aluno(Aluno *novo_aluno)
{

	printf("Digite a matr%ccula: ", 161);
	fflush(stdin);
	scanf("%d", &novo_aluno->matricula);

	Aluno a_validador = buscar_aluno(novo_aluno->matricula);

	if (novo_aluno->matricula >= 0 && a_validador.matricula == -2)
	{
		printf("Nome: ");
		fflush(stdin);
		wscanf(L"%ls", &novo_aluno->nome);

		if (novo_aluno->nome[0] != '\0' && novo_aluno->nome[0] != ' ' && novo_aluno->nome[0] > 64)
		{
			printf("Idade: ");
			fflush(stdin);
			scanf("%d", &novo_aluno->idade);

			if (novo_aluno->idade > 0 && novo_aluno->idade < 150)
			{
				printf("Turma (ou c%cdigo de espera): ", 162);
				fflush(stdin);
				scanf("%d", &novo_aluno->turma);

				Turma validador = busca_turma(novo_aluno->turma);
				if (validador.codigo != -2)
				{
					if (validador.qtd_alunos < validador.qtd_limite)
					{
						printf("\nDados cadastrados!\n");
						return 1;
					}
					else
						printf("\nTurma lotada!\n");
						return 2;
				}
				else
				{
					printf("\nTurma n%co encontrada!\n", 198);
					return 2;
				}
			}
			else
				printf("\nIdade inv%clida\n", 160);
		}
		else
			printf("\nNome inv%clido!\n", 160);
	}
	else
		printf("\nMatr%ccula inv%clida ou j%c existente!\n", 161, 160, 160);
	return 0;
}

void cadastrar_aluno(void)
{ // caso a turma esteja cheia cair na lista de espera!!!

	Aluno novo_aluno;
	int validador = entrada_dados_aluno(&novo_aluno);
	if (validador == 1)
	{
		FILE *arquivo_turmas = fopen("Turmas.bin", "r+b");
		if (arquivo_turmas)
		{
			Turma aux;
			while (fread(&aux, sizeof(Turma), 1, arquivo_turmas))
			{
				if (aux.codigo == novo_aluno.turma)
				{
					fseek(arquivo_turmas, (sizeof(Turma) * -1), SEEK_CUR);
					aux.qtd_alunos++;
					fwrite(&aux, sizeof(Turma), 1, arquivo_turmas);
					break;
				}
			}
			fclose(arquivo_turmas);
		}
		else
		{
			printf("Problema no arquivo 'Turmas.bin'\n");
			return;
		}

		FILE *arquivo_alunos = fopen("Alunos.bin", "a+b");
		if (arquivo_alunos)
		{
			fwrite(&novo_aluno, sizeof(Aluno), 1, arquivo_alunos);

			printf("\nAluno cadastrado!\n");
			fclose(arquivo_alunos);
		}
		else
		{
			printf("\nProblema no arquivo 'Alunos.bin'\n");
			return;
		}
	}
	else
		printf("\nAluno n%co cadastrado!\n", 198);
}

void imprime_lista_espera(void)
{
	Turma aux = busca_turma(-1);
	if (aux.qtd_alunos > 0)
	{
		FILE *arquivo_alunos = fopen("Alunos.bin", "rb"); //  Abre/Cria o arquivo 'Alunos.bin' "rb"(abertura para leitura de dados)

		if (arquivo_alunos != NULL) //  Se for possÃ­vel abrir/criar o arquivo 'Alunos.bin'
		{
			Aluno aluno_espera;
			while (fread(&aluno_espera, sizeof(Aluno), 1, arquivo_alunos) == 1)
			{ //  Vai rodar enquanto ela conseguir retornar uma linha vÃ¡lida
				if (aluno_espera.turma == -1)
					imprime_aluno(aluno_espera);
			}
			Turma verificador = busca_turma(-1);
			if (verificador.qtd_alunos == 0)
				printf("\nLista de espera vazia\n");
			fclose(arquivo_alunos); //  Salva as alteraÃ§Ãµes, limpando o buffer e fechando o arquivo
		}
		else
			printf("Problema no arquivo 'Alunos.bin'\n");
	}
	else
	{
		printf("\nLista de espera vazia!\n");
	}
}

void imprimir_todos_alunos(void)
{

	FILE *arquivo_alunos = fopen("Alunos.bin", "rb"); //  Abre/Cria o arquivo 'Alunos.bin' "rb"(abertura para leitura de dados)

	if (arquivo_alunos != NULL) //  Se for possÃ­vel abrir/criar o arquivo 'Alunos.bin'
	{
		Aluno todos_alunos;
		todos_alunos.turma = -2;
		while (fread(&todos_alunos, sizeof(Aluno), 1, arquivo_alunos) == 1)
		{ //  Vai rodar enquanto ela conseguir retornar uma linha vÃ¡lida
			imprime_aluno(todos_alunos);
		}
		if (todos_alunos.turma == -2)
			printf("\nN%co h%c alunos!\n", 198, 160);

		fclose(arquivo_alunos); //  Salva as alteraÃ§Ãµes, limpando o buffer e fechando o arquivo
	}
	else
		printf("Problema no arquivo 'Alunos.bin'\n");
}

void imprime_alunos_turma(Turma aux)
{

	FILE *arquivo_alunos = fopen("Alunos.bin", "rb"); //  Abre/Cria o arquivo 'Alunos.bin' "rb"(abertura para leitura de dados)

	if (arquivo_alunos != NULL)
	{ //  Se for possÃ­vel abrir/criar o arquivo 'Alunos.bin'
		Aluno aluno_turma;

		while (fread(&aluno_turma, sizeof(Aluno), 1, arquivo_alunos))
		{ //  Vai rodar enquanto ela conseguir retornar uma linha vÃ¡lida
			if (aux.codigo == aluno_turma.turma)
				imprime_aluno(aluno_turma);
		}
		fclose(arquivo_alunos); //  Salva as alteraÃ§Ãµes, limpando o buffer e fechando o arquivo
	}
	else
		printf("Problema no arquivo 'Alunos.bin'\n");
}

void editar_aluno(int matricula)
{

	FILE *arquivo_alunos = fopen("Alunos.bin", "r+b"); //  Abre/Cria o arquivo 'Alunos.bin' "w+b"(abertura para leitura de dados)

	if (arquivo_alunos != NULL) //  Se for possÃ­vel abrir/criar o arquivo 'Alunos.bin'
	{
		Aluno editar = buscar_aluno(matricula);
		if (editar.matricula != -2)
		{
			printf("***********************************\n");
			printf("\nAluno encontrado!\n\n");
			imprime_aluno(editar);
			while (1)
			{
				if (entrada_dados_aluno(&editar) == 1)
					break;
			}
			fseek(arquivo_alunos, sizeof(Aluno) * -1, SEEK_CUR);
			fwrite(&editar, sizeof(Aluno), 1, arquivo_alunos);
			fclose(arquivo_alunos); //  Salva as alteraÃ§Ãµes, limpando o buffer e fechando o arquivo
		}
		else
			printf("Aluno n%co encontrado!\n", 198);
	}
	else
		printf("Problema no arquivo 'Alunos.bin'\n");
}

void desmatricular_aluno(int matricula)
{

	FILE *arquivo_a = fopen("Alunos.bin", "r+b"); //  Abre/Cria o arquivo 'Alunos.bin' "r+b"(abertura para leitura e escrita de dados)

	int aluno_foi_encontrado = 0; //Variável booleana para aluno encontrado

	if (arquivo_a != NULL) //  Se for possível abrir/criar o arquivo 'Alunos.bin'
	{
		//Obtém o tamanho do arquivo
		fseek(arquivo_a, 0, SEEK_END);		   //Vai até o final
		int tamanho_maximo = ftell(arquivo_a); //Obtém a posição em bytes em relação ao começo
		fseek(arquivo_a, 0, SEEK_SET);		   //Volta o cursor do arquivo para o início

		Aluno desmatricular; //Variável para procurar o aluno a ser desmatriculado
		Aluno aux;			 //Variável para o deslocamento dos registros no arquivo

		int codigo; //Variável para salvar a turma em que o aluno estava

		while (fread(&desmatricular, sizeof(Aluno), 1, arquivo_a)) //Percorre o arquivo procurando pelo aluno a ser desmatriculado
		{
			if (desmatricular.matricula == matricula) //Se encontrar o aluno a ser desmatriculado
			{

				codigo = desmatricular.turma; //Salva a turma do aluno a ser desmatriculado

				if (ftell(arquivo_a) < tamanho_maximo)
				{ //Se o aluno não era o último do arquivo

					while (fread(&aux, sizeof(Aluno), 1, arquivo_a)) //Percorre o arquivo deslocando a lista
					{
						fseek(arquivo_a, sizeof(Aluno) * -2, SEEK_CUR); //Volta dois alunos para deslocar os registros à esquerda corretamente

						fwrite(&aux, sizeof(Aluno), 1, arquivo_a); //Sobrescreve o registro

						fseek(arquivo_a, sizeof(Aluno), SEEK_CUR); //Passa para o próximo aluno a ser deslocado
					}

					fseek(arquivo_a, sizeof(Aluno) * -1, SEEK_CUR); //Ao final, volta um aluno para poder truncar o duplicado no final do arquivo
				}
				else												//Se o aluno era o último do arquivo
					fseek(arquivo_a, sizeof(Aluno) * -1, SEEK_CUR); //Volta um aluno para poder truncar o arquivo, removendo um aluno duplicado

				int tamanho = ftell(arquivo_a); //Obtém a posição do novo final

				fclose(arquivo_a); //Fecha o arquivo

				//Esta parte trunca o arquivo
				HANDLE arquivo = CreateFileW(L"Alunos.bin", GENERIC_WRITE, 0, NULL, 3, FILE_ATTRIBUTE_NORMAL, NULL); //Abre o arquivo

				SetFilePointer(arquivo, tamanho, NULL, 0); //Modifica a posição do cursor

				SetEndOfFile(arquivo); //Trunca o arquivo, modificando a posição de seu final

				CloseHandle(arquivo); //Fecha o arquivo

				aluno_foi_encontrado = 1; //Muda o estado da variável booleana para aluno encontrado

				break; //Sai do escopo do while
			}
		}

		if (aluno_foi_encontrado)
		{ //Se o aluno foi encontrado

			FILE *arquivo_turmas = fopen("Turmas.bin", "r+b"); //Abre o arquivo de turmas

			Turma turma_aux; //Estrutura Turma auxiliar

			while (fread(&turma_aux, sizeof(Turma), 1, arquivo_turmas))
			{ //Percorre o arquivo

				if (turma_aux.codigo == codigo)
				{ //Se a turma do aluno desmatriculado foi encontrada

					turma_aux.qtd_alunos--; //Decrementa a quantidade de alunos

					fseek(arquivo_turmas, sizeof(Turma) * -1, SEEK_CUR); //Retrocede uma turma

					fwrite(&turma_aux, sizeof(Turma), 1, arquivo_turmas); //Sobrescreve os dados da turma

					fclose(arquivo_turmas); //Fecha o arquivo

					break; //Sai do laço
				}
			}

			printf("\nAluno desmatriculado!\n"); //Informa que o aluno foi desmatriculado
		}

		else
		{											   //Se o aluno não foi encontrado
			printf("\nAluno n%co encontrado!\n", 198); //Informa que o aluno não foi encontrado
			fclose(arquivo_a);						   //Fecha o arquivo
		}
	}
	else											  //Se não foi possível abrir ou criar o arquivo
		printf("Problema no arquivo 'Alunos.bin'\n"); //Informa o problema
}



int qtd_alunos_cadastrados(void)
{
	int contador = 0;

	FILE *arquivo = fopen("Alunos.bin", "r+b"); //  Abre/Cria o arquivo 'Alunos.bin' "rb"(abertura para leitura de dados)

	if (arquivo != NULL) //  Se for possÃ­vel abrir/criar o arquivo 'Alunos.bin'
	{
		Aluno aluno_cad;									 //  Criando registro temporÃ¡rio (visÃ­vel apenas dentro desta condicional)
		while (fread(&aluno_cad, sizeof(Aluno), 1, arquivo)) //  Vai rodar enquanto ela conseguir retornar uma linha vÃ¡lida
			contador += 1;									 // contador vai marcar a quantidade de alunos cadastrados
	}
	else
	{
		arquivo = fopen("Alunos.bin", "w+b");
		if (arquivo == NULL)
			printf("Problema no arquivo 'Alunos.bin'\n");
	}
	fclose(arquivo); //  Salva as alteraÃ§Ãµes, limpando o buffer e fechando o arquivo
	return contador;
}

void imprime_aluno(Aluno aluno_aux)
{

	printf("***************************************************************\n");
	printf("Nome: %ls\t", aluno_aux.nome);
	printf("Matr%ccula: %d\n", 161, aluno_aux.matricula);
	printf("Idade: %d\t", aluno_aux.idade);
	if (aluno_aux.turma == -1)
		printf("\n>>Aluno da lista de espera!\n");
	else
		printf("Turma: %d\n", aluno_aux.turma);
}

void atualizar_lista_espera(void)
{

	FILE *arquivo_alunos = fopen("Alunos.bin", "r+b"); // abrir o arquivo alunos
	FILE *arquivo_turmas = fopen("Turmas.bin", "r+b"); // abrir o arquivo turmas

	if (arquivo_alunos != NULL && arquivo_turmas != NULL) //  Se for possível abrir/criar o arquivo 'Alunos.bin'
	{
		Aluno aluno_aux;
		Turma turma_aux;
		while (fread(&aluno_aux, sizeof(Aluno), 1, arquivo_alunos))
		{
			if (aluno_aux.turma == -1)
			{
				//printf("%ls\n", aluno_aux.nome);
				while (fread(&turma_aux, sizeof(Turma), 1, arquivo_turmas))
				{
					if (turma_aux.codigo == aluno_aux.aux)
					{
						if (turma_aux.qtd_alunos < turma_aux.qtd_limite)
						{
							aluno_aux.turma = turma_aux.codigo;
							aluno_aux.aux = 0;

							turma_aux.qtd_alunos++;
							fseek(arquivo_turmas, sizeof(Turma)*-1, SEEK_CUR);
							fwrite(&turma_aux, sizeof(Turma), 1, arquivo_turmas); // Reescrevendo a turma
							//printf("Turma reescrita!\n");

							fseek(arquivo_alunos, sizeof(Aluno)*-1, SEEK_CUR);
							fwrite(&aluno_aux, sizeof(Aluno), 1, arquivo_alunos); // Reescrevendo o aluno
							//printf("Aluno reescrito!\n");

							fclose(arquivo_turmas); //  Salva as alterações, limpando o buffer e fechando o arquivo
							fclose(arquivo_alunos); //  Salva as alterações, limpando o buffer e fechando o arquivo
							arquivo_alunos = fopen("Alunos.bin", "r+b"); // abrir o arquivo alunos
							arquivo_turmas = fopen("Turmas.bin", "r+b"); // abrir o arquivo turmas

							fread(&turma_aux, sizeof(Turma), 1, arquivo_turmas);
							fseek(arquivo_turmas, sizeof(Turma)*-1, SEEK_CUR);
							turma_aux.qtd_alunos--;
							fwrite(&turma_aux, sizeof(Turma), 1, arquivo_turmas);
							
							fclose(arquivo_turmas);
							arquivo_turmas = fopen("Turmas.bin", "r+b");

						}
						break;
					}
				}
			}
		}
		printf("\nA lista de espera foi atualizada!\n");
		fclose(arquivo_turmas); //  Salva as alterações, limpando o buffer e fechando o arquivo
		fclose(arquivo_alunos); //  Salva as alterações, limpando o buffer e fechando o arquivo
	}
	else
		printf("Problema no arquivo 'Alunos.bin' ou 'Turmas.bin'\n");
}
