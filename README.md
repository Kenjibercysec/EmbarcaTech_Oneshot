# EmbarcaTech_Oneshot

Este projeto demonstra o controle de LEDs a partir de um botão, utilizando um timer one-shot para criar um ciclo de estados. O firmware foi desenvolvido para a plataforma Raspberry Pi Pico, utilizando o Pico SDK e CMake para compilação.

## Sumário
- Descrição
- Funcionalidades
- Estrutura do Projeto
- Detalhes do Código
  - Configuração dos Pinos
  - Implementação do Timer One-Shot
  - Loop Principal e Debounce do Botão
- Aprendizados
- Guia de Utilização
- Licença

## Descrição
O EmbarcaTech_Oneshot é um firmware simples que aciona um ciclo de estados dos LEDs (verde, azul e vermelho) a partir da pressão de um botão. O ciclo é controlado por uma função callback acionada por um timer (alarme) que dispara a cada 3000 ms. Essa implementação é ideal para aprender conceitos de timers one-shot, debounce de botões e gerenciamento de estados em sistemas embarcados.

## Funcionalidades
- Ciclo de LEDs com Timer One-Shot
- Debounce simples (100 ms) para o botão
- Utilização do Pico SDK com CMake

## Estrutura do Projeto
A estrutura do repositório é apresentada abaixo:

```
EmbarcaTech_Oneshot/
├── .gitignore
├── CMakeLists.txt
├── EmbarcaTech_Oneshot.c
├── blink.pio    # (opcional) Configurações do PIO
├── diagram.json # Diagrama do hardware
├── pico_sdk_import.cmake
└── wokwi.toml   # Configurações para simulação
```

## Detalhes do Código

### Configuração dos Pinos
No arquivo EmbarcaTech_Oneshot.c, os pinos são definidos da seguinte forma:

```c
#define BUTTON_GP 5      // Botão em GP5
#define LED_GREEN 11     // LED Verde em GP11
#define LED_BLUE 12      // LED Azul em GP12
#define LED_RED 13       // LED Vermelho em GP13
```

### Implementação do Timer One-Shot
A função callback 'timer_cb' gerencia o ciclo de estados dos LEDs:

```c
int64_t timer_cb(alarm_id_t id, void *user_data) {
    gpio_put(LED_GREEN, 0);
    gpio_put(LED_BLUE, 0);
    gpio_put(LED_RED, 0);
    switch(state) {
        case 0:
            gpio_put(LED_GREEN, 1);  // Acende LED verde
            gpio_put(LED_BLUE, 1);   // Acende LED azul
            gpio_put(LED_RED, 1);    // Acende LED vermelho
            break;
        case 1:
            gpio_put(LED_RED, 1);
            gpio_put(LED_BLUE, 1);
            break;
        case 2:
            gpio_put(LED_RED, 1);
            break;
        case 3:
            // Todos os LEDs permanecem desligados
            break;
        default:
            break;
    }
    state++;
    if (state < 4) {
        add_alarm_in_ms(3000, timer_cb, NULL, false);
    } else {
        printf("Ciclo completo. Aguardando novo acionamento...\n");
        program_active = false;
        state = 0;
    }
    return 0;
}
```

### Loop Principal e Debounce do Botão
No main(), o loop infinito monitora o botão e inicia o ciclo se acionado:

```c
while (true) {
    if (!gpio_get(BUTTON_GP) && !program_active) {
        sleep_ms(100);  // Debounce de 100 ms
        if (!gpio_get(BUTTON_GP)) {
            printf("Botão pressionado! Iniciando ciclo de LEDs.\n");
            program_active = true;
            state = 0;
            timer_cb(0, NULL);
        }
    }
    sleep_ms(10);
}
```

## Aprendizados
- Uso de timers one-shot para agendar eventos sem bloquear a execução.
- Implementação de debounce para evitar leituras incorretas do botão.
- Gerenciamento de estados para alterar o comportamento dos LEDs sequencialmente.
- Integração do projeto com o Pico SDK e CMake, facilitando a compilação e manutenção.

## Guia de Utilização

### Pré-requisitos
- Hardware: Raspberry Pi Pico ou dispositivo compatível.
- Software:
  - Raspberry Pi Pico SDK (https://github.com/raspberrypi/pico-sdk)
  - CMake (https://cmake.org/)
  - Ferramenta de upload/flash (por exemplo, picotool)

### Passos para Compilação e Upload
1. Clonar o repositório:
   ```bash
   git clone https://github.com/Kenjibercysec/EmbarcaTech_Oneshot.git
   cd EmbarcaTech_Oneshot
   ```

2. Configurar o ambiente:
   Verifique se o Pico SDK está instalado e se o arquivo pico_sdk_import.cmake aponta para o caminho correto.

3. Criar o diretório de build e compilar:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. Upload do Firmware:
   Conecte o Raspberry Pi Pico em modo bootloader e copie o arquivo .uf2 gerado para o dispositivo.

5. Teste:
   Pressione o botão conectado ao pino GP5 para iniciar o ciclo dos LEDs.

## Licença
Este projeto não possui uma licença definida. Utilize-o, adapte-o e estude-o conforme necessário.
