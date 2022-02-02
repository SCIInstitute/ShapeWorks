import os
import numpy

os.chdir(os.environ["DATA"] + "/simple")

import shapeworks
opt = shapeworks.Optimize()
opt.LoadParameterFile("simple.xml")

def callback():

    print("python callback")
    if opt.GetOptimizing():
        print("optimizing!")
    else:
        print("initializing!")
    particles = opt.GetParticleSystem()
    print(type(particles))
    print(f"shape: {particles.shape}")
    print(particles)


def before_evaluate():
    print("python: before_evaluate")
    print("particles:")
    particles = opt.GetParticleSystem()
    print(type(particles))
    print(f"shape: {particles.shape}")
    print(particles)
    print("gradients:")
    gradients = opt.GetShapeGradientMatrix()
    print(type(gradients))
    print(gradients)
    print("correspondence updates:")
    updates = opt.GetCorrespondenceUpdateMatrix()
    print(type(updates))
    print(updates)
    opt.SetCorrespondenceUpdateMatrix(updates)

    
opt.SetIterationCallbackFunction(callback)
opt.SetBeforeEvaluateCallbackFunction(before_evaluate)
opt.Run()

# TODO: check results are what would be expected (save a file with them?)
