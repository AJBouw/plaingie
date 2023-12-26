#include <ArduinoJson.h>
#include <esp_log.h>
#include <PubSubClient.h>
#include <mqtt_client.h>
#include <WiFi.h>

#include "PlaingieConf.h"

#pragma region | Router access credentials
const char* WI_FI_SSID = WI_FI_SSID_CONF;
const char* WI_FI_PASSWORD = WI_FI_PASSWORD_CONF;
#pragma endregion

#pragma region | MQTT settings
const int MQTT_BROKER_TCP_PORT = MQTT_BROKER_TCP_PORT_CONF;
const int MQTT_BROKER_WS_PORT = MQTT_BROKER_WS_PORT_CONF;

const char* MQTT_BROKER = MQTT_BROKER_CONF;
const char* MQTT_CLIENT_UID_BASE = MQTT_CLIENT_UID_BASE_CONF;
const char* MQTT_PASSWORD = MQTT_PASSWORD_CONF;
const char* MQTT_USERNAME = MQTT_USERNAME_CONF;

const uint16_t MQTT_MAX_PACKET_SIZE_OVERRIDE = 4096;

const unsigned long LIGHT_EVENT_INTERVAL = LIGHT_EVENT_INTERVAL_CONF;
const unsigned long MQTT_RECONNECT_EVENT_INTERVAL = MQTT_RECONNECT_EVENT_INTERVAL_CONF;
#pragma endregion

#pragma region | MQTT Topics
const char* ACTIVATION_REQUEST = ACTIVATION_REQUEST_CONF;
const char* ACTIVATION_RESPONSE = ACTIVATION_RESPONSE_CONF;
const char* ACTIVATION_DISTRIBUTION = ACTIVATION_DISTRIBUTION_CONF;

const char* HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA = HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA_CONF;
#pragma endregion

#pragma region | WiFi and MQTT Connection
WiFiClient espWiFiClient;
PubSubClient mqttClient(espWiFiClient);
#pragma endregion

#pragma region | Auxiliary variables
unsigned long lastMessageLightSensor_1 = 0;
unsigned long lastReconnectToMqttBrokerAttempt = 0;
unsigned long now = millis();
unsigned long reconnectToMqttAttempt = 0;
#pragma endregion

#pragma region | JSON
// Allocating JSON Document
DynamicJsonDocument docLightSensor(256);
DynamicJsonDocument docLightSensorData(256);
DynamicJsonDocument docMessage(256);
DynamicJsonDocument docMicroControllerUnit(256);

char jsonLightSensor[256];
char jsonLightSensorData[256];
char jsonMessage[256];
char jsonMicroControllerUnit[256];
#pragma endregion

#pragma region | Micro Controller Unit
int _microControllerUnitUId = 2;
/**
 * @brief Set the Micro Controller Unit UId object
 * 
 * @param uid 
 */
void setMicroControllerUnitUId(int uid) {
  _microControllerUnitUId = uid;
}

String ipId;
/// @brief Get the Micro Controller Unit its current IP addrress.
/// @return IP address represented by ipId (ip_id database attribute).

/**
 * @brief Set IP Id object
 * The Micro Controller Unit's IP address is known as the attribute IP Id.
 * @return String IP Id
 */
String getIPId() {
  return WiFi.localIP().toString();
}

String macId;
/**
 * @brief Set the MAC Id object
 * The Micro Controller Unit's MAC address is known as the attribute MAC Id.
 * @return String MAC Id
 */
String getMACId() {
  return WiFi.macAddress(); 
}

char concatMQTTClientUId[50];
char* mqttClientUId;
/**
 * @brief Concatenate MQTT Client UId object
 * MQTT Client UId is a composition of the Micro Controller Unit's name and MAC Id.
 * @param macId 
 * @return char* MQTT Client UId
 */
char* getMQTTClientUId(String macId) {
  strcpy(concatMQTTClientUId, MQTT_CLIENT_UID_BASE);
  return strcat(concatMQTTClientUId, macId.c_str());
}

bool microControllerUnitIsActivated = false;
#pragma endregion

#pragma region | IoT Device : Light Sensor
int _iotDeviceUId = 2;
/**
 * @brief Set the IoT device UId object
 * 
 * @param uid 
 */
void setIoTDeviceUId(int uid) {
  _iotDeviceUId = uid;
}

/**
 * @brief Light sensors give an analog voltage as an output voltage
 * The output voltage is between 0 and 5 Volt converted into a digital value between 0 and 1023.
 * 10k resistor from analog in 0 to ground.
 */
int brightnessMinimumValue = 0; // Set by experiment
int brightnessMaximumValue = 1023; // Set by experiment
// int levelRangeMinimum = 0;
// int levelRangeMaximum = 3;

int _currentLightIntensity_1;
void getLightIntensity(int gpio) {
  _currentLightIntensity_1 = analogRead(gpio);
}
/**
 * @brief The LDR (light sensor) has to be connected to ADC1 pin using WiFi
 * For ESP32 Wroom this is GPIO 32-36, 39
 */
