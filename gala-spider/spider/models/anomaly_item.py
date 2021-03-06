# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from spider.models.base_model_ import Model
from spider import util


class AnomalyItem(Model):
    """NOTE: This class is auto generated by the swagger code generator program.

    Do not edit the class manually.
    """

    def __init__(self, anomaly_attr: str=None, anomaly_type: str=None):  # noqa: E501
        """AnomalyItem - a model defined in Swagger

        :param anomaly_attr: The anomaly_attr of this AnomalyItem.  # noqa: E501
        :type anomaly_attr: str
        :param anomaly_type: The anomaly_type of this AnomalyItem.  # noqa: E501
        :type anomaly_type: str
        """
        self.swagger_types = {
            'anomaly_attr': str,
            'anomaly_type': str
        }

        self.attribute_map = {
            'anomaly_attr': 'anomaly_attr',
            'anomaly_type': 'anomaly_type'
        }

        self._anomaly_attr = anomaly_attr
        self._anomaly_type = anomaly_type

    @classmethod
    def from_dict(cls, dikt) -> 'AnomalyItem':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The AnomalyItem of this AnomalyItem.  # noqa: E501
        :rtype: AnomalyItem
        """
        return util.deserialize_model(dikt, cls)

    @property
    def anomaly_attr(self) -> str:
        """Gets the anomaly_attr of this AnomalyItem.


        :return: The anomaly_attr of this AnomalyItem.
        :rtype: str
        """
        return self._anomaly_attr

    @anomaly_attr.setter
    def anomaly_attr(self, anomaly_attr: str):
        """Sets the anomaly_attr of this AnomalyItem.


        :param anomaly_attr: The anomaly_attr of this AnomalyItem.
        :type anomaly_attr: str
        """

        self._anomaly_attr = anomaly_attr

    @property
    def anomaly_type(self) -> str:
        """Gets the anomaly_type of this AnomalyItem.


        :return: The anomaly_type of this AnomalyItem.
        :rtype: str
        """
        return self._anomaly_type

    @anomaly_type.setter
    def anomaly_type(self, anomaly_type: str):
        """Sets the anomaly_type of this AnomalyItem.


        :param anomaly_type: The anomaly_type of this AnomalyItem.
        :type anomaly_type: str
        """
        allowed_values = ["BELOW THRESHOLD", "ABOVE THRESHOLD"]  # noqa: E501
        if anomaly_type not in allowed_values:
            raise ValueError(
                "Invalid value for `anomaly_type` ({0}), must be one of {1}"
                .format(anomaly_type, allowed_values)
            )

        self._anomaly_type = anomaly_type
