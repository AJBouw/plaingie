from application_business_rules.interactors.audit_and_logging.custom_logging_init import custom_logging
import interface_adapters.mqtt_manager as mqtt_manager
import os
import time
"""_summary_
Mosquitto callback functions are executed when an event occurs.
"""
MQTT_RECONNECT_ATTEMPTS_MAXIMUM = os.getenv('MQTT_RECONNECT_ATTEMPTS_MAXIMUM')
MQTT_RECONNECT_INTERVAL = os.getenv('MQTT_KEEP_ALIVE_INTERVAL')


def on_connect(mqtt_client, user_data, flags, rc):
    """_summary_
    The function is called when the MQTT Client receives a CONNACK response
    from the MQTT Broker. That is, when a connection has been successfully
    established between the MQTT Client and the Broker.
    The MQTT Manager is a MQTT Client and subscribes to all topics in order to
    handle the MQTT data communication and the data storage.
    Subscribing on topics on connect will renew the subscriptions when
    reconnecting.
    Args:
        mqtt_client (_type_): _description_
        user_data (_type_): _description_
        flags (_type_): _description_
        rc (int): error code:
            0 : Connection succesful
            1 : Connection refused - incorrect protocol version
            2 : Connection refused - invalid client identifier
            3 : Connection refused - server unavailable
            4 : Connection refused - bad username and / or password
            5 : Connection refused - not authorized
            6-255 : Currently unused
    """

    custom_logging.debug('On connect results in connection: %s', rc)


def on_disconnect(mqtt_client, userdata, rc):
    """_summary_
    When the MQTT Client disconnects from the MQTT Broker, auto reconnect is
    executed. Automatic reconnection ensures to resume publishing or
    subscribing to topics when the network connection is interrupted, or the
    MQTT Broker is temporarily unavailable.
    Args:
        mqtt_client (_type_): _description_
        userdata (_type_): _description_
        rc (_type_): _description_
    """
    custom_logging.warning(
        "Disconnected | %s", str(rc)
    )

    reconnect_count = 0

    while reconnect_count < MQTT_RECONNECT_ATTEMPTS_MAXIMUM:
        custom_logging.DEBUG('Reconnecting in %s seconds...',
                             MQTT_RECONNECT_INTERVAL)
        time.sleep(MQTT_RECONNECT_INTERVAL)
        try:
            mqtt_client.reconnect()
            custom_logging.debug('Successfully reconnected MQTT Client')
            return
        except Exception as ex:
            custom_logging.error(
                'Error occured while reconnection the MQTT Client | %s',
                ex)

        reconnect_count += 1


def on_message(mqtt_client, user_data, message):
    """_summary_
    Process the published messages to a subscribed topic.
    Args:
        mqtt_client (_type_): _description_
        user_data (_type_): _description_
        message (_type_): _description_
    """
    custom_logging.debug(message.topic 
                    + ' ' + str(message.qos)
                    + ' ' + str(message.payload))
    custom_logging.debug(
        'user data %s',
        user_data)

    if message.retain == 1:
        custom_logging.warning('This is a retained message.')
    else:
        mqtt_manager.data_handler(mqtt_client, message.topic, message.payload)


def on_publish(mqtt_client, user_data, message_id):
    """_summary_
    The function is called when the published message's transmission has
    been completed.
    Args:
        mqtt_client (_type_): _description_
        user_data (_type_): _description_
        message_id (int): Unique message identifier assigned by the MQTT
        Client.
        When using QoS 1 or 2 then the MQTT Client loop will use the message id
        in order to identify messages that have not been sent.
    """
    custom_logging.error('mqtt_client + %s', mqtt_client)
    custom_logging.error('obj user_data: ' + str(user_data))
    custom_logging.error('mid: ' + str(message_id))


def on_subscribe(mqtt_client, user_data, message_id, qos):
    """_summary_
    The function is called when an MQTT Client subscribes to a topic.
    Args:
        mqtt_client (_type_): _description_
        user_data (_type_): _description_
        message_id (_type_): _description_
        qos (_type_): _description_
    """
    custom_logging.warning('mqtt_client %s', mqtt_client)
    custom_logging.warning('subscribe ' + str(message_id) + ' ' + str(qos))


def on_unsubscribe(mqtt_client, user_data, message_id):
    """_summary_
    When a MQTT Client unsubscribes to a topic.
    It is acknowledged by a UnSubAck response, which triggers the
    on_unsubscribe callback function.
    Args:
        mqtt_client (_type_): _description_
        user_data (_type_): _description_
        message_id (_type_): _description_
    """
    custom_logging.warning("Unsubscribed topic : ", str(message_id))


def on_log(mqtt_client, obj, level):

    custom_logging.error('on_log mqtt_client %s, obj %s, level %s, string %s', mqtt_client, obj, level)
