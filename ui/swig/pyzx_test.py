import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), "..", ".."))

from fuzx import *

init()
# run_unittests()

print("Selecting machine: ", select_machine(Machine.ZXSpectrum128))

deinit()
