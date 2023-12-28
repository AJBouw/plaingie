#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp_log.h>
#include <ESP32Servo.h>
#include <PubSubClient.h>
#include <mqtt_client.h>
#include <WiFi.h>

#include "PlaingieConf.h"

#pragma region | Router access credentials
const char* WI_FI_PASSWORD = WI_FI_PASSWORD_CONF;
const char* WI_FI_SSID = WI_FI_SSID_CONF;
#pragma endregion

#pragma region | MQTT settings
const int MQTT_BROKER_TCP_PORT = MQTT_BROKER_TCP_PORT_CONF;
const int MQTT_BROKER_WS_PORT = MQTT_BROKER_WS_PORT_CONF;

const char* MQTT_BROKER = MQTT_BROKER_CONF;
const char* MQTT_CLIENT_UID_BASE = MQTT_CLIENT_UID_BASE_CONF;
const char* MQTT_PASSWORD = MQTT_PASSWORD_CONF;
const char* MQTT_USERNAME = MQTT_USERNAME_CONF;

const uint16_t MQTT_MAX_PACKET_SIZE_OVERRIDE = 4096;

const unsigned long MQTT_RECONNECT_EVENT_INTERVAL = MQTT_RECONNECT_EVENT_INTERVAL_CONF;
const unsigned long MQTT_VERIFY_IS_ACTIVE_EVENT_INTERVAL = MQTT_VERIFY_IS_ACTIVE_EVENT_INTERVAL_CONF;
#pragma endregion

#pragma region | MQTT Topics
const char* ACTIVATION_DISTRIBUTION = ACTIVATION_DISTRIBUTION_CONF;
const char* ACTIVATION_REQUEST = ACTIVATION_REQUEST_CONF;
const char* ACTIVATION_RESPONSE = ACTIVATION_RESPONSE_CONF;

const char* HOME_BACKYARD_SERVO_1_COMMAND_CURRENT_POSITION = HOME_BACKYARD_SERVO_1_COMMAND_CURRENT_POSITION_CONF;
const char* HOME_BACKYARD_SERVO_1_COMMAND_START_POSITION = HOME_BACKYARD_SERVO_1_COMMAND_START_POSITION_CONF;
const char* HOME_BACKYARD_SERVO_1_SERVO_DATA = HOME_BACKYARD_SERVO_1_SERVO_DATA_CONF;
#pragma endregion

#pragma region | WiFi and MQTT Connection
WiFiClient espWiFiClient;
PubSubClient mqttClient(espWiFiClient);
#pragma endregion

#pragma region | Auxiliary variables
unsigned long lastReconnectToMqttBrokerAttempt = 0;
unsigned long lastVerifyIsActiveAttempt = 0;
unsigned long now = millis();
unsigned long reconnectToMqttAttempt = 0;
#pragma endregion

#pragma region | JSON
// Allocating JSON Document
DynamicJsonDocument docMessage(768);
DynamicJsonDocument docServo(256);
DynamicJsonDocument docServoData(256);
DynamicJsonDocument docMicroControllerUnit(256);

char jsonMessage[256];
char jsonServoData[256];
char jsonMicroControllerUnit[256];
#pragma endregion

#pragma region | Micro Controller Unit
int _microControllerUnitUId = 4;
/**
 * @brief Set the Micro Controller Unit UId object
 * 
 * @param uid 
 */
void setMicroControllerUnitUId(int uid) {
  _microControllerUnitUId = uid;
}

String ipId;
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

#pragma region | IoT Device : Servo
/**
 * @brief Initialize Servo object
 * Most boards can handle max 12 servo objects per board.
 * 
 */
Servo servo_1;

int _iotDeviceUId;
/**
 * @brief Set the IoT Device UId object
 * 
 * @param uid 
 */
void setIoTDeviceUId(int uid) {
  _iotDeviceUId = uid;
}

/**
 * @brief The servo can be connected to each pin 
 * Though, it is recommended by experience to use the following pins
 * 2, 4, 12-19, 21-23, 25-27, 32-33
 */
int gpio_servo_1 = 4;
int _gpio_servo_1;
/**
 * @brief Set the GPIO object
 * 
 * @param gpio 
 */
void setGPIO(String gpio) {
  _gpio_servo_1 = gpio.toInt();
}

