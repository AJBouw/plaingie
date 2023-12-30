from application_business_rules.interactors.audit_and_logging.custom_logging_init import custom_logging
from psycopg2.extras import RealDictCursor
import os
import psycopg2

DB_HOST = os.getenv('POSTGRES_HOST')
DB_NAME = os.getenv('POSTGRES_DB')
DB_PASS = os.getenv('POSTGRES_PASSWORD')
DB_PORT = os.getenv('POSTGRES_PORT')
DB_USER = os.getenv('POSTGRES_USER')


def connect_db():
    try:
        custom_logging.debug('Initialize database connection')
        db_connection = psycopg2.connect(
            user=DB_USER,
            password=DB_PASS,
            host=DB_HOST,
            port=DB_PORT,
            database=DB_NAME)
        # Ensure data is added to the database after writing commands
        db_connection.autocommit = True
        return db_connection
    except (Exception, psycopg2.DatabaseError) as ex:
        custom_logging.error('Error occured while establishing database \
                             connection >>> %s', ex)


def init_db_cursor():
    try:
        db_connection = connect_db()

        db_cursor = db_connection.cursor(cursor_factory=RealDictCursor)

        return db_cursor

    except (Exception, psycopg2.DatabaseError) as ex:
        custom_logging.error('Error occured while establishing database \
                             connection >>> %s', ex)
