import requests 
import json
import os
import getpass
import base64

from sys import stdout
from math import ceil

_API_KEY_NAME = 'python_script'
_LOGIN_FILE_NAME = 'shapeworksPortalLogin.txt'
_CONTACT_SUPPORT_STRING = 'Please contact support.'

serverAddress = 'http://cibc1.sci.utah.edu:8080/'


def _loginAndGetAccessToken():

    loginState = _loadLogin()
    
    if loginState is None:
        print('Login info not found in current directory.')
        loginState = _promptLogin()
        if loginState is None:
            print('Login failed!')
            return (None, None)

    accessToken = _getAccessToken(loginState['key'])
    if accessToken is None:
        # Retry login once
        print('Login info in current directory is invalid.')
        loginState = _promptLogin()
        if loginState is None:
            print('Login failed!')
            return (None, None)
        accessToken = _getAccessToken(loginState['key'])
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
        basicAuthToken = _authenticateBasicAuth(usernamePasswordHash)
        if basicAuthToken is None:
            print('Incorrect username or password')

    apiKey = _getApiKey(basicAuthToken)
    if apiKey is None:
        apiKey = _createApiKey(basicAuthToken)
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



def _getAccessToken(apiKey):
    apicall = serverAddress + 'api/v1/api_key/token'
    r = requests.post(url = apicall, params = {'key': apiKey}) 

    if(r.status_code == 200):
        data = r.json()
        accessToken = data['authToken']['token']
        return accessToken
    else:
        #print("ERROR Authenticating using api_key! Response code " + str(r.status_code))
        #print(r.text)
        return None


def _authenticateBasicAuth(usernamePasswordHash):
    apicall = serverAddress + "api/v1/user/authentication"
    r = requests.get(url = apicall, headers = {'Authorization': 'Basic ' + usernamePasswordHash}) 

    if(r.status_code == 200):
        data = r.json()
        authToken = data['authToken']['token']
        return authToken
    else:
        #print("ERROR Authenticating! Response code " + str(r.status_code))
        #print(r.text)
        return None

def _createApiKey(basicAuthToken):
    apicall = serverAddress + "api/v1/api_key"
    r = requests.post(url = apicall, params = {'name': _API_KEY_NAME, 'scope': '[\"core.data.read\"]', 'tokenDuration': '1'}, headers = {'Girder-Token': basicAuthToken}) 

    if(r.status_code == 200):
        apiKey = r.json()
        return apiKey['key']

    else:
        print("ERROR creating api key! Response code " + str(r.status_code), _CONTACT_SUPPORT_STRING)
        print(r.text)
        return None

def _getApiKey(basicAuthToken):
    apicall = serverAddress + "api/v1/api_key"
    r = requests.get(url = apicall, headers = {'Girder-Token': basicAuthToken}) 

    if(r.status_code == 200):
        apiKeys = r.json()
        for key in apiKeys:
            if key['name'] == _API_KEY_NAME:
                return key['key']

    else:
        print("ERROR getting list of api keys! Response code " + str(r.status_code), _CONTACT_SUPPORT_STRING)
        print(r.text)
        return None
    return None

def _getCollectionList(authToken):
    apicall = serverAddress + "api/v1/collection"
    r = requests.get(url = apicall, headers = {'Girder-Token': authToken}) 
    data = r.json()
    print(data)
    
def _getFolderList(authToken, folderName):
    apicall = serverAddress + "api/v1/folder"
    r = requests.get(url = apicall, params = {'text': folderName}, headers = {'Girder-Token': authToken}) 
    data = r.json()
    print(data)
    
def _downloadDataset(accessToken, filename):
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
