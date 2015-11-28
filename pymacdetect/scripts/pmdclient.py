from PyMACDetect import Client

cliClient = Client()

cliClient.connect("127.0.0.1")
print cliClient.knownMACs()
