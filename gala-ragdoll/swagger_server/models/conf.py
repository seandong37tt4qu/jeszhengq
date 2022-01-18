# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from swagger_server.models.base_model_ import Model
from swagger_server import util


class Conf(Model):
    """NOTE: This class is auto generated by the swagger code generator program.

    Do not edit the class manually.
    """

    def __init__(self, file_path: str=None, contents: str=None, host_id: str=None):  # noqa: E501
        """Conf - a model defined in Swagger

        :param file_path: The file_path of this Conf.  # noqa: E501
        :type file_path: str
        :param contents: The contents of this Conf.  # noqa: E501
        :type contents: str
        :param host_id: The host_id of this Conf.  # noqa: E501
        :type host_id: str
        """
        self.swagger_types = {
            'file_path': str,
            'contents': str,
            'host_id': str
        }

        self.attribute_map = {
            'file_path': 'filePath',
            'contents': 'contents',
            'host_id': 'hostId'
        }

        self._file_path = file_path
        self._contents = contents
        self._host_id = host_id

    @classmethod
    def from_dict(cls, dikt) -> 'Conf':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The Conf of this Conf.  # noqa: E501
        :rtype: Conf
        """
        return util.deserialize_model(dikt, cls)

    @property
    def file_path(self) -> str:
        """Gets the file_path of this Conf.

        the path of a configuration file  # noqa: E501

        :return: The file_path of this Conf.
        :rtype: str
        """
        return self._file_path

    @file_path.setter
    def file_path(self, file_path: str):
        """Sets the file_path of this Conf.

        the path of a configuration file  # noqa: E501

        :param file_path: The file_path of this Conf.
        :type file_path: str
        """

        self._file_path = file_path

    @property
    def contents(self) -> str:
        """Gets the contents of this Conf.

        the contents of the configuration file  # noqa: E501

        :return: The contents of this Conf.
        :rtype: str
        """
        return self._contents

    @contents.setter
    def contents(self, contents: str):
        """Sets the contents of this Conf.

        the contents of the configuration file  # noqa: E501

        :param contents: The contents of this Conf.
        :type contents: str
        """

        self._contents = contents

    @property
    def host_id(self) -> str:
        """Gets the host_id of this Conf.

        Take a host as a baseline to get the actual configuration as the expected configuration.  # noqa: E501

        :return: The host_id of this Conf.
        :rtype: str
        """
        return self._host_id

    @host_id.setter
    def host_id(self, host_id: str):
        """Sets the host_id of this Conf.

        Take a host as a baseline to get the actual configuration as the expected configuration.  # noqa: E501

        :param host_id: The host_id of this Conf.
        :type host_id: str
        """

        self._host_id = host_id
