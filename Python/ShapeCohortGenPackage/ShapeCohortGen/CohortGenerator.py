import vtk
import numpy as np

class CohortGenerator():
	def __init__(self):
		self.meshes = []
		self.segs = []
		self.images = []
	def get_binary_segmentations(self):
		pass
	def get_images(self):
		pass
	def write_cohort(self, wrtie_dir):
		print("Writing cohort to " + str(wrtie_dir) + ".")
	def get_cohort_csv():
		pass

class EllipsoidCohortGenerator(CohortGenerator):
	def __init__(self):
		super().__init__()
	def generate(self, num_samples=3):
		for index in range(num_samples):
			self.meshes.append(self.generate_mesh())
	def generate_mesh(self):
		pass

class SupershapesCohortGenerator(CohortGenerator):
	def __init__(self):
		super().__init__()
	def generate(self, num_samples=3):
		pass
	def generate_mesh(self, n1, n2):
		pass