from pythonTestSetup import setup

setup()

from shapeworks import *
import os

def antialiasTest():
  img = Image(os.environ["DATA"] + "1x2x2.nrrd")
  img.antialias()

  compareImg = Image(os.environ["DATA"] + "antialias1.nrrd")

  return img.compare(compareImg)

antialiasTest()