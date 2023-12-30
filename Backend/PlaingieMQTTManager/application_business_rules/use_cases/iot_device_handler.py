from application_business_rules.interactors.audit_and_logging.custom_logging_init import custom_logging
from interface_adapters.db_connector import init_db_cursor

db_cursor = init_db_cursor()


def read_iot_device_by_uid_where_is_active(iot_device_uid):
    try:
        sql = """  SELECT
                        *
                    FROM
                        iot_device
                    WHERE
                        iot_device.uid = %s
                        AND
                        iot_device.is_active = true
            """
        db_cursor.execute(sql, (iot_device_uid,))
        records = db_cursor.fetchall()
        custom_logging.debug('records result %s', records)

        if records is None:
            custom_logging.debug('records is NoneType %s', records)
            result = False
        else:
            result = True
        return result
    except Exception as ex:
        custom_logging.error('Error while retrieving IoT device | %s', ex)
