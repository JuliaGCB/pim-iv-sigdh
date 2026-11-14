# SIGDH — PIM IV

O SIGDH (Sistema Integrado de Gestão e Distribuição Hospitalar) é um protótipo acadêmico em C para a M Medical Científica, distribuidora fictícia de produtos médico-hospitalares. O programa roda no terminal e acompanha o fluxo do estoque, da entrada por lote ao registro de um pedido.

## O que o programa faz

- Cadastra produtos e clientes e verifica códigos duplicados em cada cadastro.
- Registra entradas de estoque com número do lote do fabricante, validade e saldo.
- Registra pedidos de um cliente para um lote por vez. Verifica a validade do lote na data informada no pedido e baixa o estoque quando há saldo suficiente.
- Lista produtos, clientes, lotes e pedidos e permite consultar um produto pelo código.
- Exporta a lista de produtos para `produtos.txt` e os pedidos para `pedidos.csv`, com código e nome do cliente, lote, quantidade e data.

## Como executar no Windows

1. Tenha o SQL Server Express, o SSMS, o **Microsoft ODBC Driver 18 for SQL Server** e o Code::Blocks com GCC. Use um driver com a mesma arquitetura do executável.
2. No SSMS, conecte a `localhost\SQLEXPRESS` com **Autenticação do Windows**. Abra `01-criar-banco.sql` e execute o arquivo inteiro uma vez para criar o banco `SIGDH`, as quatro tabelas e o procedimento `RegistrarPedido`. Para começar com as tabelas vazias, execute somente o script de criação. O `02-demonstracao.sql` é opcional e insere dados fictícios pelo SSMS.
3. No Code::Blocks, abra o projeto SIGDH ou crie um projeto **Console application** em **C** e copie o código para `main.c`.
4. Em **Project → Build options → Linker settings → Link libraries**, selecione o projeto e adicione `odbc32`.
5. Salve, pressione **F9** e escolha a opção **12** para testar a conexão. Se sua instância tiver outro nome, ajuste o campo `Server` na configuração de conexão do código.

A configuração do protótipo usa a conta do Windows e aceita o certificado da instância local. A autenticação, a criptografia e a validação de certificados são configuradas pelo driver ODBC.

Digite as datas com oito dígitos no formato `DDMMAAAA`. Por exemplo, `08112026` aparece como `08/11/2026` nas consultas e na exportação dos pedidos.

## Teste de exemplo

Use códigos livres no banco e anote o número interno do lote que o programa retornar.

| Opção | Dados para o teste |
| --- | --- |
| **1 — Cadastrar produto** | Código `100` e nome `Luvas` |
| **5 — Registrar entrada de estoque** | Produto `100`, quantidade `30`, lote do fabricante `LV-2026-A` e validade `08112027` |
| **7 — Cadastrar cliente** | Código `200` e nome `Clínica Central` |
| **9 — Registrar pedido** | Cliente `200`, número interno do lote criado, data `08112026` e quantidade `5` |
| **6 — Consultar estoque por lote** | O lote criado deve ter saldo de `25` unidades |
| **10 — Consultar pedidos** | Deve aparecer um pedido de `5` unidades com data `08/11/2026` |
| **11 — Exportar pedidos para planilha** | O arquivo `pedidos.csv` deve conter o pedido e o nome do cliente |

Repita a opção **9** para o mesmo lote com **26 unidades**. A tentativa deve ser recusada. Na opção **6**, o saldo deve continuar em **25** e, na **10**, deve aparecer apenas o pedido válido desse teste. Tente cadastrar novamente o mesmo código de cliente pela opção **7** para verificar o bloqueio de duplicatas.

Feche e reabra o programa. As opções **6** e **10** devem recuperar o saldo e o pedido gravados no SQL Server.

Os arquivos exportados ficam na pasta de execução do programa. O CSV usa UTF-8 com BOM para preservar os acentos. Ao importar, escolha ponto e vírgula como separador se os dados aparecerem em uma única coluna.

## Estado do projeto

Produtos, clientes, lotes e pedidos ficam gravados no SQL Server entre execuções. Ao registrar um pedido, o procedimento `RegistrarPedido` grava o pedido e baixa o saldo do lote na mesma transação. Os arquivos TXT e CSV servem como exportações para consulta externa.

## Referências

### Programação e planejamento

- **Programação Estruturada em C — unidades I a IV.** Material didático da disciplina usado para estudar estruturas de controle, vetores, matrizes, funções, ponteiros e arquivos.
- **GNU Project.** [The GNU C Reference Manual](https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html), versão 0.2.5. Referência para estruturas de controle, vetores, matrizes e funções.
- **GNU Project.** [The GNU C Library Reference Manual](https://www.gnu.org/s/libc/manual/pdf/libc.pdf), capítulo *Input/Output on Streams*. Referência para `fopen`, `fprintf` e `fclose`, usadas nas exportações.
- **PARNAS, David Lorge.** [On the Criteria to Be Used in Decomposing Systems into Modules](https://prl.khoury.northeastern.edu/img/p-tr-1971.pdf). Relatório técnico, Carnegie Mellon University, 1971. Leitura sobre a divisão de responsabilidades no código.
- **SCHWABER, Ken; SUTHERLAND, Jeff.** [The Scrum Guide](https://scrumguides.org/scrum-guide.html), 2020. Referência para o planejamento ágil do PIM IV.

### Integração com o banco

- **MICROSOFT.** [Desenvolver aplicações em C e C++ com o driver ODBC](https://learn.microsoft.com/pt-br/sql/connect/odbc/develop-cpp-applications?view=sql-server-ver17). Documentação para conectar o programa em C ao SQL Server.
- **HAERDER, Theo; REUTER, Andreas.** [Principles of Transaction-Oriented Database Recovery](https://doi.org/10.1145/289.291). *ACM Computing Surveys*, v. 15, n. 4, p. 287–317, 1983. Referência para planejar o registro do pedido e a baixa do estoque na mesma transação.
