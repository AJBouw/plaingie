from interface_adapters.mqtt_init import mqtt_client
import application_business_rules.interactors.mqtt_callback_functions as callback

# Define call back functions
mqtt_client.on_connect = callback.on_connect
mqtt_client.on_disconnect = callback.on_disconnect
mqtt_client.on_message = callback.on_message
mqtt_client.on_publish = callback.on_publish
mqtt_client.on_subscribe = callback.on_subscribe
mqtt_client.on_unsubscribe = callback.on_unsubscribe
mqtt_client.on_log = callback.on_log

# Initialize MQTT connection
mqtt_client.loop_forever()