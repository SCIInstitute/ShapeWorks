from DatasetUtils import GirderConnector

def downloadDataset(filename):
    authToken = GirderConnector._authenticate()
    GirderConnector._downloadDataset(authToken, filename)
    