String _category;
/**
 * @brief Set the Category object
 * The device's category can be light, light sensor, motion sensor, servo, and webcam.
 * @param category 
 */
void setCategory(String category) {
  _category = category;
}

String _identifier;
/**
 * @brief Set the Identifier object
 * The identifier is the name of the device.
 * @param identifier 
 */
void setIdentifier(String identifier) {
  _identifier = identifier;
}

String _locationDescription;
/**
 * @brief Set the Location Description object
 * Optionally the location description can provide additional information about the
 * device's location.
 * @param locationDescription 
 */
void setLocationDescription(String locationDescription) {
  _locationDescription = locationDescription;
}

String _locationLabel;
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
 * @brief Set the IoT Device State object
 * The IoT device can be active or inactive.
 * @param isActive 
 */
void setIoTDeviceState(bool isActive) {
  _isActive = isActive;
}

bool iotDeviceIsVerified = false;
#pragma endregion

#pragma region | Servo Data
int _currentPosition_1 = 0;
/**
 * @brief Set the Current Position object
 * 
 * @param gpio 
 * @param positionCommand 
 */
void setCurrentPosition(int gpio, int positionCommand) {
  if (gpio == gpio_servo_1) {
      servo_1.write(positionCommand);
    _currentPosition_1 = positionCommand;
  }
}

int previousPosition_1 = 0;

int _startPosition_1 = 0;
/**
 * @brief Set the Start Position object
 * 
 * @param gpio 
 * @param positionCommand 
 */
void setStartPosition(int gpio, int positionCommand) {
  if (gpio == gpio_servo_1) {
    _startPosition_1 = positionCommand;
  }
}
#pragma endregion

/**
 * @brief Set the Device object
 * 
 * @param uid 
 * @param category 
 * @param gpio 
 * @param identifier 
 * @param isActive
 * @param locationDescription
 * @param locationLabel  
 */
