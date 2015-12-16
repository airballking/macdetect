from nose.tools import *
import pymacdetect


def setup():
    global mdcClient
    
    mdcClient = pymacdetect.createClient()

def teardown():
    global mdcClient
    
    pymacdetect.destroyClient(mdcClient)

def test_client_valid():
    global mdcClient
    
    return mdcClient != None
