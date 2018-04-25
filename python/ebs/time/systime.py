

import time
from datetime import datetime
from datetime import timedelta


def _parse_epoch(value):
    vbytes = bytearray(value)
    return datetime(
        vbytes[0] * 100 + vbytes[1],          # year
        vbytes[2],                            # month
        vbytes[3],                            # day
        vbytes[4],                            # hour
        vbytes[5],                            # minutes
        vbytes[6],                            # second
        (vbytes[7] << 16 | vbytes[8]) * 1000  # microseconds
    )


class ModbusTimeMixin(object):
    _time_base_address = 2
    _time_descriptor_tags = {
        130: ("epoch", _parse_epoch),
    }
    _time_ticks_per_us = 1000

    def __init__(self, *args, **kwargs):
        super(ModbusTimeMixin, self).__init__(*args, **kwargs)
        if not hasattr(self, '_registry'):
            self._registry = {}
        if 'descriptor_tags' not in self._registry.keys():
            self._registry['descriptor_tags'] = {}
        self._registry['descriptor_tags'].update(self._time_descriptor_tags)

    @property
    def current_time(self):
        return self.parse_timestamp(self.current_timestamp)

    @property
    def current_timestamp(self):
        return self.unpack_timestamp(self._get_current_time())

    @property
    def time_offset(self):
        return self.current_timestamp - time.time()

    def parse_timestamp(self, timestamp, epoch=None):
        if epoch is None:
            epoch = self.epoch
        delta = timedelta(seconds=timestamp)
        return epoch + delta

    @staticmethod
    def pack_timestamp(timestamp):
        seconds = int(timestamp)
        milliseconds = int((timestamp - seconds) * 1000)
        return [(milliseconds & 0x0000FFFF),
                (seconds & 0xFFFF0000) >> 16,
                (seconds & 0x0000FFFF)]

    @staticmethod
    def unpack_timestamp(words):
        return (words[2] << 16) + words[1] + words[0] / 1000.0

    def _get_current_time(self):
        resp = self.read_holding_registers(self._time_base_address, count=3)
        return resp.registers

    def __getattr__(self, item):
        return super(ModbusTimeMixin, self).__getattr__(item)
