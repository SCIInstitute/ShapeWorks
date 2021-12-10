import base64
import getpass
import json
import os
from swcc.api import swcc_session
from swcc.models import Dataset

_LOGIN_FILE_NAME = 'shapeworksPortalLogin.txt'

def saveLogin(loginState):
    with open(_LOGIN_FILE_NAME, 'w') as outfile:
        json.dump(loginState, outfile)

def loadLogin():
    if not os.path.exists(_LOGIN_FILE_NAME):
        return None
    with open(_LOGIN_FILE_NAME) as json_file:
        loginState = json.load(json_file)
        return loginState
    
def getLoginDetails():
    
    print('')
    username = input("Username: ")
    password = getpass.getpass("Password: ")
    combined = username + ':' + password
    PasswordHash = base64.b64encode(password.encode()).decode("ascii")


    loginState = {'username': username, 'PasswordHash': PasswordHash}
    saveLogin(loginState)


def login():
    if getLoginDetails is not None:
        loginDetails = loadLogin()
        username = loginDetails['username']
        passwordHash = loginDetails['PasswordHash']
        password = base64.b64decode(passwordHash)
        return username,password

# getLoginDetails()

username,password = login()

#api as a context manager
with swcc_session()  as session:
    token = session.login(username, password)
    session = swcc_session(token=token).__enter__()
    print([(d.id, d.name) for d in Dataset.list()])