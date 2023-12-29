#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp_log.h>
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

const unsigned long LIGHT_EVENT_INTERVAL = LIGHT_EVENT_INTERVAL_CONF;
const unsigned long MQTT_RECONNECT_EVENT_INTERVAL = MQTT_RECONNECT_EVENT_INTERVAL_CONF;
const unsigned long MQTT_VERIFY_IS_ACTIVE_EVENT_INTERVAL = MQTT_VERIFY_IS_ACTIVE_EVENT_INTERVAL_CONF;
#pragma endregion

#pragma region | MQTT Topics
const char* ACTIVATION_DISTRIBUTION = ACTIVATION_DISTRIBUTION_CONF;
const char* ACTIVATION_REQUEST = ACTIVATION_REQUEST_CONF;
const char* ACTIVATION_RESPONSE = ACTIVATION_RESPONSE_CONF;

const char* HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH = HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH_CONF;
const char* HOME_BACKYARD_LIGHT_1_COMMAND_OPERATING_MODE = HOME_BACKYARD_LIGHT_1_COMMAND_OPERATING_MODE_CONF;
const char* HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY = HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY_CONF;

const char* HOME_BACKYARD_LIGHT_1_LIGHT_DATA = HOME_BACKYARD_LIGHT_1_LIGHT_DATA_CONF;
const char* HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA = HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA_CONF;
const char* HOME_BACKYARD_MOTION_SENSOR_1_MOTION_SENSOR_DATA = HOME_BACKYARD_MOTION_SENSOR_1_MOTION_SENSOR_DATA_CONF;
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
DynamicJsonDocument docLightData(256);
DynamicJsonDocument docLightSensorData(256);
DynamicJsonDocument docMessage(768);
DynamicJsonDocument docMicroControllerUnit(256);
DynamicJsonDocument docMotionSensorData(256);

char jsonLightData[256];
char jsonMessage[256];
char jsonMicroControllerUnit[256];
#pragma endregion

#pragma region | Micro Controller Unit
int _microControllerUnitUId = 1;

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

#pragma region | IoT Device: Light
int _iotDeviceUId;
/**
 * @brief Set the IoT Device UId object
 * 
 * @param uid 
 */
void setIoTDeviceUId(int uid) {
  _iotDeviceUId = uid;
}

int _gpio_light_1 = 4;
/**
 * @brief Set the GPIO object
 * 
 * @param gpio 
 */
void setGPIO(String gpio) {
  _gpio_light_1 = gpio.toInt();
}

String _category;
/**
 * @brief Set the Category object
 * Initially, the device's categories will be
 * light, light sensor, motion sensor, servo, and webcam.
 * 
 * @param category 
 */
void setCategory(String category) {
  _category = category;
}

String _identifier;
/**
 * @brief Set the Identifier object
 * The identifier is the name of the device.
 * 
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
 * 
 * @param locationDescription 
 */
void setLocationDescription(String locationDescription) {
  _locationDescription = locationDescription;
}

String _locationLabel;
/**
 * @brief Set the Location Label object
 * Initially. the location label will be backyard.
 * In future the location label can also be for example
 * attic, backyard, bedroom, front yard, kitchen, living room.
 * 
 * @param locationLabel 
 */
void setLocationLabel(String locationLabel) {
  _locationLabel = locationLabel;
}

bool _isActive;
/**
 * @brief Set the IoT Device State object
 * The IoT device can be active or inactive.
 * 
 * @param isActive 
 */
void setIoTDeviceState(bool isActive) {
  _isActive = isActive;
}

bool iotDeviceIsVerified = false;
#pragma endregion

#pragma region | Light Data
int _currentLightStatus_1;
int previousLightStatus_1;
/**
 * @brief Get the Light Status object
 * Read the current Light Status.
 * 
 * @param gpio 
 * @return int Value 1, the light is turned on, and
 * value is 2, the light is turned off.
 */
int getLightStatus(int gpio) {
  return digitalRead(gpio);
}

int _visibilityBright;
int _visibilityDark;
int _visibilityDim;
/**
 * @brief Set the Visibility object
 * 
 * @param visibilityBright 
 * @param visibilityDark 
 * @param visibilityDim 
 */
