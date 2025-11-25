# Proximity Detection – Projeto Final (Arduino + FreeRTOS)

Este projeto implementa um sistema de detecção de proximidade utilizando:

- Arduino UNO
- Sensor ultrassônico HC-SR04
- Três LEDs
- Pushbutton com resistor pull-down
- FreeRTOS para multitarefa real

## Objetivo do Sistema

O sistema utiliza um sensor ultrassônico para medir a distância de um objeto e controla três LEDs com comportamentos distintos. Além disso, há interação do usuário via pushbutton para ligar/desligar um dos LEDs.

As funcionalidades implementadas são:

🔴 LED1 – Controle por Botão

- O pushbutton liga/desliga o LED1 (modo toggle).
- Quando ligado, o LED1 pisca em uma frequência constante.
- Quando desligado, permanece apagado.

🟢 LED2 – Alerta de Limiar

- Acende quando a distância medida pelo sensor é menor que um valor limite (por padrão 20 cm).
- Apaga caso a distância seja maior.

🟡 LED3 – Indicação Proporcional

- Pisca com frequência proporcional à distância:
- Quanto mais próximo o objeto → mais rápido pisca
- Quanto mais distante → mais lento
- Se o sensor estiver fora do alcance, o LED3 permanece aceso continuamente.

Sensor HC-SR04 – Medição

- Mede continuamente a distância usando sinais ultrassônicos.
- Envia a distância para as outras tarefas.

FreeRTOS – Multitarefa Real

- Cada parte do sistema roda em uma task independente:
- Tarefa do botão (debounce + toggle)
- Tarefa do sensor ultrassônico
- Tarefa do LED1
- Tarefa do LED2
- Tarefa do LED3
