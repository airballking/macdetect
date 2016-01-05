from nose.tools import *
import pymacdetect_ext


def setup():
    global mdcClient
    
    mdcClient = pymacdetect_ext.createClient()

def teardown():
    global mdcClient
    
    pymacdetect_ext.destroyClient(mdcClient)

def test_client_valid():
    global mdcClient
    
    return mdcClient != None
