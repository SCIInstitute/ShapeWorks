import os
import sys
from shapeworks import *

def padfailTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.pad()
  return True

try:
	val = padfailTest1()
	print("padfailTest1 failed")
	sys.exit(1)
except:
	pass
