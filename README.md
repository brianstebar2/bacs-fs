bacs-fs
=======

A Distributed Fault-Tolerant File System


SYSTEM REQUIREMENTS
-------------------
BACS was developed on a system running Ubuntu 12.04 LTS.  However, BACS should
run on any POSIX-compliant Linux distribution.



BUILDING / COMPILING
--------------------
BACS requires a library for generating UUIDs.  On Ubuntu systems, the library
can be obtained by running the following:

$ sudo apt-get install uuid-dev

With the UUID library installed, BACS can be built using the following command:

$ make



RUNNING THE SERVER
------------------
The BACS server is launched by running the `bacs_server` executable.  The 
executable accepts the following arguments:

$ ./bacs_server [-h host_ip] [-p port]

 - host_ip - (OPTIONAL) IP address of another BACS server in the cluster
             (Since cluster functionality is not yet complete, this parameter
              currently does nothing.)
 - port    - (OPTIONAL) Port number the server should use; Defaults to 9930.



RUNNING THE CLIENT
------------------
The BACS client is launched by running the `bacs_client` executable.  The 
executable accepts the following arguments:

$ ./bacs_client <server_ip> <server_port>

 - server_ip   - IP address of any BACS server.
 - server_port - Port of the BACS server at <server_ip>.

After the client has started, use the HELP command to see the available commands
within the BACS shell:

>>HELP
