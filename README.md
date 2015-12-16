MAC Detect host detection monitor [![Build Status](https://travis-ci.org/fairlight1337/macdetect.svg?branch=master)](https://travis-ci.org/fairlight1337/macdetect) [![Coverage Status](https://coveralls.io/repos/fairlight1337/macdetect/badge.svg?branch=master&service=github)](https://coveralls.io/github/fairlight1337/macdetect?branch=master) [![Documentation Status](https://readthedocs.org/projects/macdetect/badge/?version=latest)](http://macdetect.readthedocs.org/en/latest/?badge=latest)
=================================

MAC Detect is a software framework for monitoring MAC address
residence in networks. It consists of three major components, being

  * `macdetectd`: A daemon that monitors any number of networks
    (defined by a black- or whitelist) for changes in the MAC
    addresses being present and active.

  * `macdetect-client`: A command line client and client API in C++
    that allows to a) query a running macdetectd server instance for
    its current status, and b) receiving an event stream of changes in
    the MAC addresses on the server's networks.

  * `pymacdetect`: A python extension that wraps the C++ client API
    for talking to a macdetectd server, allowing any Python program to
    make use of the MAC detection mechanisms.

The library and its adjacent software are highly WIP right now, so not
everything might work already as expected. The parts that definitely
work are:

  * Monitoring of any number of MAC based networks with `macdetectd`.
    
  * Detecting whether a new MAC appeared on a network, or disappeared
    from it.
    
  * Dynamic switching of network adapters: `macdetectd` accomodates
    for removed or newly added network interfaces on its host machine,
    notifying its clients about changes in its environment.
    
  * Switching on/off streams of MAC changes for clients: A client can
    decide whether they want to receive live updates from a server, or
    rather prefer to call the update query themselves.
    
  * Connecting, disconnecting, and requesting all known MAC addresses
    from a server using both, the C++ API, and the Python interface.

The library commits are monitored by a Travis CI instance and I try to
keep it clean and up-to-date if bugs arise. I might restructure some
internals every now and then, so brace yourself for that. The
principle interface for clients shouldn't change that much, though
(maybe function names, but the basic functionality will stay the
same).

The whole code is licensed under the GPL 2.0 license. See any header
files, source files, or the LICENSE file for more info. The copyright
belongs to me. If you want to do anything with this software that is
not compatible with GPL 2.0, message me.
