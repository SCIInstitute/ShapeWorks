from ShapeCohortGen import CohortGenerator
from ShapeCohortGen import Supershapes
from ShapeCohortGen import Ellipsoids
from ShapeCohortGen import EllipsoidJoints
from ShapeCohortGen import CohortGenUtils
from ShapeCohortGen import Tori
import os

def EllipsoidCohortGenerator(out_dir=os.getcwd()+'/generated_ellipsoid_cohort/'):
	return CohortGenerator.EllipsoidCohortGenerator(out_dir)

def SupershapesCohortGenerator(out_dir=os.getcwd()+'/generated_supershapes_cohort/'):
	return CohortGenerator.SupershapesCohortGenerator(out_dir)

def EllipsoidJointsCohortGenerator(out_dir=os.getcwd()+'/generated_ellipsoid_joint_cohort/'):
	return CohortGenerator.EllipsoidJointsCohortGenerator(out_dir)

def ToriCohortGenerator(out_dir=os.getcwd()+'/generated_tori_cohort/'):
	return CohortGenerator.ToriCohortGenerator(out_dir)

def Supershapes2DCohortGenerator(out_dir=os.getcwd()+'/generated_supershapes_2D_cohort/'):
	return CohortGenerator.Supershapes2DCohortGenerator(out_dir)

def GenerateSegmentations(generated_directories, randomize_size=False, spacing=[1.0,1.0,1.0], allow_on_boundary=False, padding=[5,5,5]):
	return CohortGenUtils.generate_segmentations(generated_directories, randomize_size, spacing, allow_on_boundary, padding)

def GenerateImages(generated_directories, blur_factor=1, foreground_mean=180, foreground_var=30, background_mean=80, background_var=30):
	return CohortGenUtils.generate_images(generated_directories, blur_factor, foreground_mean, foreground_var, background_mean, background_var)

def BBDimensions(generated_directories):
	return CohortGenUtils.bb_dimensions(generated_directories)

def Generate2DSegmentations(generated_directories, randomize_size=False, spacing=[1.0,1.0], allow_on_boundary=False, padding=[5,5]):
	return CohortGenUtils.generate_2Dsegmentations(generated_directories, randomize_size, spacing, allow_on_boundary, padding)

def Generate2DImages(generated_directories, blur_factor=1, foreground_mean=180, foreground_var=30, background_mean=80, background_var=30):
	return CohortGenUtils.generate_2Dimages(generated_directories, blur_factor, foreground_mean, foreground_var, background_mean, background_var)
