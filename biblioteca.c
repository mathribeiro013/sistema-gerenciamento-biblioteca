//Código executando sem erros na IDE "Code::Blocks 25.03"

//Inclusão das Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

//Definições para trabalho com dados
//Número máximo de livros, usuários e empréstimos
//Salvamento e backup para livros, usuários e empréstimos
#define MAX_LIVROS 100
#define ARQUIVO_LIVROS "livros.txt"
#define ARQUIVO_LIVROS_BACKUP "livros.bak"

#define MAX_USUARIOS 100
#define ARQUIVO_USUARIOS "usuarios.txt"
#define ARQUIVO_USUARIOS_BACKUP "usuarios.bak"

#define MAX_EMPRESTIMOS 100
#define ARQUIVO_EMPRESTIMOS "emprestimos.txt"
#define ARQUIVO_EMPRESTIMOS_BACKUP "emprestimos.bak"

//Structs Principais
//Data, Livro, Usuario, Emprestimo
struct Data {
    int dia;
    int mes;
    int ano;
};

struct Livro {
    int codigo;
    char titulo[100];
    char autor[80];
    char editora[60];
    int anopub;
    int disponiveis;
    bool status; // true = Ativo, false = Inativo
};

struct Usuario {
    int matricula;
    char nome[100];
    char curso[50];
    char telefone[15];
    struct Data dataCadastro;
};

struct Emprestimo {
    int codemp;
    int matriculaUsuario;
    int codigoLivro;
    struct Data dataEmprestimo;
    struct Data dataPrevista;
    bool statusEmprestimo; // true = Emprestado, false = Devolvido
};

//Struct Auxiliar para Relatório
struct ContagemLivro {
    int codigoLivro;
    int contagem;
};

//Protótipos das Funções
//Adiantamento de infomrações ao Compilador
void Menu(void);
void MenuCadastros(struct Livro *livros, int *numLivros, struct Usuario *usuarios, int *numUsuarios);
void MenuPesquisas(const struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios, const struct Emprestimo *emprestimos, int numEmprestimos);
void CadastrarLivro(struct Livro *livros, int *numLivros);
void CadastrarUsuario(struct Usuario *usuarios, int *numUsuarios);
void Emprestimo(struct Emprestimo *emprestimos, int *numEmprestimos, struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios);
void Devolucao(struct Emprestimo *emprestimos, int numEmprestimos, struct Livro *livros, int numLivros);
void Renovacao(struct Emprestimo *emprestimos, int numEmprestimos, struct Livro *livros, int numLivros);
void ListarEmprestimosAtivos(const struct Emprestimo *emprestimos, int numEmprestimos, const struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios);
void BuscarLivro(const struct Livro *livros, int numLivros);
void BuscarUsuario(const struct Usuario *usuarios, int numUsuarios);
void BuscaAvancadaLivro(const struct Livro *livros, int numLivros);
void RelatorioAtrasos(const struct Emprestimo *emprestimos, int numEmprestimos, const struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios);
void RelatorioMaisEmprestados(const struct Emprestimo *emprestimos, int numEmprestimos, const struct Livro *livros, int numLivros);

//Funções de Persistência
//Salvamento, Carregamento e Backup dos Dados
bool SalvarLivros(const struct Livro *livros, int numLivros);
int CarregarLivros(struct Livro *livros);
bool SalvarUsuarios(const struct Usuario *usuarios, int numUsuarios);
int CarregarUsuarios(struct Usuario *usuarios);
bool SalvarEmprestimos(const struct Emprestimo *emprestimos, int numEmprestimos);
int CarregarEmprestimos(struct Emprestimo *emprestimos);
void CriarBackup();

// Funções Auxiliares
//Limpeza de Buffer (\n) ou qualquer outro caractere digitado mas não lido
//Após o usuário digitar algo e pressionar "enter"
void LimpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//Limpeza do caractere de nova linha (\n)
//Após leituras de strings como o fgets()
void removeNovaLinha(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

//Função para Limpar a tela
//Sistemas Windows e Linux
void LimpaTela() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

//Função para calcular se o ano digitado
//É bissexto
bool AnoBiss(int ano) {
    return (ano % 400 == 0) || (ano % 100 != 0 && ano % 4 == 0);
}

//Função para validação de data
bool ValidaData(int d, int m, int a) {
    if (a < 1 || m < 1 || m > 12 || d < 1) return false;
    int diasNoMes;
    if (m == 2) {
        diasNoMes = AnoBiss(a) ? 29 : 28;
    } else if (m == 4 || m == 6 || m == 9 || m == 11) {
        diasNoMes = 30;
    } else {
        diasNoMes = 31;
    }
    if (d > diasNoMes) return false;
    return true;
}

//Função para calcular a Data Prevista
//Para a entrega do Livro
//Finalização do Empréstimo
struct Data calculaDataPrevista(struct Data dataInicial) {
    struct Data d = dataInicial;
    d.dia += 7;
    int diasNoMes;
    while (true) {
        if (d.mes == 2) {
            diasNoMes = AnoBiss(d.ano) ? 29 : 28;
        } else if (d.mes == 4 || d.mes == 6 || d.mes == 9 || d.mes == 11) {
            diasNoMes = 30;
        } else {
            diasNoMes = 31;
        }
        if (d.dia <= diasNoMes) {
            break;
        }
        d.dia -= diasNoMes;
        d.mes++;
        if (d.mes > 12) {
            d.mes = 1;
            d.ano++;
        }
    }
    return d;
}

//Compara as datas para Relatório de Atrasos
int comparaDatas(struct Data data1, struct Data data2) {
    if (data1.ano != data2.ano) return data1.ano - data2.ano;
    if (data1.mes != data2.mes) return data1.mes - data2.mes;
    return data1.dia - data2.dia;
}

//Função para verificar a data atual
struct Data dataAtual() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    struct Data hoje = {tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900};
    return hoje;
}

