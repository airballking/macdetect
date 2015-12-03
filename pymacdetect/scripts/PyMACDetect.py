import pymacdetect


class Client:
    def __init__(self):
        self.mdcClient = pymacdetect.createClient()
    
    def __exit__(self):
        pymacdetect.destroyClient(self.mdcClient)
    
    def connect(self, strIP):
        return pymacdetect.connectClient(self.mdcClient, strIP)
    
    def knownMACs(self):
        return pymacdetect.knownMACAddresses(self.mdcClient)
    
    def info(self):
        return pymacdetect.info(self.mdcClient);
    
    def enableStream(self, strDeviceName):
        return pymacdetect.enableStream(self.mdcClient, strDeviceName);
    
    def disableStream(self, strDeviceName):
        return pymacdetect.disableStream(self.mdcClient, strDeviceName);
