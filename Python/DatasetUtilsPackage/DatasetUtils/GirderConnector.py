import requests 
import json
import os
import getpass
import base64

from DatasetUtils import GirderAPI

_API_KEY_NAME = 'python_script'
_LOGIN_FILE_NAME = 'shapeworksPortalLogin.txt'
_CONTACT_SUPPORT_STRING = 'Please contact support: shapeworks-users@sci.utah.edu'
_VERSION = 'v0'
_USE_CASE_DATA_COLLECTION = 'use-case-data-%s' % _VERSION

serverAddress = 'http://cibc1.sci.utah.edu:8080/'


def _printDataPortalWelcome():
    print('.___________________________.')
    print('|                           |')
    print('|    ShapeWorks Portal %s   |' % _VERSION)
    print('|___________________________|')
    print()


def _login(loginState):
    if loginState is None:
        # interactive login mode
        loginState, accessToken = _loginAndGetAccessToken()
    else:
        # login using provided credentials
        if not _verifyLoginState(loginState):
            raise ValueError('Invalid login state')
        accessToken = GirderAPI._getAccessToken(loginState['key'])
    if accessToken is None:
        raise ValueError('Unable to get access token')
    return accessToken
    

def _loginAndGetAccessToken():

    loginState = _loadLogin()
    
    if loginState is None:
        print('Login info not found in current directory.')
        loginState = _promptLogin()
        if loginState is None:
            print('Login failed!')
            return (None, None)

    accessToken = GirderAPI._getAccessToken(serverAddress, loginState['key'])
    if accessToken is None:
        # Retry login once
        print('Login info in current directory is invalid.')
        loginState = _promptLogin()
        if loginState is None:
            print('Login failed!')
            return (None, None)
        accessToken = GirderAPI._getAccessToken(serverAddress, loginState['key'])
        if accessToken is None:
            print('ERROR! Login info is invalid again.', _CONTACT_SUPPORT_STRING)
            return (loginState, None)
    return (loginState, accessToken)


def _promptLogin():
    print('New ShapeWorks Portal users: Register an account at ' + serverAddress + '#?dialog=register')
    print('Returning ShapeWorks Portal users: Enter your username and password')

    basicAuthToken = None
    while basicAuthToken is None:
        print('')
        username = input("Username: ")
        password = getpass.getpass("Password: ")
        combined = username + ':' + password
        usernamePasswordHash = base64.b64encode(combined.encode()).decode("ascii")
        basicAuthToken = GirderAPI._authenticateBasicAuth(serverAddress, usernamePasswordHash)
        if basicAuthToken is None:
            print('Incorrect username or password')

    apiKey = GirderAPI._getApiKey(serverAddress, basicAuthToken, _API_KEY_NAME)
    if apiKey is None:
        apiKey = GirderAPI._createApiKey(serverAddress, basicAuthToken, _API_KEY_NAME)
    if apiKey is None:
        print('Failed to create api key.', _CONTACT_SUPPORT_STRING)
        return None
    loginState = {'username': username, 'key': apiKey}
    _saveLogin(loginState)
    return loginState


def _saveLogin(loginState):
    with open(_LOGIN_FILE_NAME, 'w') as outfile:
        json.dump(loginState, outfile)


def _loadLogin():
    if not os.path.exists(_LOGIN_FILE_NAME):
        return None
    with open(_LOGIN_FILE_NAME) as json_file:
        loginState = json.load(json_file)
        if not _verifyLoginState(loginState):
            return None
        return loginState
    return None


def _verifyLoginState(loginState):
    return 'key' in loginState and 'username' in loginState


def _downloadDatasetZip(accessToken, datasetName, destinationPath):
    # 1 get info of the use case collection
    useCaseCollection = GirderAPI._getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    # 2 get info of the dataset folder in that collection
    datasetFolder = GirderAPI._getFolderInfo(serverAddress, accessToken, parentType='collection', parentId=useCaseCollection['_id'], folderName=datasetName)
    # 3 download the dataset folder
    GirderAPI._downloadFolder(serverAddress, accessToken, path=destinationPath, folderInfo=datasetFolder)


