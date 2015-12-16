.. macdetect documentation master file, created by
   sphinx-quickstart on Wed Dec 16 20:20:35 2015.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to macdetect's documentation!
=====================================

macdetect is a software suite that allows you to detect and monitor
the presence of individual MAC addresses (read: network connected
devices) on any network you have access to.

The principle architecture includes

 * A daemon (`macdetectd <macdetectd.rst>`__): This daemon runs on a
   network continuously and monitors the MAC addresses it sees. From
   time to time it sends out broadcast pings to ask for changes in the
   network, and the rest of the time listens to all other traffic to
   see who enters, and who leaves the network.

 * Client libraries (`client <client.rst>`__): The most basic client
   library included in macdetect is a C++ library that can communicate
   to any macdetectd instance over the network. You can link your
   programs against this library, and connect to servers, request
   their network devices, and switch on and off MAC address
   notifications for them individually.

 * Python language bindings (`pymacdetect <pymacdetect.rst>`__): This
   is a wrapper class around the C++ client libraries, using Python's
   foreign language interface. Its usage is basically the same as for
   the C++ libraries, but using native Python classes, exceptions, and
   datatypes.

 * Utility classes (`utils <utils.rst>`__): A couple of utility
   classes for network communication and value representation commonly
   used by both, the daemon and the C++ client libraries.


Contents:

.. toctree::
   :maxdepth: 2



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

