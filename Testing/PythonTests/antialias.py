# from shapeworks import *
import shapeworks

def antialiasTest():
  img = Image("$DATA/1x2x2.nrrd")
  img.antialias()

  return img.compare("$DATA/antialias1.nrrd")