#!/usr/bin/python3
# ******************************************************************************
# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
# licensed under the Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#     http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
# PURPOSE.
# See the Mulan PSL v2 for more details.
# ******************************************************************************/
"""
Description: acconut method's entrance for custom commands
Class:AccountCommand
"""

from aops_cli.base_cmd import BaseCommand
from aops_utils.conf.constant import USER_LOGIN, CHANGE_PASSWORD
from aops_utils.restful.helper import make_manager_url
from aops_utils.cli_utils import cli_request


class AccountCommand(BaseCommand):
    """
    Description: accounts' operations
    Attributes:
        sub_parse: Subcommand parameters
        params: Command line parameters
    """

    def __init__(self):
        """
        Description: Instance initialization
        """
        super().__init__()
        self.add_subcommand(sub_command='account',
                            help_desc="accounts' operations")

        self.sub_parse.add_argument(
            '--action',
            help='account actions: login, change password',
            nargs='?',
            type=str,
            required=True,
            choices=['login', 'change']
        )

        self.sub_parse.add_argument(
            '--username',
            help="the user's name",
            nargs='?',
            type=str,
        )

        self.sub_parse.add_argument(
            '--password',
            help="the user's password",
            nargs='?',
            type=str
        )

        self.sub_parse.add_argument(
            '--access_token',
            help='The access token for operations',
            nargs='?',
            type=str
        )

    def do_command(self, params):
        """
        Description: Executing command
        Args:
            params(argparse.Namespace): Command line parameters
        """

        action = params.action
        action_dict = {
            'login': self.manage_requests_login,
            'change': self.manage_requests_change
        }
        return action_dict.get(action)(params)

    @staticmethod
    def manage_requests_login(params):
        """
        Description: Executing login command
        Args:
            params{dict}: Command line parameters

        """
        manager_url, header = make_manager_url(USER_LOGIN)

        pyload = {
            "username": params.username,
            "password": params.password
        }

        return cli_request('POST', manager_url, pyload, header)

    @staticmethod
    def manage_requests_change(params):
        """
        Description: Executing change password command
        Args:
            params{dict}: Command line parameters
        Returns:
            dict: response of the backend
        """
        manager_url, header = make_manager_url(CHANGE_PASSWORD)

        pyload = {
            "password": params.password
        }

        return cli_request('POST', manager_url, pyload, header, params.access_token)
