from DatasetUtils import GirderConnector


# By default tries to use locally stored login state with interactive login
def downloadDataset(filename, loginState = None):
    print('.___________________________.')
    print('|                           |')
    print('|     ShapeWorks Portal     |')
    print('|___________________________|')
    print()
    print("Downloading " + filename + " from the ShapeWorks Portal")

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

    if GirderConnector._downloadDataset(accessToken, filename):
        print(loginState['username'], 'downloaded', filename, 'from the ShapeWorks Portal.')
        return True
    
    return False

def downloadDatasetIndividualFiles(datasetName, destinationPath, loginState = None):
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
    
    if GirderConnector._downloadDatasetIndividualFiles(accessToken, datasetName, destinationPath):
        print(loginState['username'], 'downloaded the', datasetName, 'dataset from the ShapeWorks Portal.')
        return True
    
    return False
