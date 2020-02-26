from DatasetUtils import GirderConnector

def downloadDataset(datasetName, destinationPath='.', asZip = True, loginState = None):
    print('.___________________________.')
    print('|                           |')
    print('|     ShapeWorks Portal     |')
    print('|___________________________|')
    print()
    print('Downloading the', datasetName, 'dataset from the ShapeWorks Portal')
    if loginState is None:
        # interactive login mode
        loginState, accessToken = GirderConnector._loginAndGetAccessToken()
        if accessToken is None:
            return False
    else:
        # login using provided credentials
        if not GirderConnector._verifyLoginState(loginState):
            print('Invalid login state')
            return False
        accessToken = GirderConnector._getAccessToken(loginState['key'])
        if accessToken is None:
            print('Unable to get access token')
            return False
    
    if asZip:
        success = GirderConnector._downloadDatasetZip(accessToken, datasetName, destinationPath)
    else:
        success = GirderConnector._downloadDataset(accessToken, datasetName, destinationPath)
    if success:
        print(loginState['username'], 'downloaded the', datasetName, 'dataset from the ShapeWorks Portal.')
        return True
    else:
        print('FAILED to downloaded the', datasetName, 'dataset from the ShapeWorks Portal.')
        return False

def uploadNewDataset(datasetName, datasetPath, loginState = None):
    print('.___________________________.')
    print('|                           |')
    print('|     ShapeWorks Portal     |')
    print('|___________________________|')
    print()
    print('Uploading the %s dataset from %s to the ShapeWorks Portal' % (datasetName, datasetPath))
    if loginState is None:
        # interactive login mode
        loginState, accessToken = GirderConnector._loginAndGetAccessToken()
        if accessToken is None:
            return False
    else:
        # login using provided credentials
        if not GirderConnector._verifyLoginState(loginState):
            print('Invalid login state')
            return False
        accessToken = GirderConnector._getAccessToken(loginState['key'])
        if accessToken is None:
            print('Unable to get access token')
            return False

    if GirderConnector._uploadNewDataset(accessToken, datasetName, datasetPath):
        print(loginState['username'], 'uploaded the', datasetName, 'dataset to the ShapeWorks Portal.')
        return True

    return False
