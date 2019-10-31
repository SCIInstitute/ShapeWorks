import requests 

serverAddress = 'http://cibc1.sci.utah.edu:8080/'

def authenticate():
    python_api_key = 'EJkKKEU6XmYdlpRMQswfcbXDNH4z430Z5a5npigU'
    apicall = serverAddress + "api/v1/api_key/token"
    r = requests.post(url = apicall, params = {'key': python_api_key}) 

    if(r.status_code == 200):
        data = r.json()
        authToken = data['authToken']['token']
        #print('Acquired auth token: ', authToken)
        return authToken
    else:
        print("ERROR Authenticating! Response code " + str(r.status_code))
        return None

def getCollectionList(authToken):
    apicall = serverAddress + "api/v1/collection"
    r = requests.get(url = apicall, headers = {'Girder-Token': authToken}) 
    data = r.json()
    print(data)
    
def getFolderList(authToken, folderName):
    apicall = serverAddress + "api/v1/folder"
    r = requests.get(url = apicall, params = {'text': folderName}, headers = {'Girder-Token': authToken}) 
    data = r.json()
    print(data)
    
def downloadDataset(authToken, filename):
    apicall = serverAddress + "api/v1/item"
    r = requests.get(url = apicall, params = {'folderId': '5da8be03a728853200d3d5c0', 'name': filename}, headers = {'Girder-Token': authToken}) 
    data = r.json()
    if(len(data) == 0):
        print('ERROR finding', filename);
        return
    data = data[0]
    id = data['_id']
    apicall = serverAddress + 'api/v1/item/' + id + '/download'
    r = requests.get(url = apicall, headers = {'Girder-Token': authToken})
    if(r.status_code == 200):
        open(filename, 'wb').write(r.content)
        print('Successfully downloaded', filename, 'from the Girder instance.')
    else:
        print('ERROR Downloading', filename, '! Response code', r.status_code)
        return
    
def downloadUseCaseData(filename):
    authToken = authenticate()
    #getCollectionList(authToken)
    #getFolderList(authToken, 'EllipsoidFolder')
    downloadDataset(authToken, filename)