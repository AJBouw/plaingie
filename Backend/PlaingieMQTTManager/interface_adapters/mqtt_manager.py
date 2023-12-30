from application_business_rules.interactors.audit_and_logging.custom_logging_init import custom_logging
import application_business_rules.use_cases.iot_device_handler as iot_device_handler
import application_business_rules.use_cases.micro_controller_unit_handler as mcu_handler
import application_business_rules.use_cases.light_data_handler as light_data_handler
import application_business_rules.use_cases.light_sensor_data_handler as light_sensor_data_handler
import application_business_rules.use_cases.motion_sensor_data_handler as motion_sensor_data_handler
import application_business_rules.use_cases.servo_data_handler as servo_data_handler
import json
import os

HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH = os.getenv('HOME_BACKYARD_LIGHT_1_COMMAND_LIGHT_SWITCH')
HOME_BACKYARD_LIGHT_1_COMMAND_OPERATING_MODE = os.getenv('HOME_BACKYARD_LIGHT_1_COMMAND_OPERATING_MODE')
HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY = os.getenv('HOME_BACKYARD_LIGHT_1_COMMAND_VISIBILITY')
HOME_BACKYARD_LIGHT_1_LIGHT_DATA = os.getenv('HOME_BACKYARD_LIGHT_1_LIGHT_DATA')


def data_handler(mqtt_client, topic, payload):
    """_summary_

    Args:
        mqtt_client (_type_): _description_
        topic (_type_): _description_
        payload (_type_): _description_
    """
    custom_logging.warning('Unknown topic published to the MQTT Broker: %s', topic)