def _downloadFolder(accessToken, path, folder):
    if not os.path.exists(path):
        os.makedirs(path)
    # 1 download items in this folder
    items = GirderAPI._listItemsInFolder(serverAddress, accessToken, folder['_id'])
    for item in items:
        GirderAPI._downloadItem(serverAddress, accessToken, path, item)
        
    # 2 for each subfolder, create directory in the file system and download every item in the subfolder
    for subfolder in GirderAPI._getFolderList(serverAddress, accessToken, parentType='folder', parentId=folder['_id']):
        _downloadFolder(accessToken, path + '/' + subfolder['name'], subfolder)


def _downloadFolderFiles(accessToken, path, folder, parsedFileList):
    if not os.path.exists(path):
        os.makedirs(path)
    # 1 download items in this folder
    items = GirderAPI._listItemsInFolder(serverAddress, accessToken, folder['_id'])
    for item in items:
        subset = [elem for elem in parsedFileList if len(elem) == 1 and elem[0] == item['name']]
        if len(subset) > 0:
            GirderAPI._downloadItem(serverAddress, accessToken, path, item)
        
    # 2 check for subfolders
    subfolders = GirderAPI._getFolderList(serverAddress, accessToken, parentType='folder', parentId=folder['_id'])
    if subfolders:
        # 3 for each subfolder, create directory in the file system and download every item in the subfolder
        for subfolder in subfolders:
            subset = [elem[1:] for elem in parsedFileList if len(elem) > 1 and elem[0] == subfolder['name']]
            if len(subset) > 0:
                _downloadFolderFiles(accessToken, path + '/' + subfolder['name'], subfolder, subset)


def _splitPathIntoParts(path):
    allparts = []
    while 1:
        parts = os.path.split(path)
        if parts[0] == path:  # sentinel for absolute paths
            allparts.insert(0, parts[0])
            break
        elif parts[1] == path: # sentinel for relative paths
            allparts.insert(0, parts[1])
            break
        else:
            path = parts[0]
            allparts.insert(0, parts[1])
    return allparts


def _downloadDataset(accessToken, datasetName, destinationPath):
    # 1 get info of the use case collection
    useCaseCollection = GirderAPI._getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    # 2 get info of the dataset folder in that collection
    datasetFolder = GirderAPI._getFolderInfo(serverAddress, accessToken, parentType='collection', parentId=useCaseCollection['_id'], folderName=datasetName)
    # 3 download every item in the base dataset folder
    _downloadFolder(accessToken, destinationPath, datasetFolder)


def _downloadDatasetFiles(accessToken, datasetName, fileList, destinationPath):
    # 1 get info of the use case collection
    useCaseCollection = GirderAPI._getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    # 2 get info of the dataset folder in that collection
    datasetFolder = GirderAPI._getFolderInfo(serverAddress, accessToken, parentType='collection', parentId=useCaseCollection['_id'], folderName=datasetName)
    # 3 download every item in the base dataset folder
    _downloadFolderFiles(accessToken, destinationPath, datasetFolder, [_splitPathIntoParts(path) for path in fileList])


def _uploadFolder(accessToken, folderName, folderPath, parentId, parentType):
    GirderAPI._createFolder(serverAddress, accessToken, parentId, folderName, parentType=parentType)
    folderInfo = GirderAPI._getFolderInfo(serverAddress, accessToken, parentType=parentType, parentId=parentId, folderName=folderName)
    for item in os.listdir(folderPath):
        itempath = os.path.join(folderPath, item)
        if os.path.isfile(itempath):
            GirderAPI._uploadFile(serverAddress, accessToken, folderInfo['_id'], item, itempath, parentType='folder')
        else:
            _uploadFolder(accessToken, item, itempath, folderInfo['_id'], 'folder')


def _uploadNewDataset(accessToken, datasetName, datasetPath):
    useCaseCollection = GirderAPI._getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    if useCaseCollection is None:
        return False
    return _uploadFolder(accessToken, datasetName, datasetPath, useCaseCollection['_id'], parentType='collection')


def _getDatasetList(accessToken):
    useCaseCollection = GirderAPI._getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    jsonList = GirderAPI._getFolderList(serverAddress, accessToken, 'collection', useCaseCollection['_id'])
    return [element['name'] for element in jsonList]
