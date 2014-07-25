# General:

This project is a Heterogeneous Network Simulation Framework. It provides a 
web-based front-end allowing users to configure a visual representation of a 
Heterogeneous Network, and it will attempt to optimize it. The optimization 
happens by converting the visual representation into a linear optimization 
problem. The client then makes a request to the server serving the web page 
to solve the problem using a PHP script that makes calls to our Solver library.
Our solver library is written in C++ and we use the SWIG tool (www.swig.org) in order to 
generate bindings for the Solver that can be called via PHP. 

The front end we have provided is a standard HTML/CSS web page using Javascript
and jQuery for interactions and the D3.js (www.d3js.org) library for visualization.
The front end is functional but was also intended to be a model for other types 
of front-ends that could be attached to our solver. It is possible that with 
refactoring the back-end could be made more robust and provide a greater 
service to potential front-ends. Our server setup includes an apache2
server and PHP5.

This project was intended as somewhat of a proof-of-concept. It was completed
with time constraints; the provided front end and some parts of the solver 
could use refactoring. 

# Installation Requirements:

We have provided a short PHP script to function as the back-end. In order for
the Solver to be accessable to be PHP, the shared library generated using "make"
must be loaded by the PHP script. This can be done in a number of ways, however
on our test server we have done so by adding this line to /etc/php5/apache2/php.ini:

extension="solver.so"


Running "make install" after running make as root will move the shared library
to the default PHP extension location. This will load the Solver library for 
ALL PHP applications, which is not necessarily ideal. 

The amount of information that needs to be sent grows exponentially with the size
of the system. It is possible that for a larger system, you will exceed the PHP
post limit. The following line in /etc/php5/apache2/php.ini
is what we used to increase the post size on our server; there may be other ways 
to increase it dynamically:

post_max_size=1000M


# Solver Specification/Usage:


# www:

The www directory contains all of the web content that we have created. 
index.html, process.php, and everything contained in www/js and www/css are 
relevant. Everything else is test code and proof of concept. We use several 
web frameworks to create a more visual design for our website. Thanks to Sidr,
holo-web, jQuery, and D3.js for helping us to develop a more visual and 
intuitive experience.



# Debugging:

Assuming the correct system setup, all PHP error will go to the standard 
PHP log in /var/log. All apache errors will go to /var/log/apache2. If any sort 
of system error or segmentation fault were to happen during the process of 
running the shared library, it would also be logged in /var/log/apache2/error.log
In the Solver, we built in a debug mode which can be activated by uncommenting the 
"DEBUG" variable in the makefile and recompiling the shared library. This will
print a veriety of useful information such as matrix states. These files can 
get rather large, however, and they significantly slow the solver, so they 
should be disabled unless a problem is encountered. For large problems, enabling
debug will actually cause PHP to hit its internal timeout and kill itself. In 
the event of a segmentation fault a mock PHP script can be created and ran using
the terminal. This will allow the use of valgrind or other tools to debug. 
This will require the same setup as above in the php.ini file, except in 
/etc/phpt5/cli/php.ini instead.


# Developers
This project was developed in the Summer of 2014, during the Clemson Data Intensive Computing REU by
Tyler Allen of Western Carolina University and Matthew Leeds of The University
of Alabama, with Dr. James Martin as our mentor. 

