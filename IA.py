import whisper
import os
import serial
import time
from gtts import gTTS
import playsound

# Configurações da IA
nome_ia = "Aurora"
objetivo_ia = "ajudar a controlar sua casa e responder suas perguntas"

# Configuração da porta serial para ESP32
porta_serial = "COM3"  # Substitua pela porta usada pela sua ESP32
baud_rate = 9600

# Inicializar o modelo Whisper
modelo = "base"  # Modelos disponíveis: tiny, base, small, medium, large
modelo_whisper = whisper.load_model(modelo)


def transcrever_audio(caminho_audio):
    """
    Transcreve um áudio usando Whisper.
    """
    if not os.path.exists(caminho_audio):
        return None

    resultado = modelo_whisper.transcribe(caminho_audio)
    return resultado["text"]


def falar(texto):
    """
    Converte texto em fala usando gTTS e reproduz o áudio.
    """
    try:
        tts = gTTS(texto, lang="pt")
        tts.save("fala.mp3")
        playsound.playsound("fala.mp3")
        os.remove("fala.mp3")
    except Exception as e:
        print(f"Erro ao gerar fala: {e}")


def enviar_comando_esp32(comando):
    """
    Envia comandos para a ESP32 via porta serial.
    """
    try:
        with serial.Serial(porta_serial, baud_rate, timeout=1) as esp32:
            esp32.write(comando.encode())
            time.sleep(1)
            resposta = esp32.readline().decode().strip()
            return resposta
    except Exception as e:
        print(f"Erro ao enviar comando para a ESP32: {e}")
        return None


def processar_comando(texto):
    """
    Processa comandos de voz reconhecidos e toma ações.
    """
    texto = texto.lower()

    if "ligar luz" in texto:
        resposta = enviar_comando_esp32("LIGAR_LUZ")
        return "Luz ligada!" if resposta else "Erro ao ligar a luz."

    elif "desligar luz" in texto:
        resposta = enviar_comando_esp32("DESLIGAR_LUZ")
        return "Luz desligada!" if resposta else "Erro ao desligar a luz."

    elif "qual seu nome" in texto:
        return f"Meu nome é {nome_ia}. Meu objetivo é {objetivo_ia}."

    else:
        return f"Desculpe, não entendi o comando: {texto}"


def main():
    print(f"{nome_ia} está ativa e pronta para ajudar!")
    falar(f"Olá, meu nome é {nome_ia}. Estou pronta para ajudar você!")

    while True:
        print("\nGrave um comando de voz (nome do arquivo: 'audio.wav')")
        input("Pressione Enter para continuar...")

        # Transcrever o áudio gravado
        print("Transcrevendo o áudio...")
        texto_transcrito = transcrever_audio("audio.wav")
        if texto_transcrito:
            print(f"Você disse: {texto_transcrito}")
            falar(f"Você disse: {texto_transcrito}")

            # Processar o comando
            resposta = processar_comando(texto_transcrito)
            print(f"{nome_ia}: {resposta}")
            falar(resposta)
        else:
            print("Erro: Não consegui transcrever o áudio.")
            falar("Desculpe, não consegui entender o áudio.")


if __name__ == "__main__":
    main()
