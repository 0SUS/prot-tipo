#include <stdio.h>
#include <math.h> 
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "lib/ssd1306.h"
#include "hardware/clocks.h"
#include "matriz_led.pio.h"
#include "pico/bootrom.h"

// Definição dos pinos
#define JOY_X 26
#define I2C_SDA 14
#define I2C_SCL 15
#define OUT_PIN 7
#define Botao_B 6

// Constantes do PID
#define INTEGRAL_MAX 5.0
#define INTEGRAL_MIN -5.0

// Variáveis do PID
float Kp = 0.5, Ki = 0.1, Kd = 0.4;  // Valores ajustáveis
float erro_anterior = 0, integral = 0;
ssd1306_t display;
PIO pio = pio0;
uint offset, sm;

// Função do PID
float pid(float erro) {
    float derivativo = erro - erro_anterior;
    integral += erro;

    // Limitação da integral para evitar acúmulo excessivo
    if (integral > INTEGRAL_MAX) integral = INTEGRAL_MAX;
    if (integral < INTEGRAL_MIN) integral = INTEGRAL_MIN;

    erro_anterior = erro;

    // Pequeno erro -> Reduz acúmulo de integral
    if (fabs(erro) < 0.2) {
        integral *= 0.9; // Decaimento leve da integral
    }

    float saida = (Kp * erro) + (Ki * integral) + (Kd * derivativo);

    // Limita a saída entre -2 e 2
    if (saida > 2) saida = 2;
    if (saida < -2) saida = -2;

    return roundf(saida);  // Retorna valor arredondado para posição da matriz
}

// Função para definir cor RGB na matriz de LEDs
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char B = b * 255;
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Botão B coloca em modo de boot
void buttonb_irq_handler(uint gpio, uint32_t events) {
    uint32_t frame[25] = {0};
    for (int i = 0; i < 25; i++)
        pio_sm_put_blocking(pio, sm, frame[i] ? 0xFFFFFF : 0);
    ssd1306_fill(&display, 0);
    ssd1306_send_data(&display);
    reset_usb_boot(0, 0);
}

// Inicializa a matriz de LEDs
void init_matriz_led() {
    offset = pio_add_program(pio, &matriz_led_program);
    sm = pio_claim_unused_sm(pio, true);
    matriz_led_program_init(pio, sm, offset, OUT_PIN);
}

// Atualiza a matriz de LEDs conforme a linha e a correção do PID
void atualizar_sensores(int posicao, int correcao) {
    int centro = 2;  // Centro da linha
    double color_alpha = 0.05;  // Reduz brilho dos LEDs
    uint32_t frame[25] = {0}; // Matriz de 5x5

    // Linha do meio (posição real da linha) - Azul
    for (int i = 0; i < 5; i++) {
        if ((centro - posicao) == i) {
            frame[5 + i] = matrix_rgb(color_alpha * 1, color_alpha * 0, color_alpha * 0); // Azul
        }
    }

    // Linha abaixo (correção do PID) - Verde
    for (int i = 0; i < 5; i++) {
        if ((centro - correcao) == i) {
            frame[14 - i] = matrix_rgb(color_alpha * 0, color_alpha * 1, color_alpha * 0); // Verde
        }
    }

    // Atualiza a matriz de LEDs
    for (int i = 0; i < 25; i++) {
        pio_sm_put_blocking(pio, sm, frame[i]);
    }
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(JOY_X);
    init_matriz_led(); // Inicializa a matriz de LEDs

    // Inicializa I2C e display
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&display, 128, 64, false, 0x3C, i2c1);
    ssd1306_config(&display);

    gpio_init(Botao_B);
    gpio_set_dir(Botao_B, GPIO_IN);
    gpio_pull_up(Botao_B);
    gpio_set_irq_enabled_with_callback(Botao_B, GPIO_IRQ_EDGE_FALL, true, &buttonb_irq_handler);

    while (1) {
        // Leitura do joystick
        adc_select_input(0);
        uint16_t x_val = adc_read();
        int posicao = ((x_val - 2048) * 6) / 4095;
    
        // Calcula a correção do PID
        float erro = posicao;
        int correcao = (int)pid(erro); // Converte saída PID para int
    
        // Atualiza LEDs
        atualizar_sensores(posicao, correcao);
    
        // Exibe informações no display
        ssd1306_fill(&display, 0);

        char buffer[20];
        snprintf(buffer, sizeof(buffer), "Erro: %d", (int)erro);
        ssd1306_draw_string(&display, buffer, 0, 0);

        snprintf(buffer, sizeof(buffer), "PID: %d", (int)correcao);
        ssd1306_draw_string(&display, buffer, 0, 10);

        snprintf(buffer, sizeof(buffer), "Linha: %d", posicao);
        ssd1306_draw_string(&display, buffer, 0, 20);

        snprintf(buffer, sizeof(buffer), "Corr: %d", correcao);
        ssd1306_draw_string(&display, buffer, 0, 30);

        ssd1306_send_data(&display);

        sleep_ms(50);
    }
}
