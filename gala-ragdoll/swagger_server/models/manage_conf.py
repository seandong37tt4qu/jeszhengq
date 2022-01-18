# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from swagger_server.models.base_model_ import Model
from swagger_server import util


class ManageConf(Model):
    """NOTE: This class is auto generated by the swagger code generator program.

    Do not edit the class manually.
    """

    def __init__(self, file_path: str=None, contents: str=None, domain_path: str=None):  # noqa: E501
        """ManageConf - a model defined in Swagger

        :param file_path: The file_path of this ManageConf.  # noqa: E501
        :type file_path: str
        :param contents: The contents of this ManageConf.  # noqa: E501
        :type contents: str
        :param domain_path: The domain_path of this ManageConf.  # noqa: E501
        :type domain_path: str
        """
        self.swagger_types = {
            'file_path': str,
            'contents': str,
            'domain_path': str
        }

        self.attribute_map = {
            'file_path': 'filePath',
            'contents': 'contents',
            'domain_path': 'domainPath'
        }

        self._file_path = file_path
        self._contents = contents
        self._domain_path = domain_path

    @classmethod
    def from_dict(cls, dikt) -> 'ManageConf':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The ManageConf of this ManageConf.  # noqa: E501
        :rtype: ManageConf
        """
        return util.deserialize_model(dikt, cls)

    @property
    def file_path(self) -> str:
        """Gets the file_path of this ManageConf.

        the path of a configuration file  # noqa: E501

        :return: The file_path of this ManageConf.
        :rtype: str
        """
        return self._file_path

    @file_path.setter
    def file_path(self, file_path: str):
        """Sets the file_path of this ManageConf.

        the path of a configuration file  # noqa: E501

        :param file_path: The file_path of this ManageConf.
        :type file_path: str
        """

        self._file_path = file_path

    @property
    def contents(self) -> str:
        """Gets the contents of this ManageConf.

        the contents of the configuration file  # noqa: E501

        :return: The contents of this ManageConf.
        :rtype: str
        """
        return self._contents

    @contents.setter
    def contents(self, contents: str):
        """Sets the contents of this ManageConf.

        the contents of the configuration file  # noqa: E501

        :param contents: The contents of this ManageConf.
        :type contents: str
        """

        self._contents = contents

    @property
    def domain_path(self) -> str:
        """Gets the domain_path of this ManageConf.

        the path of a configuration file in confTarcking system  # noqa: E501

        :return: The domain_path of this ManageConf.
        :rtype: str
        """
        return self._domain_path

    @domain_path.setter
    def domain_path(self, domain_path: str):
        """Sets the domain_path of this ManageConf.

        the path of a configuration file in confTarcking system  # noqa: E501

        :param domain_path: The domain_path of this ManageConf.
        :type domain_path: str
        """

        self._domain_path = domain_path