void setVisibility(int visibilityBright, int visibilityDark, int visibilityDim) {
  _visibilityBright = visibilityBright;
  _visibilityDark = visibilityDark;
  _visibilityDim = visibilityDim;
}

String _currentOperatingMode_1;
/**
 * @brief Set the Operating Mode object
 * The operation mode can be manual, brightness, motion, and complete.
 * 
 * @param operatingMode 
 */
void setOperatingMode(String operatingMode) {
  Serial.println("operatingMode");
  Serial.println(operatingMode);
  _currentOperatingMode_1 = operatingMode;
  Serial.println("current");
  Serial.println(_currentOperatingMode_1);
}

String previousOperatingMode_1;
#pragma endregion

#pragma region | Light Sensor Data
int lightIntensity_1;
#pragma endregion

#pragma region | Motion Sensor Data
bool motionIsDetected_1 = false;
bool motionIsDetected_2 = false;
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
  _gpio_light_1 = gpio.toInt();
  _identifier = identifier;
  _isActive = isActive;
  _locationDescription = locationDescription;
  _locationLabel = locationLabel;
}

/**
 * @brief Set the Light Data object
 * 
 * @param status 
 * @param operatingMode 
 * @param visibilityBright 
 * @param visibilityDark 
 * @param visibilityDim 
 */
void setLightData(int status, String operatingMode, int visibilityBright, int visibilityDark, int visibilityDim) {
  _currentLightStatus_1 = status;
  _currentOperatingMode_1 = operatingMode;
  _visibilityBright = visibilityBright;
  _visibilityDark = visibilityDark;
  _visibilityDim = visibilityDim;
}

/**
 * @brief Set the Light Status object
 * 
 * @param gpio 
 * @param lightCommand 
 * @return int Value 1, turn the light on, and
 * value 2, turn the light off.
 */
void setLightStatusByApp(int gpio, int lightCommand) {
  if (lightCommand != previousLightStatus_1) {
    digitalWrite(gpio, lightCommand);
    _currentLightStatus_1 = lightCommand;
  }
  else if (lightCommand == previousLightStatus_1) {
    Serial.print("No action required");
  }
  else {
    Serial.print("Invalid light command");
  }
}

/**
 * @brief Set the Light Status object by light intensity
 * 
 * @param gpio 
 * @param lightIntensity 
 */
void setLightStatusByLightIntensity(int gpio, int lightIntensity) {
  Serial.println("_visibilityDark " + _visibilityDark);
  Serial.println("_visiblityDim" + _visibilityDim);
  Serial.println("_visibilityBright " + _visibilityBright);

  if ((lightIntensity <= _visibilityDark) && (previousLightStatus_1 != 1)) {
    digitalWrite(gpio, HIGH);
    _currentLightStatus_1 = 1;
  }
  else if ((lightIntensity > _visibilityDark) && (lightIntensity <= _visibilityDim) && (previousLightStatus_1 != 1)) {
    digitalWrite(gpio, HIGH);
    _currentLightStatus_1 = 1;
  }
  else if ((lightIntensity > _visibilityDim) && (lightIntensity <= _visibilityBright) && (previousLightStatus_1 != 0)) {
    digitalWrite(gpio, LOW);
    _currentLightStatus_1 = 0;
  }
  else if ((lightIntensity > _visibilityBright) && (previousLightStatus_1 != 0)) {
    digitalWrite(gpio, LOW);
    _currentLightStatus_1 = 0;
  }
  else {
    _currentLightStatus_1 = getLightStatus(gpio);
  }
}

/**
 * @brief Set the Light Status object by motion
 * 
 * @param gpio 
 * @param motionIsDetected 
 */
void setLightStatusByMotion(int gpio, bool motionIsDetected) {
  if ((motionIsDetected == 1) && (lightIntensity_1 <= _visibilityDim) && (previousLightStatus_1 != 1)) {
    digitalWrite(gpio, HIGH);
    _currentLightStatus_1 = 1;
  }
  else if ((motionIsDetected == 0) && (previousLightStatus_1 != 0)) {
    digitalWrite(gpio, LOW);
    _currentLightStatus_1 = 0;
  }
}

