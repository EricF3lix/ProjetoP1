#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//criar uma nova struct para compor a listaPronta, somente com os dados necessários(br, km, tipoAlerta e upvotes)


typedef struct usuario {
    int idUsuario;
    long horarioEP;
    int br;
    float km;
    char tipoAlerta;
    int upvotes;
    struct usuario *proximo;
} Usuario;


typedef struct dadosProcessados{
    int br;
    float km;
    char tipoAlerta;
    int upvotes;
    struct dadosProcessados *proximo;
} DadosProcessados;

typedef struct pair {
    int br;
    float km;
    char tipoAlerta;
    int upvotes;
    struct pair *proximo;
} Pair;


typedef struct {
    Pair **lista;
    int capacidade;
} HashTable;

// operacao de HASH  //tam 223
int hash(int br, char tipo) {
    int valor = tipo;
    int indice = (valor * 1000 + br)%223;
    return indice;
}



void inicializaHash(HashTable *tabela) {
    tabela->capacidade = 223;
    tabela->lista = calloc(223,  sizeof(Pair*)); //iniciar a hash com tudo NULL
    if (tabela->lista == NULL) {
        printf("Erro ao alocar memória.\n");
        return;
    }

}


void insereHash(HashTable *tabela, int br, float km, char tipo) {
    int indice = hash(br, tipo);
    Pair *atual = tabela->lista[indice];

    while (atual != NULL) {
        if (atual->br == br && atual->km == km && atual->tipoAlerta == tipo) {
            atual->upvotes++;
            return;
        }
        atual = atual->proximo;
    }

    Pair *novo = malloc(sizeof(Pair));
    if (!novo) {
        printf("não foi possivel alocar memoria\n");
        return;
    }

    novo->br = br;
    novo->km = km;
    novo->tipoAlerta = tipo;
    novo->upvotes = 1;

    novo->proximo = tabela->lista[indice];
    tabela->lista[indice] = novo;
}


void carregaArquivo(Usuario **listaDeUsuarios, char *nome) {
    FILE *arquivo = fopen(nome, "r");
    if (!arquivo) {
        printf("Não foi possível abrir o arquivo %s\n", nome);
        return;
    }

    char titulo[200];
    fgets(titulo, sizeof(titulo), arquivo);

    while (1) {
        Usuario *novoUsuario = malloc(sizeof(Usuario));
        if (!novoUsuario) {
            printf("Erro de memória\n");
            return;
        }

        if (fscanf(arquivo, "%d;%ld;%d;%f;%c",
            &novoUsuario->idUsuario,
            &novoUsuario->horarioEP,
            &novoUsuario->br,
            &novoUsuario->km,
            &novoUsuario->tipoAlerta) != 5) {

            break; // Sai do loop.
        }

        novoUsuario->upvotes = 0;
        novoUsuario->proximo = *listaDeUsuarios;
        *listaDeUsuarios = novoUsuario;
    }

    printf("Arquivo carregado: %s\n", nome);
    fclose(arquivo);
}

