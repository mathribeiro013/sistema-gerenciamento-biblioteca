![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)

# 📚 Sistema de Gerenciamento de Biblioteca

## Visão Geral

Projeto de sistema de gestão de acervo e empréstimos de uma biblioteca universitária, desenvolvido em **C puro**.

Este sistema vai além de um CRUD básico. Ele simula um ambiente transacional de back-end, focando em:

* **Lógica de Negócio** (cálculo de prazos, controle de inventário e validação de datas).
* **Persistência de Dados** (salvamento/carregamento de dados estruturados em arquivos binários).
* **Relatórios Gerenciais** para análise de acervo e fluxo de empréstimos.

---

## ⚙️ Funcionalidades Chave

O sistema oferece as seguintes operações, controlando as transações de empréstimos e o inventário:

### Transações e Lógica

* **Empréstimo:** Calcula a **Data Prevista de Devolução** (7 dias) e controla a disponibilidade do acervo, incluindo lógica de **ano bissexto** no cálculo das datas.
* **Devolução:** Finaliza a transação, retorna o exemplar ao acervo e emite um **AVISO DE ATRASO** se o prazo for excedido.
* **Relatórios Gerenciais:** Geração de rankings de **Livros Mais Emprestados** e listagem de **Empréstimos em Atraso**.

### Persistência de Dados

* Utiliza manipulação de arquivos binários para persistência de Livros, Usuários e Empréstimos.
* Implementa um sistema de **Backup** automático na saída (`.bak`).

---

## 📝 Guia Rápido de Uso do Sistema

### 1. MENU PRINCIPAL (Inicial)

Ao iniciar, escolha o número da operação desejada:

| OPÇÃO | AÇÃO PRINCIPAL |
| :---: | :--- |
| *1* | Cadastros (Livro, Usuário) |
| *2* | Empréstimo |
| *3* | Devolução |
| *4* | Renovação de Empréstimo |
| *5* | Pesquisas e Relatórios |
| *0* | Salvar e Encerrar Programa |

### 2. OPÇÕES DE CADASTRO (MENU 1)

Acessa a seção para registrar novos itens.

* **1. Cadastrar Livro:** O sistema gera o **Código** automaticamente. O livro é registrado como **Ativo**.
* **2. Cadastrar Usuário:** O sistema gera a **Matrícula** automaticamente (a partir de 1000).

### 3. OPERAÇÕES PRINCIPAIS

#### 3.1. EMPRÉSTIMO (OPÇÃO 2)

1.  Insira o **Código do Livro**. Deve haver cópias *disponíveis*.
2.  Insira a **Matrícula do Usuário**.
3.  O sistema calcula e exibe a **Data Prevista** de devolução (*7 dias* após a data atual).

#### 3.2. DEVOLUÇÃO (OPÇÃO 3)

1.  Insira o **Código do Livro** e a **Matrícula do Usuário**.
2.  O sistema busca o empréstimo ativo.
3.  Se a *Data Prevista* foi ultrapassada, o sistema emitirá um aviso de **ATRASO**.
4.  O status do empréstimo é alterado para **Devolvido** e a cópia do livro é retornada ao acervo.

#### 3.3. RENOVAÇÃO (OPÇÃO 4)

1.  Insira o **Código do Livro** e a **Matrícula do Usuário**.
2.  O prazo é estendido por mais 7 dias a partir da *Data Atual*.
3.  O sistema informa o **Novo Prazo** de devolução.

### 4. PESQUISAS E RELATÓRIOS (MENU 5)

Acessa as opções de consulta e análise de dados.

| OPÇÃO | TIPO DE PESQUISA / RELATÓRIO | COMO USAR |
| :---: | :--- | :--- |
| *1* | **Pesquisar Livro** | Busca por *Código*, *Título*, *Autor* (parcial) ou *Busca Avançada*. |
| *2* | **Pesquisar Usuário** | Busca por *Matrícula* ou *Nome* (parcial). |
| *3* | **Empréstimos ATIVOS** | Lista todos os empréstimos não devolvidos. |
| *4* | **Empréstimos em ATRASO** | Lista empréstimos ATIVOS cuja data de devolução prevista já passou. |
| *5* | **Livros Mais Emprestados** | Exibe um ranking (ordem decrescente) dos livros mais populares. |

### 5. ENCERRAMENTO (OPÇÃO 0)

* Todos os dados são **salvos** nos arquivos .txt.
* É executado o processo de **Backup** para os arquivos .bak.
* O programa é finalizado com segurança.

---

## 🛠️ Detalhes Técnicos e Modelagem

O sistema é modelado com `structs` que simulam entidades de banco de dados, estabelecendo relações entre as chaves primárias.

| Struct | Campos Chave | Propósito |
| :--- | :--- | :--- |
| **`struct Livro`** | `codigo` (ID), `disponiveis` | Controle de inventário. |
| **`struct Usuario`** | `matricula` (ID) | Chave primária de acesso. |
| **`struct Emprestimo`** | `codigoLivro`, `matriculaUsuario`, `dataPrevista` | Entidade transacional que gerencia os prazos. |

## ▶️ Como Compilar e Rodar

Para executar o projeto em seu ambiente, você precisa de um compilador C (como GCC, MinGW, ou usar uma IDE como Code::Blocks).

1.  **Clone o Repositório e Acesse a Pasta:**
    ```bash
    git clone [https://github.com/mathribeiro013/sistema-gerenciamento-biblioteca](https://github.com/mathribeiro013/sistema-gerenciamento-biblioteca)
    cd sistema-gerenciamento-biblioteca
    ```
2.  **Renomeie o Arquivo:** Certifique-se de que o seu código principal (`MAPA.txt`) foi renomeado para, por exemplo, `biblioteca.c`.
3.  **Compile o Código:**
    ```bash
    gcc -o biblioteca biblioteca.c -std=c99
    ```
4.  **Execute o Programa:**
    ```bash
    ./biblioteca
    ```
    *(Em sistemas Windows, use `biblioteca.exe`)*