//Função para Busca de Livros por código
int BuscaLivroPorCodigo(const struct Livro *livros, int numLivros, int codigo) {
    for (int i = 0; i < numLivros; i++) {
        if (livros[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

//Função de Busca de Usuários por matrícula
int BuscaUsuarioPorMatricula(const struct Usuario *usuarios, int numUsuarios, int matricula) {
    for (int i = 0; i < numUsuarios; i++) {
        if (usuarios[i].matricula == matricula) {
            return i;
        }
    }
    return -1;
}

//Função para Busca de Empréstimos Ativos
//Retorna -1 por padrão para "não encontrado"
int BuscaEmprestimoAtivo(const struct Emprestimo *emprestimos, int numEmprestimos, int codLivro, int matUsuario) {
    for (int i = 0; i < numEmprestimos; i++) {
        if (emprestimos[i].codigoLivro == codLivro && emprestimos[i].matriculaUsuario == matUsuario && emprestimos[i].statusEmprestimo == true) {
            return i;
        }
    }
    return -1;
}

//Funções de Exibição
//Exibir informações de Livros
void ExibirLivroEmLista(const struct Livro *livro) {
    printf("\n--- RESULTADO ---\n");
    printf("Codigo: %d\n", livro->codigo);
    printf("Titulo: %s\n", livro->titulo);
    printf("Autor: %s\n", livro->autor);
    printf("Editora: %s\n", livro->editora);
    printf("Ano de Publicacao: %d\n", livro->anopub);
    printf("Copias Disponiveis: %d\n", livro->disponiveis);
    printf("Status: %s\n", livro->status ? "Ativo" : "Inativo");
    printf("-----------------\n");
}

//Exibir informações de Usuários
void ExibirUsuarioEmLista(const struct Usuario *usuario) {
    printf("\n--- RESULTADO ---\n");
    printf("Matricula: %d\n", usuario->matricula);
    printf("Nome: %s\n", usuario->nome);
    printf("Curso: %s\n", usuario->curso);
    printf("Telefone: %s\n", usuario->telefone);
    printf("Data de Cadastro: %02d/%02d/%04d\n", usuario->dataCadastro.dia, usuario->dataCadastro.mes, usuario->dataCadastro.ano);
    printf("-----------------\n");
}

//Exibir informações de Empréstimos
void ExibirEmprestimoEmLista(const struct Emprestimo *emprestimo, const struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios, bool isAtraso) {
    int idxUsuario = BuscaUsuarioPorMatricula(usuarios, numUsuarios, emprestimo->matriculaUsuario);
    int idxLivro = BuscaLivroPorCodigo(livros, numLivros, emprestimo->codigoLivro);

    const char *nomeUsuario = (idxUsuario != -1) ? usuarios[idxUsuario].nome : "Usuario N/A";
    const char *tituloLivro = (idxLivro != -1) ? livros[idxLivro].titulo : "Livro N/A";

    printf("\n--- EMPRESTIMO #%d ---\n", emprestimo->codemp);
    printf("Livro: %s (Cod: %d)\n", tituloLivro, emprestimo->codigoLivro);
    printf("Usuario: %s (Matr: %d)\n", nomeUsuario, emprestimo->matriculaUsuario);
    printf("Data Emprestimo: %02d/%02d/%04d\n", emprestimo->dataEmprestimo.dia, emprestimo->dataEmprestimo.mes, emprestimo->dataEmprestimo.ano);
    printf("Data Prevista: %02d/%02d/%04d\n", emprestimo->dataPrevista.dia, emprestimo->dataPrevista.mes, emprestimo->dataPrevista.ano);

    if (isAtraso) {
        printf("** STATUS: ATRASADO **\n");
    } else {
        printf("Status: Ativo\n");
    }
    printf("------------------------\n");
}

//Exibição de Menu
void MenuCadastros(struct Livro *livros, int *numLivros, struct Usuario *usuarios, int *numUsuarios) {
    int opcao;
    do {
        LimpaTela();
        printf("--- SUBMENU: CADASTROS ---\n");
        printf(" 1. Cadastrar Livro\n");
        printf(" 2. Cadastrar Usuario\n");
        printf(" 0. Voltar ao Menu Principal\n");
        printf("\nEscolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf("\nOpcao invalida. Digite um numero.\n");
            LimpaBuffer();
            opcao = -1;
            continue;
        }
        LimpaBuffer();
        LimpaTela();
        switch(opcao) {
            case 1: CadastrarLivro(livros, numLivros); break;
            case 2: CadastrarUsuario(usuarios, numUsuarios); break;
            case 0: break;
            default: printf("\nOpcao invalida. Tente novamente.\n"); break;
        }
        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }
    } while (opcao != 0);
    LimpaTela();
}

//Função para Cadastro de Livros
void CadastrarLivro(struct Livro *livros, int *numLivros) {
    if (*numLivros >= MAX_LIVROS) {
        printf("\nERRO: Limite maximo de livros atingido (%d).\n", MAX_LIVROS);
        return;
    }

    struct Livro novoLivro;
    printf("--- CADASTRO DE LIVRO ---\n");

    novoLivro.codigo = (*numLivros > 0) ? livros[*numLivros - 1].codigo + 1 : 1;
    printf("Codigo do Livro: %d\n", novoLivro.codigo);

    printf("Digite o Titulo (max 99 chars): ");
    fgets(novoLivro.titulo, 100, stdin);
    removeNovaLinha(novoLivro.titulo);

    printf("Digite o Autor (max 79 chars): ");
    fgets(novoLivro.autor, 80, stdin);
    removeNovaLinha(novoLivro.autor);

    printf("Digite a Editora (max 59 chars): ");
    fgets(novoLivro.editora, 60, stdin);
    removeNovaLinha(novoLivro.editora);

    do {
        printf("Digite o Ano de Publicacao (AAAA): ");
        if (scanf("%d", &novoLivro.anopub) != 1) {
            printf("ERRO: Entrada invalida. Digite um numero.\n");
            LimpaBuffer();
        } else {
            break;
        }
    } while (true);
    LimpaBuffer();

    do {
        printf("Digite a Quantidade Total de Copias: ");
        if (scanf("%d", &novoLivro.disponiveis) != 1) {
            printf("ERRO: Entrada invalida. Digite um numero.\n");
            LimpaBuffer();
        } else if (novoLivro.disponiveis <= 0) {
            printf("ERRO: A quantidade deve ser maior que zero.\n");
        } else {
            break;
        }
    } while (true);
    LimpaBuffer();

    novoLivro.status = true;

    livros[*numLivros] = novoLivro;
    (*numLivros)++;

    printf("\nSUCESSO: Livro '%s' cadastrado com Codigo %d.\n", novoLivro.titulo, novoLivro.codigo);

    if (!SalvarLivros(livros, *numLivros)) {
        printf("AVISO: Falha ao salvar dados do livro.\n");
    }
}


//Função para cadastro de Usuários
void CadastrarUsuario(struct Usuario *usuarios, int *numUsuarios) {
    if (*numUsuarios >= MAX_USUARIOS) {
        printf("\nERRO: Limite maximo de usuarios atingido (%d).\n", MAX_USUARIOS);
        return;
    }

    struct Usuario novoUsuario;
    printf("--- CADASTRO DE USUARIO ---\n");

    novoUsuario.matricula = (*numUsuarios > 0) ? usuarios[*numUsuarios - 1].matricula + 1 : 1000;
    printf("Matricula: %d\n", novoUsuario.matricula);

    printf("Digite o Nome (max 99 chars): ");
    fgets(novoUsuario.nome, 100, stdin);
    removeNovaLinha(novoUsuario.nome);

    printf("Digite o Curso (max 49 chars): ");
    fgets(novoUsuario.curso, 50, stdin);
    removeNovaLinha(novoUsuario.curso);

    printf("Digite o Telefone (max 14 chars): ");
    fgets(novoUsuario.telefone, 15, stdin);
    removeNovaLinha(novoUsuario.telefone);

    do {
        printf("Digite a Data de Cadastro (dd/mm/aaaa): ");
        if (scanf("%d/%d/%d", &novoUsuario.dataCadastro.dia, &novoUsuario.dataCadastro.mes, &novoUsuario.dataCadastro.ano) != 3) {
            printf("ERRO: Formato de data invalido. Use dd/mm/aaaa.\n");
            LimpaBuffer();
            continue;
        }
        LimpaBuffer();
        if (!ValidaData(novoUsuario.dataCadastro.dia, novoUsuario.dataCadastro.mes, novoUsuario.dataCadastro.ano)) {
            printf("ERRO: Data inserida e invalida.\n");
            continue;
        }
        break;
    } while (true);

    usuarios[*numUsuarios] = novoUsuario;
    (*numUsuarios)++;

    printf("\nSUCESSO: Usuario '%s' cadastrado com Matricula %d.\n", novoUsuario.nome, novoUsuario.matricula);

    if (!SalvarUsuarios(usuarios, *numUsuarios)) {
        printf("AVISO: Falha ao salvar dados do usuario.\n");
    }
}

//Função para realização de Empréstimo de Livros
void Emprestimo(struct Emprestimo *emprestimos, int *numEmprestimos, struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios) {
    if (*numEmprestimos >= MAX_EMPRESTIMOS) {
        printf("\nERRO: Limite maximo de emprestimos atingido (%d).\n", MAX_EMPRESTIMOS);
        return;
    }

    int codigoLivro, matriculaUsuario, idxLivro, idxUsuario;
    struct Emprestimo novoEmprestimo;

    printf("--- REGISTRO DE EMPRESTIMO ---\n");

    printf("Digite o Codigo do Livro: ");
    if (scanf("%d", &codigoLivro) != 1) {
        printf("ERRO: Codigo invalido.\n"); LimpaBuffer(); return;
    }
    LimpaBuffer();
    idxLivro = BuscaLivroPorCodigo(livros, numLivros, codigoLivro);

    if (idxLivro == -1 || !livros[idxLivro].status) {
        printf("ERRO: Livro nao encontrado ou inativo.\n");
        return;
    }
    if (livros[idxLivro].disponiveis <= 0) {
        printf("ERRO: O livro '%s' nao tem copias disponiveis.\n", livros[idxLivro].titulo);
        return;
    }

    printf("Digite a Matricula do Usuario: ");
    if (scanf("%d", &matriculaUsuario) != 1) {
        printf("ERRO: Matricula invalida.\n"); LimpaBuffer(); return;
    }
    LimpaBuffer();
    idxUsuario = BuscaUsuarioPorMatricula(usuarios, numUsuarios, matriculaUsuario);

    if (idxUsuario == -1) {
        printf("ERRO: Usuario nao encontrado.\n");
        return;
    }

    novoEmprestimo.codemp = (*numEmprestimos > 0) ? emprestimos[*numEmprestimos - 1].codemp + 1 : 1;
    novoEmprestimo.codigoLivro = codigoLivro;
    novoEmprestimo.matriculaUsuario = matriculaUsuario;
    novoEmprestimo.statusEmprestimo = true;
    novoEmprestimo.dataEmprestimo = dataAtual();
    novoEmprestimo.dataPrevista = calculaDataPrevista(novoEmprestimo.dataEmprestimo);

    livros[idxLivro].disponiveis--;
    emprestimos[*numEmprestimos] = novoEmprestimo;
    (*numEmprestimos)++;

    printf("\nSUCESSO: Emprestimo registrado. Devolucao Prevista: %02d/%02d/%04d\n", novoEmprestimo.dataPrevista.dia, novoEmprestimo.dataPrevista.mes, novoEmprestimo.dataPrevista.ano);
    if (!SalvarEmprestimos(emprestimos, *numEmprestimos) || !SalvarLivros(livros, numLivros)) {
        printf("AVISO: Falha ao salvar dados.\n");
    }
}

//Função para registro de Devolução
void Devolucao(struct Emprestimo *emprestimos, int numEmprestimos, struct Livro *livros, int numLivros) {
    int codigoLivro, matriculaUsuario, idxEmprestimo, idxLivro;

    printf("--- REGISTRO DE DEVOLUCAO ---\n");

    printf("Digite o Codigo do Livro devolvido: ");
    if (scanf("%d", &codigoLivro) != 1) { printf("ERRO: Codigo invalido.\n"); LimpaBuffer(); return; }
    LimpaBuffer();

    printf("Digite a Matricula do Usuario: ");
    if (scanf("%d", &matriculaUsuario) != 1) { printf("ERRO: Matricula invalida.\n"); LimpaBuffer(); return; }
    LimpaBuffer();

    idxEmprestimo = BuscaEmprestimoAtivo(emprestimos, numEmprestimos, codigoLivro, matriculaUsuario);

    if (idxEmprestimo == -1) {
        printf("\nERRO: Nao foi encontrado um emprestimo ATIVO.\n");
        return;
    }

    idxLivro = BuscaLivroPorCodigo(livros, numLivros, codigoLivro);

    if (comparaDatas(emprestimos[idxEmprestimo].dataPrevista, dataAtual()) < 0) {
        printf("\nAVISO: O emprestimo estava em ATRASO!\n");
    }

    emprestimos[idxEmprestimo].statusEmprestimo = false;
    if (idxLivro != -1) {
        livros[idxLivro].disponiveis++;
    }

    printf("\nSUCESSO: Devolucao registrada.\n");

    if (!SalvarEmprestimos(emprestimos, numEmprestimos) || !SalvarLivros(livros, numLivros)) {
        printf("AVISO: Falha ao salvar dados.\n");
    }
}

//Função para Renovação de Empréstimos
void Renovacao(struct Emprestimo *emprestimos, int numEmprestimos, struct Livro *livros, int numLivros) {
    int codigoLivro, matriculaUsuario, idxEmprestimo;
    struct Data novaDataEmprestimo;

    printf("--- RENOVACAO DE EMPRESTIMO ---\n");

    printf("Digite o Codigo do Livro a ser renovado: ");
    if (scanf("%d", &codigoLivro) != 1) { printf("ERRO: Codigo invalido.\n"); LimpaBuffer(); return; }
    LimpaBuffer();

    printf("Digite a Matricula do Usuario: ");
    if (scanf("%d", &matriculaUsuario) != 1) { printf("ERRO: Matricula invalida.\n"); LimpaBuffer(); return; }
    LimpaBuffer();

    idxEmprestimo = BuscaEmprestimoAtivo(emprestimos, numEmprestimos, codigoLivro, matriculaUsuario);

    if (idxEmprestimo == -1) {
        printf("\nERRO: Nao foi encontrado um emprestimo ATIVO.\n");
        return;
    }

    printf("\nEmprestimo Encontrado. Prazo atual: %02d/%02d/%04d.\n",
           emprestimos[idxEmprestimo].dataPrevista.dia, emprestimos[idxEmprestimo].dataPrevista.mes, emprestimos[idxEmprestimo].dataPrevista.ano);

    novaDataEmprestimo = dataAtual();
    emprestimos[idxEmprestimo].dataEmprestimo = novaDataEmprestimo;
    emprestimos[idxEmprestimo].dataPrevista = calculaDataPrevista(novaDataEmprestimo);

    printf("\nRENOVACAO CONCLUIDA! Novo prazo de devolucao: %02d/%02d/%04d\n",
           emprestimos[idxEmprestimo].dataPrevista.dia, emprestimos[idxEmprestimo].dataPrevista.mes, emprestimos[idxEmprestimo].dataPrevista.ano);

    if (!SalvarEmprestimos(emprestimos, numEmprestimos)) {
        printf("\nFalha ao salvar os dados permanentemente.\n");
    }
}

//Função para listar Empréstimos Ativos
void ListarEmprestimosAtivos(const struct Emprestimo *emprestimos, int numEmprestimos, const struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios) {
    printf("--- RELATORIO: EMPRESTIMOS ATIVOS ---\n");
    int count = 0;

    for (int i = 0; i < numEmprestimos; i++) {
        if (emprestimos[i].statusEmprestimo == true) {
            ExibirEmprestimoEmLista(&emprestimos[i], livros, numLivros, usuarios, numUsuarios, false); // false = não está em atraso (ainda)
            count++;
        }
    }

    if (count == 0) {
        printf("Nao ha emprestimos ativos no momento.\n");
    } else {
        printf("\nTotal de emprestimos ativos: %d\n", count);
    }
}

//Funções de Persistência de Dados
//Função para Salvamento de Livros
bool SalvarLivros(const struct Livro *livros, int numLivros) {
    FILE *f = fopen(ARQUIVO_LIVROS, "wb");
    if (f == NULL) return false;
    fwrite(livros, sizeof(struct Livro), numLivros, f);
    fclose(f);
    return true;
}

//Função para Carregamento de Livros
int CarregarLivros(struct Livro *livros) {
    FILE *f = fopen(ARQUIVO_LIVROS, "rb");
    if (f == NULL) return 0;

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    fseek(f, 0, SEEK_SET);

    int numLivros = tamanho / sizeof(struct Livro);
    if (numLivros > MAX_LIVROS) numLivros = MAX_LIVROS;

    fread(livros, sizeof(struct Livro), numLivros, f);
    fclose(f);
    return numLivros;
}

//Função para Salvamento de Usuários
bool SalvarUsuarios(const struct Usuario *usuarios, int numUsuarios) {
    FILE *f = fopen(ARQUIVO_USUARIOS, "wb");
    if (f == NULL) return false;
    fwrite(usuarios, sizeof(struct Usuario), numUsuarios, f);
    fclose(f);
    return true;
}

//Função para Carregamento de Usuários
int CarregarUsuarios(struct Usuario *usuarios) {
    FILE *f = fopen(ARQUIVO_USUARIOS, "rb");
    if (f == NULL) return 0;

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    fseek(f, 0, SEEK_SET);

    int numUsuarios = tamanho / sizeof(struct Usuario);
    if (numUsuarios > MAX_USUARIOS) numUsuarios = MAX_USUARIOS;

    fread(usuarios, sizeof(struct Usuario), numUsuarios, f);
    fclose(f);
    return numUsuarios;
}

//Função para Salvamento de Empréstimos
bool SalvarEmprestimos(const struct Emprestimo *emprestimos, int numEmprestimos) {
    FILE *f = fopen(ARQUIVO_EMPRESTIMOS, "wb");
    if (f == NULL) return false;
    fwrite(emprestimos, sizeof(struct Emprestimo), numEmprestimos, f);
    fclose(f);
    return true;
}

//Função para Carregamento de Empréstimos
int CarregarEmprestimos(struct Emprestimo *emprestimos) {
    FILE *f = fopen(ARQUIVO_EMPRESTIMOS, "rb");
    if (f == NULL) return 0;

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    fseek(f, 0, SEEK_SET);

    int numEmprestimos = tamanho / sizeof(struct Emprestimo);
    if (numEmprestimos > MAX_EMPRESTIMOS) numEmprestimos = MAX_EMPRESTIMOS;

    fread(emprestimos, sizeof(struct Emprestimo), numEmprestimos, f);
    fclose(f);
    return numEmprestimos;
}

//Função de Exibição de Simulação de Backup
//É feito um salvamento e carregamento de dados
//Porém, não um Backup propriamente dito
//Infelizmente, realizar esta tarefa foge das minhas
//Atuais capacidades
void CriarBackup() {
    printf("Backup dos arquivos de dados realizado (simulado).\n");
}

//Funções de Pesquisa e Relatórios
//Função de Busca de Livros com parcial de texto
void BuscaLivroPorString(const struct Livro *livros, int numLivros, const char *busca, bool buscarAutor, int *resultados) {
    char termo[100];
    strcpy(termo, busca);
    for (int i = 0; termo[i]; i++) { termo[i] = tolower((unsigned char)termo[i]); }
    *resultados = 0;
    printf("\n--- INICIANDO PESQUISA POR %s: '%s' ---\n", buscarAutor ? "AUTOR" : "TITULO", busca);

    for (int i = 0; i < numLivros; i++) {
        char alvo[100];
        if (buscarAutor) { strcpy(alvo, livros[i].autor); }
        else { strcpy(alvo, livros[i].titulo); }
        for (int j = 0; alvo[j]; j++) { alvo[j] = tolower((unsigned char)alvo[j]); }
        if (strstr(alvo, termo) != NULL) {
            ExibirLivroEmLista(&livros[i]);
            (*resultados)++;
        }
    }

    printf("\nTotal de resultados encontrados: %d\n", *resultados);
    if (*resultados == 0) { printf("Nenhum livro encontrado com o termo '%s'.\n", busca); }
}

//Função de Busca de Livros
void BuscarLivro(const struct Livro *livros, int numLivros) {
    int tipoBusca, codigo, idx, resultados;
    char termo[100];
    LimpaTela();
    printf("--- ESCOLHA O TIPO DE PESQUISA DE LIVRO ---\n");
    printf(" 1. Por Codigo\n");
    printf(" 2. Por Titulo (parcial)\n");
    printf(" 3. Por Autor (parcial)\n");
    printf(" 4. Busca Avancada (Titulo E Autor)\n");
    printf("\nOpcao: ");
    if (scanf("%d", &tipoBusca) != 1) { printf("\nOpcao invalida.\n"); LimpaBuffer(); return; }
    LimpaBuffer();
    switch (tipoBusca) {
        case 1:
            printf("Digite o Codigo do Livro: ");
            if (scanf("%d", &codigo) != 1) { printf("\nEntrada invalida.\n"); LimpaBuffer(); return; }
            LimpaBuffer();
            idx = BuscaLivroPorCodigo(livros, numLivros, codigo);
            if (idx != -1) {
                ExibirLivroEmLista(&livros[idx]);
            } else { printf("\nERRO: Livro com codigo %d nao encontrado.\n", codigo); }
            break;
        case 2:
            printf("Digite o Titulo (ou parte dele): ");
            fgets(termo, 100, stdin);
            removeNovaLinha(termo);
            BuscaLivroPorString(livros, numLivros, termo, false, &resultados);
            break;
        case 3:
            printf("Digite o Autor (ou parte dele): ");
            fgets(termo, 100, stdin);
            removeNovaLinha(termo);
            BuscaLivroPorString(livros, numLivros, termo, true, &resultados);
            break;
        case 4:
            BuscaAvancadaLivro(livros, numLivros);
            break;
        default:
            printf("\nOpcao de busca invalida.\n");
            break;
    }
}

//Função de Busca de Usuários
void BuscarUsuario(const struct Usuario *usuarios, int numUsuarios) {
    int tipoBusca, matricula, idx;
    char termo[100];

    printf("--- PESQUISA DE USUARIO ---\n");
    printf(" 1. Buscar por Matricula\n");
    printf(" 2. Buscar por Nome (parcial)\n");
    printf("\nOpcao: ");
    if (scanf("%d", &tipoBusca) != 1) { printf("\nEntrada invalida.\n"); LimpaBuffer(); return; }
    LimpaBuffer();

    if (tipoBusca == 1) {
        printf("Digite a Matricula do Usuario: ");
        if (scanf("%d", &matricula) != 1) { printf("\nEntrada invalida.\n"); LimpaBuffer(); return; }
        LimpaBuffer();
        idx = BuscaUsuarioPorMatricula(usuarios, numUsuarios, matricula);
        if (idx != -1) {
            ExibirUsuarioEmLista(&usuarios[idx]);
        } else { printf("\nERRO: Usuario com matricula %d nao encontrado.\n", matricula); }
    } else if (tipoBusca == 2) {
        printf("Digite o Nome (ou parte dele): ");
        fgets(termo, 100, stdin);
        removeNovaLinha(termo);

        char busca_lower[100];
        strcpy(busca_lower, termo);
        for (int i = 0; busca_lower[i]; i++) { busca_lower[i] = tolower((unsigned char)busca_lower[i]); }

        printf("\n--- INICIANDO PESQUISA POR NOME: '%s' ---\n", termo);
        int count = 0;
        for (int i = 0; i < numUsuarios; i++) {
            char nome_alvo[100];
            strcpy(nome_alvo, usuarios[i].nome);
            for (int j = 0; nome_alvo[j]; j++) { nome_alvo[j] = tolower((unsigned char)nome_alvo[j]); }

            if (strstr(nome_alvo, busca_lower) != NULL) {
                ExibirUsuarioEmLista(&usuarios[i]);
                count++;
            }
        }
        printf("\nTotal de resultados encontrados: %d\n", count);
        if (count == 0) { printf("Nenhum usuario encontrado com o termo '%s'.\n", termo); }
    } else {
        printf("\nOpcao de busca invalida.\n");
    }
}

//Função de Busca Avançada para Livros
void BuscaAvancadaLivro(const struct Livro *livros, int numLivros) {
    char titulo_termo[100], autor_termo[100];
    int resultados = 0;

    printf("--- BUSCA AVANCADA DE LIVRO (Titulo E Autor) ---\n");
    printf("Digite o Titulo (ou parte, ENTER para ignorar): ");
    fgets(titulo_termo, 100, stdin);
    removeNovaLinha(titulo_termo);

    printf("Digite o Autor (ou parte, ENTER para ignorar): ");
    fgets(autor_termo, 100, stdin);
    removeNovaLinha(autor_termo);

    for (int i = 0; titulo_termo[i]; i++) titulo_termo[i] = tolower((unsigned char)titulo_termo[i]);
    for (int i = 0; autor_termo[i]; i++) autor_termo[i] = tolower((unsigned char)autor_termo[i]);

    printf("\n--- RESULTADOS DA PESQUISA COMBINADA ---\n");

    for (int i = 0; i < numLivros; i++) {
        char titulo_alvo[100], autor_alvo[100];
        strcpy(titulo_alvo, livros[i].titulo);
        strcpy(autor_alvo, livros[i].autor);

        for (int j = 0; titulo_alvo[j]; j++) titulo_alvo[j] = tolower((unsigned char)titulo_alvo[j]);
        for (int j = 0; autor_alvo[j]; j++) autor_alvo[j] = tolower((unsigned char)autor_alvo[j]);

        bool match_titulo = (strlen(titulo_termo) == 0 || strstr(titulo_alvo, titulo_termo) != NULL);
        bool match_autor = (strlen(autor_termo) == 0 || strstr(autor_alvo, autor_termo) != NULL);

        if (match_titulo && match_autor) {
            ExibirLivroEmLista(&livros[i]);
            resultados++;
        }
    }

    printf("\nTotal de resultados encontrados: %d\n", resultados);
    if (resultados == 0) { printf("Nenhum livro encontrado com os criterios especificados.\n"); }
}

//Função para Relatório de Empréstimos em Atraso
void RelatorioAtrasos(const struct Emprestimo *emprestimos, int numEmprestimos, const struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios) {
    struct Data hoje = dataAtual();
    printf("--- RELATORIO: EMPRESTIMOS EM ATRASO (Data Atual: %02d/%02d/%04d) ---\n", hoje.dia, hoje.mes, hoje.ano);
    int count = 0;

    for (int i = 0; i < numEmprestimos; i++) {
        if (emprestimos[i].statusEmprestimo == true) {
            if (comparaDatas(emprestimos[i].dataPrevista, hoje) < 0) {
                ExibirEmprestimoEmLista(&emprestimos[i], livros, numLivros, usuarios, numUsuarios, true); // true = está em atraso
                count++;
            }
        }
    }

    if (count == 0) { printf("Nao ha emprestimos em atraso no momento.\n"); }
    else { printf("\nTotal de emprestimos em atraso: %d\n", count); }
}

//Cunção de Comparação de Contagens
//Usada no Relatório de Livros mais Emprestados
int comparaContagens(const void *a, const void *b) {
    return ((struct ContagemLivro *)b)->contagem - ((struct ContagemLivro *)a)->contagem;
}

//Função de Relatório de Livros mais Emprestados
void RelatorioMaisEmprestados(const struct Emprestimo *emprestimos, int numEmprestimos, const struct Livro *livros, int numLivros) {
    struct ContagemLivro contagens[MAX_LIVROS] = {0};
    int numContagens = 0;
    int rank = 1;

    for (int i = 0; i < numEmprestimos; i++) {
        int codigo = emprestimos[i].codigoLivro;
        bool encontrado = false;

        for (int j = 0; j < numContagens; j++) {
            if (contagens[j].codigoLivro == codigo) {
                contagens[j].contagem++;
                encontrado = true;
                break;
            }
        }

        if (!encontrado && numContagens < MAX_LIVROS) {
            contagens[numContagens].codigoLivro = codigo;
            contagens[numContagens].contagem = 1;
            numContagens++;
        }
    }

    qsort(contagens, numContagens, sizeof(struct ContagemLivro), comparaContagens);

    printf("--- RELATORIO: LIVROS MAIS EMPRESTADOS ---\n");

    for (int i = 0; i < numContagens; i++) {
        int idxLivro = BuscaLivroPorCodigo(livros, numLivros, contagens[i].codigoLivro);

        if (idxLivro != -1) {
            printf("\n--- RANK #%d ---\n", rank++);
            printf("Livro: %s\n", livros[idxLivro].titulo);
            printf("Codigo: %d\n", contagens[i].codigoLivro);
            printf("Total de Emprestimos: %d\n", contagens[i].contagem);
            printf("-----------------\n");
        }
    }

    if (numContagens == 0) { printf("Nenhum emprestimo registrado ainda.\n"); }
    else { printf("\nTotal de livros com emprestimos registrados: %d\n", numContagens); }
}

//Funções de Menus
//Menu Príncipal
void Menu() {
    printf("-----------------------------------------\n");
    printf("------------ BIBLIOTECA MAPA ------------\n");
    printf("-----------------------------------------\n");
    printf("\n 1. Cadastros (Livro, Usuario)\n");
    printf(" 2. Emprestimo\n");
    printf(" 3. Devolucao\n");
    printf(" 4. Renovacao de Emprestimo\n");
    printf(" 5. Pesquisas e Relatorios\n");
    printf(" 0. Encerrar Programa\n");
    printf("\n-----------------------------------------\n");
}

//SubMenu para Pesquisas e Relatórios
void MenuPesquisas(const struct Livro *livros, int numLivros, const struct Usuario *usuarios, int numUsuarios, const struct Emprestimo *emprestimos, int numEmprestimos) {
    int opcao;
    do {
        LimpaTela();
        printf("--- SUBMENU: PESQUISAS E RELATORIOS ---\n");
        printf("--- Pesquisa de Livros ---\n");
        printf(" 1. Pesquisar Livro\n");
        printf("--- Pesquisa de Usuarios ---\n");
        printf(" 2. Pesquisar Usuario\n");
        printf("--- Relatorios ---\n");
        printf(" 3. Listar Emprestimos ATIVOS\n");
        printf(" 4. Relatorio de Emprestimos em ATRASO\n");
        printf(" 5. Relatorio de Livros Mais Emprestados\n");
        printf(" 0. Voltar ao Menu Principal\n");
        printf("\nEscolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) { printf("\nOpcao invalida. Digite um numero.\n"); LimpaBuffer(); opcao = -1; continue; }
        LimpaBuffer();
        switch(opcao) {
            case 1: LimpaTela(); BuscarLivro(livros, numLivros); break;
            case 2: LimpaTela(); BuscarUsuario(usuarios, numUsuarios); break;
            case 3: LimpaTela(); ListarEmprestimosAtivos(emprestimos, numEmprestimos, livros, numLivros, usuarios, numUsuarios); break;
            case 4: LimpaTela(); RelatorioAtrasos(emprestimos, numEmprestimos, livros, numLivros, usuarios, numUsuarios); break;
            case 5: LimpaTela(); RelatorioMaisEmprestados(emprestimos, numEmprestimos, livros, numLivros); break;
            case 0: break;
            default: printf("\nOpcao invalida. Tente novamente.\n"); break;
        }
        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }
    } while (opcao != 0);
    LimpaTela();
}

//Abertura do Programa
int main(){
    struct Livro livros[MAX_LIVROS];
    int numLivros = 0;
    struct Usuario usuarios[MAX_USUARIOS];
    int numUsuarios = 0;
    struct Emprestimo emprestimos[MAX_EMPRESTIMOS];
    int numEmprestimos = 0;
    int opcao;

    printf("Inicializando sistema...\n");
    numLivros = CarregarLivros(livros);
    numUsuarios = CarregarUsuarios(usuarios);
    numEmprestimos = CarregarEmprestimos(emprestimos);

    printf("Dados carregados: Livros: %d, Usuarios: %d, Emprestimos: %d\n\n", numLivros, numUsuarios, numEmprestimos);

    do {
        Menu();
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf("\nOpcao invalida. Por favor, digite um numero.\n");
            LimpaBuffer();
            opcao = -1;
            continue;
        }
        LimpaBuffer();
        LimpaTela();

        switch(opcao){
            case 1: MenuCadastros(livros, &numLivros, usuarios, &numUsuarios); break;
            case 2: Emprestimo(emprestimos, &numEmprestimos, livros, numLivros, usuarios, numUsuarios); break;
            case 3: Devolucao(emprestimos, numEmprestimos, livros, numLivros); break;
            case 4: Renovacao(emprestimos, numEmprestimos, livros, numLivros); break;
            case 5: MenuPesquisas(livros, numLivros, usuarios, numUsuarios, emprestimos, numEmprestimos); break;
            case 0:
                printf("\n--- Encerrando o programa ---\n");
                SalvarLivros(livros, numLivros);
                SalvarUsuarios(usuarios, numUsuarios);
                SalvarEmprestimos(emprestimos, numEmprestimos);
                CriarBackup();
                printf("\nDados salvos e backup finalizado. Ate logo!\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                break;
        }
        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
            LimpaTela();
        }
    } while (opcao != 0);

    return 0;
}
//Encerramento do Programa
