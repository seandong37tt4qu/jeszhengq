# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from swagger_server.models.base_model_ import Model
from swagger_server import util


class BaseResponse(Model):
    """NOTE: This class is auto generated by the swagger code generator program.

    Do not edit the class manually.
    """

    def __init__(self, code: int=None, msg: str=None):  # noqa: E501
        """BaseResponse - a model defined in Swagger

        :param code: The code of this BaseResponse.  # noqa: E501
        :type code: int
        :param msg: The msg of this BaseResponse.  # noqa: E501
        :type msg: str
        """
        self.swagger_types = {
            'code': int,
            'msg': str
        }

        self.attribute_map = {
            'code': 'code',
            'msg': 'msg'
        }

        self._code = code
        self._msg = msg

    @classmethod
    def from_dict(cls, dikt) -> 'BaseResponse':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The BaseResponse of this BaseResponse.  # noqa: E501
        :rtype: BaseResponse
        """
        return util.deserialize_model(dikt, cls)

    @property
    def code(self) -> int:
        """Gets the code of this BaseResponse.


        :return: The code of this BaseResponse.
        :rtype: int
        """
        return self._code

    @code.setter
    def code(self, code: int):
        """Sets the code of this BaseResponse.


        :param code: The code of this BaseResponse.
        :type code: int
        """

        self._code = code

    @property
    def msg(self) -> str:
        """Gets the msg of this BaseResponse.


        :return: The msg of this BaseResponse.
        :rtype: str
        """
        return self._msg

    @msg.setter
    def msg(self, msg: str):
        """Sets the msg of this BaseResponse.


        :param msg: The msg of this BaseResponse.
        :type msg: str
        """

        self._msg = msg
