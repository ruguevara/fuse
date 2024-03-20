import sys
import os
import atexit, termios
import time
sys.path.append(os.path.join(os.path.dirname(__file__), "..", ".."))

from fuzx import *

old_settings=None

def init_any_key():
   global old_settings
   old_settings = termios.tcgetattr(sys.stdin)
   new_settings = termios.tcgetattr(sys.stdin)
   new_settings[3] = new_settings[3] & ~(termios.ECHO | termios.ICANON) # lflags
   new_settings[6][termios.VMIN] = 0  # cc
   new_settings[6][termios.VTIME] = 0 # cc
   termios.tcsetattr(sys.stdin, termios.TCSADRAIN, new_settings)


@atexit.register
def term_any_key():
   global old_settings
   if old_settings:
      termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)


def any_key():
   ch_set = []
   ch = os.read(sys.stdin.fileno(), 1)
   while ch is not None and len(ch) > 0:
      ch_set.append(ch[0])
      ch = os.read(sys.stdin.fileno(), 1)
   return ch_set

def print_attrs(data):
    for i in range(0, len(data), 32):
        # print compact form
        print("".join([f"{x:02x}" for x in data[i:i+32]]))


#
init_any_key()

zx = Fuzx.machine()
zx.select_machine(Machine.ZXSpectrum128)
zx.settings.emulation_speed = 100000
zx.load_tape_wait("/Users/ruguevara/projects/speccy/DivMMC/ShamaZX/Demos/128K/breakspace.tap")

frame = 0
while True:
    key = any_key()
    if len(key) > 0:
        break
    zx.do_opcodes()
    zx.do_events()
    if frame % 500 == 0:
        print_attrs(zx.screen_data[-768:])
        print()
    frame += 1


# machine.run_unittests()
# machine.reset()
# print(zx.get_info())
# print(zx.get_info().timings)

# while True:
#     zx.do_opcodes()
#     print(zx.processor)
#     # print(f"PC = {zx.processor.pc}")
#     zx.do_events()

#     key = any_key()
#     if len(key) > 0:
#         break
#     time.sleep(1/50)

Fuzx.destroy()
