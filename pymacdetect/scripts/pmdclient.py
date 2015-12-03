from PyMACDetect import Client

cliClient = Client()

ip = "127.0.0.1"
#ip = "192.168.178.26"
#ip = "192.168.178.25"

if cliClient.connect(ip):
    print cliClient.knownMACs()
    cliClient.enableStream("eth1")
    
    while True:
        info = cliClient.info()
        
        if info:
            print info
else:
    print "Unable to connect to '" + ip + "'"
