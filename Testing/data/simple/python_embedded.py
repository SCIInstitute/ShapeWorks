print("line 1")
print("line 2")
import shapeworks
print("line 3")

opt = shapeworks.Optimize

def callback():
    print("python callback")
    particles = opt.GetParticleSystem()
    print(type(particles))
    print(particles)

def run(optimizer):
    print("python run me!")
    global opt
    opt = optimizer
    opt.SetIterationCallbackFunction(callback)

