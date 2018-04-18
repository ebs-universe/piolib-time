

import time
from datetime import datetime
from datetime import timedelta
from ebs.modbus.client import ModbusServerException
from ebs.modbus.device import ModbusDevice


def _parse_epoch(value):
    bytes = bytearray(value)
    rval = datetime(
        bytes[0] * 100 + bytes[1],          # year
        bytes[2],                           # month
        bytes[3],                           # day
        bytes[4],                           # hour
        bytes[5],                           # minutes
        bytes[6],                           # second
        (bytes[7] << 16 | bytes[8]) * 1000  # microseconds
    )
    return rval


class ModbusTimeMixin(object):
    _time_base_address = 2
    _time_descriptor_tags = {
        130: ("epoch", _parse_epoch),
    }
    _time_ticks_per_us = 1000

    def __init__(self, *args, **kwargs):
        self._descriptors = {}
        super(ModbusTimeMixin, self).__init__(*args, **kwargs)
        if not hasattr(self, '_registry'):
            self._registry = {}
        if 'descriptor_tags' not in self._registry.keys():
            self._registry['descriptor_tags'] = {}
        self._registry['descriptor_tags'].update(self._time_descriptor_tags)

    @property
    def current_time(self):
        return self.parse_timestamp(self.current_timestamp)

    def parse_timestamp(self, timestamp):
        delta = timedelta(seconds=timestamp)
        return self.epoch + delta

    @property
    def current_timestamp(self):
        words = self._get_current_time()
        return (words[2] << 16) + words[1] + words[0]/1000.0

    def _get_current_time(self):
        response = self.read_holding_registers(self._time_base_address, count=3)
        return response.registers

    @property
    def time_offset(self):
        return self.current_timestamp - time.time()

    def __getattr__(self, item):
        return super(ModbusTimeMixin, self).__getattr__(item)
