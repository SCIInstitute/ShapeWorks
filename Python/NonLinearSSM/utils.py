import numpy as np
import torch
import math
import glob

class DatasetSuperShapes:
    def __init__(self, numpy_data):
        self.data = numpy_data
        self.xdim = self.data.shape[1:]
        self.num_samples = self.data.shape[0]
        self.data = torch.from_numpy(self.data)
        
    def sample(self, n):
        X = self.data[np.random.randint(self.data.shape[0], size=n)]
        return X


class Dataset:
    def __init__(self, numpy_data):
        self.data = numpy_data
        self.xdim = self.data.shape[1:]
        self.num_samples = self.data.shape[0]
        self.data = torch.from_numpy(self.data)
    
    def get_id(self, i):
        return self.data[i, ...]
        
    def datasize(self):
        return self.data.shape[0]

    def sample(self, n):
        X = self.data[np.random.randint(self.data.shape[0], size=n)]
        return X

def super_formula_3D(m, n1, n2, n3, a, b, numPoints=100000):
    def super_formula_2D(m, n1, n2, n3, a, b, theta):
        r = abs((1 / a) * np.cos(m * theta / 4.0))**n2  +  abs((1 / b) * np.sin(m * theta / 4.0))**n3
        return r**(-1 / n1)

    numPointsRoot = round(math.sqrt(numPoints))
    theta = np.linspace(-math.pi, math.pi, endpoint=True, num=numPointsRoot)
    phi = np.linspace(-math.pi / 2.0, math.pi/2.0, endpoint=True, num=numPointsRoot)
    theta, phi = np.meshgrid(theta, phi)
    theta, phi = theta.flatten(), phi.flatten()
    r1 = super_formula_2D(m, n1, n2, n3, a, b, theta)
    r2 = super_formula_2D(m, n1, n2, n3, a, b, phi)
    x = r1 * r2 * np.cos(theta) * np.cos(phi)
    y = r1 * r2 * np.sin(theta) * np.cos(phi)
    z = r2 * np.sin(phi)

    return x, y, z

def generate_3D_supershapes_point_cloud(N, M, d=3, m=3):
    n1 = np.random.chisquare(4)
    n2 = np.random.chisquare(4)
    n3 = np.random.chisquare(4)
    points_cloud = np.zeros([N, M, d])
    a = 1
    b = 1
    for i in range(N):
        X, Y, Z =  super_formula_3D(m, n1, n2, n3, a, b, M)
        points = np.column_stack((X,Y,Z))
        points_cloud[i, ...] = points[:, :2]
    return points_cloud

def load_shape_matrix(particle_dir, N, M, d=3, particle_system='world'):
    point_files = sorted(glob.glob(f'{particle_dir}/*_{particle_system}.particles'))
    if len(point_files)==0:
        point_files = sorted(glob.glob(f'{particle_dir}/*_world.particles'))

    if len(point_files) != N:
        raise ValueError(f"Inconsistent particle files for {N} subjects")
    else:
        data = np.zeros([N, M, d])
        for i in range(len(point_files)):
            nm = point_files[i]
            data[i, ...] = np.loadtxt(nm)[:, :3]

    return data


def get_particle_statistics(data):
	data = data.reshape(data.shape[0], -1)
	mu = data.mean(0)
	covmat = np.cov(data.T)
	return [mu, covmat]

class DictMap(dict):
    """
    Example:
    m = DictMap({'first_name': 'Eduardo'}, last_name='Pool', age=24, sports=['Soccer'])
    """
    def __init__(self, *args, **kwargs):
        super(DictMap, self).__init__(*args, **kwargs)
        for arg in args:
            if isinstance(arg, dict):
                for k, v in arg.items():
                    self[k] = v

        if kwargs:
            for k, v in kwargs.items():
                self[k] = v

    def __getattr__(self, attr):
        return self.get(attr)

    def __setattr__(self, key, value):
        self.__setitem__(key, value)

    def __setitem__(self, key, value):
        super(DictMap, self).__setitem__(key, value)
        self.__dict__.update({key: value})

    def __delattr__(self, item):
        self.__delitem__(item)

    def __delitem__(self, key):
        super(DictMap, self).__delitem__(key)
        del self.__dict__[key]