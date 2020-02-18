import requests 
import json
import os
import getpass
import base64

from DatasetUtils import GirderAPI

_API_KEY_NAME = 'python_script'
_LOGIN_FILE_NAME = 'shapeworksPortalLogin.txt'
_CONTACT_SUPPORT_STRING = 'Please contact support.'
_USE_CASE_DATA_COLLECTION = 'use-case-data-v0'

serverAddress = 'http://cibc1.sci.utah.edu:8080/'


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


# returns True if success
def _downloadDatasetZip(accessToken, filename):
    apicall = serverAddress + "api/v1/item"
    response = requests.get(url = apicall, params = {'folderId': '5e15245f0a02fb02ba24268a', 'name': filename}, headers = {'Girder-Token': accessToken}) 
    data = response.json()
    if(len(data) == 0):
        print('ERROR finding', filename)
        return
    data = data[0]
    id = data['_id']
    apicall = serverAddress + 'api/v1/item/' + id + '/download'
    response = requests.get(url = apicall, headers = {'Girder-Token': accessToken}, stream=True)
    if response.status_code == 200:
        chunkSize = 1048576 # Download 1 MB at a time
        totalNumChunks = ceil(int(response.headers['Content-Length']) / chunkSize)
        chunkIndex = 0

        handle = open(filename, "wb")
        for chunk in response.iter_content(chunk_size=chunkSize):
            if not chunk:  # filter out keep-alive new chunks
                continue
            handle.write(chunk)

            chunkIndex += 1
            stdout.write('\r[%d/%d MB]' % (chunkIndex, totalNumChunks))
        stdout.write('\n')
        handle.close()
        return True
    else:
        print('ERROR Downloading', filename, '! Response code', response.status_code, _CONTACT_SUPPORT_STRING)
        return False


# returns True if success
def _downloadFolder(accessToken, path, folder):
    # 1 download items in this folder
    items = GirderAPI._listItemsInFolder(serverAddress, accessToken, folder['_id'])
    failure = False
    for item in items:
        failure = not GirderAPI._downloadItem(serverAddress, accessToken, path, item) or failure
        
    # 2 check for subfolders
    subfolders = GirderAPI._getFolderInfo(serverAddress, accessToken, parentType='folder', parentId=folder['_id'])
    if subfolders:
        # 3 for each subfolder, create directory in the file system and download every item in the subfolder
        for subfolder in subfolders:
            subpath = path + '/' + subfolder['name']
            if not os.path.exists(subpath):
                os.makedirs(subpath)
            failure = not _downloadFolder(accessToken, subpath, subfolder) or failure

    return not failure


# returns True if success
def _downloadDataset(accessToken, datasetName, destinationPath):

    print('Collection: %s' % _USE_CASE_DATA_COLLECTION)
    # 1 get info of the use case collection
    useCaseCollection = GirderAPI._getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    if useCaseCollection is None:
        return False

    # 2 get info of the dataset folder in that collection
    datasetFolder = GirderAPI._getFolderInfo(serverAddress, accessToken, parentType='collection', parentId=useCaseCollection['_id'], folderName=datasetName)
    if datasetFolder is None:
        return False
    
    # 3 create destination directory in the file system and download every item in the base dataset folder
    if not os.path.exists(destinationPath):
        os.makedirs(destinationPath)

    return _downloadFolder(accessToken, destinationPath, datasetFolder)


# returns True if success, False if failed to upload 1 or more files
def _uploadFolder(accessToken, folderName, folderPath, parentId, parentType):
    if GirderAPI._createFolder(serverAddress, accessToken, parentId, folderName, parentType=parentType) is None:
        return True
    folderInfo = GirderAPI._getFolderInfo(serverAddress, accessToken, parentType=parentType, parentId=parentId, folderName=folderName)
    if folderInfo is None:
        return True

    failure = False
    for item in os.listdir(folderPath):
        itempath = os.path.join(folderPath, item)
        if os.path.isfile(itempath):
            failure = not GirderAPI._uploadFile(serverAddress, accessToken, folderInfo['_id'], item, itempath, parentType='folder') or failure
        else:
            failure = not _uploadFolder(accessToken, item, itempath, folderInfo['_id'], 'folder') or failure
    return not failure


# returns True if success, False if failure
def _uploadNewDataset(accessToken, datasetName, datasetPath):

    print('Collection: %s' % _USE_CASE_DATA_COLLECTION)

    useCaseCollection = GirderAPI._getCollectionInfo(serverAddress, accessToken, _USE_CASE_DATA_COLLECTION)
    if useCaseCollection is None:
        return False

    return _uploadFolder(accessToken, datasetName, datasetPath, useCaseCollection['_id'], parentType='collection')
