import pymacdetect

mdcliClient = pymacdetect.createClient()

print pymacdetect.connectClient(mdcliClient, "192.168.178.22")#127.0.0.1")
print pymacdetect.knownMACAddresses(mdcliClient)

pymacdetect.destroyClient(mdcliClient)
