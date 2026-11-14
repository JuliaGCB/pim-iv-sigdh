#include <stdio.h>
#include <locale.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>

// Envia os valores digitados separadamente do SQL, na ordem dos sinais ?.
static int executarSQL(const char *sql, char *valores[], int total,
                       const char *colunas[], int quantidadeColunas, FILE *arquivo)
{
    SQLCHAR configuracao[] =
        "Driver={ODBC Driver 18 for SQL Server};"
        "Server=localhost\\SQLEXPRESS;Database=SIGDH;"
        "Trusted_Connection=yes;Encrypt=yes;TrustServerCertificate=yes;";
    SQLHENV ambiente = SQL_NULL_HENV;
    SQLHDBC conexao = SQL_NULL_HDBC;
    SQLHSTMT consulta = SQL_NULL_HSTMT;
    SQLLEN tamanhos[4];
    int conectado = 0, sucesso = 0, linhas = 0;
    SQLSMALLINT tipoErro = SQL_HANDLE_ENV;
    SQLHANDLE origemErro = SQL_NULL_HANDLE;

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &ambiente)))
        goto finalizar;
    origemErro = ambiente;
    if (!SQL_SUCCEEDED(SQLSetEnvAttr(ambiente, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)) ||
        !SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC, ambiente, &conexao)))
        goto finalizar;

    tipoErro = SQL_HANDLE_DBC;
    origemErro = conexao;
    SQLSetConnectAttr(conexao, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
    if (!SQL_SUCCEEDED(SQLDriverConnectA(conexao, NULL, configuracao, SQL_NTS,
                                         NULL, 0, NULL, SQL_DRIVER_NOPROMPT)))
        goto finalizar;
    conectado = 1;

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, conexao, &consulta)))
        goto finalizar;
    tipoErro = SQL_HANDLE_STMT;
    origemErro = consulta;

    for (int i = 0; i < total; i++) {
        tamanhos[i] = SQL_NTS;
        if (!SQL_SUCCEEDED(SQLBindParameter(consulta, i + 1, SQL_PARAM_INPUT,
                                            SQL_C_CHAR, SQL_WVARCHAR, 59, 0,
                                            valores[i], 0, &tamanhos[i])))
            goto finalizar;
    }

    if (!SQL_SUCCEEDED(SQLExecDirectA(consulta, (SQLCHAR *)sql, SQL_NTS)))
        goto finalizar;

    if (quantidadeColunas > 0) {
        SQLRETURN resultado;
        while (SQL_SUCCEEDED(resultado = SQLFetch(consulta))) {
            linhas++;
            for (int i = 0; i < quantidadeColunas; i++) {
                char valor[256];
                SQLLEN tamanho;
                if (arquivo != NULL && quantidadeColunas == 6) {
                    // Le o texto Unicode do banco e converte para UTF-8 no CSV.
                    SQLWCHAR texto[256];
                    if (SQLGetData(consulta, i + 1, SQL_C_WCHAR, texto,
                                   sizeof texto, &tamanho) != SQL_SUCCESS ||
                        WideCharToMultiByte(CP_UTF8, 0, (const WCHAR *)texto, -1,
                                            valor, sizeof valor, NULL, NULL) == 0)
                        goto finalizar;
                } else if (!SQL_SUCCEEDED(SQLGetData(consulta, i + 1, SQL_C_CHAR, valor,
                                                     sizeof valor, &tamanho))) {
                    goto finalizar;
                }
                if (arquivo == NULL) {
                    printf("%s: %s\n", colunas[i], valor);
                } else if (quantidadeColunas == 2) {
                    if (i > 0) fputs(" - ", arquivo);
                    fputs(valor, arquivo);
                } else {
                    if (i > 0) fputc(';', arquivo);
                    fputc('"', arquivo);
                    if (valor[0] && (valor[0] == '=' || valor[0] == '+' ||
                                     valor[0] == '-' || valor[0] == '@'))
                        fputc('\'', arquivo);
                    for (int j = 0; valor[j]; j++) {
                        if (valor[j] == '"') fputc('"', arquivo);
                        fputc(valor[j], arquivo);
                    }
                    fputc('"', arquivo);
                }
            }
            fputc('\n', arquivo == NULL ? stdout : arquivo);
        }
        if (resultado != SQL_NO_DATA) goto finalizar;
        if (linhas == 0 && arquivo == NULL) puts("Nenhum registro encontrado.");
    }
    sucesso = 1;

