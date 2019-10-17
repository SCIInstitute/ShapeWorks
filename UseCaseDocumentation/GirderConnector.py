import requests 

serverAddress = 'http://155.98.59.38:8080/'

def authenticate():
    authenticationHash = 'c2hhcGV3b3JrczpzaGFwZXdvcmtzdXNlcg=='
    
    apicall = serverAddress + "api/v1/user/authentication"
    r = requests.get(url = apicall, headers = {'Authorization': 'Basic ' + authenticationHash}) 

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
    r = requests.get(url = apicall, params = {'text': filename, 'limit': 1}, headers = {'Girder-Token': authToken}) 
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