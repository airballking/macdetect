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


Running macdetect + the PyMACDetect UI
--------------------------------------

To run the macdetect environment, you have to start two components:

 * The macdetect daemon. There is no automated start-script yet, so
   you will have to run it youself. Also, you have to reference the
   config file to load for this daemon instance. In the base macdetect
   folder, after compiling everything, run:
   
   ``$ sudo ./bin/macdetectd -d -c macdetectd/config/default.config``
   
   The ``-d`` switch runs the server in daemon mode; you can close the
   terminal after this. All server status output will go to
   ``/var/log/syslog``. If you want to follow that output
   continuously, just tail it: ``$ tail -f /var/log/syslog``.
   
   If you leave out the ``-d`` switch, the server will not run in
   daemon mode, but in your currently open shell. This is preferable
   for just trying it out.
   
 * The PyMACDetect client. In the base macdetect folder, after
   compiling, run:
   
   ``$ ./pymacdetect/scripts/desktop.sh``
   
   After that, click ``Connect``, choose ``localhost``, and click
   ``Connect`` again. You should now see a notice form the server that
   a client has requested a connection.
   
   On the left pane of the client window, you will now see all
   monitored network interfaces of the server, their type, and their
   current monitoring status (which defaults to ``off``). Initially,
   all known MAC addresses and their evidences are requested from the
   server and show up in the right pane list. Clicking the checkmark
   on an interface on the left will enable/disable live-updates of MAC
   address detection for this interface.


Contents:

.. toctree::
   :maxdepth: 2



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
