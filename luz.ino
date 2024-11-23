#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "ProjetoX";  // Nome da rede Wi-Fi criada pela ESP Central
const char *password = "eu_assisto_xvideos_pae";  // Senha da rede Wi-Fi criada pela ESP Central
const char *serverName = "http://192.168.4.1";  // IP da ESP Central (padrão para ESP32 no modo AP)

void setup() {
  Serial.begin(115200);

  // Conecta-se à rede Wi-Fi criada pela ESP Central
  WiFi.begin(ssid, password);
  
  // Aguarda a conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi");
}

void loop() {
  HTTPClient http;

  // Comando para ligar o LED
  http.begin(serverName + String("/led/on"));
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    Serial.println("Comando para ligar o LED enviado com sucesso");
  } else {
    Serial.println("Erro ao enviar comando para ligar o LED");
  }
  http.end();

  delay(5000);  // Espera 5 segundos antes de enviar o próximo comando

  // Comando para desligar o LED
  http.begin(serverName + String("/led/off"));
  httpCode = http.GET();
  
  if (httpCode > 0) {
    Serial.println("Comando para desligar o LED enviado com sucesso");
  } else {
    Serial.println("Erro ao enviar comando para desligar o LED");
  }
  http.end();

  delay(5000);  // Espera 5 segundos antes de repetir os comandos
}
