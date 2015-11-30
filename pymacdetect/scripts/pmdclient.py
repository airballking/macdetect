from PyMACDetect import Client

cliClient = Client()

cliClient.connect("192.168.178.26")#"127.0.0.1")
print cliClient.knownMACs()
