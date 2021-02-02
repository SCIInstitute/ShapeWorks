import os
import shapeworks as sw
import numpy as np

def findReferenceImageIndex(shapeSegList):
	# Step 1: Bring all segmentations to the same image size if needed

	# compute maximum image size in each dimension
	x_dims, y_dims, z_dims = [], [], []
	for shapeSeg in shapeSegList:
		dims = shapeSeg.dims()
		x_dims.append(dims[0])
		y_dims.append(dims[1])
		z_dims.append(dims[2])

	# define the maximum image size in the given dataset in case different images have different sizes
	x_max_dim = max(x_dims)
	y_max_dim = max(y_dims)
	z_max_dim = max(z_dims) 
	# this is the common size that we need to pad individual segmentations 
	# to compute the mean shape and distance to the mean shape
	
	# Step 2: Compute mean shape
	meanShape = np.zeros((x_max_dim, y_max_dim, z_max_dim))
	padValue  = 0

	# let's keep a list for the padded ones to compute distances to the mean shape next
	paddedList = [] 
	for shapeSeg in shapeSegList:
		dims   = shapeSeg.dims()

		# convert to numpy array
		shapeSeg_array = shapeSeg.toArray()
		
		# the numpy array needs to be permuted to match the shapeworks image dimensions
		shapeSeg_array = np.transpose(shapeSeg_array,(2,1,0))
		
		# now let's pad and still maintain the shape in the center of the image domain
		x_diff   = x_max_dim - dims[0]
		before_x = x_diff // 2
		after_x  = x_max_dim - (dims[0] + before_x)
		
		y_diff   = y_max_dim - dims[1]
		before_y = y_diff // 2
		after_y  = y_max_dim - (dims[1] + before_y)
		
		z_diff   = z_max_dim - dims[2]
		before_z = z_diff // 2
		after_z  = z_max_dim - (dims[2] + before_z)
		
		paddedList.append( np.pad(shapeSeg_array, ( (before_x, after_x), 
													(before_y, after_y), 
													(before_z, after_z)
												  )
								 )
						 )
		meanShape += paddedList[-1]
	meanShape /= len(shapeSegList)

	# Step 3: Compute distance to mean shape

	distances = np.zeros((len(shapeSegList),))
	for ii, segPadded in enumerate(paddedList):
		distances[ii] = np.linalg.norm(meanShape - segPadded)

	# Step 4: Select the shape sample that is closest to the mean shape
	referenceIdx = distances.argmin()

	return referenceIdx

# Saves a list of shapeworks images in a directory and returns the saved file paths
# this could be used to save final and intermediate results (if needed)
def saveImages(outDir,        # path to the directory where we want to save the images
                swImageList,   # list of shapeworks images to be saved
                swImageNames,  # list of image names to be used as filenames
                extension        = 'nrrd',
                compressed       = False, # use false to load in paraview 
                verbose          = True):

    outFiles = []
    if (len(swImageList) != len(swImageNames)):
        print('swImageNames list is not consistent with number of images in swImageList')
        return

    # create the output directory in case it does not exist
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    for curImg, curName in zip(swImageList, swImageNames):
        filename = outDir + curName + '.' + extension
        if verbose:
            print('Writing: ' + filename)
        curImg.write(filename, compressed=compressed)
        outFiles.append(filename)
    return outFiles