int gpio_light_sensor_1 = 36;

int _gpio_light_sensor_1;
/**
 * @brief Set the GPIO object
 * 
 * @param gpio 
 */
void setGPIO(String gpio) {
  _gpio_light_sensor_1 = gpio.toInt();
}

int previousLightIntensity_1;

String _category = "light sensor";
/**
 * @brief Set the Category object
 * The device's category can be light, light sensor, motion sensor, servo, and webcam.
 * @param category 
 */
void setCategory(String category) {
  _category = category;
}

String _identifier = "light-sensor-1";
/**
 * @brief Set the Identifier object
 * The identifier is the name of the device.
 * @param identifier 
 */
void setIdentifier(String identifier) {
  _identifier = identifier;
}

String _locationDescription = "backyard fence";
/**
 * @brief Set the Location Description object
 * Optionally the location description can provide additional information about the
 * device's location.
 * @param locationDescription 
 */
void setLocationDescription(String locationDescription) {
  _locationDescription = locationDescription;
}

String _locationLabel = "backyard";
/**
 * @brief Set the Location Label object
 * The location label can be for example attic, backyard, bedroom, front yard, kitchen, living room.
 * @param locationLabel 
 */
void setLocationLabel(String locationLabel) {
  _locationLabel = locationLabel;
}

bool _isActive;
/**
 * @brief Set the IoT ddevice state object
 * The IoT device can be active or inactive.
 * @param isActive 
 */
void setIoTDeviceState(bool isActive) {
  _isActive = isActive;
}

bool iotDeviceIsVerified = false;
#pragma endregion

/**
 * @brief Set the IoT device object
 * 
 * @param uid 
 * @param category
 * @param gpio 
 * @param identifier 
 * @param isActive
 * @param locationDescription
 * @param locationLabel
 */
void setIoTDevice(int uid, String gpio, String identifier, bool isActive, String category, String locationDescription, String locationLabel) {
  _iotDeviceUId = uid;
  _category = category;
  _gpio_light_sensor_1 = gpio.toInt();
  _identifier = identifier;
  _isActive = isActive;
  _locationDescription = locationDescription;
  _locationLabel = locationLabel;
}

/**
 * @brief Create JSON Activation Request object
 * 
 * @param macId 
 * @param mqttClientUId 
 * @return char* 
 */
char* getJSONActivationRequest(String macId, String mqttClientUId) {
  // Clear JsonDocument
  docMicroControllerUnit.clear();

  // Serialize a JsonDocument into a MQTT message
  docMicroControllerUnit["macId"] = macId;
  docMicroControllerUnit["mqttClientUId"] = mqttClientUId;

  // TODO: verify if this is okay or use size_t n
  // Save a few CPU cycles by passing the size of the payload
  size_t n = serializeJson(docMicroControllerUnit, jsonMicroControllerUnit);
  
  // mqttClient.publish(HOME_BACKYARD_LIGHT_1_LIGHT_DATA, jsonData, n);
  serializeJson(docMicroControllerUnit, jsonMicroControllerUnit);
  
  return jsonMicroControllerUnit;
}

/**
 * @brief Create JSON Light Sensor Data object
 * 
 * @param gpio 
 * @param iotDeviceUId 
 * @param lightIntensity 
 * @return char* 
 */
char* getJSONLightSensorData(int iotDeviceUId, int lightIntensity) {
  // Clear JsonDocument
  docLightSensorData.clear();

  // Serialize a JsonDocument into a MQTT message
  docLightSensorData["iot_device_uid"] = iotDeviceUId;
  docLightSensorData["light_intensity"] = lightIntensity;

  // TODO: verify if this is okay or use size_t n
  // Save a few CPU cycles by passing the size of the payload
  size_t n = serializeJson(docLightSensorData, jsonLightSensorData);
  
  serializeJson(docLightSensorData, jsonLightSensorData);
  Serial.println("json light sensor " + String(jsonLightSensorData));
  return jsonLightSensorData;
}

/**
 * @brief Connect to WiFi network
 * Get the IP address, and MAC address.
 * Set the IP Id, MAC Id, and MQTT Client Id object.
 */
void connectToWiFi() {
  Serial.print("ESP connecting to WiFi network ");
  
  // Connect to the WiFi network
  WiFi.begin(WI_FI_SSID, WI_FI_PASSWORD);

  // Wait until the connection has been confirmed, before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  ipId = getIPId();
  Serial.println("ESP connected to IP address " + ipId);
  
  macId = getMACId();
  Serial.println("ESP mac address " + macId);

  mqttClientUId = getMQTTClientUId(macId);
  Serial.println(mqttClientUId);
}

