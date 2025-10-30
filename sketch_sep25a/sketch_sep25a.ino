#include <OneWire.h>
#include <DallasTemperature.h>

#define PINO_DADOS 4  // GPIO4 do DS18B20

// Inicialização do sensor
OneWire oneWire(PINO_DADOS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor;
bool sensorConectado = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");

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