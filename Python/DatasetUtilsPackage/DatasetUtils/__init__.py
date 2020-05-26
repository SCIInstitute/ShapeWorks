from DatasetUtils import GirderConnector

## Returns None if failed to get list, otherwise returns list of dataset names: ['ellipsoid', 'left_atrium', ...]
def getDatasetList(loginState = None):
    GirderConnector._printDataPortalWelcome()
    print('Getting dataset list from the ShapeWorks Portal')
    accessToken = GirderConnector._login(loginState)
    return GirderConnector._getDatasetList(accessToken)


def downloadDataset(datasetName, destinationPath='.', fileList = None, asZip = True, loginState = None):
    GirderConnector._printDataPortalWelcome()
    print('Downloading the', datasetName, 'dataset from the ShapeWorks Portal')
    accessToken = GirderConnector._login(loginState)
    
    if fileList is not None:
        print('Downloading', len(fileList), 'specified files')
        GirderConnector._downloadDatasetFiles(accessToken, datasetName, fileList, destinationPath)
    elif asZip:
        GirderConnector._downloadDatasetZip(accessToken, datasetName, destinationPath)
    else:
        GirderConnector._downloadDataset(accessToken, datasetName, destinationPath)

    print('Downloaded the', datasetName, 'dataset from the ShapeWorks Portal.')


## fileList is list of file path strings 
def downloadDatasetFiles(datasetName, fileList, destinationPath='.', loginState = None):
    GirderConnector._printDataPortalWelcome()
    print('Downloading', len(fileList), 'specified files from the', datasetName, 'dataset from the ShapeWorks Portal')
    accessToken = GirderConnector._login(loginState)

    GirderConnector._downloadDatasetFiles(accessToken, datasetName, fileList, destinationPath)

    print('Downloaded', len(fileList), 'specified files from the', datasetName, 'dataset from the ShapeWorks Portal')


def uploadNewDataset(datasetName, datasetPath, loginState = None):
    GirderConnector._printDataPortalWelcome()
    print('Uploading the %s dataset from %s to the ShapeWorks Portal' % (datasetName, datasetPath))
    accessToken = GirderConnector._login(loginState)

    GirderConnector._uploadNewDataset(accessToken, datasetName, datasetPath)
    
    print('Uploaded the', datasetName, 'dataset to the ShapeWorks Portal.')
