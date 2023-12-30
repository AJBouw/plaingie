from application_business_rules.interactors.audit_and_logging.custom_logging_init import custom_logging
import os
import paho.mqtt.client as paho_mqtt
import ssl, uuid

# MQTT Broker info
MQTT_BROKER_TCP_PORT = os.getenv('MQTT_BROKER_TCP_PORT')
MQTT_BROKER_WS_PORT = os.getenv('MQTT_BROKER_WS_PORT')
# MQTT Broker address
MQTT_BROKER_URL = os.getenv('MQTT_BROKER_URL')
# MQTT Client unique ID
MQTT_CLIENT_UID = 'PythonServer-' + (hex(uuid.getnode()))
MQTT_KEEP_ALIVE_INTERVAL = os.getenv('MQTT_KEEP_ALIVE_INTERVAL')
# MQTT credentials
MQTT_PASSWORD = os.getenv('MQTT_PASSWORD')
MQTT_USERNAME = os.getenv('MQTT_USERNAME')

# Set connection for MQTT Client with unique ID
mqtt_client = paho_mqtt.Client(MQTT_CLIENT_UID)
# Configure client authentication
mqtt_client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

try:
    mqtt_client.connect(MQTT_BROKER_URL,
                        int(MQTT_BROKER_TCP_PORT),
                        int(MQTT_KEEP_ALIVE_INTERVAL))
except Exception as ex:
    custom_logging.error('Error occoured while establishing MQTT Broker \
                          connection | %s', ex)
