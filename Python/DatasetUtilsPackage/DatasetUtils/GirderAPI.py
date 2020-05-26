import requests 
import json

from sys import stdout
from math import ceil
from datetime import datetime

import os


_ERROR_LOG_FILE = 'portal_error_log.txt'
_MB_PER_CHUNK = 128
_CHUNK_SIZE = _MB_PER_CHUNK * 1048576 # Download 128 MB at a time

## Utility function to write to the error log file
def _writeToErrorLog(infoDict):
    with open(_ERROR_LOG_FILE, 'a') as filehandle:
        json.dump(infoDict, filehandle)
        filehandle.write("\n")
    print('Please send %s to the developers for debugging' % _ERROR_LOG_FILE)


## Returns response or writes details to log file and raises ValueError
def _makeRequest(requestFunction, url, params, headers, actionMessage, data, printError=True):
    if data:
        response = requestFunction(url = url, params = params, headers = headers, data = data, stream = True) 
    else:
        response = requestFunction(url = url, params = params, headers = headers, stream = True) 

    if response.status_code == 200:
        return response

    if printError:
        # Write debug info to file since return code was error
        print(response.status_code, 'ERROR while', actionMessage)
        _writeToErrorLog({
                'function': 'POST' if requestFunction is requests.post else 'GET' if requestFunction is requests.get else 'OTHER',
                'url': url, 
                'params': params, 
                'headers': headers, 
                'response code': response.status_code, 
                'response text': response.text,
                'action message': actionMessage, 
                'datetime': datetime.now().strftime("%m/%d/%Y, %H:%M:%S")})
    raise ValueError('Response code %s while %s', str(response.status_code), actionMessage)


## Returns response or writes details to log file and raises ValueError
def _makeGetRequest(url, params, headers, actionMessage, printError=True):
    return _makeRequest(requestFunction = requests.get, url = url, params = params, headers = headers, actionMessage = actionMessage, data = None, printError = printError)


## Returns response or writes details to log file and raises ValueError
def _makePostRequest(url, params, headers, actionMessage, data = None, printError=True):
    return _makeRequest(requestFunction = requests.post, url = url, params = params, headers = headers, actionMessage = actionMessage, data = data, printError = printError)


def getAccessToken(serverAddress, apiKey):
    response = _makePostRequest(
        url = serverAddress + 'api/v1/api_key/token', 
        params = {'key': apiKey}, 
        headers = None, 
        actionMessage = 'getting access token')

    return response.json()['authToken']['token']


def authenticateBasicAuth(serverAddress, usernamePasswordHash):
    response = _makeGetRequest(
        url = serverAddress + 'api/v1/user/authentication', 
        params = None, 
        headers = {'Authorization': 'Basic ' + usernamePasswordHash}, 
        actionMessage = 'authenticating using basic auth',
        printError=False)
    
    return response.json()['authToken']['token']


def _createApiKey(serverAddress, basicAuthToken, apiKeyName):
    response = _makePostRequest(
        url = serverAddress + 'api/v1/api_key', 
        params = {'name': apiKeyName, 'scope': '[\"core.data.read\", \"core.data.write\"]', 'tokenDuration': '1'}, 
        headers = {'Girder-Token': basicAuthToken}, 
        actionMessage = 'creating api key: %s' % apiKeyName)

    return response.json()['key']


def _getApiKey(serverAddress, basicAuthToken, apiKeyName):
    response = _makeGetRequest(
        url = serverAddress + 'api/v1/api_key', 
        params = None, 
        headers = {'Girder-Token': basicAuthToken}, 
        actionMessage = 'getting api key: %s' % apiKeyName)
    
    keyDict = response.json()
    for key in keyDict:
        if key['name'] == apiKeyName:
            return key['key']

    _writeToErrorLog(keyDict)
    raise ValueError('Failed to find %s in list of api keys.' % apiKeyName)


def _getCollectionInfo(serverAddress, accessToken, collectionName):
    actionMessage = 'finding collection: %s' % collectionName
    response = _makeGetRequest(
        url = serverAddress + "api/v1/collection", 
        params = {'text': collectionName}, 
        headers = {'Girder-Token': accessToken}, 
        actionMessage = actionMessage)
    collectionList = response.json()
    if len(collectionList) == 0:
        raise ValueError('ERROR %s. Found 0 collections.' % actionMessage)
    collection = collectionList[0]
    if len(collectionList) > 1:
        print('WARNING', actionMessage)
        print('Found %d collections.' % len(collectionList))
        print('Using collection with id = %s' % collection['_id'])
    return collection


## Returns list of folders that are children of the parent
#  The optional folderName parameter filters the results to folders with matching names
def _getFolderList(serverAddress, accessToken, parentType, parentId, folderName = None):
    params = {'parentType': parentType, 'parentId': parentId}
    # Optional folder name parameter
    if folderName is not None:
        params['name'] = folderName
    response = _makeGetRequest(
        url = serverAddress + "api/v1/folder", 
        params = params, 
        headers = {'Girder-Token': accessToken}, 
        actionMessage = 'getting folder list for: %s, %s, %s' % (parentType, parentId, folderName))
    return response.json()


