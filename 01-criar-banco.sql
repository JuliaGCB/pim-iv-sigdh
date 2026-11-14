-- Execute uma vez no SQL Server Management Studio.
CREATE DATABASE SIGDH;
GO
USE SIGDH;
GO

CREATE TABLE dbo.Produto (
    Codigo INT PRIMARY KEY,
    Nome NVARCHAR(59) NOT NULL
);

CREATE TABLE dbo.Cliente (
    Codigo INT PRIMARY KEY,
    Nome NVARCHAR(59) NOT NULL
);

CREATE TABLE dbo.Lote (
    IdLote INT IDENTITY(1,1) PRIMARY KEY,
    CodigoProduto INT NOT NULL REFERENCES dbo.Produto(Codigo),
    NumeroFabricante NVARCHAR(29) NOT NULL,
    Validade DATE NOT NULL,
    Saldo INT NOT NULL CHECK (Saldo >= 0)
);

CREATE TABLE dbo.Pedido (
    IdPedido INT IDENTITY(1,1) PRIMARY KEY,
    CodigoCliente INT NOT NULL REFERENCES dbo.Cliente(Codigo),
    IdLote INT NOT NULL REFERENCES dbo.Lote(IdLote),
    Quantidade INT NOT NULL CHECK (Quantidade > 0),
    DataPedido DATE NOT NULL
);
GO

-- O pedido e a baixa precisam ser confirmados juntos.
CREATE PROCEDURE dbo.RegistrarPedido
    @CodigoCliente INT,
    @IdLote INT,
    @Quantidade INT,
    @DataPedido DATE
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;

    IF @Quantidade IS NULL OR @Quantidade <= 0 OR @DataPedido IS NULL
        THROW 50001, N'Informe uma quantidade positiva e a data do pedido.', 1;

    BEGIN TRY
        BEGIN TRANSACTION;

        -- A própria atualização exige saldo suficiente e validade em dia.
        UPDATE dbo.Lote
        SET Saldo = Saldo - @Quantidade
        WHERE IdLote = @IdLote
          AND Saldo >= @Quantidade
          AND Validade >= @DataPedido;

        IF @@ROWCOUNT = 0
            THROW 50002, N'Lote inexistente, vencido ou sem saldo suficiente.', 1;

        -- A chave estrangeira exige um cliente cadastrado.
        INSERT INTO dbo.Pedido (CodigoCliente, IdLote, Quantidade, DataPedido)
        VALUES (@CodigoCliente, @IdLote, @Quantidade, @DataPedido);

        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;
        THROW;
    END CATCH;
END;
GO
