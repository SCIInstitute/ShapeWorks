import requests 
import json

from sys import stdout
from math import ceil


def _makeRequest(requestFunction, url, params, headers, actionMessage, printError):
    response = requestFunction(url = url, params = params, headers = headers, stream = True) 
    if response.status_code == 200:
        return response
    if printError:
        print(response.status_code, 'ERROR while', actionMessage)
        from datetime import datetime
        errorLogFile = 'portal_error_log.txt'
        with open(errorLogFile, 'a') as filehandle:
            json.dump({
                'function': 'POST' if requestFunction is requests.post else 'GET' if requestFunction is requests.get else 'OTHER',
                'url': url, 
                'params': params, 
                'headers': headers, 
                'response code': response.status_code, 
                'response text': response.text,
                'action message': actionMessage, 
                'datetime': datetime.now().strftime("%m/%d/%Y, %H:%M:%S")}
                , filehandle)
            filehandle.write("\n")
        print('Please send %s to the developers for debugging' % errorLogFile)
    return None


def _makeGetRequest(url, params, headers, actionMessage, printError=True):
    return _makeRequest(requestFunction = requests.get, url = url, params = params, headers = headers, actionMessage = actionMessage, printError = printError)


def _makePostRequest(url, params, headers, actionMessage, printError=True):
    return _makeRequest(requestFunction = requests.post, url = url, params = params, headers = headers, actionMessage = actionMessage, printError = printError)


def _getAccessToken(serverAddress, apiKey):
    response = _makePostRequest(
        url = serverAddress + 'api/v1/api_key/token', 
        params = {'key': apiKey}, 
        headers = None, 
        actionMessage = 'getting access token',
        printError=False)

    return None if response is None else response.json()['authToken']['token']


def _authenticateBasicAuth(serverAddress, usernamePasswordHash):
    response = _makeGetRequest(
        url = serverAddress + 'api/v1/user/authentication', 
        params = None, 
        headers = {'Authorization': 'Basic ' + usernamePasswordHash}, 
        actionMessage = 'authenticating using basic auth',
        printError=False)
    
    return None if response is None else response.json()['authToken']['token']


def _createApiKey(serverAddress, basicAuthToken, apiKeyName):
    response = _makePostRequest(
        url = serverAddress + 'api/v1/api_key', 
        params = {'name': apiKeyName, 'scope': '[\"core.data.read\"]', 'tokenDuration': '1'}, 
        headers = {'Girder-Token': basicAuthToken}, 
        actionMessage = 'creating api key: %s' % apiKeyName)

    return None if response is None else response.json()['key']


def _getApiKey(serverAddress, basicAuthToken, apiKeyName):
    response = _makeGetRequest(
        url = serverAddress + 'api/v1/api_key', 
        params = None, 
        headers = {'Girder-Token': basicAuthToken}, 
        actionMessage = 'finding api key: %s' % apiKeyName)
    
    if response is None:
        return None
    
    for key in response.json():
        if key['name'] == apiKeyName:
            return key['key']

    return None


def _getCollectionInfo(serverAddress, accessToken, collectionName):
    actionMessage = 'finding collection: %s' % collectionName
    response = _makeGetRequest(
        url = serverAddress + "api/v1/collection", 
        params = {'text': collectionName}, 
        headers = {'Girder-Token': accessToken}, 
        actionMessage = actionMessage)
    if response is None:
        return None
    collectionList = response.json()
    if len(collectionList) == 0:
        print('ERROR', actionMessage)
        print('Found 0 collections with name = %s' % collectionName)
        return None
    collection = collectionList[0]
    if len(collectionList) > 1:
        print('WARNING', actionMessage)
        print('Found %d collections with name = %s.' % (len(collectionList), collectionName))
        print('Using collection with id = %s' % collection['_id'])
    return collection


def _getFolderInfo(serverAddress, accessToken, parentType, parentId, folderName = None):
    actionMessage = 'finding folder: %s' % folderName
    params = {'parentType': parentType, 'parentId': parentId}
    # Optional folder name parameter
    if folderName is not None:
        params['name'] = folderName
    response = _makeGetRequest(
        url = serverAddress + "api/v1/folder", 
        params = params, 
        headers = {'Girder-Token': accessToken}, 
        actionMessage = actionMessage)
    if response is None:
        return None
    folderList = response.json()
    # if folder name was not specified, return entire list of folders
    if folderName is None:
        return folderList
    if len(folderList) == 0:
        print('ERROR', actionMessage)
        print('Found 0 folders with name = %s' % folderName)
        return None
    folder = folderList[0]
    if len(folderList) > 1:
        print('WARNING', actionMessage)
        print('Found %d folders with name = %s.' % (len(folderList), folderName))
        print('Using folder with id = %s' % folder['_id'])
    return folder


def _listItemsInFolder(serverAddress, accessToken, folderId):
    actionMessage = 'listing items in folder'
    response = _makeGetRequest(
        url = serverAddress + "api/v1/item", 
        params = {'folderId': folderId}, 
        headers = {'Girder-Token': accessToken}, 
        actionMessage = actionMessage)

    return None if response is None else response.json()
    

def _downloadItem(serverAddress, accessToken, path, item):
    response = _makeGetRequest(
        url = serverAddress + 'api/v1/item/' + item['_id'] + '/download', 
        params = None,
        headers = {'Girder-Token': accessToken},
        actionMessage = 'downloading item %s' % item['name']
    )
    filename = path + '/' + item['name']
    if response is None:
        return False

    chunkSize = 1048576 # Download 1 MB at a time
    totalNumChunks = ceil(int(response.headers['Content-Length']) / chunkSize)
    chunkIndex = 0

    with open(filename, "wb") as filehandle:
        for chunk in response.iter_content(chunk_size=chunkSize):
            if not chunk:  # filter out keep-alive new chunks
                continue
            filehandle.write(chunk)

            chunkIndex += 1
            stdout.write('\r%s [%d/%d MB]' % (filename, chunkIndex, totalNumChunks))
        stdout.write('\n')
    return True
