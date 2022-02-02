import requests 
import json
import os
import getpass
import base64

from DatasetUtils import GirderAPI

_API_KEY_NAME = 'python_script'
_LOGIN_FILE_NAME = 'shapeworksPortalLogin.txt'
_CONTACT_SUPPORT_STRING = 'Please contact support: shapeworks-dev-support@sci.utah.edu'
_VERSION = 'v2'
_USE_CASE_DATA_COLLECTION = 'use-case-data-%s' % _VERSION

serverAddress = 'https://girder.shapeworks-cloud.org/'


def printDataPortalWelcome():
    print(' _____    ___      .     ')
    print('|     |  /   \    / \    ')
    print('| ShapeWorks Portal  \   ')
    print('|_____|  \___/  /_____\  ')
    print()


def login(loginState):
    if loginState is None:
        # interactive login mode
        loginState, accessToken = _loginAndGetAccessToken()
    else:
        # login using provided credentials
        if not _verifyLoginState(loginState):
            raise ValueError('Invalid login state')
        accessToken = GirderAPI.getAccessToken(loginState['key'])
    if accessToken is None:
        raise ValueError('Unable to get access token. %' % _CONTACT_SUPPORT_STRING)
    return accessToken
    

## Interactively prompts the user to log in
def _loginAndGetAccessToken():

    loginState = _loadLogin()
    
    if loginState is None:
        print('Login info not found in current directory.')
        loginState = _promptLogin()
        if loginState is None:
            print('Login failed!')
            return (None, None)
    print(f"Using Server: {serverAddress}")
    accessToken = GirderAPI.getAccessToken(serverAddress, loginState['key'])
    if accessToken is None:
        # Retry login once
        print('Login info in current directory is invalid.')
        loginState = _promptLogin()
        if loginState is None:
            print('Login failed!')
            return (None, None)
        accessToken = GirderAPI.getAccessToken(serverAddress, loginState['key'])
    return (loginState, accessToken)


def _promptLogin():
    username = ""
    if 'SW_PORTAL_LOGIN' in os.environ:
        print('Using portal login from $SW_PORTAL_LOGIN')
        combined = os.environ['SW_PORTAL_LOGIN']
        username = combined.split(":")[0]
        usernamePasswordHash = base64.b64encode(combined.encode()).decode("ascii")
        try:
            basicAuthToken = GirderAPI.authenticateBasicAuth(serverAddress, usernamePasswordHash)
        except ValueError as e:
            print('SW_PORTAL_LOGIN failed.')
            quit()
    else:
        print('New ShapeWorks Portal users: Register an account at ' + serverAddress + '#?dialog=register')
        print('\nReturning ShapeWorks Portal users: Enter your username and password:')
        basicAuthToken = None
        while basicAuthToken is None:
            print('')
            username = input("Username: ")
            password = getpass.getpass("Password: ")
            combined = username + ':' + password
            usernamePasswordHash = base64.b64encode(combined.encode()).decode("ascii")
            try:
                basicAuthToken = GirderAPI.authenticateBasicAuth(serverAddress, usernamePasswordHash)
            except ValueError as e:
                print('Incorrect username or password.')
    try:
        apiKey = GirderAPI.getApiKey(serverAddress, basicAuthToken, _API_KEY_NAME)
    except ValueError as e:
        apiKey = GirderAPI.createApiKey(serverAddress, basicAuthToken, _API_KEY_NAME)

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


def _downloadFolder(accessToken, path, folder, parsedFileList = None):
    if not os.path.exists(path):
        os.makedirs(path)
    # 1 download items in this folder
    items = GirderAPI.listItemsInFolder(serverAddress, accessToken, folder['_id'])
    for item in items:
        subset = None if not parsedFileList else [elem for elem in parsedFileList if len(elem) == 1 and elem[0] == item['name']]
        if subset is None or len(subset) > 0:
            GirderAPI.downloadItem(serverAddress, accessToken, path, item)
        
    # 2 for each subfolder, create directory in the file system and download every item in the subfolder
    for subfolder in GirderAPI.getFolderList(serverAddress, accessToken, parentType='folder', parentId=folder['_id']):
        subset = None if not parsedFileList else [elem[1:] for elem in parsedFileList if len(elem) > 1 and elem[0] == subfolder['name']]
        if subset is None or len(subset) > 0:
            _downloadFolder(accessToken, path + '/' + subfolder['name'], subfolder, subset)


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


