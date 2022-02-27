import shapeworks as sw
import numpy as np
import glob

INPUT_DIR  = 'meshes_all'

class ComputeAutoRegularizationParams:
    def __init__(self):
        self.number_of_organs = 0
        self.file_names =[]
        self.all_meshes = []
        self.all_meshes_center = []
        self.meshes = []
        

    def load_files(self, organ_keyword):
        self.file_names = sorted(glob.glob(f'{INPUT_DIR}/*{organ_keyword}*.vtk'))
        print(f"{len(self.file_names)} Files Loaded")
        for file_name in self.file_names:
            print(f"Loading {file_name} -----")
            mesh = sw.Mesh(file_name)
            self.meshes.append(mesh)
    
    def load_all_organ_files(self, organ_ar):
        for organ_keyword in organ_ar:
            meshes_k = []
            meshes_k_center = []
            file_names = sorted(glob.glob(f'{INPUT_DIR}/*{organ_keyword}*.vtk'))
            print(f"{len(file_names)} Files Loaded for organ {organ_keyword}")
            for file_name in file_names:
                print(f"Loading {file_name} -----")
                mesh = sw.Mesh(file_name)
                center = mesh.centerOfMass()
                meshes_k.append(mesh)
                meshes_k_center.append(center)
            self.all_meshes.append(meshes_k)
            self.all_meshes_center.append(meshes_k_center)
            print(f'{organ_keyword} organ loaded')
    
    def compute_between_reg(self):
        all_meshes_center = np.array(self.all_meshes_center)
        print(f"original obj is {all_meshes_center}")
        between_organ_mean = np.mean(all_meshes_center, axis=0)
        print(f"between organ_ mean is {between_organ_mean}")
       
        #
        all_meshes_center = all_meshes_center - between_organ_mean
        
        print(y)
        organs = all_meshes_center.shape[0]
        shapes = all_meshes_center.shape[1]
        print("organs and shapes are", organs, shapes)
        z = np.zeros((organs*3, shapes))
        for organ in range(organs):
            for shape in range(shapes):
                z[organ*3:(organ+1)*3, shape] = all_meshes_center[organ, shape]


        between_shapes_mean = np.mean(all_meshes_center, axis=1)
        print(f"between shapes mean is {between_shapes_mean}")
        objective_matrix = np.transpose(np.transpose(all_meshes_center) - between_shapes_mean)
        print(f"objective matrix is {objective_matrix}")
        u, w, v = np.linalg.svd(objective_matrix)
        w = np.sort(w)
        between_start = w[-1]
        val = 0.0
        for j in range(0, w.shape[0]-1):
            val += (w[j] * w[j])
        val /= (w.shape[0] - 1)
        between_end = val

        print(f"Start is 5% of {between_start * between_start} and end is {between_end}")



    
    def compute_distance_matrix(self):
        self.N = len(self.meshes)
        self.D = np.zeros((self.N, self.N))
        for i in range(len(self.meshes)):
            for j in range(len(self.meshes)):
                if i == j:
                    self.D[i, j] = 1
                #elif i > j:
                    #self.D[i, j] = self.D[j, i]
                else:
                    a = self.meshes[i].distance(self.meshes[j]).getFieldMean("distance")
                    b = self.meshes[j].distance(self.meshes[i]).getFieldMean("distance")
                    self.D[i, j] = ((a + b)/2) * ((a + b)/2)
                    print(f"distance of {i} and {j} is {self.D[i, j]}")



    def compute_reg_param(self):
        centering_matrix = np.identity(self.N)
        ones_N = np.ones(self.N)

        centering_matrix = centering_matrix - (1/self.N)*(np.outer(ones_N, ones_N))
        self.obj_matrix = -0.5 * centering_matrix *  self.D  * centering_matrix
        
        u, w, v = np.linalg.svd(self.obj_matrix)
        w = np.sort(w)
        self.start = w[-1]
        val = 0.0
        for j in range(0, w.shape[0]-1):
            val += (w[j] * w[j])
        val /= (w.shape[0] - 1)
        self.end = val

        print(f"Start is 5% of {self.start * self.start} and end is {self.end}")

if __name__=="__main__":
    ob = ComputeAutoRegularizationParams()
    # ob.load_files('08')
    # ob.compute_distance_matrix()
    # ob.compute_reg_param()
    organ_ar = ['_20_', '_21_', '_22_']
    ob.load_all_organ_files(organ_ar)
    ob.compute_between_reg()


 
 
