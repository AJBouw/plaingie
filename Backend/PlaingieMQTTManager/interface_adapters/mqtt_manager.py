from application_business_rules.interactors.audit_and_logging.custom_logging_init import custom_logging
import application_business_rules.use_cases.iot_device_handler as iot_device_handler
import application_business_rules.use_cases.light_data_handler as light_data_handler
import application_business_rules.use_cases.light_sensor_data_handler as light_sensor_data_handler
import application_business_rules.use_cases.motion_sensor_data_handler as motion_sensor_data_handler
import json
import os

HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH = os.getenv('HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH')
HOME_BACKYARD_LIGHT_1_COMMAND_OPERATING_MODE = os.getenv('HOME_BACKYARD_LIGHT_1_COMMAND_OPERATING_MODE')
HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY = os.getenv('HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY')
HOME_BACKYARD_LIGHT_1_LIGHT_DATA = os.getenv('HOME_BACKYARD_LIGHT_1_LIGHT_DATA')
HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA = os.getenv('HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA')
HOME_BACKYARD_MOTION_SENSOR_1_MOTION_SENSOR_DATA = os.getenv('HOME_BACKYARD_MOTION_SENSOR_1_MOTION_SENSOR_DATA')
HOME_BACKYARD_SERVO_1_COMMAND_CURRENT_POSITION = os.getenv('HOME_BACKYARD_SERVO_1_COMMAND_CURRENT_POSITION')
HOME_BACKYARD_SERVO_1_COMMAND_START_POSITION = os.getenv('HOME_BACKYARD_SERVO_1_COMMAND_START_POSITION')


def data_handler(mqtt_client, topic, payload):
    """_summary_

    Args:
        mqtt_client (_type_): _description_
        topic (_type_): _description_
        payload (_type_): _description_
    """
    if topic == HOME_BACKYARD_LIGHT_1_LIGHT_DATA:
        handle_light_data(payload)
    elif topic == HOME_BACKYARD_LIGHT_SENSOR_1_LIGHT_SENSOR_DATA:
        handle_light_sensor_data(payload)
    elif topic == HOME_BACKYARD_MOTION_SENSOR_1_MOTION_SENSOR_DATA:
        handle_motion_sensor_data(payload)
    elif topic == HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH:
        pass
    elif topic == HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH:
        pass
    elif topic == HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY:
        pass
    else:
        custom_logging.warning('Unknown topic published to \
                               the MQTT Broker: %s', topic)


def handle_light_data(payload):
    try:
        deserialize_iot_device_uid(payload)
        # Validate iot_device_uid is_active
        iot_device_is_active = iot_device_handler.read_iot_device_by_uid_where_is_active(iot_device_uid)
        if iot_device_is_active:
            deserialize_light_data(payload)
            light_data_handler.create_light_data_record(iot_device_uid, operating_mode, status, visibility_bright, visibility_dark, visibility_dim)
    except Exception as ex:
        custom_logging.warning('Error occured while attempting to store light data | %s', ex) 


def deserialize_light_data(payload):
    global operating_mode
    global status
    global visibility_bright
    global visibility_dark
    global visibility_dim

    json_dictionary = json.loads(str(payload.decode('utf-8')))
    operating_mode = json_dictionary['operating_mode']
    status = json_dictionary['status']
    visibility_bright = json_dictionary['visibility_bright']
    visibility_dark = json_dictionary['visibility_dark']
    visibility_dim = json_dictionary['visibility_dim']


def handle_light_sensor_data(payload):
    try:
        deserialize_iot_device_uid(payload)
        # Validate iot_device_uid is_active
        iot_device_is_active = iot_device_handler.read_iot_device_by_uid_where_is_active(iot_device_uid)
        if iot_device_is_active:
            deserialize_light_sensor_data(payload)
            light_sensor_data_handler.create_light_sensor_data_record(iot_device_uid, light_intensity)
    except Exception as ex:
        custom_logging.warning('Error occured while attempting to store light sensor data | %s', ex) 


def deserialize_light_sensor_data(payload):
    global light_intensity
    json_dictionary = json.loads(str(payload.decode('utf-8')))
    light_intensity = json_dictionary['light_intensity']


def handle_motion_sensor_data(payload):
    try:
        deserialize_iot_device_uid(payload)
        # Validate iot_device_uid is_active
        iot_device_is_active = iot_device_handler.read_iot_device_by_uid_where_is_active(iot_device_uid)
        if iot_device_is_active:
            deserialize_motion_sensor_data(payload)
            motion_sensor_data_handler.create_motion_sensor_data_record(iot_device_uid, motion_is_detected)
    except Exception as ex:
        custom_logging.warning('Error occured while attempting to store motion sensor data | %s', ex) 


def deserialize_motion_sensor_data(payload):
    global motion_is_detected
    json_dictionary = json.loads(str(payload.decode('utf-8')))
    motion_is_detected = json_dictionary['motion_is_detected']


def deserialize_iot_device_uid(payload):
    global iot_device_uid
    json_dictionary = json.loads(str(payload.decode('utf-8')))
    iot_device_uid = json_dictionary['iot_device_uid']