def downloadDataset(accessToken, datasetName, destinationPath, fileList = None):
    # 1 get info of the use case collection
    useCaseCollection = GirderAPI.getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    # 2 get info of the dataset folder in that collection
    datasetFolder = GirderAPI.getFolderInfo(serverAddress, accessToken, parentType='collection', parentId=useCaseCollection['_id'], folderName=datasetName)
    # 3 download every item in the base dataset folder
    if fileList:
        _downloadFolder(accessToken, destinationPath, datasetFolder, [_splitPathIntoParts(path) for path in fileList])
    else:
        _downloadFolder(accessToken, destinationPath, datasetFolder)


def downloadDatasetZip(accessToken, datasetName, destinationPath):
    # 1 get info of the use case collection
    useCaseCollection = GirderAPI.getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    # 2 get info of the dataset folder in that collection
    datasetFolder = GirderAPI.getFolderInfo(serverAddress, accessToken, parentType='collection', parentId=useCaseCollection['_id'], folderName=datasetName)
    # 3 download the dataset folder
    GirderAPI.downloadFolder(serverAddress, accessToken, path=destinationPath, folderInfo=datasetFolder)


def _uploadFolder(accessToken, folderName, folderPath, parentId, parentType, existingFileList = []):
    try:
        folderInfo = GirderAPI.getFolderInfo(serverAddress, accessToken, parentType=parentType, parentId=parentId, folderName=folderName)
        print('Skip creating folder ' + folderPath)
    except:
        GirderAPI.createFolder(serverAddress, accessToken, parentId, folderName, parentType=parentType)

    folderInfo = GirderAPI.getFolderInfo(serverAddress, accessToken, parentType=parentType, parentId=parentId, folderName=folderName)
    for item in os.listdir(folderPath):
        # must use / here to match girder file paths
        itempath = os.path.join(folderPath, item)
        if os.path.isfile(itempath):
            if itempath in existingFileList:
                print('Skip uploading ' + itempath)
            else:
                GirderAPI.uploadFile(serverAddress, accessToken, folderInfo['_id'], item, itempath, parentType='folder')
        else:
            _uploadFolder(accessToken, item, itempath, folderInfo['_id'], 'folder', existingFileList)


## Uploads dataset to the data portal without overwriting anything.
## To replace files, delete them on the data portal before uploading.
def uploadDataset(accessToken, datasetName, datasetPath):
    useCaseCollection = GirderAPI.getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    if useCaseCollection is None:
        return False
    
    existingFileList = []
    if datasetName in getDatasetList(accessToken):
        # Get list of existing files to avoid overriding
        existingFileList = [os.path.join(datasetPath, f) for f in getFileList(accessToken, datasetName)]
    return _uploadFolder(accessToken, datasetName, datasetPath, useCaseCollection['_id'], parentType='collection', existingFileList=existingFileList)


def getDatasetList(accessToken):
    useCaseCollection = GirderAPI.getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    jsonList = GirderAPI.getFolderList(serverAddress, accessToken, 'collection', useCaseCollection['_id'])
    return [element['name'] for element in jsonList]


def getFileList(accessToken, datasetName):
    useCaseCollection = GirderAPI.getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    datasetFolder = GirderAPI.getFolderInfo(serverAddress, accessToken, parentType='collection', parentId=useCaseCollection['_id'], folderName=datasetName)

    # Add items and folders of base directory
    items = GirderAPI.listItemsInFolder(serverAddress, accessToken, datasetFolder['_id'])
    subfolders = [(sf, '') for sf in GirderAPI.getFolderList(serverAddress, accessToken, parentType='folder', parentId=datasetFolder['_id'])]
    itemPaths = [element['name'] for element in items]

    # Iterate through subfolders to get all the items
    while len(subfolders) > 0:
        subfolder = subfolders.pop(0)
        path = subfolder[1] + subfolder[0]['name'] + os.path.sep
        items = GirderAPI.listItemsInFolder(serverAddress, accessToken, subfolder[0]['_id'])
        itemPaths += [path + element['name'] for element in items]
        subfolders = [(sf, path) for sf in GirderAPI.getFolderList(serverAddress, accessToken, parentType='folder', parentId=subfolder[0]['_id'])] + subfolders

    return itemPaths

