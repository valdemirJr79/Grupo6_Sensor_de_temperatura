#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PINO_DADOS 4  // GPIO4 do DS18B20

// Credenciais Wi-Fi (as mesmas do outro código)
const char *ssid = "REDEWORK";
const char *password = "Acessonet05";

// Inicialização do sensor
OneWire oneWire(PINO_DADOS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor;
bool sensorConectado = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");

  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Inicializa o sensor DS18B20
  sensors.begin();

  if (sensors.getAddress(sensor, 0)) {
    sensorConectado = true;
    sensors.setResolution(sensor, 12);
    Serial.println("Sensor encontrado!");
  } else {
    Serial.println("Sensor NÃO encontrado. Verifique a conexão.");
  }
}

void loop() {
  if (!sensorConectado) {
    Serial.println("Sensor ainda não conectado.");
    delay(2000);
    return;
  }

  sensors.requestTemperatures();
  float soma = 0;
  int leituras = 5;

  for (int i = 0; i < leituras; i++) {
    float leitura = sensors.getTempC(sensor);

    if (leitura == DEVICE_DISCONNECTED_C) {
      Serial.println("Erro: Sensor desconectado!");
      return;
    }

    soma += leitura;
    delay(50);
  }

  float temperaturaMedia = soma / leituras;
  Serial.print("Temperatura média: ");
  Serial.print(temperaturaMedia);
  Serial.println(" °C");

  delay(1000);
}
