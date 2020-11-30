from ShapeCohortGen import Supershapes,Ellipsoids

class CohortGenerator():
	def __init__(self,out_dir):
		self.out_dir = out_dir
		self.dts = []
		self.segs = []
		self.meshes = []
		self.images = []
	def get_segmentations(self):
		if not self.images:
			print("Error: No segmentations have been generated. Call generate to generate segmentations.")
			return
		return self.segs
	def get_images(self):
		if not self.images:
			print("Error: No images have been generated. Call generate_images to generate images.")
			return
		return self.images
	def get_meshes(self):
		pass
	def get_cohort_csv():
		pass

class EllipsoidCohortGenerator(CohortGenerator):
	def __init__(self,out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3):
		Ellipsoids.get_ellispoids(num_samples,self.out_dir)

class SupershapesCohortGenerator(CohortGenerator):
	def __init__(self, out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3, m=3, start_id=0, size=98):
		self.dts = Supershapes.get_shapes(m, num_samples, start_id, self.out_dir, size)
		self.segs = Supershapes.get_segmentations(self.dts, self.out_dir)
		return self.segs
	def generate_images(self, blur_factor=1, foreground_mean=180, foreground_var=30, background_mean=100, background_var=30):
		if not self.segs:
			print("Error: No segmentations to generate images from. Run 'generate' first.")
			return
		self.images = Supershapes.get_images(self.segs, self.out_dir, blur_factor, foreground_mean, foreground_var, background_mean, background_var)
		return self.images
	def generate_meshes(self):
		if not self.dts:
			print("Error: No shapes to generate meshes from. Run 'generate' first.")
			return
		self.images = Supershapes.get_meshes(self.dts, self.out_dir)
		return self.meshes
	def get_distance_transforms(self):
		if not self.images:
			print("Error: No distance transforms have been generated. Call 'generate' to generate distance transforms.")
			return
		return self.dts
