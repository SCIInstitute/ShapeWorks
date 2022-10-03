from ShapeCohortGen import Supershapes,Ellipsoids,EllipsoidJoints,CohortGenUtils,Tori

class CohortGenerator():
	def __init__(self,out_dir):
		self.out_dir = out_dir
		self.meshes = []
		self.contours = []
		self.segs = []
		self.images = []

class EllipsoidCohortGenerator(CohortGenerator):
	def __init__(self,out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3, randomize_center=True, randomize_rotation=True, randomize_x_radius=True, randomize_y_radius=True, randomize_z_radius=True):
		self.meshes = Ellipsoids.generate(num_samples, self.out_dir, randomize_center, randomize_rotation, randomize_x_radius, randomize_y_radius, randomize_z_radius)
		return self.meshes

class SupershapesCohortGenerator(CohortGenerator):
	def __init__(self, out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3, randomize_center=True, randomize_rotation=True, m=3, start_id=0, size=20):
		self.meshes = Supershapes.generate(num_samples, self.out_dir, randomize_center, randomize_rotation, m, start_id, size)
		return self.meshes


class EllipsoidJointsCohortGenerator(CohortGenerator):
	def __init__(self,out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3, randomize_center=True, randomize_x_radius=True, randomize_y_radius=True, randomize_z_radius=True,mode_size=False,mode_rotation=True,separation=2):
		self.meshes = EllipsoidJoints.generate(num_samples, self.out_dir, randomize_center, randomize_x_radius, randomize_y_radius, randomize_z_radius,mode_size,mode_rotation,separation)
		return self.meshes

class Supershapes2DCohortGenerator(CohortGenerator):
	def __init__(self, out_dir):
		super().__init__(out_dir)

	def generate(self, num_samples=3, m=3, n1_degree=4.0, n2_degree=None, n3_degree=None, default_n=5.0, seed=41):
		self.contours = Supershapes.generate_2D(num_samples, 250, self.out_dir, m, n1_degree, n2_degree, n3_degree, default_n, seed)
		return self.contours

class ToriCohortGenerator(CohortGenerator):
	def __init__(self,out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3, randomize_center=True, randomize_rotation=True, randomize_ring_radius=True, randomize_cross_section_radius=True):
		self.meshes = Tori.generate(num_samples, self.out_dir, randomize_center, randomize_rotation, randomize_ring_radius, randomize_cross_section_radius)
		return self.meshes
