function Mostrar-Menu {
    Clear-Host
    Write-Host "=== Sistema de Sensores ==="
    Write-Host "1. Gerar dados de teste"
    Write-Host "2. Organizar dados por sensor"
    Write-Host "3. Consultar leitura por instante"
    Write-Host "4. Sair"
    Write-Host "============================="
}

function Executar-Geracao {
    $inicio = Read-Host "Digite a data/hora INICIAL (DD/MM/AAAA_HH:MM:SS)"
    $fim = Read-Host "Digite a data/hora FINAL   (DD/MM/AAAA_HH:MM:SS)"
    $entrada = Read-Host "Digite sensores e tipos separados por espaco (ex: TEMP float PRES int)"

    $args = "$inicio $fim $entrada"
    .\gerar_dados.exe $args.Split(" ")
    Pause
}

function Executar-Organizacao {
    if (-Not (Test-Path "dados_gerados.txt")) {
        Write-Host "Arquivo 'dados_gerados.txt' nao encontrado. Gere os dados primeiro."
    } else {
        .\organizar_dados.exe dados_gerados.txt
    }
    Pause
}

function Executar-Consulta {
    $sensor = Read-Host "Digite o nome do sensor (ex: TEMP)"
    $datahora = Read-Host "Digite a data/hora desejada (DD/MM/AAAA_HH:MM:SS)"
    .\consultar_sensor.exe $sensor $datahora
    Pause
}

Do {
    Mostrar-Menu
    $opcao = Read-Host "Escolha uma opcao (1-4)"

    Switch ($opcao) {
        "1" { Executar-Geracao }
        "2" { Executar-Organizacao }
        "3" { Executar-Consulta }
        "4" { Write-Host "Encerrando..."; break }
        Default { Write-Host "Opcao invalida."; Pause }
    }
} While ($true)
