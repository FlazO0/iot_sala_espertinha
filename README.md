
# **Projeto de Sala Automática com ESP32 e IA**

Este projeto visa criar uma **sala inteligente** utilizando ESP32s para controlar dispositivos como **luz**, **ventilador** e **TV**. O sistema é composto por um **ESP32 Central**, que se comunica com os **ESP32 Locais** via **Bluetooth** para controlar os dispositivos em tempo real. A TV é controlada via **Infra-vermelho** e os outros dispositivos (luz e ventilador) são controlados através de **relés**.

---

## **Sumário**
1. [Visão Geral](#visão-geral)
2. [Estrutura do Projeto](#estrutura-do-projeto)
3. [Pré-requisitos](#pré-requisitos)
4. [Como Usar](#como-usar)
5. [Estrutura de Código](#estrutura-de-código)
6. [Configuração dos Dispositivos](#configuração-dos-dispositivos)
7. [Contribuindo](#contribuindo)
8. [Licença](#licença)

---

## **Visão Geral**

Este projeto utiliza o ESP32 para criar uma solução de automação para controlar dispositivos como luz, ventilador e TV em uma sala, utilizando uma comunicação eficiente via Bluetooth entre dispositivos. O código está dividido em duas partes principais:

- **ESP32 Central**: Recebe os comandos de controle (via serial ou IA) e os transmite via Bluetooth para os dispositivos locais.
- **ESP32 Local**: Recebe os comandos via Bluetooth e executa as ações correspondentes (ligar/desligar luz, ventilador ou TV).

O objetivo é criar uma solução modular e sem fios, minimizando o uso de cabos e mantendo a flexibilidade para controlar diferentes dispositivos de uma maneira eficiente e centralizada.

---

## **Estrutura do Projeto**

Este projeto é dividido nas seguintes partes:

1. **Código do ESP32 Central**: Recebe comandos do Python (ou outra fonte) e os envia via Bluetooth para os dispositivos locais.
2. **Código do ESP32 Local**: Recebe os comandos Bluetooth e aciona os dispositivos (luz/ventilador via relé, TV via Infra-vermelho).
3. **Comunicação**: A comunicação entre o ESP32 Central e os ESP32 Locais é realizada via Bluetooth Low Energy (BLE).

---

## **Pré-requisitos**

- **Hardware**:
  - Módulos ESP32 (pelo menos 2, um como Central e outros como Locais)
  - Relé para controlar dispositivos como luz e ventilador
  - LED infravermelho para controlar a TV
  - Fonte de alimentação para os dispositivos

- **Software**:
  - **Arduino IDE** (para programar os ESP32)
  - **Biblioteca IRremoteESP8266** (para controle de TV via IR)
  - **Biblioteca BLE** (para comunicação Bluetooth)

---

## **Como Usar**

1. **Instale as bibliotecas necessárias**:
   - Abra o **Arduino IDE** e instale as bibliotecas:
     - **IRremoteESP8266**
     - **BLEDevice**
     - **BLEUtils**
     - **BLEServer**

2. **Configuração dos Dispositivos**:
   - Carregue o código para o **ESP32 Central** em um dos seus módulos ESP32.
   - Carregue o código para os **ESP32 Locais** nos módulos correspondentes.
   - Conecte os dispositivos (relé e LED IR) aos pinos apropriados.

3. **Execução**:
   - Conecte a **ESP32 Central** ao seu PC via Serial (para receber comandos).
   - Envie os comandos da ESP32 Central para os dispositivos locais via Bluetooth.
   - Controle os dispositivos (luz, ventilador e TV) de acordo com os comandos recebidos.

---

## **Estrutura de Código**

### **Código do ESP32 Central**
O código do ESP32 Central se conecta ao dispositivo via Bluetooth, espera comandos via Serial e repassa esses comandos para os ESP32 Locais.

```cpp
// Código do ESP32 Central (Bluetooth + Serial)
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLECharacteristic.h>
#include <BLE2902.h>

// UUIDs
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "abcd1234-abcd-abcd-abcd-abcdefabcdef"

BLEServer *pServer;
BLECharacteristic *pCharacteristic;
BLEAdvertising *pAdvertising;

String comando = "";

// Função para iniciar o BLE
void initBLE() {
  BLEDevice::init("ESP32 Central");
  pServer = BLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  pCharacteristic->setValue("Esperando comando...");
  pService->start();
  pAdvertising->start();
  Serial.println("ESP32 Central ready...");
}

// Função para enviar comandos via Serial
void setup() {
  Serial.begin(115200);
  initBLE();
}

void loop() {
  if (Serial.available()) {
    comando = Serial.readString();

    // Envia comando via Bluetooth
    if (comando == "ligar_luz") {
      pCharacteristic->setValue("ligar_luz");
      pCharacteristic->notify();
    }
    else if (comando == "desligar_luz") {
      pCharacteristic->setValue("desligar_luz");
      pCharacteristic->notify();
    }
    else if (comando == "ligar_ventilador") {
      pCharacteristic->setValue("ligar_ventilador");
      pCharacteristic->notify();
    }
    else if (comando == "desligar_ventilador") {
      pCharacteristic->setValue("desligar_ventilador");
      pCharacteristic->notify();
    }
    else if (comando == "ligar_tv") {
      pCharacteristic->setValue("ligar_tv");
      pCharacteristic->notify();
    }
    else if (comando == "desligar_tv") {
      pCharacteristic->setValue("desligar_tv");
      pCharacteristic->notify();
    }
  }
}
```

### **Código do ESP32 Local (Relé + TV via IR)**
O código do ESP32 Local recebe os comandos Bluetooth e aciona os dispositivos, como luz, ventilador e TV.

```cpp
// Código do ESP32 Local (Relé + TV via IR)
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLECharacteristic.h>

#define PINO_RELE_LUZ 12
#define PINO_RELE_VENTILADOR 14
#define PINO_IR 4

IRsend irsend(PINO_IR);  // Controle IR para TV

void setup() {
  Serial.begin(115200);
  pinMode(PINO_RELE_LUZ, OUTPUT);
  pinMode(PINO_RELE_VENTILADOR, OUTPUT);
  irsend.begin();
}

void loop() {
  if (Serial.available()) {
    String comando = Serial.readString();

    if (comando == "ligar_luz") {
      digitalWrite(PINO_RELE_LUZ, HIGH);
    } else if (comando == "desligar_luz") {
      digitalWrite(PINO_RELE_LUZ, LOW);
    } else if (comando == "ligar_ventilador") {
      digitalWrite(PINO_RELE_VENTILADOR, HIGH);
    } else if (comando == "desligar_ventilador") {
      digitalWrite(PINO_RELE_VENTILADOR, LOW);
    } else if (comando == "ligar_tv") {
      irsend.sendNEC(0x1FE48B7, 32);  // Código de ligar TV
    } else if (comando == "desligar_tv") {
      irsend.sendNEC(0x1FE58A7, 32);  // Código de desligar TV
    }
  }
}
```

---

## **Configuração dos Dispositivos**

1. **Relé para Luz e Ventilador**: 
   - Conecte os pinos do relé aos pinos configurados no código (12 para luz e 14 para ventilador).
   - Certifique-se de que o relé está controlando corretamente os dispositivos.

2. **Controle IR para TV**:
   - Conecte o LED IR ao pino configurado no código (pino 4).
   - Modifique os códigos IR de acordo com o seu controle remoto de TV.

---

## **Licença**

Este projeto está licenciado sob a licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.

---
