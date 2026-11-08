#include <stdio.h>
#include <locale.h>

// Retorna a posição do produto ou -1 se o código não existir.
int buscarProduto(int codigos[], int quantidadeProdutos, int codigoBuscado)
{
    for (int i = 0; i < quantidadeProdutos; i++) {
        if (codigos[i] == codigoBuscado) {
            return i;
        }
    }

    return -1;
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

void mostrarData(int data)
{
    int ano = data / 10000;
    int mes = (data / 100) % 100;
    int dia = data % 100;

    printf("%02d/%02d/%04d\n", dia, mes, ano);
}

int main(void)
{
    setlocale(LC_ALL, "Portuguese");

    int opcao;
    int codigos[5];
    char nomes[5][60];

    // Também indica a próxima posição livre nos vetores.
    int quantidadeProdutos = 0;

    //Lotes dos produtos
    int codigosProdutosDosLotes[10];
    int saldosDosLotes[10];
    int quantidadeLotes = 0;
    char numerosLotes[10][30];
    int validadesDosLotes[10];

    //Cadastro dos clientes
    int codigosClientes[5];
    char nomesClientes[5][60];
    int quantidadeClientes = 0;

    // Registro dos pedidos
    // O mesmo índice liga o cliente, o lote, a quantidade e a data de cada pedido.
    int clientesDosPedidos[10];
    int lotesDosPedidos[10];
    int quantidadesDosPedidos[10];
    int datasDosPedidos[10];
    int quantidadePedidos = 0;

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
        printf("0- Sair\n\n");

        printf("Digite uma opção: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Digite uma opção numérica. Encerrando o programa.\n");
            break; // Aqui o break encerra o do...while.
        }

        printf("\n========================================\n");

        switch (opcao) {
            case 1:
                printf("Cadastro de produto selecionado.\n\n");

                if (quantidadeProdutos == 5) {
                    printf("Cadastro cheio. Limite de 5 produtos.\n");
                    break;
                }

                printf("Informe o código do produto: ");

                if (scanf("%d", &codigos[quantidadeProdutos]) == 1) {
                    if (buscarProduto(codigos, quantidadeProdutos,
                                      codigos[quantidadeProdutos]) != -1) {
                        printf("Esse código já está cadastrado.\n");
                        break; // Sai do switch e volta ao menu.
                    }

                    printf("Informe o nome do produto: ");

                    // Lê até 59 caracteres e aceita espaços entre palavras.
                    if (scanf(" %59[^\n]", nomes[quantidadeProdutos]) == 1) {
                        quantidadeProdutos++;
                        printf("Produto cadastrado.\n");
                    }
                } else {
                        printf("Código inválido. Digite um número inteiro.\n");

                        // Descarta o que foi digitado até o Enter ou o fim da entrada.
                        int caractere;
                        do {
                            caractere = getchar();
                        } while (caractere != '\n' && caractere != EOF);
                    }

                break;

            case 2:
                printf("Consulta de produtos\n\n");

                if (quantidadeProdutos == 0) {
                    printf("Nenhum produto cadastrado.\n");
                } else {
                    // O mesmo índice liga o código ao nome do produto.
                    for (int i = 0; i < quantidadeProdutos; i++) {
                        printf("Código do produto: %d\n", codigos[i]);
                        printf("Nome do produto: %s\n\n", nomes[i]);
                    }
                }
                break;

            case 3:
                printf("Exportar lista de produtos.\n\n");
                 {
                if(quantidadeProdutos == 0){
                    printf("Nenhum produto para exportar.\n");
                    break;
                }

                // "w" cria o arquivo ou substitui o conteúdo da exportação anterior.
                FILE *arquivo = fopen("produtos.txt", "w");

                if(arquivo == NULL){
                    printf("Não foi possível abrir o arquivo.\n");
                    break;
                }

                int gravacaoOK = 1;

                for(int i = 0; i < quantidadeProdutos; i++){
                    if(fprintf(arquivo, "%d - %s\n", codigos [i], nomes[i]) < 0){
                        gravacaoOK = 0;
                        break;
                    }
                }

                // Fecha o arquivo e verifica se os dados foram gravados.

                if(fclose(arquivo) != 0){
                    gravacaoOK = 0;
                }

                if(gravacaoOK == 1){
                    printf("Lista exportada para produtos.txt.\n");
                } else{
                    printf("Erro ao gravar a lista de produtos.\n");
                }

                break;
            }

            case 4:
                printf("Consultar produto por código.\n\n");
                {
                int codigoBuscado;

                printf("Informe o código do produto: ");
                if (scanf("%d", &codigoBuscado) != 1){
                    printf("Código invãlido. Digite um número inteiro.\n");

                    int caractere;
                    do{
                        caractere = getchar();
                    }while(caractere != '\n' && caractere != EOF);
                    break;
                }

                int posicao = buscarProduto(codigos, quantidadeProdutos, codigoBuscado);

                if(posicao == -1){
                    printf("Produto não encontrado.\n");
                }else{
                    printf("código do produto: %d\n", codigos[posicao]);
                    printf("Nome do produto: %s\n", nomes[posicao]);
                }



            break;
            }

            case 5:
                printf("Registrar entrada de estoque.\n\n");

                {
                if(quantidadeLotes == 10){
                    printf("Limite de lotes atingido.\n");
                    break;
                }

                int codigoProduto;
                int quantidadeRecebida;

                printf("Informe o código do produto: ");
                if(scanf("%d", &codigoProduto) != 1){ //Verificação
                    printf("O código precisa ser um número inteiro. \n");
                    int caractere;
                    do{
                        caractere = getchar(); //limpam a entrada caso a pessoa digite texto onde o programa espera um número.
                    }while(caractere != '\n' && caractere != EOF);//"continue lendo enquanto ainda não chegou ao Enter e ainda há entrada para ler"
                    break;
                }

                //Só cria o lote se o produto existir no cadastro.
                if (buscarProduto(codigos, quantidadeProdutos, codigoProduto) == -1){
                    printf("Produto não encontrado.\n");
                    break;
                }

                printf("Informe a quantidade recebida: ");
                if(scanf("%d", &quantidadeRecebida) != 1){
                    printf("A quantidade precisa ser um número inteiro.\n");
                    int caractere;
                    do{
                        caractere = getchar(); // limpam a entrada caso a pessoa digite texto onde o programa espera um número.
                    }while(caractere != '\n' && caractere !=EOF); //"continue lendo enquanto ainda não chegou ao Enter e ainda há entrada para ler"
                    break;
                }

                if(quantidadeRecebida <= 0){
                    printf("A quantidade deve ser maior que zero.\n");
                    break;
                }

                printf("Informe o número do lote do fabricante: "); //Numero do fabricante

                if(scanf("%29s", numerosLotes[quantidadeLotes]) != 1){
                    printf("Não foi possível ler o número do lote.\n");
                    break;
                }

                printf("Informe a validade (DDMMAAAA): ");
                int validade = lerData();

                if (validade == 0) {
                    printf("Validade inválida. Digite oito dígitos, como 08012027.\n");
                    break;
                }

                validadesDosLotes[quantidadeLotes] = validade;

                //Guarda os dois dados na mesma posição e conta o novo lote.
                codigosProdutosDosLotes[quantidadeLotes] = codigoProduto;
                saldosDosLotes[quantidadeLotes] = quantidadeRecebida;
                quantidadeLotes++;

                printf("Lote interno %d registrado com %d unidades.\n",
                       quantidadeLotes, quantidadeRecebida);

                break;
            }

            case 6:
                printf("Estoque por lote\n\n");

                if(quantidadeLotes == 0){
                    printf("Nenhum lote registrado.\n");
                    break;
                }

                for(int i = 0; i < quantidadeLotes; i++){
                    printf("Lote interno: %d\n", i + 1);
                    printf("Lote do fabricante: %s\n", numerosLotes[i]);
                    printf("Validade: ");
                    mostrarData(validadesDosLotes[i]);
                    printf("Código do produto: %d\n", codigosProdutosDosLotes[i]);
                    printf("Saldo: %d unidades\n\n", saldosDosLotes[i]);
                }

                break;

            case 7:
                printf("Cadastro de clientes.\n\n");
                {
            if(quantidadeClientes == 5){
                printf("Cadastro cheio. Limite de 5 clientes.\n");
                break;
            }

            int codigoCliente;

            printf("Informe o código do cliente: ");
            if(scanf("%d", &codigoCliente) != 1){
                printf("Código Inválido. Digite um número inteiro.\n");

                int caractere;

                do{
                    caractere = getchar();
                }while(caractere != '\n' && caractere != EOF);
                break;
            }

            if(buscarProduto(codigosClientes, quantidadeClientes, codigoCliente) != -1){
                printf("Esse código da cliente já está cadastrado.\n");
                break;
            }

             printf("Informe o nome ou a razão social do cliente: ");
             if(scanf(" %59[^\n]", nomesClientes[quantidadeClientes]) == 1){
                codigosClientes[quantidadeClientes] = codigoCliente;
                quantidadeClientes++;
                printf("Cliente Cadastrado.\n");
             }
            break;
            }

            case 8:
                printf("Consulta de clientes.\n\n");

                if(quantidadeClientes == 0){
                    printf("Nenhum cliente cadastrado");
                    break;
                }

                for(int i = 0; i < quantidadeClientes; i++){
                    printf("Código do cliente: %d\n", codigosClientes[i]);
                    printf("Nome do cliente: %s\n", nomesClientes[i]);
                    printf("\n");
                }

                break;

            case 9:
                printf("Registrar pedidos.\n\n");

                {
                    if(quantidadePedidos == 10){
                        printf("Limite de 10 pedidos atingido.\n");
                        break;
                    }

                    if(quantidadeClientes == 0 || quantidadeLotes == 0){
                        printf("Cadastre pelo menos um cliente e um lote antes do pedido.\n");
                        break;
                    }
                    int codigoCliente;
                    int numeroLote;

                    printf("Informe o código do cliente:");
                    if(scanf("%d", &codigoCliente) != 1 ){
                        printf("Código Inválido. Digite um número inteiro.\n");
                        int caractere;

                        do{
                            caractere = getchar();
                        }while(caractere != '\n' && caractere != EOF);
                        break;
                    }

                    int posicaoCliente = buscarProduto(codigosClientes, quantidadeClientes, codigoCliente);

                    if(posicaoCliente == -1){
                        printf("Cliente não encontrado.\n");
                        break;
                    }

                    printf("Informe o número interno do lote: ");
                    if(scanf("%d", &numeroLote) != 1){
                        printf("Numero de lote inválido. Digite um número inteiro.\n");
                        int caractere;

                        do{
                            caractere = getchar();
                        }while(caractere != '\n' && caractere != EOF);
                        break;
                    }

                    if(numeroLote < 1 || numeroLote > quantidadeLotes){
                        printf("Lote não encontrado");
                        break;
                    }

                    int posicaoLote = numeroLote -1;

                    //Verificar se a data de validade está dentro do prazo
                    printf("Informe a data do pedido (DDMMAAAA): ");
                    int dataPedido = lerData();

                    if (dataPedido == 0) {
                        printf("Data do pedido inválida. Digite oito dígitos, como 08112026.\n");
                        break;
                    }

                    //Compara a validade do lote com a data informada no pedido.
                    if(validadesDosLotes[posicaoLote] < dataPedido){
                        printf("Esse lote está vencido na data do pedido");
                        break;
                    }

                    printf("Cliente selecionado: %s\n", nomesClientes[posicaoCliente]);
                    printf("Lote selecionado: %s\n", numerosLotes[posicaoLote]);
                    printf("Saldo disponível: %d unidades\n", saldosDosLotes[posicaoLote]);

                    //Quantidade do Pedido
                    int quantidadePedida;

                    //scanf retorna 1 quando consegue ler um número inteiro.
                    printf("Informe a quantidade do pedido: ");
                    if(scanf("%d", &quantidadePedida) != 1){
                        printf("A quantidade precisa ser um número inteiro.\n");

                        //Descarta a entrada inválida até o Enter ou o fim da entrada
                        int caractere;
                        do{
                            caractere = getchar();
                        }while(caractere != '\n' && caractere != EOF);
                        break; //Sai do case 9 e volta o menu
                    }

                    //Um pedido precisa ter pelo menos uma unidade
                    if(quantidadePedida <= 0){
                        printf("A quantidade deve ser maior que zero.\n");
                        break;
                    }

                    //posicaoLote indica qual Lote foi escolhido
                    //Usamos essa posição para consultar o saldo daquele lote.
                    if(quantidadePedida > saldosDosLotes[posicaoLote]){
                        printf("Estoque insuficiente para esse pedido.\n");
                        break;
                    }

                    //Guarda todos os dados na próxima posição livre.
                    clientesDosPedidos[quantidadePedidos] = codigoCliente;
                    lotesDosPedidos[quantidadePedidos] = numeroLote;
                    quantidadesDosPedidos[quantidadePedidos] = quantidadePedida;
                    datasDosPedidos[quantidadePedidos ] = dataPedido;

                    //Desconta a quantidade somente do lote escolhido.
                    saldosDosLotes[posicaoLote] -= quantidadePedida;

                    quantidadePedidos++;

                    printf("Pedido %d registrado com sucesso.\n", quantidadePedidos);
                    printf("Novo saldo do lote: %d unidades.\n", saldosDosLotes[posicaoLote]);






                 break;
                }

            case 10:
                printf("Consulta de pedidos.\n\n");

                if(quantidadePedidos == 0){
                    printf("Nenhum pedido registrado");
                    break;
                }

                for(int i = 0; i < quantidadePedidos; i++){
                    //O primeiro pedido está na posição 0 e aparece como pedido 1.
                    printf("Pedido: %d\n", i+1);

                    //O mesmo índice i seleciona todos os dados do pedido.
                    printf("Código do cliente: %d\n", clientesDosPedidos[i]);
                    printf("Número interno do lote: %d\n", lotesDosPedidos[i]);
                    printf("Quantidade: %d unidades\n", quantidadesDosPedidos[i]);
                    printf("Data do pedido: ");
                    mostrarData(datasDosPedidos[i]);
                    printf("\n");
                }
                break;

            case 11: {
                if (quantidadePedidos == 0) {
                    printf("Nenhum pedido para exportar.\n");
                    break;
                }

                // Cria o arquivo ou substitui a exportacao anterior.
                FILE *arquivo = fopen("pedidos.csv", "w");
                if (arquivo == NULL) {
                    printf("Não foi possível abrir pedidos.csv.\n");
                    break;
                }

                int gravacaoOk = fprintf(
                    arquivo, "Pedido;Código do cliente;Nome do cliente;Lote;Quantidade;Data\n"
                ) >= 0;

                for (int i = 0; i < quantidadePedidos && gravacaoOk; i++) {
                    // O pedido guarda o codigo. A busca encontra o nome no cadastro.
                    int codigoCliente = clientesDosPedidos[i];
                    int posicaoCliente = buscarProduto(codigosClientes, quantidadeClientes, codigoCliente);

                    if (posicaoCliente == -1) {
                        gravacaoOk = 0;
                        break;
                    }

                    // Campos entre aspas aceitam ponto e virgula no nome.
                    // Uma aspa do nome precisa aparecer duas vezes no CSV.
                    char nomeParaCsv[119];
                    int destino = 0;

                    for (int j = 0; nomesClientes[posicaoCliente][j] != '\0'; j++) {
                        char letra = nomesClientes[posicaoCliente][j];

                        if (letra == '"') {
                            nomeParaCsv[destino++] = '"';
                        }

                        nomeParaCsv[destino++] = letra;
                    }
                    nomeParaCsv[destino] = '\0';

                    // Converte AAAAMMDD para DD/MM/AAAA no arquivo.
                    int data = datasDosPedidos[i];
                    int ano = data / 10000;
                    int mes = (data / 100) % 100;
                    int dia = data % 100;

                    gravacaoOk = fprintf(arquivo, "%d;%d;\"%s\";%d;%d;%02d/%02d/%04d\n",
                                         i + 1, codigoCliente, nomeParaCsv,
                                         lotesDosPedidos[i], quantidadesDosPedidos[i],
                                         dia, mes, ano) >= 0;
                }

                if (fclose(arquivo) != 0) {
                    gravacaoOk = 0;
                }

                if (gravacaoOk) {
                    printf("Pedidos exportados para pedidos.csv.\n");
                } else {
                    printf("Erro ao gravar pedidos.csv.\n");
                }

                break;
            }

            case 0:
                printf("Você decidiu sair do sistema.\n");
                break;

            default:
                printf("Opção inválida. Digite um número de 0 a 11.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}
