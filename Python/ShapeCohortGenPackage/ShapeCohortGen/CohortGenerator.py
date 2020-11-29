from ShapeCohortGen import Supershapes

class CohortGenerator():
	def __init__(self,out_dir):
		self.out_dir = out_dir
		self.meshes = []
		self.segs = []
		self.images = []
	def get_binary_segmentations(self):
		pass
	def get_images(self):
		pass
	def get_cohort_csv():
		pass

class EllipsoidCohortGenerator(CohortGenerator):
	def __init__(self,out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3):
		for index in range(num_samples):
			self.meshes.append(self.generate_mesh())
	def generate_mesh(self):
		pass

class SupershapesCohortGenerator(CohortGenerator):
	def __init__(self, out_dir):
		super().__init__(out_dir)
	def generate(self, num_samples=3, m=3, size=98, start_id=0, blur_factor=1):
		implicit_dir = Supershapes.get_shapes(m, num_samples, blur_factor, start_id, self.out_dir, size)
		self.segs = Supershapes.get_segmentation(implicit_dir, self.out_dir)
		# dtDir = get_distance_transforms(segDir, outDir)
		# imgDir = get_image(blur_factor, segDir, outDir,intensityDiff)
		return self.segs
