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
Description: diag method's entrance for custom commands
Class:DiagCommand
"""
import sys
import time

from adoctor_cli.base_cmd import BaseCommand
from aops_utils.cli_utils import add_start_and_end, add_access_token, cli_request, request_without_print
from aops_utils.cli_utils import print_row_from_result
from aops_utils.conf.constant import DIAG_EXECUTE_DIAG, DIAG_GET_PROGRESS, DIAG_GET_REPORT_LIST
from aops_utils.conf.constant import DIAG_GET_TASK
from aops_utils.restful.helper import make_diag_url
from aops_utils.restful.status import SUCCEED
from aops_utils.time_utils import time_check_generate
from aops_utils.validate import name_check, str_split

SECONDS = 5  # polling interval


class DiagCommand(BaseCommand):
    """
    Description: start the diag part
    """

    def __init__(self):
        """
        Description: Instance initialization
        """
        super().__init__()
        self.add_subcommand(sub_command='diag',
                            help_desc="diagnosis operations")
        self.sub_parse.add_argument(
            '--host_list',
            nargs='?',
            type=str,
            help='host ips',
            required=True)

        self.sub_parse.add_argument(
            '--tree_list',
            nargs='?',
            type=str,
            required=True,
            help='trees list')

        self.sub_parse.add_argument(
            '--interval',
            nargs='?',
            type=int,
            default=10,
            help='diagnosis interval')

        add_start_and_end(self.sub_parse)
        add_access_token(self.sub_parse)

    def do_command(self, params):
        """
        Description: Executing command
        Args:
            params: Command line parameters
        """

        starttime = params.start
        endtime = params.end
        time_list = time_check_generate(starttime, endtime)

        self.manage_requests_diag(params, time_list)

    @staticmethod
    def manage_requests_diag(params, time_list):
        """
        Description: Executing diag command
        Args:
            params: Command line parameters
            time_list: time_list with start time and end time.
        Returns:
            dict: response body
        """

        hosts = str_split(params.host_list)
        trees = str_split(params.tree_list)
        name_check(hosts)
        name_check(trees)
        diag_url, header = make_diag_url(DIAG_EXECUTE_DIAG)
        times = 4

        pyload = {
            "host_list": hosts,
            "time_range": time_list,
            "tree_list": trees,
            "interval": params.interval
        }
        result = request_without_print('POST', diag_url, pyload, header, params.access_token)

        if result.get('code') != SUCCEED:
            print("diag execute error: please try again")
            sys.exit(0)
        total = result['expected_report_num']

        print("Diagnosis task start......")

        task_id = result['task_id']
        print("The task id is: ", task_id)
        pyload = {
            "task_list": [task_id]
        }
        diag_url, header = make_diag_url(DIAG_GET_TASK)

        while times:
            time.sleep(1)
            result = request_without_print('POST', diag_url, pyload, header, params.access_token)
            if result.get('code') != SUCCEED:
                print("Task info query failed, please check your diag_scheduler or database.")
                sys.exit(0)
            if len(result.get('task_infos')) != 0:

                print("The diagnosis execution will run with:\n"
                      "hosts: {}\n"
                      "trees: {}\n"
                      "If there exists differences from your inputs,"
                      "please check whether hosts and trees are valid."
                      .format(result.get('task_infos')[0].get('host_list'),
                              result.get('task_infos')[0].get('tree_list')))

                DiagCommand.query_diag_process(pyload, params, total)
                print("Diagnosis task complete.")
                diag_url, header = make_diag_url(DIAG_GET_REPORT_LIST)
                pyload = {
                    "task_id": task_id
                }
                result = request_without_print('POST', diag_url, pyload, header, params.access_token)
                result_info = result.pop('result', [])
                print(result)
                return print_row_from_result(result_info)
            times -= 1
        print("There is no task can be found in diagnosis scheduler, please try again.")

    @staticmethod
    def query_diag_process(pyload, params, total):
        finished = 0
        wait_time = 0
        diag_url, header = make_diag_url(DIAG_GET_PROGRESS)

        while finished != total:
            try:
                time.sleep(SECONDS)
                result = request_without_print('POST', diag_url, pyload, header, params.access_token)

                if result.get('code') != SUCCEED:
                    print('Execution failed, please check your connection or params.')
                    sys.exit(0)
                process = result.get('result')[0].get("progress")

                if finished != process:
                    finished = process
                    wait_time = 0
                else:
                    wait_time = wait_time + 1

                if wait_time == 10:
                    print("Diagnosis Execution timeout, please check connection or fault trees.")
                    sys.exit(0)

                print('Finished: {0}, Total: {1}'.format(finished, total))

            except ConnectionError:
                print("Connection failed, please try again.")
                sys.exit(0)