/**
 * @brief MQTT Callback function
 * Mosquitto callback function is executed when an event occurs.
 * Before permitting MQTT communication the activation must be successfully completed.
 * Each topic the Micro Controller Unit is subscribed
 * the payload is parsed with deserializeJson()
 * @param topic 
 * @param payload 
 * @param length 
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  if (!microControllerUnitIsActivated && !iotDeviceIsVerified && (String(topic) == ACTIVATION_RESPONSE)) {
    // Clear JsonDocument
    docMicroControllerUnit.clear();

    // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
    DeserializationError deserializationError = deserializeJson(docMicroControllerUnit, (const byte*)payload);

    // If deserialization operation was not successful (Ok).
    if (deserializationError) {
      Serial.print(deserializationError.c_str());
    }

    int uid = 2;
    int status_code = 200;
    String information = "2";
    String message = "OK";

    if ((uid = _microControllerUnitUId) && (status_code == 200)) {
      microControllerUnitIsActivated = true;
    }
  }
  else if (microControllerUnitIsActivated && !iotDeviceIsVerified && (String(topic) == ACTIVATION_DISTRIBUTION)) {
    // Clear JsonDocument
    docMessage.clear();

    // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
    DeserializationError deserializationError = deserializeJson(docMessage, (const byte*)payload);

    // If deserialization operation was not successful (Ok).
    if (deserializationError) {
      Serial.print(deserializationError.c_str());
    }

    int microControllerUnitUId = 1;
    int iotDeviceUId = 1;
    String category = "light sensor";
    String gpio = "36";
    String identifier = "light-sensor-1";
    bool isActive = true;
    String locationDescription = "backyard fence";
    String locationLabel = "backyard";
    
    iotDeviceIsVerified = true;
  }
}

/**
 * @brief Connect to MQTT Broker
 * The project utilises the default 15 seconds keepalive interval.
 * By experimental experience the default is sufficient.
 */
void connectToMQTTBroker() {
  // Configure the MQTT Client
  mqttClient.setServer(MQTT_BROKER, MQTT_BROKER_TCP_PORT);
  
  // Set the buffer to handle the JSON.
  // A buffer overflow of the message buffer will result in a callback not being invoked.
  mqttClient.setBufferSize(MQTT_MAX_PACKET_SIZE_OVERRIDE);

  // Set the MQTT message callback handler function.
  mqttClient.setCallback(callback);
}

/**
 * @brief Reconnect to MQTT Broker
 * Verify the MQTT connection, and reconnect if the Micro Controller Unit
 * is not connected.
 * Subscribe when reconnecting in order to ensure subscription.
 * Publish the activation request in order to enable MQTT communication.
 */
void reconnectToMQTTBroker() {
  now = millis();

  while ((!mqttClient.connected()) && ((now - lastReconnectToMqttBrokerAttempt)  > MQTT_RECONNECT_EVENT_INTERVAL)) {
    microControllerUnitIsActivated = false;
    iotDeviceIsVerified = false;

    lastReconnectToMqttBrokerAttempt = now;
    reconnectToMqttAttempt++;

    if (mqttClient.connect(mqttClientUId, MQTT_USERNAME, MQTT_PASSWORD)){
      mqttClient.subscribe(ACTIVATION_RESPONSE);
      mqttClient.subscribe(ACTIVATION_DISTRIBUTION);

      mqttClient.publish(ACTIVATION_REQUEST, getJSONActivationRequest(macId, mqttClientUId));
    }
    else {
      Serial.print("Error MQTT connection. Trying to reconnect within 5 seconds.");
    }
  }
  
  lastReconnectToMqttBrokerAttempt = 0;
  reconnectToMqttAttempt = 0;
}

# pragma region | Setup
/**
 * @brief Executed oonece, after the power-up or reset.
 * Used to initialze variables, pin/gpio modes, and start using libaries,
 * including initializing the connection with the WiFi network and
 * the MQTT Broker.
 */
void setup() {
  Serial.begin(115200);
  
  pinMode(gpio_light_sensor_1, INPUT);

  connectToWiFi();
  connectToMQTTBroker();
}
#pragma endregion

#pragma region | Loop
/**
 * @brief Executed initialy after setup.
 * Handles the main tasks: maintaining the connection with the
 * WiFi network and the MQTT Broker in order to process MQTT communication.
 */
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
    reconnectToMQTTBroker();
  }

  if (!mqttClient.connected()) {
    reconnectToMQTTBroker();
  }
  
  mqttClient.loop();
  
  getLightIntensity(gpio_light_sensor_1);
  now = millis();
  iotDeviceIsVerified = true;
  if (iotDeviceIsVerified && (_currentLightIntensity_1 != previousLightIntensity_1) && ((now - lastMessageLightSensor_1) > LIGHT_EVENT_INTERVAL)) {
    lastMessageLightSensor_1 = now;
    previousLightIntensity_1 = _currentLightIntensity_1;
    
    mqttClient.publish(HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA, getJSONLightSensorData(_iotDeviceUId, _currentLightIntensity_1));
  }
}
#pragma endregion