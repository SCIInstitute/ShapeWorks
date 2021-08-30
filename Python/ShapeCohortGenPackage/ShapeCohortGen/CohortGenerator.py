from ShapeCohortGen import Supershapes,Ellipsoids,EllipsoidJoints,CohortGenUtils,Tori

class CohortGenerator():
	def __init__(self,out_dir):
		self.out_dir = out_dir
		self.meshes = []
		self.contours = []
		self.segs = []
		self.images = []
	def generate_segmentations(self, randomize_size=True, spacing=[1.0,1.0,1.0], allow_on_boundary=True):
		if not self.meshes:
			print("Error: No meshes have been generated to get segmentations from.\n Call 'generate' first.")
			return
		self.segs = CohortGenUtils.generate_segmentations(self.meshes, self.out_dir, randomize_size, spacing, allow_on_boundary)
		return self.segs
	def generate_images(self, blur_factor=1, foreground_mean=180, foreground_var=30, background_mean=80, background_var=30):
		if not self.segs:
			print("Error: No segmentations have been generated to get images from.\n Call 'generate_segmentations' first.")
			return
		self.images = CohortGenUtils.generate_images(self.segs, self.out_dir, blur_factor, foreground_mean, foreground_var, background_mean, background_var)
		return self.images

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

	def generate_segmentations(self, randomize_size=True, spacing=[1.0,1.0,1.0], allow_on_boundary=True):
		raise RuntimeError("Unsupported")

	def generate_images(self, blur_factor=1, foreground_mean=180, foreground_var=30, background_mean=80, background_var=30):
		raise RuntimeError("Unsupported")

class ToriCohortGenerator(CohortGenerator):
	def __init__(self,out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3, randomize_center=True, randomize_rotation=True, randomize_ring_radius=True, randomize_cross_section_radius=True):
		self.meshes = Tori.generate(num_samples, self.out_dir, randomize_center, randomize_rotation, randomize_ring_radius, randomize_cross_section_radius)
		return self.meshes