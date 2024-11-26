import requests
import speech_recognition as sr
import pyttsx3
import serial

# Substitua pela sua chave de API
API_KEY = "AIzaSyAW8NSrWa8g0VSXKv2UqMhV7C7I-b7kL4s"
ENDPOINT = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-latest:generateContent"
historico = []

# Configuração para ESP32 (ajuste porta e baudrate)
ESP32_PORT = "COM3"  # Substitua pela porta da sua ESP32
ESP32_BAUDRATE = 9600

# Inicializando TTS
engine = pyttsx3.init()

def falar(texto):
    """Faz o sistema falar."""
    engine.say(texto)
    engine.runAndWait()

def chamar_gemini(pergunta):
    """Chama a API Gemini para obter uma resposta da IA."""
    global historico
    headers = {
        "Content-Type": "application/json",
    }
    payload = {
        "contents": [
            {
                "parts": [
                    {
                        "text": pergunta
                    }
                ]
            }
        ],
        "generationConfig": {
            "temperature": 0.7,
            "maxOutputTokens": 50,
            "topP": 0.8,
            "topK": 10
        }
    }

    url_completa = f"{ENDPOINT}?key={API_KEY}"

    response = requests.post(url_completa, json=payload, headers=headers)

    if response.status_code == 200:
        data = response.json()
        resposta = data.get("candidates", [{}])[0].get("content", {}).get("parts", [{}])[0].get("text", "Sem resposta")
        historico.append({"pergunta": pergunta, "resposta": resposta})
        return resposta
    else:
        print(f"Erro na API: {response.status_code} - {response.text}")
        return "Desculpe, não consegui processar sua pergunta."

def verificar_comando(pergunta):
    """Verifica se a pergunta é um comando para a ESP32."""
    comandos = {
        "ligar luz": "LIGAR",
        "desligar luz": "DESLIGAR"
    }

    for chave, comando in comandos.items():
        if chave in pergunta.lower():
            return comando
    return None

def enviar_comando_esp32(comando):
    """Envia comandos para a ESP32 via porta serial."""
    try:
        with serial.Serial(ESP32_PORT, ESP32_BAUDRATE, timeout=1) as esp32:
            esp32.write(comando.encode())
            print(f"Comando enviado: {comando}")
    except Exception as e:
        print(f"Erro ao enviar comando para ESP32: {e}")

def escutar_microfone():
    """Captura áudio do microfone e converte em texto."""
    recognizer = sr.Recognizer()
    with sr.Microphone() as source:
        print("Estou ouvindo...")
        recognizer.adjust_for_ambient_noise(source)
        try:
            audio = recognizer.listen(source, timeout=5)
            texto = recognizer.recognize_google(audio, language="pt-BR")
            print(f"Você disse: {texto}")
            return texto
        except sr.WaitTimeoutError:
            print("Nenhuma fala detectada.")
        except sr.UnknownValueError:
            print("Não entendi o que você disse.")
        except sr.RequestError as e:
            print(f"Erro ao acessar o serviço de reconhecimento: {e}")
        return None

# Loop principal
while True:
    pergunta = escutar_microfone()
    if pergunta:
        comando = verificar_comando(pergunta)
        if comando:
            enviar_comando_esp32(comando)
            falar(f"Comando {comando.lower()} enviado.")
        else:
            resposta = chamar_gemini(pergunta)
            falar(resposta)
