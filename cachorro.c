#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
typedef struct
{
    int matr;
    char nome[25];
    int nota1;
    int nota2;
    int excluido;
} reg_aluno;
// TODO: Implementar um indice (em memoria) para facilitar a busca dos registros do arquivo
// TODO: Alterar as funcoes de busca, insercao e exclusao para usar o indice

typedef struct
{
    int matr;
    int pos_seek;
} Indice;

typedef struct lista
{
    Indice i;
    struct lista *prox;
} Lista;

void mostra(FILE *arq)
{
    reg_aluno aluno;
    rewind(arq);
    while (fread(&aluno, sizeof(aluno), 1, arq))
        if (aluno.excluido == 0)
            printf("%d\t%s\t%d\t%d\n", aluno.matr, aluno.nome, aluno.nota1, aluno.nota2);
}

void printa_lista(Lista *ini){
  Lista *p = ini;
  while(p != NULL){
    printf("\n Pos_seek = %d", p->i.pos_seek);
    p = p->prox;
  }
}
int pesquisa(FILE *arq, int matr, reg_aluno *al)
{
    reg_aluno aluno;
    rewind(arq);
    while (fread(&aluno, sizeof(aluno), 1, arq))
        if (aluno.matr == matr && aluno.excluido == 0)
        {
            *al = aluno;
            return 1;
        }
    return 0;
}
void exclui(FILE *arq, int matr)
{
    reg_aluno aluno;
    if (pesquisa(arq, matr, &aluno))
    {
        int excl = 1;
        printf("Excluindo: %s\n", aluno.nome);
        fseek(arq, -1 * sizeof(int), SEEK_CUR);
        fwrite(&excl, sizeof(int), 1, arq);
        fflush(arq);
    }
}

void lista_cria(Lista **inicio)
{
    *inicio = (Lista *)malloc(sizeof(Lista));
    *inicio = NULL;
}

void lista_insere(Lista **ini, Indice i){
    Lista *ant = NULL;

    Lista *p = *ini;

    while (p != NULL && p->i.pos_seek < i.pos_seek){
        ant = p;
        p = p->prox;
    }

    Lista *novo = (Lista *)malloc(sizeof(Lista));

    novo->i.pos_seek = i.pos_seek;

    if (ant == NULL){
        novo->prox = *ini;
        *ini = novo;
    }

    else{
        novo->prox = ant->prox;
        ant->prox = novo;
    }
}


void inclui(FILE *arq, Lista *it)
{
    reg_aluno aluno;
    Indice indice_;
    fseek(arq, 0, SEEK_END);
    printf("Informe os dados do aluno (matr, nome, nota1 e nota2) \n");
    scanf("%d%s%d%d", &aluno.matr, aluno.nome, &aluno.nota1, &aluno.nota2);
    indice_.matr = aluno.matr;
    indice_.pos_seek = ftell(arq);
    lista_insere(&it, indice_);
    aluno.excluido = 0;
    fwrite(&aluno, sizeof(aluno), 1, arq);
}
int main(void)
{
    int matr, op;
    reg_aluno aluno;
    Lista *lista;
    lista_cria(&lista);
    FILE *arq;
    if (access("alunos.dat", F_OK) == 0)
        arq = fopen("alunos.dat", "r+"); // arquivo existe
    else
        arq = fopen("alunos.dat", "w+"); // arquivo nao existia
    do
    {
        printf("\nMenu\n 1. Mostrar todos\n 2. Pesquisar\n 3. Incluir\n 4. Excluir\n 5. Sair\nInforme uma opcao: ");
        scanf("%d", &op);
        switch (op)
        {
        case 1:
            printf("\nAlunos gravados no arquivo: \n");
            mostra(arq);
            break;
        case 2:
            printf("\nDigite a matricula a ser buscada: ");
            scanf("%d", &matr);
            if (pesquisa(arq, matr, &aluno))
            {
                printf("\nAluno encontrado:\n");
                printf("%d\t%s\t%d\t%d\n", aluno.matr, aluno.nome, aluno.nota1, aluno.nota2);
            }
            else
                printf("\nA matricula %d nao foi encontrada!\n", matr);
            break;
        case 3:
            inclui(arq, lista);
            break;
        case 4:
            printf("\nDigite a matricula a ser excluida: ");
            scanf("%d", &matr);
            exclui(arq, matr);
            break;
        case 5:
            printf("\nSaindo...\n\n");
            break;
        case 6:
          printa_lista(lista);
          break;
        default:
            printf("\nOpcao invalida!\n");
            break;
        }
    } while (op != 5);
    fclose(arq);
    return 0;
}