finalizar:
    if (!sucesso) {
        SQLCHAR estado[6], mensagem[512];
        puts("O SQL Server nao concluiu a operacao.");
        if (origemErro != SQL_NULL_HANDLE &&
            SQL_SUCCEEDED(SQLGetDiagRecA(tipoErro, origemErro, 1, estado, NULL,
                                         mensagem, sizeof mensagem, NULL)))
            printf("%s: %s\n", (char *)estado, (char *)mensagem);
    }
    if (consulta != SQL_NULL_HSTMT) SQLFreeHandle(SQL_HANDLE_STMT, consulta);
    if (conectado) SQLDisconnect(conexao);
    if (conexao != SQL_NULL_HDBC) SQLFreeHandle(SQL_HANDLE_DBC, conexao);
    if (ambiente != SQL_NULL_HENV) SQLFreeHandle(SQL_HANDLE_ENV, ambiente);
    return sucesso;
}

static int lerInteiro(const char *pergunta, int *valor)
{
    printf("%s", pergunta);
    if (scanf("%d", valor) == 1) return 1;
    puts("Digite um numero inteiro.");
    int caractere;
    do {
        caractere = getchar();
    } while (caractere != '\n' && caractere != EOF);
    return 0;
}

//retorna 1 se a data existe no calendário, ou 0 se for inválido
int dataValida(int data){
    int ano = data / 10000;
    int mes = (data/100) % 100;
    int dia = data % 100;
    int diasPorMes[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (ano < 1000 || ano > 9999 || mes < 1 || mes > 12 || dia < 1 ){
        return 0;
    }

    int anoBissexto = (ano % 400 == 0) || (ano % 4 == 0 && ano % 100 != 0);

    if(anoBissexto){
        diasPorMes[1] = 29;
    }

    return dia <= diasPorMes[mes - 1];

}

// Le DDMMAAAA e devolve AAAAMMDD. Retorna 0 para uma data invalida.
int lerData(void)
{
    char texto[9]; // Oito digitos e o '\0' que termina o texto.
    int numero = 0;

    if (scanf("%8s", texto) != 1) {
        return 0;
    }

    // Se sobrou algo na linha, descarta a entrada e rejeita a data.
    int caractere = getchar();
    if (caractere != '\n' && caractere != EOF) {
        while (caractere != '\n' && caractere != EOF) {
            caractere = getchar();
        }
        return 0;
    }

    for (int i = 0; i < 8; i++) {
        if (texto[i] < '0' || texto[i] > '9') {
            return 0;
        }

        // Converte cada caractere em um digito e monta o numero.
        numero = numero * 10 + (texto[i] - '0');
    }

    int dia = numero / 1000000;
    int mes = (numero / 10000) % 100;
    int ano = numero % 10000;

    // Guarda primeiro o ano para comparar as datas em ordem cronologica.
    int data = ano * 10000 + mes * 100 + dia;

    if (dataValida(data) == 0) {
        return 0;
    }

    return data;
}

int main(void)
{
    setlocale(LC_ALL, "Portuguese");

    int opcao;
    const char *produtos[] = {"Codigo do produto", "Nome"};
    const char *clientes[] = {"Codigo do cliente", "Nome"};
    const char *lotes[] = {"Lote interno", "Codigo do produto", "Lote do fabricante",
                           "Validade", "Saldo"};
    const char *pedidos[] = {"Pedido", "Codigo do cliente", "Nome do cliente",
                             "Lote", "Quantidade", "Data"};
    const char *sqlProdutos = "SELECT Codigo, Nome FROM dbo.Produto ORDER BY Codigo";
    const char *sqlPedidos =
        "SELECT p.IdPedido, p.CodigoCliente, c.Nome, p.IdLote, p.Quantidade, "
        "CONVERT(char(10), p.DataPedido, 103) FROM dbo.Pedido p "
        "JOIN dbo.Cliente c ON c.Codigo = p.CodigoCliente ORDER BY p.IdPedido";

    do {
        printf("\n========================================\n");
        printf("Bem vindo a M Medical Cientifica\n");
        printf("Sistema de estoque e pedidos\n\n");
        printf("1- Cadastrar produto\n");
        printf("2- Consultar produtos\n");
        printf("3- Exportar lista de produtos\n");
        printf("4- Consultar produto por código\n");
        printf("5- Registrar entrada de estoque\n");
        printf("6- Consultar estoque por lote\n");
        printf("7- Cadastrar cliente\n");
        printf("8- Consultar clientes\n");
        printf("9- Registrar pedido\n");
        printf("10- Consultar pedidos\n");
        printf("11- Exportar pedidos para planilha\n");
        printf("12- Testar conexao com o SQL Server\n");
        printf("0- Sair\n\n");

        printf("Digite uma opção: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Digite uma opção numérica. Encerrando o programa.\n");
            break; // Aqui o break encerra o do...while.
        }

        printf("\n========================================\n");

        switch (opcao) {
            case 1:
            case 7: {
                int codigo;
                char codigoTexto[16], nome[60];
                if (!lerInteiro("Informe o codigo: ", &codigo)) break;
                printf("Informe o nome: ");
                if (scanf(" %59[^\n]", nome) != 1) break;
                snprintf(codigoTexto, sizeof codigoTexto, "%d", codigo);
                char *valores[] = {codigoTexto, nome};
                const char *sql = opcao == 1
                    ? "INSERT INTO dbo.Produto (Codigo, Nome) VALUES (?, ?)"
                    : "INSERT INTO dbo.Cliente (Codigo, Nome) VALUES (?, ?)";
                if (executarSQL(sql, valores, 2, NULL, 0, NULL))
                    puts(opcao == 1 ? "Produto cadastrado." : "Cliente cadastrado.");
                break;
            }

            case 2:
                executarSQL(sqlProdutos, NULL, 0, produtos, 2, NULL);
                break;

            case 3:
            case 11: {
                const char *nomeArquivo = opcao == 3 ? "produtos.txt" : "pedidos.csv";
                FILE *arquivo = fopen(nomeArquivo, "w");
                if (arquivo == NULL) {
                    puts("Nao foi possivel abrir o arquivo.");
                    break;
                }
                int ok;
                if (opcao == 3) {
                    ok = executarSQL(sqlProdutos, NULL, 0, produtos, 2, arquivo);
                } else {
                    // O BOM ajuda o editor de planilhas a reconhecer UTF-8.
                    fputs("\xEF\xBB\xBF", arquivo);
                    fputs("Pedido;Codigo do cliente;Nome do cliente;Lote;Quantidade;Data\n", arquivo);
                    ok = executarSQL(sqlPedidos, NULL, 0, pedidos, 6, arquivo);
                }
                if (ferror(arquivo)) ok = 0;
                if (fclose(arquivo) != 0) ok = 0;
                if (ok) printf("Arquivo exportado: %s\n", nomeArquivo);
                else puts("A exportacao falhou.");
                break;
            }

            case 4: {
                int codigo;
                char codigoTexto[16];
                if (!lerInteiro("Informe o codigo do produto: ", &codigo)) break;
                snprintf(codigoTexto, sizeof codigoTexto, "%d", codigo);
                char *valores[] = {codigoTexto};
                executarSQL("SELECT Codigo, Nome FROM dbo.Produto WHERE Codigo = ?",
                            valores, 1, produtos, 2, NULL);
                break;
            }

            case 5: {
                int codigo, quantidade;
                char codigoTexto[16], quantidadeTexto[16], validadeTexto[16], numeroLote[30];
                if (!lerInteiro("Informe o codigo do produto: ", &codigo) ||
                    !lerInteiro("Informe a quantidade recebida: ", &quantidade)) break;
                if (quantidade <= 0) {
                    puts("A quantidade deve ser maior que zero.");
                    break;
                }
                printf("Informe o numero do lote do fabricante: ");
                if (scanf("%29s", numeroLote) != 1) break;
                printf("Informe a validade (DDMMAAAA): ");
                int validade = lerData();
                if (validade == 0) {
                    puts("Validade invalida. Informe oito digitos, como 12122027.");
                    break;
                }
                snprintf(codigoTexto, sizeof codigoTexto, "%d", codigo);
                snprintf(quantidadeTexto, sizeof quantidadeTexto, "%d", quantidade);
                snprintf(validadeTexto, sizeof validadeTexto, "%08d", validade);
                char *valores[] = {codigoTexto, numeroLote, validadeTexto, quantidadeTexto};
                // A chave estrangeira exige que o produto exista no banco.
                if (executarSQL("INSERT INTO dbo.Lote (CodigoProduto, NumeroFabricante, Validade, Saldo) "
                                "OUTPUT INSERTED.IdLote VALUES (?, ?, CONVERT(date, ?, 112), ?)",
                                valores, 4, lotes, 1, NULL))
                    puts("Entrada registrada no SQL Server.");
                break;
            }

            case 6:
                executarSQL("SELECT IdLote, CodigoProduto, NumeroFabricante, "
                            "CONVERT(char(10), Validade, 103), Saldo FROM dbo.Lote ORDER BY IdLote",
                            NULL, 0, lotes, 5, NULL);
                break;

            case 8:
                executarSQL("SELECT Codigo, Nome FROM dbo.Cliente ORDER BY Codigo",
                            NULL, 0, clientes, 2, NULL);
                break;

            case 9: {
                int cliente, lote, quantidade;
                char clienteTexto[16], loteTexto[16], quantidadeTexto[16], dataTexto[16];
                if (!lerInteiro("Informe o codigo do cliente: ", &cliente) ||
                    !lerInteiro("Informe o numero interno do lote: ", &lote)) break;
                printf("Informe a data do pedido (DDMMAAAA): ");
                int data = lerData();
                if (data == 0) {
                    puts("Data do pedido invalida.");
                    break;
                }
                if (!lerInteiro("Informe a quantidade do pedido: ", &quantidade)) break;
                snprintf(clienteTexto, sizeof clienteTexto, "%d", cliente);
                snprintf(loteTexto, sizeof loteTexto, "%d", lote);
                snprintf(quantidadeTexto, sizeof quantidadeTexto, "%d", quantidade);
                snprintf(dataTexto, sizeof dataTexto, "%08d", data);
                char *valores[] = {clienteTexto, loteTexto, quantidadeTexto, dataTexto};
                // O procedimento grava o pedido e baixa o saldo na mesma transacao.
                if (executarSQL("{CALL dbo.RegistrarPedido (?, ?, ?, ?)}",
                                valores, 4, NULL, 0, NULL))
                    puts("Pedido registrado no SQL Server. Consulte o novo saldo na opcao 6.");
                break;
            }

            case 10:
                executarSQL(sqlPedidos, NULL, 0, pedidos, 6, NULL);
                break;

            case 12: {
                const char *teste[] = {"Conexao OK"};
                executarSQL("SELECT 1", NULL, 0, teste, 1, NULL);
                break;
            }

            case 0:
                printf("Você decidiu sair do sistema.\n");
                break;

            default:
                printf("Opção inválida. Digite um número de 0 a 12.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}
