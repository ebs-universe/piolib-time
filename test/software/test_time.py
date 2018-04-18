
from __future__ import print_function

import time
from ebs.modbus.device import ModbusDevice
from ebs.modbus.hotplug import ModbusHotplugMixin
from ebs.ucdm.descriptor import ModbusDescriptorMixin
from ebs.time.sync import ModbusTimeSyncMixin


class TestDevice(ModbusTimeSyncMixin,
                 ModbusDescriptorMixin,
                 ModbusHotplugMixin,
                 ModbusDevice):

    def __init__(self, *args, **kwargs):
        super(TestDevice, self).__init__(*args, **kwargs)

    def print_timeinfo(self):
        print('Epoch             : {0}'.format(self.epoch))
        print('Current Time      : {0}'.format(self.current_time))
        print('Current Timestamp : {0}'.format(self.current_timestamp))
        print('Current Host Time : {0}'.format(time.time()))
        print('Time Offset       : {0:.3f} ms'.format(self.time_offset * 1000))


if __name__ == '__main__':
    device = TestDevice(5, method='rtu', port='/dev/ttyACM1')
    device.connect()
    device.read_descriptors()

    print(device.lib_versions)
    print()

    device.print_timeinfo()

    print('\nTrying to synchronize time ... \n')
    device.sync_time(force=False)

    device.print_timeinfo()