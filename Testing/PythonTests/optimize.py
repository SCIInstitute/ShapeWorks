import os
import numpy

os.chdir(os.environ["DATA"] + "/simple")

import shapeworks
opt = shapeworks.Optimize()
opt.LoadParameterFile("simple.xml")

def callback():
    #print("python callback") (please don't print anything unless test fails)
    particles = opt.GetParticleSystem()
    #print(type(particles))
    #print(particles)

opt.SetIterationCallbackFunction(callback)
opt.Run()

# TODO: check results are what would be expected (save a file with them?)
