#include <Arduino_FreeRTOS.h>

/* -------------------------------
   DEFINIÇÃO DE PINOS
--------------------------------*/
#define BUTTON_PIN 7   // Botão (pull-down externo)
#define LED1 8         // Pisca ao ligar pelo botão
#define LED2 9         // Limiar de distância
#define LED3 10        // Pisca proporcional à distância

#define TRIG_PIN 3
#define ECHO_PIN 5

/* -------------------------------
   VARIÁVEIS GLOBAIS
--------------------------------*/
volatile bool led1Enabled = false;  // LED1 ligado/desligado pelo botão
volatile long distanciaAtual = 999; // Distância medida pelo sensor

/* Para debounce */
int lastReading = LOW;
int stableState = LOW;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

/* -------------------------------
   PROTÓTIPOS DAS TAREFAS
--------------------------------*/
void TaskButton(void *pvParameters);
void TaskSonar(void *pvParameters);
void TaskLED1(void *pvParameters);
void TaskLED2(void *pvParameters);
void TaskLED3(void *pvParameters);

/* -------------------------------
   SETUP
--------------------------------*/
void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  /* Criação das tarefas */
  xTaskCreate(TaskButton, "Button", 128, NULL, 3, NULL);
  xTaskCreate(TaskSonar,  "Sonar",  128, NULL, 2, NULL);
  xTaskCreate(TaskLED1,   "LED1",   128, NULL, 1, NULL);
  xTaskCreate(TaskLED2,   "LED2",   128, NULL, 1, NULL);
  xTaskCreate(TaskLED3,   "LED3",   128, NULL, 1, NULL);

  /* Inicia o escalonador */
  vTaskStartScheduler();
}

void loop() {
  
}

/* --------------------------------------------------
   TASK1: Ler o botão e alternar LED1 (toggle)
---------------------------------------------------*/
void TaskButton(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    int reading = digitalRead(BUTTON_PIN);

    // Debounce
    if (reading != lastReading) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != stableState) {

        // Borda de subida → toggle LED1
        if (reading == HIGH) {
          led1Enabled = !led1Enabled;
          Serial.print("LED1 agora está: ");
          Serial.println(led1Enabled ? "LIGADO" : "DESLIGADO");
        }

        stableState = reading;
      }
    }

    lastReading = reading;
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

/* --------------------------------------------------
   TASK2: Sensor ultrassônico
---------------------------------------------------*/
void TaskSonar(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout 30ms

    if (duration == 0) {
      distanciaAtual = -1; // fora do range
    } else {
      distanciaAtual = duration * 0.034 / 2; // fórmula oficial
    }

    Serial.print("Distância: ");
    Serial.print(distanciaAtual);
    Serial.println(" cm");

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

/* --------------------------------------------------
   TASK3: LED1 pisca se estiver ativado pelo botão
---------------------------------------------------*/
void TaskLED1(void *pvParameters) {
  (void) pvParameters;

  const int blinkTime = 300; // ms

  for (;;) {
    if (led1Enabled) {
      digitalWrite(LED1, HIGH);
      vTaskDelay(blinkTime / portTICK_PERIOD_MS);
      digitalWrite(LED1, LOW);
      vTaskDelay(blinkTime / portTICK_PERIOD_MS);
    } else {
      digitalWrite(LED1, LOW);
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  }
}

/* --------------------------------------------------
   TASK4: LED2 acende se distância < limiar
---------------------------------------------------*/
void TaskLED2(void *pvParameters) {
  (void) pvParameters;

  const int threshold = 20; // cm

  for (;;) {
    if (distanciaAtual > 0 && distanciaAtual < threshold) {
      digitalWrite(LED2, HIGH);
    } else {
      digitalWrite(LED2, LOW);
    }

    vTaskDelay(80 / portTICK_PERIOD_MS);
  }
}

/* --------------------------------------------------
   TASK5: LED3 pisca proporcional à distância
---------------------------------------------------*/
void TaskLED3(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    if (distanciaAtual == -1) {
      // Fora do range -> aceso continuamente
      digitalWrite(LED3, HIGH);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    } else {
      // Quanto mais perto, mais rápido
      int t = distanciaAtual * 5; 
      if (t < 50) t = 50;         // limiar mínimo

      digitalWrite(LED3, HIGH);
      vTaskDelay(t / portTICK_PERIOD_MS);

      digitalWrite(LED3, LOW);
      vTaskDelay(t / portTICK_PERIOD_MS);
    }
  }
}
