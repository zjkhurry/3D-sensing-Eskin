import serial
import serial.tools.list_ports
import time
import math

import asyncio
from bleak import BleakClient
from bleak import BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic
import numpy as np
from qtdrawing import drawing

address = "54:6C:0E:1B:C4:f8"
name = "Touch Skin"
BT_UUID = "0000FFF0-0000-1000-8000-00805F9B34FB"
serviceUUID = "0000fff0-0000-1000-8000-00805f9b34fb"
descriptorUUID = "00002902-0000-1000-8000-00805f9b34fb"
characteristicUUID = "0000fff5-0000-1000-8000-00805f9b34fb"

# List all available ports
ports = list(serial.tools.list_ports.comports())
for p in ports:
    print(p.device)

port_name = input("Enter the port name: ")

# Connect to the Arduino
ser = serial.Serial('COM' + port_name, 115200)

# Wait for the Arduino to reset
# input("Press any key after the Arduino has been reset")

x, y, z, t = 100, 100, 300, 50
x_t, y_t, z_t = 0, 0, 0
x_max, y_max, z_max = 200, 300, 380
x_min, y_min, z_min = -200, 60, 10

L0, L1, L2, L3 = 900, 1050, 980, 1500
theta3, theta4, theta5, theta6 = 0, 0, 0, 0
# Send the initial position
s = "$KMS:" + str(x) + "," + str(y) + "," + str(z) + "," + str(t) + "!"
ser.write(s.encode('ascii'))

draw = drawing()

f = open("data.txt", "w+")
#缓存数据，string类型，用于写入文件
cache = str()


#扫描设备，找到指定名称的设备
async def scan():
    devices = await BleakScanner.discover()
    for d in devices:
        if d.name == name:
            print("Found Touch Skin")
            return d
    return None


#处理通知
def notification_handler(characteristic: BleakGATTCharacteristic,
                         data: bytearray):
    # print(f"{characteristic.description}: {data}")
    #将接收到的数据每两个字节转换为一个int
    a = np.array([
        int.from_bytes(data[i:i + 2], byteorder='little')
        for i in range(0, len(data), 2)
    ])

    asyncio.create_task(update(a))


def kinematics_analysis(x, y, z, alpha):
    global theta3, theta4, theta5, theta6
    x1 = x * 10
    y1 = y * 10
    z1 = z * 10
    alpha1 = alpha / 10.0
    if (x == 0):
        theta6 = 0.0
    else:
        theta6 = math.atan2(x1, y1) * 180.0 / math.pi
    y1 = math.sqrt(x1**2 + y1**2)
    y1 = y1 - L3 * math.cos(alpha1 * math.pi / 180.0)
    z1 = z1 - L0 - L3 * math.sin(alpha1 * math.pi / 180.0)
    if (z1 < -10):
        return 1
    if (math.sqrt(y1**2 + z1**2) > L1 + L2):
        return 2
    ccc = math.acos(y1 / math.sqrt(y1**2 + z1**2))
    bbb = (y1**2 + z1**2 + L1**2 - L2**2) / (2 * L1 * math.sqrt(y1**2 + z1**2))
    if (bbb > 1 or bbb < -1):
        return 3
    if (z1 < 0):
        zf_flag = -1
    else:
        zf_flag = 1
    theta5 = ccc * zf_flag + math.acos(bbb)
    theta5 = theta5 * 180.0 / math.pi
    if (theta5 > 180 or theta5 < 0):
        return 4
    aaa = -(y1**2 + z1**2 - L1**2 - L2**2) / (2 * L1 * L2)
    if (aaa > 1 or aaa < -1):
        return 5
    theta4 = math.acos(aaa)
    theta4 = 180 - theta4 * 180.0 / math.pi
    if (theta4 > 135 or theta4 < -135):
        return 6
    theta3 = alpha1 + theta4 - theta5
    if (theta3 > 90 or theta3 < -90):
        return 7
    return 0


