#!/usr/bin/env python
# encoding: utf-8

# Copyright (C) 2016 Chintalagiri Shashank
#
# This file is part of libtime.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""
Docstring for sync_modbus.py
"""

import time
from pymodbus.client.sync import ModbusSerialClient as ModbusClient
from pymodbus import other_message
from pymodbus import register_write_message
from pymodbus import register_read_message


SLAVE_NREGS = 200


def get_client():
    port = '/dev/ttyACM1'
    mclient = ModbusClient(method='rtu', port=port,
                           timeout=0.1, baudrate=256000)
    mclient.connect()
    return mclient


def get_time():
    ct = time.time()
    cseconds = int(ct)
    cmilliseconds = int((ct - cseconds) * 1000)
    return cseconds, cmilliseconds


def test_bit(int_type, offset):
    mask = 1 << offset
    return int_type & mask


def get_excst(client, saddress):
    request = other_message.ReadExceptionStatusRequest(unit=saddress)
    response = client.execute(request)
    return response.status


if __name__ == '__main__':
    mb_client = get_client()
    address = 5
    time_address_base = 1
    excstatus = get_excst(mb_client, address)
    if test_bit(excstatus, 1):
        seconds, milliseconds = get_time()
        frac = milliseconds
        ts = [(seconds & 0xFFFF0000) >> 16,
              (seconds & 0x0000FFFF),
              (frac & 0xFFFF0000) >> 16,
              (frac & 0x0000FFFF)]
        print seconds, milliseconds
        request = register_write_message.WriteMultipleRegistersRequest(
            address=time_address_base + 8, values=ts, unit=address
        )
        mb_client.execute(request)
        request = register_read_message.ReadWriteMultipleRegistersRequest(
            read_address=time_address_base, read_count=3,
            write_address=time_address_base + 11, write_registers=[0x0000],
            unit=address
        )
        mb_client.execute(request)
        seconds, milliseconds = get_time()
        frac = milliseconds
        ts = [(seconds & 0xFFFF0000) >> 16,
              (seconds & 0x0000FFFF),
              (frac & 0xFFFF0000) >> 16,
              (frac & 0x0000FFFF)]
        print seconds, milliseconds
        request = register_write_message.WriteMultipleRegistersRequest(
            address=time_address_base + 8, values=ts, unit=address
        )
        mb_client.execute(request)
    else:
        seconds, milliseconds = get_time()
        frac = milliseconds
        ts = [(seconds & 0xFFFF0000) >> 16,
              (seconds & 0x0000FFFF),
              (frac & 0xFFFF0000) >> 16,
              (frac & 0x0000FFFF)]
        print seconds, milliseconds

        request = register_read_message.ReadHoldingRegistersRequest(
            address=time_address_base, count=3, unit=address
        )
        response = mb_client.execute(request)
        seconds = (response.registers[2] << 16) + response.registers[1]
        milliseconds = (response.registers[0])
        print seconds, milliseconds

        seconds, milliseconds = get_time()
        frac = milliseconds
        ts = [(seconds & 0xFFFF0000) >> 16,
              (seconds & 0x0000FFFF),
              (frac & 0xFFFF0000) >> 16,
              (frac & 0x0000FFFF)]
        print seconds, milliseconds
