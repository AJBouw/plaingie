from application_business_rules.interactors.audit_and_logging.custom_logging_init import custom_logging
from interface_adapters.db_connector import connect_db
from interface_adapters.db_connector import init_db_cursor


db_cursor = init_db_cursor()
db_connection = connect_db()


def create_servo_data_record(iot_device_uid, current_position, start_position):
    try:
        sql = """
                INSERT INTO
                    servo_data (iot_device_uid, current_position, start_position)
                VALUES
                    (%s, %s, %s)
            """
        query_values = (iot_device_uid, current_position, start_position)
        db_cursor.execute(sql, query_values)
        db_connection.commit()
        custom_logging.debug("Successfully completed inserting light data")

    except Exception as ex:
        custom_logging.error('Error while attempting to insert an entry to the database table servo_data | %s', ex)
