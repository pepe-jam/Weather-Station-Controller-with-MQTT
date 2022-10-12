#include <ArduinoJson.h>

#include "network.hpp"
#include "sensors.hpp"
#include "utils.hpp"

// Interval in microseconds how often readings should be made and a message should be sent via MQTT.
const unsigned int messageInterval = 5000;

// Sensors
HumidityTemperatureSensor humidityTemperatureSensor;
PressureTemperatureSensor pressureTemperatureSensor(messageInterval);
LightSensor lightSensor;
RainSensor rainSensor;
GasSensor gasSensor;

void setup() {
  SETUP_SERIAL(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  connectToWifi();
  connectToMQTT(messageInterval * 2);
}

void loop() {
  if (haveMillisPassed(messageInterval)) {
    // Light the internal LED to let it blink when doing sensor readings.
    digitalWrite(LED_BUILTIN, LOW);

    // Construct a JSON message.
    StaticJsonDocument<200> payload;  // TODO optimize byte size to actual size of JSON contents
    payload["temperature"] = String(humidityTemperatureSensor.readTemperature());
    payload["humidity"] = String(humidityTemperatureSensor.readHumidity());
    payload["pressure"] = String(pressureTemperatureSensor.readPressure());
    payload["rain"] = String(rainSensor.read());
    payload["windSpeed"] = String();
    payload["windDirection"] = String();
    payload["lightIntensity"] = String(lightSensor.read());
    auto gasReadings = gasSensor.read();
    payload["airQuality"]["acetone"] = gasReadings["C3H6O"];
    payload["airQuality"]["ammonium"] = gasReadings["NH4"];
    payload["airQuality"]["carbonDioxide"] = gasReadings["CO2"];
    payload["airQuality"]["carbonMonoxide"] = gasReadings["CO"];
    payload["airQuality"]["ethanol"] = gasReadings["C2H6O"];
    payload["airQuality"]["toluene"] = gasReadings["C7H8"];

    sendMQTTMessage("weather", payload);

    digitalWrite(LED_BUILTIN, HIGH);
    LOG_LN();
  }
}
