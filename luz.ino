#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ProjetoX";  // Nome da rede Wi-Fi criada pela ESP Central
const char *password = "eu_assisto_xvideos_pae";  // Senha da rede Wi-Fi criada pela ESP Central

WebServer server(80);  // Servidor HTTP

// Pino para controlar o ventilador via relé
const int luzPin = 23;

void setup() {
  Serial.begin(115200);


  // Definindo IP estático
  IPAddress local_IP(192, 168, 4, 150);  // IP fixo desejado (mude conforme sua rede)
  IPAddress gateway(192, 168, 4, 1);     // Gateway da sua rede (geralmente o roteador)
  IPAddress subnet(255, 255, 255, 0);    // Máscara de sub-rede (geralmente 255.255.255.0)

  // Configurando IP estático
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falha ao configurar o IP estático.");
  }

  // Conecta-se à rede Wi-Fi criada pela ESP Central
  WiFi.begin(ssid, password);
  
  // Aguarda a conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi");

  // Configura o pino da luz
  pinMode(luzPin, OUTPUT);
  digitalWrite(luzPin, LOW);  // Desliga a luz inicialmente

  // Rota para ligar a luz
  server.on("/ligarLuz", HTTP_GET, []() {
    Serial.println("Ligar luz.");
    digitalWrite(luzPin, HIGH);  // Liga a luz
    server.send(200, "text/plain", "Luz Ligada");
  });

  // Rota para desligar a luz
  server.on("/desligarLuz", HTTP_GET, []() {
    Serial.println("Desligar luz.");
    digitalWrite(luzPin, LOW);  // Desliga a luz
    server.send(200, "text/plain", "Luz Desligada");
  });

  server.on("/statusLuz", HTTP_GET, []() {
    String status = (digitalRead(luzPin) == HIGH) ? "Ligado" : "Desligado";
    server.send(200, "text/plain", "Status da Luz: " + status);
  });

  // Inicia o servidor
  server.begin();
}

void loop() {
  server.handleClient();  // Processa as requisições HTTP
}
