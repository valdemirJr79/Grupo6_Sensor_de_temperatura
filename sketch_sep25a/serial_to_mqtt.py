import serial
import paho.mqtt.client as mqtt
import time

# Configurações da porta serial
serial_port = 'COM7'
baud_rate = 115200 # Deve ser IGUAL ao Serial.begin() no seu ESP32

# Configurações do broker MQTT
broker_address = "test.mosquitto.org"
broker_port = 1883
topic = "esp32/dados_sensores"
client_id = "Python-Serial-Publisher"

# --- Callbacks MQTT ---

# Assinatura correta para a V2 (5 argumentos)
def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Conectado ao broker MQTT!")
    else:
        print(f"Falha na conexão, código de retorno: {reason_code}")

# CORRIGIDO: Assinatura correta para a V2 (5 argumentos)
# A biblioteca envia rc, reason_code e properties
def on_disconnect(client, userdata, rc, reason_code, properties):
    print("Desconectado do broker MQTT!")

# Assinatura correta para a V2 (5 argumentos)
def on_publish(client, userdata, mid, reason_code, properties):
    # Este callback é opcional, mas bom para debug
    print(f"Mensagem (mid: {mid}) publicada.")

# --- Inicialização ---

# Usa a API Versão 2
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id)

# Define os callbacks
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_publish = on_publish

# Conecta ao broker MQTT
try:
    client.connect(broker_address, broker_port, 60)
except Exception as e:
    print(f"Não foi possível conectar ao MQTT: {e}")
    exit(1) # Sai do script se não puder conectar

# Inicia o loop do MQTT em uma thread separada
client.loop_start()

try:
    # Abre a porta serial
    ser = serial.Serial(serial_port, baud_rate, timeout=1)
    print(f"Porta serial {serial_port} aberta. Lendo dados...")

    while True:
        # Lê uma linha da porta serial (ignora bytes malformados)
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        
        # Se a linha não estiver vazia
        if line:
            
            # Filtro para publicar APENAS os dados de temperatura
            if line.startswith("Temperatura média:"):
                print(f"Dados recebidos da serial: {line}")
                
                # Publica a mensagem no tópico
                result = client.publish(topic, line)
                
                if result.rc != mqtt.MQTT_ERR_SUCCESS:
                     print(f"Falha ao enfileirar a mensagem: {result.rc}")
            else:
                # Ignora o "lixo" do boot, mas mostra no console
                print(f"Serial (ignorando): {line}")

except serial.SerialException as e:
    print(f"Erro ao abrir a porta serial: {e}")
except KeyboardInterrupt:
    print("Programa encerrado pelo usuário.")
finally:
    # Limpa as conexões
    print("Fechando conexões...")
    client.loop_stop()
    client.disconnect()
    if 'ser' in locals() and ser.is_open:
        ser.close()
    print("Conexão MQTT e porta serial fechadas.")