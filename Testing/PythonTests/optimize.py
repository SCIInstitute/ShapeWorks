import os
import numpy

os.chdir(os.environ["DATA"] + "/simple")

import shapeworks
opt = shapeworks.Optimize()
opt.LoadParameterFile("simple.xml")

def callback():
    print("python callback")
    particles = opt.GetParticleSystem()
    print(type(particles))
    print(particles)

opt.SetIterationCallbackFunction(callback)
opt.Run()
