import os
import configparser

CONFIG = "D:\code\A-Ops\gala-spider\config\gala-spider.conf"

# analysis configuration
cf = configparser.ConfigParser()
if os.path.exists(CONFIG):
    cf.read(CONFIG, encoding="utf-8")
else:
    cf.read("config/gala-spider.conf", encoding="utf-8")

db_agent = cf.get("global", "data_source")
ui_agent = cf.get("global", "ui_source")

kafka_topic = cf.get("kafka", "topic")
kafka_broker = cf.get("kafka", "broker")

neo4j_addr = cf.get("neo4j", "address")
neo4j_uname = cf.get("neo4j", "username")
neo4j_pwd = cf.get("neo4j", "password")

base_table = cf.get("table_info", "base_table_name")
other_table = cf.get("table_info", "other_table_name")
exclude_ip = cf.get("option", "exclude_addr")