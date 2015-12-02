from PyMACDetect import Client

cliClient = Client()

ip = "127.0.0.1"
#ip = "192.168.178.26"

if cliClient.connect(ip):
    print cliClient.knownMACs()
else:
    print "Unable to connect to '" + ip + "'"
