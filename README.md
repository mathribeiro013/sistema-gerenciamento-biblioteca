![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)

# 📚 Sistema de Gerenciamento de Biblioteca

## Visão Geral

Projeto de sistema de gestão de acervo e empréstimos de uma biblioteca universitária, desenvolvido em **C puro**.

Este sistema vai além de um CRUD básico. Ele simula um ambiente transacional de back-end, focando em:

* **Lógica de Negócio** (cálculo de prazos, controle de inventário e validação de datas).
* **Persistência de Dados** (salvamento/carregamento de dados estruturados).
* **Relatórios Gerenciais** para análise de acervo e fluxo de empréstimos.

## ⚙️ Funcionalidades Chave

O sistema opera com um menu interativo, oferecendo as seguintes capacidades:

### Gerenciamento de Acervo e Usuários (CRUD)

* **Livros:** Cadastro, alteração, exclusão (lógica) e listagem do acervo.
* **Usuários:** Cadastro, alteração, exclusão (lógica) e listagem dos membros.

### Transações de Empréstimo (Lógica de Negócio)

* **Empréstimo:** Registra a transação, calcula a **Data Prevista de Devolução** (7 dias a partir da data atual) e decrementa as cópias disponíveis no inventário.
* **Devolução:** Finaliza o empréstimo, retorna a cópia ao acervo e aplica uma validação de prazo, emitindo um **AVISO DE ATRASO** se a Data Prevista for excedida.
* **Renovação:** Estende o prazo de um empréstimo ativo por mais 7 dias.

### Pesquisas e Relatórios Gerenciais

* **Pesquisas:** Busca de Livros por Código, Título, Autor (parcial) e Busca Avançada (combinação Título **E** Autor).
* **Empréstimos em Atraso:** Lista todos os empréstimos ativos cujo prazo de devolução já expirou.
* **Livros Mais Emprestados:** Gera um ranking dos títulos mais populares do acervo, essencial para gestão de inventário.

## 🧱 Estrutura de Dados e Modelagem

O código utiliza `structs` para modelar as entidades, simulando tabelas e relacionamentos de um banco de dados.

| Struct | Campos Chave | Detalhe de Implementação |
| :--- | :--- | :--- |
| **`struct Data`** | `dia`, `mes`, `ano` | Usada para validação de datas e cálculo de prazos (inclui lógica de ano bissexto). |
| **`struct Livro`** | `codigo`, `disponiveis`, `totalExemplares` | Controle de inventário e status (ativo/inativo). |
| **`struct Usuario`** | `matricula` (auto-incremento), `status` | Status para exclusão lógica de usuários. |
| **`struct Emprestimo`** | `codigoLivro`, `matriculaUsuario`, `dataPrevista` | Atua como a tabela transacional, conectando Livro e Usuário através de seus IDs. |

### Persistência

Os dados (Livros, Usuários e Empréstimos) são salvos e carregados utilizando **arquivos binários** (`.txt`), garantindo que o estado do sistema seja mantido entre execuções.

## ▶️ Como Compilar e Rodar

Para executar o projeto em seu ambiente, você precisa de um compilador C (como GCC, MinGW, ou usar uma IDE como Code::Blocks).

1.  **Clone o Repositório:**
    ```bash
    git clone [INSIRA A URL DO SEU REPOSITÓRIO AQUI]
    cd nome-do-seu-repositorio
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
