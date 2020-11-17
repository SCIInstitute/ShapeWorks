class CohortGenerator():
	def __init__(self, num_samples):
		self.num_samples = num_samples
	def get_binary_segmentations(self):
		pass
	def get_images(self):
		pass
	def write_cohort(self, wrtie_dir):
		print("Writing cohort to " + str(wrtie_dir) + ".")
	def get_cohort_csv():
		pass

class EllipsoidCohortGenerator(CohortGenerator):
	def __init__(self, num_samples):
		super().__init__(num_samples)
	def generate(self):
		pass

class SupershapesCohortGenerator(CohortGenerator):
	def __init__(self, num_samples):
		super().__init__(num_samples)
	def generate(self):
		pass