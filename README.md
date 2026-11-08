# SIGDH — PIM IV

O SIGDH (Sistema Integrado de Gestão e Distribuição Hospitalar) é um protótipo acadêmico em C para a M Medical Científica, distribuidora fictícia de produtos médico-hospitalares. O programa roda no terminal e acompanha o fluxo do estoque, da entrada por lote ao registro de um pedido.

## O que o programa faz

- Cadastra produtos e clientes e verifica códigos duplicados em cada cadastro.
- Registra entradas de estoque com número do lote do fabricante, validade e saldo.
- Registra pedidos de um cliente para um lote por vez. Verifica a validade do lote na data informada no pedido e baixa o estoque quando há saldo suficiente.
- Lista produtos, clientes, lotes e pedidos e permite consultar um produto pelo código.
- Exporta a lista de produtos para `produtos.txt` e os pedidos para `pedidos.csv`, com código e nome do cliente, lote, quantidade e data.

## Como executar no Code::Blocks

1. Baixe o arquivo `.c` deste repositório.
2. No Code::Blocks com GCC, crie um projeto **Console application** e selecione **C**.
3. Abra o `main.c` criado pelo projeto e substitua seu conteúdo pelo código baixado.
4. Salve e pressione **F9** para compilar e executar.

Digite as datas com oito dígitos no formato `DDMMAAAA`. Por exemplo, `08112026` aparece como `08/11/2026` nas consultas e na exportação dos pedidos.

## Teste de exemplo

Faça os passos na mesma execução do programa.

| Opção | Dados para o teste |
| --- | --- |
| **1 — Cadastrar produto** | Código `100` e nome `Luvas` |
| **5 — Registrar entrada de estoque** | Produto `100`, quantidade `30`, lote do fabricante `LV-2026-A` e validade `08112027` |
| **7 — Cadastrar cliente** | Código `200` e nome `Clínica Central` |
| **9 — Registrar pedido** | Cliente `200`, lote interno `1`, data `08112026` e quantidade `5` |
| **6 — Consultar estoque por lote** | O lote interno `1` deve ter saldo de `25` unidades |
| **10 — Consultar pedidos** | Deve aparecer um pedido de `5` unidades com data `08/11/2026` |
| **11 — Exportar pedidos para planilha** | O arquivo `pedidos.csv` deve conter o pedido e o nome do cliente |

Os arquivos exportados ficam na pasta de execução do programa. Ao importar o CSV em um editor de planilhas, selecione ponto e vírgula como separador se os dados aparecerem em uma única coluna.

## Estado do projeto

Esta versão mantém os registros na memória durante a execução. Ao encerrar o programa, os cadastros são reiniciados. Os arquivos TXT e CSV servem como exportações para consulta externa.

O protótipo comporta até 5 produtos, 5 clientes, 10 lotes e 10 pedidos por execução. A integração com o SQL Server está planejada para salvar e recuperar os registros entre execuções.