void liberaMemoriaDaListaTotal(Usuario *lista) {
    Usuario *atual = lista;
    while (atual != NULL) {
        Usuario *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
}

void liberaMemoriaDaListaAuxiliar(DadosProcessados *lista){
    DadosProcessados *atual = lista;
    while (atual != NULL) {
        DadosProcessados *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
}
    

void percorreListaJuntando(Usuario **listaDeUsuariosTotal, DadosProcessados **listaPronta, HashTable *tabela) {
    Usuario *noAtual = *listaDeUsuariosTotal;
    while (noAtual != NULL) {
        float km = noAtual->km;
        if ((km - (int)km) < 0.5) {
            km = (int)km;

        } else {
            km = (int)km + 0.5;
        }
        noAtual->km = km;
        insereHash(tabela, noAtual->br, noAtual->km, noAtual->tipoAlerta);
        noAtual = noAtual->proximo;
    }
    noAtual = *listaDeUsuariosTotal;


    for (int i = 0; i < tabela->capacidade; i++) {
        Pair *atual = tabela->lista[i];
        while (atual != NULL) {
            DadosProcessados *novo = malloc(sizeof(DadosProcessados));
            if (!novo) {
                printf("Erro  ao alocar memória\n");
                return; }

            novo->br = atual->br;
            novo->km = atual->km;
            novo->tipoAlerta = atual->tipoAlerta;
            novo->upvotes = atual->upvotes;
            if (*listaPronta==NULL){
                *listaPronta = novo;
                novo->proximo = NULL;
            } else {
                novo->proximo = *listaPronta;
                *listaPronta = novo;
            }

            atual = atual->proximo;
        }
    }

    liberaMemoriaDaListaTotal(*listaDeUsuariosTotal);
    // lembrar de apagar a hash
}



void salvaDadosProcessados(DadosProcessados*lista){
    FILE *arquivo = fopen("alertas_processados.csv", "w");
    fprintf(arquivo, "br;km;tipoAlerta;upvotes\n");
    DadosProcessados *noAtual = lista;
    while(noAtual!=NULL){
        fprintf(arquivo, "%d;%.1f;%c;%d\n", 
            noAtual->br,
            noAtual->km,
            noAtual->tipoAlerta, 
            noAtual->upvotes);
        
        noAtual = noAtual->proximo;
    }
    fclose(arquivo);
}


void salvaArquivoDoUsuario(DadosProcessados *lista) {
    FILE *arquivo = fopen("alertas_por_br.csv", "w");
    if (!arquivo) { printf("Não foi possível abrir o arquivo\n"); return; }

    fprintf(arquivo, "br;km;tipoAlerta;upvotes\n");
    DadosProcessados *atual = lista;
    while (atual != NULL) {
        fprintf(arquivo, "%d;%.1f;%c;%d\n",
            atual->br, atual->km, atual->tipoAlerta, atual->upvotes);
        atual = atual->proximo;
    }

    fclose(arquivo);
}

void imprimeRelatorio(DadosProcessados *lista) {
    printf("br;km;tipoAlerta;upvotes\n");
    DadosProcessados *atual = lista;
    while (atual != NULL) {
        printf("%d;%.1f;%c;%d\n",
            atual->br, atual->km, atual->tipoAlerta, atual->upvotes);
        atual = atual->proximo;
    }
}

void relatorioDealertaParaUsuario(DadosProcessados *lista) {
    DadosProcessados *listaAux = NULL;
    DadosProcessados *noAtual;
    int BR;
    float inicio, fim;
    printf("Informe BR e intervalo de km [BR INICIO FIM]: ");
    scanf("%d %f %f", &BR, &inicio, &fim);

    noAtual = lista;
    while (noAtual != NULL) {
        if (noAtual->br == BR && noAtual->km >= inicio && noAtual->km <= fim) {
            DadosProcessados *achou = malloc(sizeof(DadosProcessados));
            achou->br = noAtual->br;
            achou->km = noAtual->km;
            achou->tipoAlerta = noAtual->tipoAlerta;
            achou->upvotes = noAtual->upvotes;
            achou->proximo = listaAux;
            listaAux = achou;
        }
        noAtual = noAtual->proximo;
    }

    salvaArquivoDoUsuario(listaAux);
    imprimeRelatorio(listaAux);
    liberaMemoriaDaListaAuxiliar(listaAux);
    ; //cuidado
}


void relatorioTodasBr(DadosProcessados *lista) {
    DadosProcessados *listaAux = NULL;

    DadosProcessados *noAtual = lista;
    while(noAtual != NULL) {
        
        int totalUpvotes = 0;
        int achouNoAux = 0;
        DadosProcessados *verifica = listaAux;
        while (verifica != NULL) {
            if (verifica->br == noAtual->br && verifica->tipoAlerta == noAtual->tipoAlerta) {
                achouNoAux = 1;
                break;
            }
            verifica = verifica->proximo;
        }

    
        if (!achouNoAux) {
           
           DadosProcessados *noDois = lista; 
            while(noDois != NULL) {
                if (noAtual->br == noDois->br && noAtual->tipoAlerta == noDois->tipoAlerta) {
                   
                    totalUpvotes += noDois->upvotes; 
                }
                noDois = noDois->proximo;
            }
            
           
            DadosProcessados *novo = malloc(sizeof(Usuario));
            if (!novo) { printf("Erro de alocação de memória.\n"); return; }
            
            
            novo->br = noAtual->br;
            novo->tipoAlerta = noAtual->tipoAlerta;
            novo->upvotes = totalUpvotes; 
            
          
            novo->proximo = listaAux;
            listaAux = novo;
        }
        
        noAtual = noAtual->proximo;
    }

    FILE *arquivo = fopen("alertas_br_todas.csv", "w");
    if (!arquivo) { 
        printf("Não foi possível abrir o arquivo alertas_br_todas.csv\n"); 
        liberaMemoriaDaListaAuxiliar(listaAux);
        return; 
    }
    
    fprintf(arquivo,"br;tipoAlerta;upvotes\n"); 
    DadosProcessados *aux = listaAux;
    while(aux != NULL){
        fprintf(arquivo, "%d;%c;%d\n", aux->br, aux->tipoAlerta, aux->upvotes);
    
        printf("%d;%c;%d\n", aux->br, aux->tipoAlerta, aux->upvotes);

        aux = aux->proximo;
    }

    
    liberaMemoriaDaListaAuxiliar(listaAux);
    fclose(arquivo);
}






int validaEscolha(char escolha, char opcoesValidas[]) {
    while (1) {
        for (int i = 0; i < 4; i++) {
            if (escolha == opcoesValidas[i]) return escolha - '0';
        }
        printf("Entrada inválida. Tente novamente: ");
        scanf(" %c", &escolha);
    }
}

int menu() {
    char escolha;
    printf("Olá, seja bem-vindo ao UEIZI. Digite:\n");
    printf("0 - Sair do UEIZI\n");
    printf("1 - Procurar perigo em determinado trecho de uma BR\n");
    printf("2 - Ver todos os perigos cadastrados no aplicativo\n");
    printf("Escolha: ");
    scanf(" %c", &escolha);
    char opcoesValidas[4] = {'0','1','2','3'};
    return validaEscolha(escolha, opcoesValidas);
}



int main() {
    HashTable tabela;
    inicializaHash(&tabela);
    Usuario *listaDeUsuariosTotal = NULL;
    DadosProcessados *listaPronta = NULL;

    char *arquivos [] = 
    {"alertas_1000_1.csv",
    "alertas_1000_2.csv",
    "alertas_1000000.csv",
    "alertas_100000_1.csv",
    "alertas_100000_2.csv"};
    for (int i = 0  ; i < 5 ; i++){
        carregaArquivo(&listaDeUsuariosTotal, arquivos[i]);
    }




    percorreListaJuntando(&listaDeUsuariosTotal, &listaPronta, &tabela);

    int repetindo = 1;
    while (repetindo) {
        int opcoes = menu();
        switch(opcoes) {
            case 0:
                repetindo = 0;
                salvaDadosProcessados(listaPronta);
                printf("Programa finalizado\n");
                break;
            case 1:
                relatorioDealertaParaUsuario(listaPronta);
                break;
            case 2:
                relatorioTodasBr(listaPronta);
                break;
            default:
                printf("Opção inválida\n");
                break;
        }
    }

    return 0;
}