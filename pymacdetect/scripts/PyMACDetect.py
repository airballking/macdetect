import pymacdetect


class Client:
    def __init__(self):
        self.mdcClient = pymacdetect.createClient()
    
    def __exit__(self):
        pymacdetect.destroyClient(self.mdcClient)
    
    def connect(self, strIP):
        pymacdetect.connectClient(self.mdcClient, strIP)
    
    def knownMACs(self):
        return pymacdetect.knownMACAddresses(self.mdcClient)
