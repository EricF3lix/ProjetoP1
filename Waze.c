#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

//============================ TUDO QUE MEXE COM A TABELA HASH =======================================
int hash(int br, char tipo) {
    int valor = tipo;
    int indice = (valor * 1000 + br)%223;
    return indice;
}



void inicializaHash(HashTable *tabela) {
    tabela->capacidade = 223;
    tabela->lista = calloc(223,  sizeof(Pair*));
    if (tabela->lista == NULL) {
        printf("Erro ao alocar memória.\n");
        return;
    }

}


void insereHash(HashTable *tabela, int br, float km, char tipo, int upvotes) {
    int indice = hash(br, tipo);
    Pair *atual = tabela->lista[indice];

    while (atual != NULL) {
        if (atual->br == br && atual->km == km && atual->tipoAlerta == tipo) {
            atual->upvotes+=upvotes;
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
    novo->upvotes = upvotes;
    
    novo->proximo = tabela->lista[indice];
    tabela->lista[indice] = novo;
}
// ====================================================================================

// ========================= TODAS AS MANIPULAÇÕES DE ARQUIVO ==============================================
void carregaArquivo(Usuario **listaDeUsuarios, char *nome) {
    FILE *arquivo = fopen(nome, "r");
    if (!arquivo) {
        printf("Não foi possível abrir o arquivo %s\n", nome);
        return;
    }

    char titulo[150];
    fgets(titulo, sizeof(titulo), arquivo);

    while (1) {
        Usuario *novoUsuario = malloc(sizeof(Usuario));
        if (!novoUsuario) {
            printf("Erro de memória\n");
            fclose(arquivo);
            return;
        }

        if (fscanf(arquivo, "%d;%ld;%d;%f;%c",
            &novoUsuario->idUsuario,
            &novoUsuario->horarioEP,
            &novoUsuario->br,
            &novoUsuario->km,
            &novoUsuario->tipoAlerta) != 5) {
            free(novoUsuario);
            break; 
        }

        novoUsuario->upvotes = 0;
        novoUsuario->proximo = *listaDeUsuarios;
        *listaDeUsuarios = novoUsuario;
    }

    printf("Arquivo carregado: %s\n", nome);
    fclose(arquivo);
}

void salvaDadosProcessados(DadosProcessados*lista){
    FILE *arquivo = fopen("alertas_processados.csv", "w");
    if (!arquivo){
        printf("nao foi possivel abrir o arquivo.\n");
        return;
    }
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

void carregaArquivoProcessado(DadosProcessados **lista) {
    FILE *arquivo = fopen("alertas_processados.csv", "r");
    if (!arquivo) {
        return;
    }
    char titulo[150];
    fgets(titulo, sizeof(titulo), arquivo); 
    while (1) {
        DadosProcessados *novo = malloc(sizeof(DadosProcessados));
        if (!novo) {
            printf("Erro na alocação de memória.\n");
            fclose(arquivo);
            break;
        }

        if (fscanf(arquivo, "%d;%f;%c;%d", 
            &novo->br, 
            &novo->km, 
            &novo->tipoAlerta, 
            &novo->upvotes) != 4) 
            {
            free(novo);
            break;      
        }

        novo->proximo = *lista;
        *lista = novo;
    }

    fclose(arquivo);
}


void salvaArquivoDoUsuario(DadosProcessados *lista) {
    FILE *arquivo = fopen("alertas_por_br.csv", "w");
    if (!arquivo) { 
        printf("Não foi possível abrir o arquivo\n"); 
        return; 
    }

    fprintf(arquivo, "br;km;tipoAlerta;upvotes\n");
    DadosProcessados *atual = lista;
    while (atual != NULL) {
        fprintf(arquivo, "%d;%.1f;%c;%d\n",
            atual->br, atual->km, atual->tipoAlerta, atual->upvotes);
        atual = atual->proximo;
    }

    fclose(arquivo);
}


void salvaRelatorioTodasBr(DadosProcessados *lista){
    FILE *arquivo = fopen("alertas_br_todas.csv", "w");
    if (!arquivo){
        printf("Não foi possível abrir o arquivo.\n");
        return;
    }
    fprintf(arquivo, "br;tipoAlerta;quantidade\n");
    DadosProcessados *aux = lista;
    while(aux!=NULL){
        fprintf(arquivo, "%d;%c;%d\n", aux->br, aux->tipoAlerta, aux->upvotes);
        printf("%d;%c;%d\n", aux->br, aux->tipoAlerta, aux->upvotes);
        aux = aux->proximo;
    }

    fclose(arquivo);
}
// =============================================================================================
// ================================ MANIPULAÇÃO DE LISTAS E DA TABELA HASH =====================================
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
void liberaListaHash(Pair *lista) {
    Pair *atual = lista;
    while (atual != NULL) {
        Pair *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
}

void liberaHash(HashTable *tabela) {
    for (int i = 0; i < tabela->capacidade; i++) {
        liberaListaHash(tabela->lista[i]);
        }


    free(tabela->lista);
}


    

void juntaKm(Usuario **lista, HashTable *tabela){
    Usuario *noAtual = *lista;
    while (noAtual != NULL) {
        float km = noAtual->km;
        if ((km - (int)km) < 0.5) {
            km = (int)km;

        } else {
            km = (int)km + 0.5;
        }
        noAtual->km = km;
        insereHash(tabela, noAtual->br, noAtual->km, noAtual->tipoAlerta, 1);
        noAtual = noAtual->proximo;
    }
    noAtual = *lista;

}

void percorreHash(Usuario **listaDeUsuariosTotal, DadosProcessados **listaPronta, HashTable *tabela) {
    
    if(*listaDeUsuariosTotal == NULL){
        DadosProcessados *noAtual = *listaPronta;
        while(noAtual!=NULL){
            insereHash(tabela, noAtual->br, noAtual->km, noAtual->tipoAlerta, noAtual->upvotes);
            noAtual = noAtual->proximo;
        }
        return;
    } 
    
    juntaKm(listaDeUsuariosTotal, tabela);

    
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
}
// ========================================================================================
// ================================= MANIPULAÇÃO DOS RELATORIOS =========================================
void imprimeRelatorio(DadosProcessados *lista) {
    printf("br;km;tipoAlerta;upvotes\n");
    DadosProcessados *atual = lista;
    while (atual != NULL) {
        printf("%d;%.1f;%c;%d\n",
            atual->br, atual->km, atual->tipoAlerta, atual->upvotes);
        atual = atual->proximo;
    }
}

//  ============================== DESENHO COLORIDO ===============================================

void desenhaTrechoFiltrado(DadosProcessados *listaFiltrada, int brEscolhida, float kmInicio, float kmFim) {
    
    char linha[100][10];  
    for (int i = 0; i < 100; i++){
        linha[i][0] = '\0';
    }

    float intervalo = kmFim - kmInicio;

    DadosProcessados *atual = listaFiltrada;
    while (atual != NULL) {
        int pos = (int)(((atual->km - kmInicio) / intervalo) * 99);

        if (pos >= 0 && pos < 100) {
            int len = strlen(linha[pos]);
            if (len < 9) {
                linha[pos][len] = atual->tipoAlerta;
                linha[pos][len + 1] = '\0';
            }
        }

        atual = atual->proximo;
    }

    printf("\n");
    printf("\033[1;37mRepresentação visual dos alertas da BR-%d (km %.1f a %.1f):\033[0m\n",
           brEscolhida, kmInicio, kmFim);
    
    printf("      ");
    for (int i = 0; i < 106; i++) printf("=");
    printf("\n");

    printf("Km %.0f | ", kmInicio);

    for (int i = 0; i < 100; i++) {
        if (linha[i][0] == '\0') {
            printf(".");
        } else {
            for (int j = 0; linha[i][j] != '\0'; j++) {
                switch (linha[i][j]) {
                    case 'A': 
                        printf("\033[31mA\033[0m"); 
                        break; 
                    case 'B': 
                        printf("\033[33mB\033[0m"); 
                        break; 
                    case 'C': 
                        printf("\033[32mC\033[0m"); 
                        break;
                    case 'D': 
                        printf("\033[36mD\033[0m"); 
                        break;
                    case 'E': 
                        printf("\033[35mE\033[0m"); 
                        break; 
                    case 'F': 
                        printf("\033[34mF\033[0m"); 
                        break;
                    default:  
                        printf("%c", linha[i][j]); 
                        break;
                }
            }
        }
    }

    printf("| %.0f km\n", kmFim);
    printf("      ");
    for (int i = 0; i < 106; i++) printf("=");
    printf("\n");
}
// =============================================================================================================

void relatorioDealertaParaUsuario(DadosProcessados *lista) {
    DadosProcessados *listaAux = NULL;
    DadosProcessados *noAtual;
    int BR;
    float inicio, fim;
    printf("Informe BR e intervalo de km [BR INICIO FIM]: ");
    scanf("%d %f %f", &BR, &inicio, &fim);
    getchar();
    float intervalo = fim - inicio;
    
    if (intervalo <= 0) {
        printf("\nIntervalo inválido. O km final deve ser maior que o inicial.\n");
        return;
    }

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
    
    if (listaAux == NULL) {
        printf("\nNenhum alerta encontrado na BR-%d entre os km %.1f e %.1f.\n",
               BR, inicio, fim);
        return;
    }

    imprimeRelatorio(listaAux);
    desenhaTrechoFiltrado(listaAux, BR, inicio, fim);
    liberaMemoriaDaListaAuxiliar(listaAux); 
}


void relatorioTodasBr(HashTable *tabela) {
    DadosProcessados *listaAux = NULL;

    for (int i = 0; i < tabela->capacidade; i++) {
        Pair *atual = tabela->lista[i];
        while (atual != NULL) {
            DadosProcessados *noLista = listaAux;
            int achou = 0;
            while (noLista != NULL) {
                if (noLista->br == atual->br && noLista->tipoAlerta == atual->tipoAlerta) {
                    noLista->upvotes += atual->upvotes;  
                    achou = 1;
                    break;
                }
                noLista = noLista->proximo;
            }

           
            if (!achou ) {
                DadosProcessados *novo = malloc(sizeof(DadosProcessados));
                novo->br = atual->br;
                novo->tipoAlerta = atual->tipoAlerta;
                novo->upvotes = atual->upvotes;
                
                novo->proximo = listaAux;
                listaAux = novo;
            }

            atual = atual->proximo;
        }
    }

    salvaRelatorioTodasBr(listaAux);
    liberaMemoriaDaListaAuxiliar(listaAux);
}

// ============================================================================================
// ============================== MAIN, MENU E VALIDAÇÃO DE ESCOLHAS ==========================

int menu() {
    char escolha[10];
    printf("Olá, seja bem-vindo ao UEIZI. Digite:\n");
    printf("0 - Sair do UEIZI\n");
    printf("1 - Procurar perigo em determinado trecho de uma BR\n");
    printf("2 - Ver todos os perigos cadastrados no aplicativo\n");
    printf("Escolha: ");
    
    fgets(escolha, sizeof(escolha), stdin);

    if (strlen(escolha)!=2){
        return -1;
    }
    int opcao = escolha[0] - '0';
    return opcao;
}



int main() {
    HashTable tabela;
    inicializaHash(&tabela);
    Usuario *listaDeUsuariosTotal = NULL;
    DadosProcessados *listaPronta = NULL;
    char *arquivos [] = 
    {"alertas_1000_1.csv",
    "alertas_1000_2.csv",
    "alertas_100000_1.csv",
    "alertas_100000_2.csv",
    "alertas_1000000.csv"};
    
    carregaArquivoProcessado(&listaPronta);

    if (listaPronta == NULL){
        for (int i = 0  ; i < 5 ; i++){
            carregaArquivo(&listaDeUsuariosTotal, arquivos[i]);
        }
        
    }
    percorreHash(&listaDeUsuariosTotal, &listaPronta, &tabela);

    int repetindo = 1;
    while (repetindo) {
        int opcoes = menu();
        switch(opcoes) {
            
            case 0:
                repetindo = 0;
                salvaDadosProcessados(listaPronta);
                printf("Programa finalizado\n");
                liberaMemoriaDaListaAuxiliar(listaPronta);
                liberaHash(&tabela);
                break;
            
                case 1:
                relatorioDealertaParaUsuario(listaPronta);
                break;
            case 2:
                relatorioTodasBr(&tabela);
                break;
            default:
                printf("Opção inválida\n");
                break;
        }
    }

    return 0;
}