#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>

// Definições dos pinos conforme o diagrama de simulação
#define BUTTON_GP 5      // Botão em GP5
#define LED_GREEN 11     // LED Verde em GP11
#define LED_BLUE  12     // LED Azul em GP12
#define LED_RED   13     // LED Vermelho em GP13

// Variáveis de controle do ciclo
volatile bool program_active = false;
volatile int state = 0;

/**
 * Função callback do alarme (temporizador one-shot).
 * Executa a mudança de estado do ciclo de LEDs.
 */
int64_t timer_cb(alarm_id_t id, void *user_data) {
    // Desliga todos os LEDs antes de definir o estado atual
    gpio_put(LED_GREEN, 0);
    gpio_put(LED_BLUE, 0);
    gpio_put(LED_RED, 0);

    // Define os LEDs de acordo com o estado atual
    switch(state) {
        case 0:
            // Estado 0: acende todos os LEDs
            gpio_put(LED_GREEN, 1);
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_RED, 1);
            break;
        case 1:
            // Estado 1: acende apenas os LEDs vermelho e azul
            gpio_put(LED_RED, 1);
            gpio_put(LED_BLUE, 1);
            break;
        case 2:
            // Estado 2: acende apenas o LED vermelho
            gpio_put(LED_RED, 1);
            break;
        case 3:
            // Estado 3: mantém todos os LEDs desligados (já estão desligados)
            break;
        default:
            break;
    }
    state++;

    // Se ainda não completou o ciclo (4 estados), agenda a próxima mudança
    if (state < 4) {
        add_alarm_in_ms(3000, timer_cb, NULL, false);
    } else {
        // Ciclo completo: reseta as variáveis para permitir novo acionamento
        printf("Ciclo completo. Aguardando novo acionamento...\n");
        program_active = false;
        state = 0;
    }
    return 0;
}

int main(void) {
    stdio_init_all();

    // Configura os pinos dos LEDs como saída
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);

    // Configura o pino do botão como entrada com pull-up interno
    gpio_init(BUTTON_GP);
    gpio_set_dir(BUTTON_GP, GPIO_IN);
    gpio_pull_up(BUTTON_GP);

    printf("Aguardando pressionar o botão...\n");

    while (true) {
        // Se o botão for pressionado e o programa não estiver em execução...
        if (!gpio_get(BUTTON_GP) && !program_active) {
            sleep_ms(100);  // Debounce simples
            if (!gpio_get(BUTTON_GP)) {
                printf("Botão pressionado! Iniciando ciclo de LEDs.\n");
                program_active = true;
                state = 0;
                // Inicia o ciclo chamando o callback imediatamente
                timer_cb(0, NULL);
            }
        }
        sleep_ms(10);
    }
    return 0;
}
