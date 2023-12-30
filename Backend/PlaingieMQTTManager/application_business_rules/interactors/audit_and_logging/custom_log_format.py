from logging.handlers import TimedRotatingFileHandler
from pythonjsonlogger import jsonlogger
import logging
import os
import sys

"""_summary_
Custom logging contains a custom logging format, including the ability
to handle custom storage.
"""

LOG_FILE = 'plaingie_mqtt_manager.log'
LOGGING_FORMAT_CONSOLE = logging.Formatter(os.getenv('LOGGING_FORMAT_CONSOLE'), datefmt=os.getenv('LOGGING_DATETIME_FORMAT'))
LOGGING_FORMAT_JSON = jsonlogger.JsonFormatter(os.getenv('LOGGING_FORMAT_JSON'), datefmt=os.getenv('LOGGING_DATETIME_FORMAT'))


class LogFormatting:
    def get_console_handler():
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setFormatter(LOGGING_FORMAT_JSON)
        return console_handler

    def get_file_handler():
        file_handler = logging.FileHandler(LOG_FILE)
        file_handler.setFormatter(LOGGING_FORMAT_JSON)
        return file_handler

    def get_custom_logging(logger_name):
        logger = logging.getLogger(logger_name)
        logger.setLevel(logging.DEBUG)
        logger.addHandler(LogFormatting.get_console_handler())
        logger.addHandler(LogFormatting.get_file_handler())
        logger.propagate = False
        return logger
