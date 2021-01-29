from ShapeCohortGen import Supershapes,Ellipsoids,EllipsoidJoints,CohortGenUtils

class CohortGenerator():
	def __init__(self,out_dir):
		self.out_dir = out_dir
		self.meshes = []
		self.segs = []
		self.images = []
	def generate_segmentations(self, randomize_size=True, spacing=[1,1,1], allow_on_boundary=True):
		if not self.meshes:
			print("Error: No meshes have been generated to get segmentations from.\n Call 'generate' first.")
			return
		self.segs = CohortGenUtils.generate_segmentations(self.meshes, self.out_dir, randomize_size, spacing, allow_on_boundary)
		return self.segs
	def generate_images(self):
		if not self.segs:
			print("Error: No segmentations have been generated to get images from.\n Call 'generate_segmentations' first.")
			return
		self.images = CohortGenUtils.generate_images(self.segs, self.out_dir, blur_factor=1, foreground_mean=180, foreground_var=30, background_mean=80, background_var=30)
		return self.images

class EllipsoidCohortGenerator(CohortGenerator):
	def __init__(self,out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3, randomize_center=True, randomize_rotation=True):
		self.meshes = Ellipsoids.generate(num_samples, self.out_dir, randomize_center, randomize_rotation)
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
	def generate(self, num_samples=3, randomize_center=True, separation=2):
		self.meshes = EllipsoidJoints.generate(num_samples, self.out_dir, randomize_center,separation)
		return self.meshes