## Get info for the specified folder
def _getFolderInfo(serverAddress, accessToken, parentType, parentId, folderName):
    folderList = _getFolderList(serverAddress, accessToken, parentType, parentId, folderName)

    actionMessage = 'finding folder: %s' % folderName
    if len(folderList) == 0:
        raise ValueError('ERROR %s. Found 0 folders' % actionMessage)
    folder = folderList[0]
    if len(folderList) > 1:
        print('WARNING', actionMessage)
        print('Found %d folders.' % len(folderList))
        print('Using folder with id = %s' % folder['_id'])
    return folder


def _listItemsInFolder(serverAddress, accessToken, folderId):
    actionMessage = 'listing items in folder'
    response = _makeGetRequest(
        url = serverAddress + "api/v1/item", 
        params = {'folderId': folderId}, 
        headers = {'Girder-Token': accessToken}, 
        actionMessage = actionMessage)
    return response.json()
    

## Downloads a file to path/
def _downloadItem(serverAddress, accessToken, path, item):
    response = _makeGetRequest(
        url = serverAddress + 'api/v1/item/' + item['_id'] + '/download', 
        params = None,
        headers = {'Girder-Token': accessToken},
        actionMessage = 'downloading item %s' % item['name']
    )
    filename = path + '/' + item['name']
    fileSize = int(response.headers['Content-Length'])
    chunkIndex = 0

    with open(filename, "wb") as filehandle:
        for chunk in response.iter_content(chunk_size=_CHUNK_SIZE):
            if not chunk:  # filter out keep-alive new chunks
                continue
            filehandle.write(chunk)
            chunkIndex += 1
            stdout.write('\r%s [%d/%d MB]' % (filename, chunkIndex*_MB_PER_CHUNK, ceil(fileSize / _CHUNK_SIZE * _MB_PER_CHUNK)))
        stdout.write('\n')


## Downloads a folder to path/ as a .zip
def _downloadFolder(serverAddress, accessToken, path, folderInfo):
    response = _makeGetRequest(
        url = serverAddress + 'api/v1/folder/' + folderInfo['_id'] + '/download', 
        params = None,
        headers = {'Girder-Token': accessToken},
        actionMessage = 'downloading folder %s' % folderInfo['name']
    )
    filename = path + '/' + folderInfo['name'] + '.zip'
    chunkIndex = 0

    with open(filename, "wb") as filehandle:
        for chunk in response.iter_content(chunk_size=_CHUNK_SIZE):
            if not chunk:  # filter out keep-alive new chunks
                continue
            filehandle.write(chunk)

            chunkIndex += 1
            stdout.write('\r%s [%d MB]' % (filename, chunkIndex*_MB_PER_CHUNK))
        stdout.write('\n')


def _createFolder(serverAddress, accessToken, parentId, name, parentType='folder'):
    actionMessage = 'creating folder %s' % name
    response = _makePostRequest(
        url = serverAddress + 'api/v1/folder', 
        params = {'parentId': parentId, 'name': name, 'parentType': parentType},
        headers = {'Girder-Token': accessToken},
        actionMessage = actionMessage
    )


def _uploadFile(serverAddress, accessToken, parentId, name, path, parentType='folder'):
    filesize = os.stat(path).st_size

    # Create an upload-in-progress
    response = _makePostRequest(
        url = serverAddress + 'api/v1/file', 
        params = {'parentId': parentId, 'name': name, 'parentType': parentType, 'size': filesize, 'mimeType': 'application/octet-stream'},
        headers = {'Girder-Token': accessToken},
        actionMessage = 'Creating upload-in-progress for %s' % name
    )
    uploadid = response.json()['_id']
    offset = 0
    chunkIndex = 0

    with open(path, 'rb') as filehandle:
        stdout.write('\r%s [%d/%d MB]' % (path, chunkIndex*_MB_PER_CHUNK, ceil(filesize / _CHUNK_SIZE * _MB_PER_CHUNK)))
        # TODO clean up this while loop
        while(True):
            chunk = filehandle.read(_CHUNK_SIZE)
            if chunk:
                response = _makePostRequest(
                        url = serverAddress + 'api/v1/file/chunk', 
                        params = {'uploadId': uploadid, 'offset': offset},
                        headers = {'Girder-Token': accessToken},
                        actionMessage = 'Uploading chunk %d for %s' % (chunkIndex, name),
                        data = chunk
                    )
                offset += _CHUNK_SIZE
                chunkIndex += 1
                stdout.write('\r%s [%d/%d MB]' % (path, chunkIndex*_MB_PER_CHUNK, ceil(filesize / _CHUNK_SIZE * _MB_PER_CHUNK)))
            else:
                break
        stdout.write('\n')
