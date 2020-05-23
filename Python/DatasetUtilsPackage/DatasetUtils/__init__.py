from DatasetUtils import GirderConnector

## Returns None if failed to get list, otherwise returns list of dataset names: ['ellipsoid', 'left_atrium', ...]
def getDatasetList(loginState = None):
    GirderConnector._printDataPortalWelcome()
    print('Getting dataset list from the ShapeWorks Portal')
    accessToken = GirderConnector._login(loginState)
    if not accessToken:
        return None
    return GirderConnector._getDatasetList(accessToken)


def downloadDataset(datasetName, destinationPath='.', fileList = None, asZip = True, loginState = None):
    GirderConnector._printDataPortalWelcome()
    print('Downloading the', datasetName, 'dataset from the ShapeWorks Portal')
    accessToken = GirderConnector._login(loginState)
    if not accessToken:
        return False
    
    if fileList is not None:
        print('Downloading', len(fileList), 'specified files')
        success = GirderConnector._downloadDatasetFiles(accessToken, datasetName, fileList, destinationPath)
    elif asZip:
        success = GirderConnector._downloadDatasetZip(accessToken, datasetName, destinationPath)
    else:
        success = GirderConnector._downloadDataset(accessToken, datasetName, destinationPath)

    if success:
        print('Downloaded the', datasetName, 'dataset from the ShapeWorks Portal.')
        return True
    else:
        print('FAILED to download the', datasetName, 'dataset from the ShapeWorks Portal.')
        return False

## fileList is list of strings 
def downloadDatasetFiles(datasetName, fileList, destinationPath='.', loginState = None):
    GirderConnector._printDataPortalWelcome()
    print('Downloading', len(fileList), 'specified files from the', datasetName, 'dataset from the ShapeWorks Portal')
    accessToken = GirderConnector._login(loginState)
    if not accessToken:
        return False

    success = GirderConnector._downloadDatasetFiles(accessToken, datasetName, fileList, destinationPath)
    if success:
        print('Downloaded', len(fileList), 'specified files from the', datasetName, 'dataset from the ShapeWorks Portal')
        return True
    else:
        print('FAILED to download', len(fileList), 'specified files from the', datasetName, 'dataset from the ShapeWorks Portal')
        return False


def uploadNewDataset(datasetName, datasetPath, loginState = None):
    GirderConnector._printDataPortalWelcome()
    print('Uploading the %s dataset from %s to the ShapeWorks Portal' % (datasetName, datasetPath))
    accessToken = GirderConnector._login(loginState)
    if not accessToken:
        return False

    if GirderConnector._uploadNewDataset(accessToken, datasetName, datasetPath):
        print('Uploaded the', datasetName, 'dataset to the ShapeWorks Portal.')
        return True
    else:
        print('FAILED to upload the', datasetName, 'dataset to the ShapeWorks Portal.')
        return False
