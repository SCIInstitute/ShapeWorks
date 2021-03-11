# a helper function that saves a list of shapeworks images in a directory
# this could be used to save final and intermediate results (if needed)
def save_images(outDir,        # path to the directory where we want to save the images
                swImageList,   # list of shapeworks images to be saved
                swImageNames,  # list of image names to be used as filenames
                extension        = 'nrrd',
                compressed       = False, # use false to load in paraview
                verbose          = True):

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
