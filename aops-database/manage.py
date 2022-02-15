#!/usr/bin/python3
# ******************************************************************************
# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
# licensed under the Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#     http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN 'AS IS' BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
# PURPOSE.
# See the Mulan PSL v2 for more details.
# ******************************************************************************/
"""
Time:
Author:
Description: Manager that start aops-database
"""
import argparse
import sqlalchemy
from flask import Flask

from aops_database.function.helper import create_tables, ENGINE, SESSION
from aops_database.factory.table import User
from aops_database.factory.mapping import MAPPINGS
from aops_database.proxy.deploy import DeployDatabase
from aops_database.proxy.account import UserDatabase
from aops_database.conf.constant import DEFAULT_TASK_INFO
from aops_utils.log.log import LOGGER
from aops_utils.restful.status import SUCCEED


def init_user():
    """
    Initialize user, add a default user: admin
    """
    try:
        create_tables(ENGINE)
    except sqlalchemy.exc.SQLAlchemyError:
        raise sqlalchemy.exc.SQLAlchemyError("create tables fail")

    proxy = UserDatabase()
    if not proxy.connect(SESSION):
        raise ValueError("connect to mysql fail")

    data = {
        "username": "admin",
        "password": "changeme"
    }
    res = proxy.select([User.username], {"username": data['username']})
    # user has been added to database, return
    if res[1]:
        return

    res = proxy.add_user(data)
    if res != SUCCEED:
        raise ValueError("add admin user fail")

    LOGGER.info("initialize default admin user succeed")


def init_es():
    """
    Initialize elasticsearch index and add default task
    """
    proxy = DeployDatabase()
    if not proxy.connect():
        raise ValueError("connect to elasticsearch fail")

    for index_name, body in MAPPINGS.items():
        # es.delete_index(index_name)
        res = proxy.create_index(index_name, body)
        if not res:
            raise ValueError("create elasticsearch index %s fail", index_name)

    LOGGER.info("create elasticsearch index succeed")
    data = {
        "username": "",
        "task_list": [""]
    }
    for default_task in DEFAULT_TASK_INFO:
        data["username"] = default_task["username"]
        data["task_list"][0] = default_task["task_id"]
        task_name = default_task["task_name"]
        res = proxy.get_task(data)
        if res[1]["task_infos"]:
            LOGGER.info("default task %s has existed, ignore", task_name)
            continue
        res = proxy.add_task(default_task)


def init_database():
    """
    Initialize database
    """
    init_user()
    init_es()


def run_app(name):
    """
    Run database server
    """
    if name == 'database':
        init_database()

    module_name = 'aops_' + name
    app = Flask(module_name)

    module = __import__(module_name, fromlist=[module_name])
    for blue, api in module.BLUE_POINT:
        api.init_app(app)
        app.register_blueprint(blue)

    try:
        config = getattr(module.conf.configuration, name)
    except AttributeError:
        raise AttributeError("There is no config named %s" % name)

    ip = config.get('IP')
    port = config.get('PORT')
    app.run(host=ip, port=port)


if __name__ == "__main__":
    argparser = argparse.ArgumentParser()
    argparser.add_argument('--name', required=True)
    args = argparser.parse_args()
    run_app(args.name)
