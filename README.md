# ISU NCDC 2014 WebApp

This is the web app for the Spring 2014 ISU National Cyber Defense Compeition.  It is designed to be intentially insecure and should not be used in any production system.

## Prereqs
* Originally developed on Ubuntu 12.04
* sudo apt-get update
* sudo apt-get install cmake
* sudo apt-get install gcc 
* sudo apt-get install build-essential
* sudo apt-get install libfcgi-dev

## Development
* sudo apt-get install git-core
* git clone https://github.com/benjholla/Raphters.git
* cd build
* cmake ../
* make

### Recommended IDE is Eclipse for C/C++
[http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/keplersr1](http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/keplersr1)

[http://www.cmake.org/Wiki/Eclipse_CDT4_Generator](http://www.cmake.org/Wiki/Eclipse_CDT4_Generator)

Requires Java JRE:

* sudo apt-get install openjdk-7-jre

To generate Eclipse project files run:

* cd build
* cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../

## Deployment
* sudo apt-get install lighttpd
* sudo mkdir /var/fastcgi
* sudo ln -s /etc/lighttpd/conf-available/10-fastcgi.conf /etc/lighttpd/conf-enabled/10-fastcgi.conf

### Pushing Code to Production
* cd build
* ./deploy

## Testing
Go to [http://localhost/webapp](http://localhost/webapp)

## Framework Background
This project is built on Raphters, a web framework for C based on the rapht architectural pattern (see RAPHT).

Q: Why would you want to build a web application in C? Didn't that idea die out 
with CGI?

A: Good question. C is fast and fun, when you use well-tested code it can be secure too. It also has a low memory foot-print. The aim of the project is that all of the usual functionality that you have in other frameworks (cookie handling, session management, templating etc) will be implemented via loosly coupled components with clean APIs.
