import os
from ctypes import cdll

_lib_path = os.path.join(os.path.dirname(__file__), 'build', 'gRPCNvidia.so')
nvidiagRPC = cdll.LoadLibrary(_lib_path)
