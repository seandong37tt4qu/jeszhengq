# coding: utf-8

from __future__ import absolute_import
from datetime import date, datetime  # noqa: F401

from typing import List, Dict  # noqa: F401

from spider.models.base_model_ import Model
from spider.models.anomaly_item import AnomalyItem
from spider import util


class Anomaly(Model):
    """NOTE: This class is auto generated by the swagger code generator program.

    Do not edit the class manually.
    """

    def __init__(self, status: str=None, anomalyitem: List[AnomalyItem]=None):  # noqa: E501
        """Anomaly - a model defined in Swagger

        :param status: The status of this Anomaly.  # noqa: E501
        :type status: str
        :param anomalyitem: The anomalyitem of this Anomaly.  # noqa: E501
        :type anomalyitem: List[AnomalyItem]
        """
        self.swagger_types = {
            'status': str,
            'anomalyitem': List[AnomalyItem]
        }

        self.attribute_map = {
            'status': 'status',
            'anomalyitem': 'anomalyitem'
        }

        self._status = status
        self._anomalyitem = anomalyitem

    @classmethod
    def from_dict(cls, dikt) -> 'Anomaly':
        """Returns the dict as a model

        :param dikt: A dict.
        :type: dict
        :return: The Anomaly of this Anomaly.  # noqa: E501
        :rtype: Anomaly
        """
        return util.deserialize_model(dikt, cls)

    @property
    def status(self) -> str:
        """Gets the status of this Anomaly.


        :return: The status of this Anomaly.
        :rtype: str
        """
        return self._status

    @status.setter
    def status(self, status: str):
        """Sets the status of this Anomaly.


        :param status: The status of this Anomaly.
        :type status: str
        """
        allowed_values = ["ANOMALY_NO", "ANOMALY_YES", "ANOMALY_LACKING"]  # noqa: E501
        if status not in allowed_values:
            raise ValueError(
                "Invalid value for `status` ({0}), must be one of {1}"
                .format(status, allowed_values)
            )

        self._status = status

    @property
    def anomalyitem(self) -> List[AnomalyItem]:
        """Gets the anomalyitem of this Anomaly.


        :return: The anomalyitem of this Anomaly.
        :rtype: List[AnomalyItem]
        """
        return self._anomalyitem

    @anomalyitem.setter
    def anomalyitem(self, anomalyitem: List[AnomalyItem]):
        """Sets the anomalyitem of this Anomaly.


        :param anomalyitem: The anomalyitem of this Anomaly.
        :type anomalyitem: List[AnomalyItem]
        """

        self._anomalyitem = anomalyitem