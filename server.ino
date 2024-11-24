#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>  // Incluindo a biblioteca HTTPClient

const char *ssid = "ProjetoX";  // Nome da rede Wi-Fi criada pela ESP Central
const char *password = "eu_assisto_xvideos_pae";  // Senha da rede Wi-Fi criada pela ESP Central

WebServer server(80);  // Servidor HTTP

void setup() {
  Serial.begin(115200);

  // Cria o ponto de acesso (AP)
  WiFi.softAP(ssid, password);
  Serial.println("Ponto de acesso criado");
  Serial.print("IP do ponto de acesso: ");
  Serial.println(WiFi.softAPIP());  // Mostra o IP da ESP32 no modo AP

  // Rota para ligar o ventilador
  server.on("/ligarVentilador", HTTP_GET, []() {
    Serial.println("Comando para ligar o ventilador enviado.");
    HTTPClient http;
    http.begin("http://192.168.4.2/ligarVentilador");  // IP da ESP Auxiliar 1 (ventilador)
    int httpCode = http.GET();
    if (httpCode == 200) {
      Serial.println("Ventilador ligado com sucesso.");
      server.send(200, "text/plain", "Ventilador Ligado");
    } else {
      Serial.println("Erro ao ligar o ventilador.");
      server.send(500, "text/plain", "Erro ao ligar o ventilador");
    }
    http.end();
  });

  // Rota para desligar o ventilador
  server.on("/desligarVentilador", HTTP_GET, []() {
    Serial.println("Comando para desligar o ventilador enviado.");
    HTTPClient http;
    http.begin("http://192.168.4.2/desligarVentilador");  // IP da ESP Auxiliar 1 (ventilador)
    int httpCode = http.GET();
    if (httpCode == 200) {
      Serial.println("Ventilador desligado com sucesso.");
      server.send(200, "text/plain", "Ventilador Desligado");
    } else {
      Serial.println("Erro ao desligar o ventilador.");
      server.send(500, "text/plain", "Erro ao desligar o ventilador");
    }
    http.end();
  });

  // Rota para ligar a luz
  server.on("/ligarLuz", HTTP_GET, []() {
    Serial.println("Comando para ligar a luz enviado.");
    HTTPClient http;
    http.begin("http://192.168.4.3/ligarLuz");  // IP da ESP Auxiliar 2 (luz)
    int httpCode = http.GET();
    if (httpCode == 200) {
      Serial.println("Luz ligada com sucesso.");
      server.send(200, "text/plain", "Luz Ligada");
    } else {
      Serial.println("Erro ao ligar a luz.");
      server.send(500, "text/plain", "Erro ao ligar a luz");
    }
    http.end();
  });

  // Rota para desligar a luz
  server.on("/desligarLuz", HTTP_GET, []() {
    Serial.println("Comando para desligar a luz enviado.");
    HTTPClient http;
    http.begin("http://192.168.4.3/desligarLuz");  // IP da ESP Auxiliar 2 (luz)
    int httpCode = http.GET();
    if (httpCode == 200) {
      Serial.println("Luz desligada com sucesso.");
      server.send(200, "text/plain", "Luz Desligada");
    } else {
      Serial.println("Erro ao desligar a luz.");
      server.send(500, "text/plain", "Erro ao desligar a luz");
    }
    http.end();
  });

  // Inicia o servidor
  server.begin();
}

void loop() {
  server.handleClient();  // Processa as requisições HTTP
}
