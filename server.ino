#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>  // Biblioteca para mDNS


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

  if (MDNS.begin("flazo0")) {
    Serial.println("mDNS iniciado com sucesso");
  } else {
    Serial.println("Falha ao iniciar o mDNS");
  }

  server.on("/", HTTP_GET, []() {
      String html = "<!DOCTYPE html>";
      html += "<html lang=\"pt-br\">";
      html += "<head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>ProjetoX - by flazo0</title><style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:'Trebuchet MS',sans-serif;background-color:#0f0f0f;color:#fff;display:flex;justify-content:center;align-items:center;height:100vh;flex-direction:column}.container{text-align:center}h1{font-size:2.5rem;margin-bottom:30px}.group-container{display:flex;flex-wrap:wrap;justify-content:center;gap:20px;margin-top:60px}.group{display:flex;flex-direction:column;align-items:center;gap:15px}.group h2{font-size:1.5rem;margin-bottom:10px}.control-btn{background-color:#1e1e1e;color:#fff;border:none;padding:15px 30px;font-size:1.2rem;cursor:pointer;border-radius:8px;transition:background-color 0.3s;width:200px;margin:5px 0}.control-btn:hover{background-color:#2d2d2d}.ok{font-family:'Courier New',sans-serif;font-size:12px;margin-top:30px}</style></head>";
      html += "<body><div class=\"container\"><h1 style=\"color:#ffc9c9;\">Painel de Controle</h1>";
      html += "<div class=\"group-container\">";
      html += "<div class=\"group\"><h2>Ventilador</h2><button class=\"control-btn\" onclick=\"toggleDevice('ventilador', 'ligar')\">Ligar</button><button class=\"control-btn\" onclick=\"toggleDevice('ventilador', 'desligar')\">Desligar</button><button class=\"control-btn\" onclick=\"getStatus('ventilador')\">Status</button></div>";
      html += "<div class=\"group\"><h2>Luz</h2><button class=\"control-btn\" onclick=\"toggleDevice('luz', 'ligar')\">Ligar</button><button class=\"control-btn\" onclick=\"toggleDevice('luz', 'desligar')\">Desligar</button><button class=\"control-btn\" onclick=\"getStatus('luz')\">Status</button></div>";
      html += "<div class=\"group\"><h2>Tomada</h2><button class=\"control-btn\" onclick=\"toggleDevice('tomada', 'ligar')\">Ligar</button><button class=\"control-btn\" onclick=\"toggleDevice('tomada', 'desligar')\">Desligar</button><button class=\"control-btn\" onclick=\"getStatus('tomada')\">Status</button></div>";
      html += "</div>";
      html += "<div class=\"ok\"><p>© 2024 FlazO0. Todos os direitos reservados.</p></div>";
      html += "</div>";
      html += "<script>";
      html += "function toggleDevice(device, action) {";
      html += "const ipMap = {'ventilador': '192.168.4.100', 'luz': '192.168.4.150', 'tomada': '192.168.4.200'};";
      html += "const url = `http://${ipMap[device]}/${action}${device.charAt(0).toUpperCase() + device.slice(1)}`;";
      html += "fetch(url).then(response => response.text()).then(data => {";
      html += "console.log(`${device} ${action} com sucesso!`);";
      html += "alert(`${device.charAt(0).toUpperCase() + device.slice(1)} ${action} com sucesso!`);";
      html += "}).catch(error => {";
      html += "console.error('Erro ao controlar o dispositivo:', error);";
      html += "alert(`Erro ao ${action} ${device.charAt(0).toUpperCase() + device.slice(1)}.`);";
      html += "});";
      html += "}";

      html += "function getStatus(device) {";
      html += "const ipMap = {'ventilador': '192.168.4.100', 'luz': '192.168.4.150', 'tomada': '192.168.4.200'};";
      html += "const url = `http://${ipMap[device]}/status${device.charAt(0).toUpperCase() + device.slice(1)}`;";
      html += "fetch(url).then(response => response.text()).then(data => {";
      html += "alert(`Status de ${device.charAt(0).toUpperCase() + device.slice(1)}: ${data}`);";
      html += "}).catch(error => {";
      html += "console.error('Erro ao obter o status do dispositivo:', error);";
      html += "alert(`Erro ao obter o status de ${device.charAt(0).toUpperCase() + device.slice(1)}.`);";
      html += "});";
      html += "}";
      html += "</script>";
      html += "</body></html>";

      server.send(200, "text/html", html);
  });


  // Inicia o servidor
  server.begin();
}

void sendHTTPRequest(String device, String action) {
  HTTPClient http;
  String ipMap = "";

  // Mapeia os dispositivos para seus respectivos IPs
  if (device == "Ventilador") {
    ipMap = "192.168.4.100";
  } else if (device == "Luz") {
    ipMap = "192.168.4.150";
  } else if (device == "Ext") {
    ipMap = "192.168.4.200";
  }

  // Corrigindo a chamada para toUpperCase

  // Converte o primeiro caractere para maiúsculo
  char firstCharUpperCase = toupper(device.charAt(0)); 
  String formattedDevice = String(firstCharUpperCase) + device.substring(1);

  // Monta a URL
  String url = "http://" + ipMap + "/" + action + formattedDevice;
  http.begin(url);  // Inicia a requisição HTTP
  int httpCode = http.GET();  // Envia a requisição GET

  if (httpCode > 0) {
    String payload = http.getString();  // Obtém a resposta
    Serial.println("Sucesso");
  } else {
    Serial.println("Erro ao conectar ao dispositivo " + device + ". Código: " + String(httpCode) + "URL: " + url);
  }

  http.end();  // Finaliza a requisição HTTP
}


void loop() {
  server.handleClient();  // Processa as requisições HTTP

  // Lê comandos da porta serial
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();  // Remove espaços em branco no início e no final

    // Envia requisição HTTP para o dispositivo correspondente
    if (comando == "LIGAR_VENTILADOR") {
      sendHTTPRequest("Ventilador", "ligar");
    } else if (comando == "DESLIGAR_VENTILADOR") {
      sendHTTPRequest("Ventilador", "desligar");

    } else if (comando == "LIGAR_LUZ") {
      sendHTTPRequest("Luz", "ligar");
    } else if (comando == "DESLIGAR_LUZ") {
      sendHTTPRequest("Luz", "desligar");
    } else if (comando == "LIGAR_EXTENSAO") {
      sendHTTPRequest("Ext", "ligar");
    } else if (comando == "DESLIGAR_EXTENSAO") {
      sendHTTPRequest("Ext", "desligar");
    } else if (comando == "STATUS_VENTILADOR") {
      sendHTTPRequest("Ventilador", "status");
    } else if (comando == "STATUS_LUZ") {
      sendHTTPRequest("Luz", "status");
    } else if (comando == "STATUS_EXTENSAO") {
      sendHTTPRequest("Ext", "status");
    } else {
      Serial.println("Comando inválido");
    }
  }
}
