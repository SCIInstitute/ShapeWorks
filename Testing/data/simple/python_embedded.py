import shapeworks

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