void setIoTDevice(int uid, String category, String gpio, String identifier, bool isActive, String locationDescription, String locationLabel) {
  _iotDeviceUId = uid;
  _category = category;
  _gpio_servo_1 = gpio.toInt();
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
  docMicroControllerUnit["mac_id"] = macId;
  docMicroControllerUnit["mqtt_client_uid"] = mqttClientUId;

  // TODO: verify if this is okay or use size_t n
  // Save a few CPU cycles by passing the size of the payload
  // size_t n = serializeJson(docMicroControllerUnit, jsonMicroControllerUnit);
  
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

char* getJSONServoData(int iotDeviceUId, int currentPosition, int startPosition) {
  // Clear JsonDocument
  docServoData.clear();

  // Serialize a JsonDocument into a MQTT message
  docServoData["iot_device_uid"] = iotDeviceUId;
  docServoData["current_position"] = currentPosition;
  docServoData["start_position"] = startPosition;

  // TODO: verify if this is okay or use size_t n
  // Save a few CPU cycles by passing the size of the payload
  size_t n = serializeJson(docServoData, jsonServoData);
  
  serializeJson(docServoData, jsonServoData);
  Serial.println("json servo sensor " + String(jsonServoData));
  return jsonServoData;
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

  if (!microControllerUnitIsActivated && !iotDeviceIsVerified && (String(topic) == ACTIVATION_REQUEST)) {
    // Clear JsonDocument
    docMicroControllerUnit.clear();

    // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
    DeserializationError deserializationError = deserializeJson(docMicroControllerUnit, (const byte*)payload);

    // If deserialization operation was not successful (Ok).
    if (deserializationError) {
      Serial.print(deserializationError.c_str());
    }

    // Extracting values from JsonDocument
    String information = docMicroControllerUnit["information"];
    String message = docMicroControllerUnit["message"];
    int statusCode = docMicroControllerUnit["status_code"];
    int uid = docMicroControllerUnit["uid"];

    if ((uid = _microControllerUnitUId) && (statusCode == 200)) {
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

    int iotDeviceUId = docMessage["uid"];
    String category = docMessage["category"];
    String gpio = docMessage["gpio"];
    String identifier = docMessage["identifier"];
    bool isActive = docMessage["is_active"];
    String locationDescription = docMessage["location_description"];
    String locationLabel = docMessage["location_label"];
    int microControllerUnitUId = docMessage["micro_controller_unit_uid"];

    int startPosition = docMessage["start_position"];
    if (microControllerUnitUId == _microControllerUnitUId) {
      setIoTDevice(iotDeviceUId, category, gpio, identifier, isActive, locationDescription, locationLabel);
      setStartPosition(_gpio_servo_1, startPosition);
      
      iotDeviceIsVerified = true;
    }
  }
  else if (microControllerUnitIsActivated && iotDeviceIsVerified) {
    if (String(topic) == HOME_BACKYARD_SERVO_1_COMMAND_CURRENT_POSITION) {
      // Clear JsonDocument
      docServo.clear();

      // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
      DeserializationError deserializationError = deserializeJson(docServo, (const byte*)payload);

      // If deserialization operation was not successful (Ok).
      if (deserializationError) {
        Serial.print(deserializationError.c_str());
      }

      // Extracting values from JsonDocument
      int iotDeviceUId = docServo["uid"];
      int currentPosition = docServo["current_position"];
      Serial.println(iotDeviceUId);
      Serial.println(currentPosition);

      if ((iotDeviceUId == _iotDeviceUId) && currentPosition != _currentPosition_1) {
        setCurrentPosition(gpio_servo_1, currentPosition);

        // Clear JsonDocument
        docServoData.clear();

        // Serialize a JsonDocument into a MQTT message
        docServoData["uid"] = _iotDeviceUId;
        docServoData["current_position"] = _currentPosition_1;
        docServoData["start_position"] = _startPosition_1;

        // Save a few CPU cycles by passing the size of the payload
        size_t n = serializeJson(docServoData, jsonServoData);

        mqttClient.publish(HOME_BACKYARD_SERVO_1_SERVO_DATA, jsonServoData, n);
      }
    }

    if (String(topic) == HOME_BACKYARD_SERVO_1_COMMAND_START_POSITION) {
      // Clear JsonDocument
      docServo.clear();

      // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
      DeserializationError deserializationError = deserializeJson(docServo, (const byte*)payload);

      // If deserialization operation was not successful (Ok).
      if (deserializationError) {
        Serial.print(deserializationError.c_str());
      }

      // Extracting values from JsonDocument
      int iotDeviceUId = docServo["uid"];
      int startPosition = docServo["start_position"];
      Serial.println(iotDeviceUId);
      Serial.println(startPosition);

      if ((iotDeviceUId == _iotDeviceUId) && startPosition != _startPosition_1) {
        setStartPosition(gpio_servo_1, startPosition);

        // Clear JsonDocument
        docServoData.clear();

        // Serialize a JsonDocument into a MQTT message
        docServoData["uid"] = _iotDeviceUId;
        docServoData["current_position"] = _currentPosition_1;
        docServoData["start_position"] = _startPosition_1;

        // Save a few CPU cycles by passing the size of the payload
        size_t n = serializeJson(docServoData, jsonServoData);

        mqttClient.publish(HOME_BACKYARD_SERVO_1_SERVO_DATA, jsonServoData, n);
      }
    }
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
      mqttClient.subscribe(HOME_BACKYARD_SERVO_1_COMMAND_CURRENT_POSITION);
      mqttClient.subscribe(HOME_BACKYARD_SERVO_1_COMMAND_START_POSITION);

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
  
  servo_1.setPeriodHertz(50); // Standard 50hz
  servo_1.attach(gpio_servo_1, 1000, 2000);

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
  if (!servo_1.attached()) {
    servo_1.setPeriodHertz(50);
    servo_1.attach(gpio_servo_1, 1000, 2000);
  }

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
    reconnectToMQTTBroker();
  }

  if (!mqttClient.connected()) {
    reconnectToMQTTBroker();
  }
  
  mqttClient.loop();

  now = millis();
  if (((!microControllerUnitIsActivated && !iotDeviceIsVerified) || (microControllerUnitIsActivated && !iotDeviceIsVerified)) && ((now - lastVerifyIsActiveAttempt) > MQTT_VERIFY_IS_ACTIVE_EVENT_INTERVAL)) {
    lastVerifyIsActiveAttempt = now;
    mqttClient.publish(ACTIVATION_REQUEST, getJSONActivationRequest(macId, mqttClientUId));
  }
}
#pragma endregion