/**
 * @brief Create JSON Activation Request object
 * 
 * @param mqttClientUId 
 * @param macId 
 * @return char* 
 */
char* getJSONActivationRequest( String macId, String mqttClientUId) {
  // Clear JsonDocument
  docMicroControllerUnit.clear();

  // Serialize a JsonDocument into a MQTT message
  docMicroControllerUnit["mac_id"] = macId;
  docMicroControllerUnit["mqtt_client_uid"] = mqttClientUId;

  // Produce a minified JSON document
  serializeJson(docMicroControllerUnit, jsonMicroControllerUnit);
  
  return jsonMicroControllerUnit;
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

    int iotDeviceUId = docMessage[0]["uid"];
    String category = docMessage[0]["category"];
    String gpio = docMessage[0]["gpio"];
    String identifier = docMessage[0]["identifier"];
    bool isActive = docMessage[0]["is_active"];
    String locationDescription = docMessage[0]["location_description"];
    String locationLabel = docMessage[0]["location_label"];
    int microControllerUnitUId = docMessage[0]["micro_controller_unit_uid"];
    
    String operatingMode = docMessage["operating_mode"];
    int status = docMessage["status"];
    int visibilityBright = docMessage["visibility_bright"];
    int visibilityDark = docMessage["visbility_dark"];
    int visibilityDim = docMessage["visibility_dim"];

    Serial.println("set iot device");
    Serial.println(iotDeviceUId);
    Serial.println(microControllerUnitUId);
    Serial.println(_microControllerUnitUId);
    if (microControllerUnitUId == _microControllerUnitUId) {
      setIoTDevice(iotDeviceUId, category, gpio, identifier, isActive, locationDescription, locationLabel);
      setVisibility(visibilityBright, visibilityDark, visibilityDim);

      if (operatingMode == "manual") {
        setLightStatusByApp(_gpio_light_1, status);
      }
      else if (operatingMode == "brightness") {
        setLightStatusByLightIntensity(_gpio_light_1, lightIntensity_1);
      }
      else if (operatingMode == "motion") {
        setLightStatusByMotion(_gpio_light_1, motionIsDetected_1);
      }

      iotDeviceIsVerified = true;
    }
  }
  else if (microControllerUnitIsActivated && iotDeviceIsVerified) {
    if (String(topic) == HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH) {
      // Clear JsonDocument
      docLightData.clear();

      // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
      DeserializationError deserializationError = deserializeJson(docLightData, (const byte*)payload);

      // If deserialization operation was not successful (Ok).
      if (deserializationError) {
        Serial.print(deserializationError.c_str());
      }

      // Extracting values from JsonDocument
      int iotDeviceUId = docLightData["uid"];
      int lightStatus = docLightData["status"];
      Serial.println(iotDeviceUId);
      Serial.println(lightStatus);
      Serial.println(_iotDeviceUId);
      Serial.println(_currentLightStatus_1);

      if ((lightStatus == 0) || (lightStatus == 1)) {
        if ((iotDeviceUId == _iotDeviceUId) && (lightStatus != _currentLightStatus_1) && (_currentOperatingMode_1 != "brightness")) {
          setLightStatusByApp(_gpio_light_1, lightStatus);

          // Clear JsonDocument
          docLightData.clear();

          // Serialize a JsonDocument into a MQTT message
          docLightData["uid"] = _iotDeviceUId;
          docLightData["status"] = lightStatus;
          docLightData["operating_mode"] = _currentOperatingMode_1;
          docLightData["visibility_bright"] = _visibilityBright;
          docLightData["visibility_dark"] = _visibilityDark;
          docLightData["visibility_dim"] = _visibilityDim;
          
          // Save a few CPU cycles by passing the size of the payload
          // Produce a minified JSON document
          size_t n = serializeJson(docLightData, jsonLightData);
          
          mqttClient.publish(HOME_BACKYARD_LIGHT_1_LIGHT_DATA, jsonLightData, n);

          previousLightStatus_1 = _currentLightStatus_1;
        }
      }
    }

    if (String(topic) == HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA) {
      docLightSensorData.clear();
      Serial.println("topic light sensor");
      // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
      DeserializationError deserializationError = deserializeJson(docLightSensorData, (const byte*)payload);

      // If deserialization operation was not successful (Ok).
      if (deserializationError) {
        Serial.print(deserializationError.c_str());
      }
      Serial.println("_operatingMode " + _currentOperatingMode_1);
      if (_currentOperatingMode_1 == "brightness") {
        // Extracting values from JsonDocument
        int iotDeviceUId = docLightSensorData["uid"];
        int lightIntensity = docLightSensorData["light_intensity"];
        Serial.println("iot device uid " + iotDeviceUId);
        Serial.println("lightIntensity");
        Serial.println(lightIntensity);
        Serial.println(lightIntensity_1);
        if ((iotDeviceUId == _iotDeviceUId) && (lightIntensity != lightIntensity)) {
          lightIntensity_1 = lightIntensity;
          Serial.println("SetLightStatusByLightIntensity");
          setLightStatusByLightIntensity(_gpio_light_1, lightIntensity_1);

          // Clear JsonDocument
          docLightData.clear();

          // Serialize a JsonDocument into a MQTT message
          docLightData["uid"] = _iotDeviceUId;
          docLightData["status"] = _currentLightStatus_1;
          docLightData["operating_mode"] = _currentOperatingMode_1;
          docLightData["visibility_bright"] = _visibilityBright;
          docLightData["visibility_dark"] = _visibilityDark;
          docLightData["visibility_dim"] = _visibilityDim;
          
          // Save a few CPU cycles by passing the size of the payload
          // Produce a minified JSON document
          size_t n = serializeJson(docLightData, jsonLightData);
          
          mqttClient.publish(HOME_BACKYARD_LIGHT_1_LIGHT_DATA, jsonLightData, n);
          
          previousLightStatus_1 = _currentLightStatus_1;
        }
      }
    }

    if (String(topic) == HOME_BACKYARD_MOTION_SENSOR_1_MOTION_SENSOR_DATA) {
      if (_currentOperatingMode_1 == "motion") {
        docMotionSensorData.clear();

        // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
        DeserializationError deserializationError = deserializeJson(docMotionSensorData, (const byte*)payload);

        // If deserialization operation was not successful (Ok).
        if (deserializationError) {
          Serial.print(deserializationError.c_str());
        }

        // Extracting values from JsonDocument
        int iotDeviceUId = docMotionSensorData["uid"];
        bool motionIsDetected = docMotionSensorData["motion_is_detected"];
        
        if ((iotDeviceUId == _iotDeviceUId) && (motionIsDetected != motionIsDetected_1)) {
          setLightStatusByMotion(_gpio_light_1, motionIsDetected);

          // Clear JsonDocument
          docLightData.clear();

          // Serialize a JsonDocument into a MQTT message
          docLightData["uid"] = _iotDeviceUId;
          docLightData["status"] = _currentLightStatus_1;
          docLightData["operating_mode"] = _currentOperatingMode_1;
          docLightData["visibility_bright"] = _visibilityBright;
          docLightData["visibility_dark"] = _visibilityDark;
          docLightData["visibility_dim"] = _visibilityDim;
          
          // Save a few CPU cycles by passing the size of the payload
          // Produce a minified JSON document
          size_t n = serializeJson(docLightData, jsonLightData);
          
          mqttClient.publish(HOME_BACKYARD_LIGHT_1_LIGHT_DATA, jsonLightData, n);

          previousLightStatus_1 = _currentLightStatus_1;
        }
      }
    }

    if (String(topic) == HOME_BACKYARD_LIGHT_1_COMMAND_OPERATING_MODE) {
      docLightData.clear();

      // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
      DeserializationError deserializationError = deserializeJson(docLightData, (const byte*)payload);

      // If deserialization operation was not successful (Ok).
      if (deserializationError) {
        Serial.print(deserializationError.c_str());
      }

      // Extracting values from JsonDocument
      int iotDeviceUId = docLightData["uid"];
      String operatingMode = docLightData["operating_mode"];
      Serial.println("iotDeviceUId " + String(iotDeviceUId));
      Serial.println("operating mode " + operatingMode);

      Serial.println("Set operating mode");
      setOperatingMode(operatingMode);
      Serial.println("_operatingMode " + String(_currentOperatingMode_1));
      Serial.println("previousOperatingMode " + String(previousOperatingMode_1));
      Serial.println("_currentLightStatus_1");
      Serial.println(_currentLightStatus_1);
      Serial.println("previouslightstatus");
      Serial.println(previousLightStatus_1);
      if ((operatingMode != previousOperatingMode_1) && (operatingMode== "manual")) {
        setLightStatusByApp(_gpio_light_1, 0);
      }
      else if ((operatingMode != previousOperatingMode_1) && (operatingMode == "brightness")) {
        Serial.println("lightIntensity");
        Serial.println(lightIntensity_1);
        setLightStatusByLightIntensity(_gpio_light_1, lightIntensity_1);
      }
      else if ((operatingMode != previousOperatingMode_1) && (operatingMode == "motion")) {
        Serial.println("motion is detected");
        Serial.println(String(motionIsDetected_1));
        setLightStatusByMotion(_gpio_light_1, motionIsDetected_1);
      }
      previousOperatingMode_1 = _currentOperatingMode_1;
      previousLightStatus_1 = _currentLightStatus_1;
    }

    if (String(topic) == HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY) {
      docLightData.clear();

      // Cast payload to pointer-to-const to disable ArduinoJson's zero-copy mode
      DeserializationError deserializationError = deserializeJson(docLightData, (const byte*)payload);

      // If deserialization operation was not successful (Ok).
      if (deserializationError) {
        Serial.print(deserializationError.c_str());
      }

      // Extracting values from JsonDocument
      int iotDeviceUId = docLightData["uid"];
      int visibilityBright = docLightData["visibility_bright"];
      int visibilityDark = docLightData["visibility_dark"];
      int visibilityDim = docLightData["visibility_dim"];

      if (((iotDeviceUId == _iotDeviceUId) && (visibilityBright != _visibilityBright)) || ((iotDeviceUId == _iotDeviceUId) && (visibilityDark != _visibilityDark)) || ((iotDeviceUId == _iotDeviceUId) && (visibilityDim != _visibilityDim))) {
        setVisibility(visibilityBright, visibilityDark, visibilityDim);
      }
      
      // Clear JsonDocument
      docLightData.clear();

      // Serialize a JsonDocument into a MQTT message
      docLightData["uid"] = _iotDeviceUId;
      docLightData["operating_mode"] = _currentOperatingMode_1;
      docLightData["status"] = _currentLightStatus_1;
      docLightData["visibility_bright"] = _visibilityBright;
      docLightData["visibility_dark"] = _visibilityDark;
      docLightData["visibility_dim"] = _visibilityDim;
      
      // Save a few CPU cycles by passing the size of the payload
      // Produce a minified JSON document
      size_t n = serializeJson(docLightData, jsonLightData);
      
      mqttClient.publish(HOME_BACKYARD_LIGHT_1_LIGHT_DATA, jsonLightData, n);
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
      mqttClient.subscribe(ACTIVATION_DISTRIBUTION);
      mqttClient.subscribe(ACTIVATION_RESPONSE);
      mqttClient.subscribe(HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH);
      mqttClient.subscribe(HOME_BACKYARD_LIGHT_1_COMMAND_OPERATING_MODE);
      mqttClient.subscribe(HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY);
      mqttClient.subscribe(HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA);
      mqttClient.subscribe(HOME_BACKYARD_MOTION_SENSOR_1_MOTION_SENSOR_DATA);

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
  
  pinMode(_gpio_light_1, OUTPUT);

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

  now = millis();
  if (((!microControllerUnitIsActivated && !iotDeviceIsVerified) || (microControllerUnitIsActivated && !iotDeviceIsVerified)) && ((now - lastVerifyIsActiveAttempt) > MQTT_VERIFY_IS_ACTIVE_EVENT_INTERVAL)) {
    lastVerifyIsActiveAttempt = now;
    mqttClient.publish(ACTIVATION_REQUEST, getJSONActivationRequest(macId, mqttClientUId));
  }
}
#pragma endregion
