# Simulador de Algoritmo PID para Robô Seguidor de Linha

Este projeto implementa a simulação de um algoritmo PID acoplado a um robô seguidor de linha, utilizando a placa **BitDogLab**. A simulação inclui um joystick para simular a mudança da linha e a matriz de LEDs da BitDogLab para representar a leitura dos sensores e a correção aplicada pelo PID.

## 📌 Funcionalidades

- **Simulação de um robô seguidor de linha** utilizando um joystick para movimentar a linha virtual.
- **Matriz de LEDs** para representar os sensores de luz do robô e a resposta do controlador PID.
- **Display OLED SSD1306** para exibir os valores de erro e correção calculados pelo PID.
- **Controle por PID** para ajustar a posição do robô na linha com base na leitura simulada dos sensores.
- **Botão de reinicialização (Bootloader)** para facilitar a atualização do firmware.

## 🎯 Objetivos

- Implementar e visualizar o funcionamento de um **controlador PID** aplicado a um robô seguidor de linha.
- Explorar o uso de **ADC**, **PWM**, **I2C** e **PIO** na placa **BitDogLab**.
- Simular o comportamento do robô de forma intuitiva e interativa.

## 🛠️ Hardware Utilizado

- **Placa BitDogLab**
- **Display OLED SSD1306** (I2C)
- **Joystick analógico** (ADC - GPIO26)
- **Botão B (Bootloader)** (GPIO6)
- **Matriz de LEDs** integrada na BitDogLab (PIO - GPIO7)

## ⚙️ Como Funciona

1. O **joystick** move a linha simulada, alterando o erro.
2. O **PID calcula a correção** com base no erro detectado.
3. A **matriz de LEDs** exibe a linha simulada e a correção aplicada pelo PID.
4. O **display OLED** exibe os valores do erro e da correção.
5. O **botão B** permite reiniciar a placa em modo bootloader para atualizar o firmware.

## 📦 Estrutura do Projeto

```
/
│   ├── ADC_DisplayC.c   # Implementação do algoritmo PID e controle do hardware
│   ├── matriz_led.pio  # Programa PIO para controle da matriz de LEDs
├── lib/                 # Bibliotecas auxiliares
│   ├── ssd1306.h       # Header do display OLED
│   ├── ssd1306.c       # Driver para o display OLED
│   ├── font.h          # Definição de fontes para o display
└── README.md            # Este arquivo
```

## 🚀Como Executar



Conecte a **BitDogLab** no modo **BOOTSEL** e copie o arquivo `.uf2` gerado para a unidade montada.

3\. Reinicie a placa e visualize a simulação!

## 🔧 Ajuste de Parâmetros PID

Os valores do **Kp, Ki e Kd** podem ser ajustados diretamente no código para modificar a resposta do sistema:

```c
float Kp = 0.5;  // Ganho proporcional
float Ki = 0.2;  // Ganho integral
float Kd = 0.1;  // Ganho derivativo
```

##