def kinematics_move(x, y, z, mtime):
    global theat3, theta4, theta5, theta6
    flag = 0
    mmin = 0
    for i in range(400, 1350, 1):
        if (0 == kinematics_analysis(x, y, z, i)):
            if (i >= mmin):
                mmin = i
            flag = 1
            break
    if (flag == 1):
        kinematics_analysis(x, y, z, mmin)
        pwm0 = round(1500 - 2000.0 * theta6 / 270.0)
        pwm1 = round(1500 + 2000.0 * (theta5 - 90) / 270.0)
        pwm2 = round(1500 + 2000.0 * theta4 / 270.0)
        pwm3 = round(1500 - 2000.0 * theta3 / 270.0)
        s = "{#000P" + str(pwm0).zfill(4) + "T" + str(mtime).zfill(
            4) + "!#001P" + str(pwm1).zfill(4) + "T" + str(mtime).zfill(
                4) + "!#002P" + str(pwm2).zfill(4) + "T" + str(mtime).zfill(
                    4) + "!#003P" + str(pwm3).zfill(4) + "T" + str(
                        mtime).zfill(4) + "!}"
        ser.write(s.encode('ascii'))
        print(s)
    else:
        print("can't reach")


async def update(a):
    global x_t, y_t, z_t
    print(a[:5])
    x = max(min(round(-a[1] / 164) + 200, x_max), x_min)
    y = max(min(round(a[0] / 218) + y_min, y_max), y_min)
    z = max(min(round(a[2] / 164 + 100), z_max), z_min)
    t = math.sqrt((x_t - x)**2 + (y_t - y)**2 + (z_t - z)**2) * 5
    t = max(round(t), 50)
    s = "$KMS:" + str(x) + "," + str(y) + "," + str(z) + "," + str(t) + "!"
    # ser.write(s.encode('ascii'))
    kinematics_move(x, y, z, t)
    x_t = x
    y_t = y
    z_t = z
    print(s)
    #将数据加入缓存,中间用逗号隔开
    global cache
    cache += str(a[0]) + "," + str(a[1]) + "," + str(a[2]) + "," + str(
        a[3]) + "," + str(a[4]) + "\n"
    if cache.count("\n") > 100:
        #缓存中的数据超过100行，写入文件
        f.write(cache)
        cache = str()

    draw.update_data(a)
    # match a[3]:
    #     case 4:
    #         draw.show_text("Flick to left")
    #     case 8:
    #         draw.show_text("Flick to right")
    #     case 16:
    #         draw.show_text("Flick to up")
    #     case 32:
    #         draw.show_text("Flick to down")
    #     case _:
    #         draw.show_text("")
    # match a[4]:
    #     case 1:
    #         draw.show_text("Touch bottom")
    #     case 2:
    #         draw.show_text("Touch left")
    #     case 4:
    #         draw.show_text("Touch top")
    #     case 8:
    #         draw.show_text("Touch right")
    #     case 16:
    #         draw.show_text("Touch center")
    #     case 32:
    #         draw.show_text("Tap bottom")
    #     case 64:
    #         draw.show_text("Tap left")
    #     case 128:
    #         draw.show_text("Tap top")
    #     case 256:
    #         draw.show_text("Tap right")
    #     case 512:
    #         draw.show_text("Tap center")
    #     case 1024:
    #         draw.show_text("Double tap bottom")
    #     case 2048:
    #         draw.show_text("Double tap left")
    #     case 4096:
    #         draw.show_text("Double tap top")
    #     case 8192:
    #         draw.show_text("Double tap right")
    #     case 16384:
    #         draw.show_text("Double tap center")
    #     case _:
    #         draw.show_text("")


#主函数
async def main():
    global characteristicUUID
    device = await scan()
    if not device:
        print("Touch Skin not found")
        return
    print(device)
    async with BleakClient(device) as client:
        print("Connected to Touch Skin")
        print(client)
        for s in client.services:
            for char in s.characteristics:
                if char.description == "DataRead":
                    print("Found DataRead")
                    characteristicUUID = char.uuid
        await client.start_notify(characteristicUUID, notification_handler)
        print("Notification registered")
        draw.show_plot()
        #进入循环，等待通知
        while True:
            if not client.is_connected:
                print("Disconnected")
                f.write(cache)
                f.close()
                break
            await asyncio.sleep(1)
        # await client.stop_notify(characteristicUUID)


asyncio.run(main())

# Close the connection
ser.close()