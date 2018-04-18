
import time
from ebs.modbus.client import ModbusServerException
from ebs.modbus.device import ModbusDevice
from .systime import ModbusTimeMixin


class ModbusTimeSyncMixin(ModbusTimeMixin):
    _time_sync_request_bit = 1 << 1

    def __init__(self, *args, **kwargs):
        self._descriptors = {}
        super(ModbusTimeSyncMixin, self).__init__(*args, **kwargs)

    @staticmethod
    def get_host_time():
        ct = time.time()
        cseconds = int(ct)
        cmilliseconds = int((ct - cseconds) * 1000)
        return cseconds, cmilliseconds

    def _build_host_timestamp(self):
        seconds, milliseconds = self.get_host_time()
        ts = [(seconds & 0xFFFF0000) >> 16,
              (seconds & 0x0000FFFF),
              (milliseconds & 0x0000FFFF)]
        return ts

    def sync_time(self, force=False):
        if not force and \
                not self.exception_status & self._time_sync_request_bit:
            return

        _write_address = self._time_base_address + 3
        _trigger_address = self._time_base_address + 5

        ts = self._build_host_timestamp()
        self.write_registers(address=_write_address, values=ts)

        self.readwrite_registers(
            address=self._time_base_address, read_count=3,
            write_address=_trigger_address, write_registers=[0x0000]
        )

        ts = self._build_host_timestamp()
        self.write_registers(address=_write_address, values=ts)

    def __getattr__(self, item):
        return super(ModbusTimeSyncMixin, self).__getattr__(item)
