# ISU NCDC 2014 WebApp

This is the web app for the Spring 2014 ISU National Cyber Defense Competition.  It is designed to be intentionally insecure and should not be used in any production system.

## Prereqs
* Developed and tested on Ubuntu 12.04 LTS
* sudo apt-get update
* sudo apt-get install cmake
* sudo apt-get install gcc 
* sudo apt-get install build-essential
* sudo apt-get install libfcgi-dev
* sudo apt-get install libunwind7-dev
* sudo apt-get install liburiparser-dev
* sudo apt-get install mysql-server
* sudo apt-get install libmysqlclient-dev
* sudo apt-get install lighttpd
* sudo apt-get install cgilib

## Setup
* sudo ln -s /etc/lighttpd/conf-available/10-fastcgi.conf /etc/lighttpd/conf-enabled/10-fastcgi.conf

## Development
* sudo apt-get install git-core
* git clone https://github.com/benjholla/NCDC2014.git
* cd NCDC2014
* ./run-local
* Go to [http://localhost:8080/](http://localhost:8080/)

### Recommended IDE is Eclipse for C/C++
[http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/keplersr1](http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/keplersr1)

[http://www.cmake.org/Wiki/Eclipse_CDT4_Generator](http://www.cmake.org/Wiki/Eclipse_CDT4_Generator)

Requires Java JRE:
* sudo apt-get install openjdk-7-jre

To generate Eclipse project files run:
* cd NCDC2014
* mkdir -p build
* cd build
* cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../
* Inside Eclipse, File->Import->General->Existing Projects into Workspace

## Deployment
* cd NCDC2014
* ./deploy
* Go to [http://localhost/](http://localhost/)

## Framework Background
> This project is built on Raphters, a web framework for C based on the rapht architectural pattern (see RAPHT).
>     
>     Q: Why would you want to build a web application in C? Didn't that idea die out with CGI?
>     
>     A: Good question. C is fast and fun, when you use well-tested code it can be secure too. It also has a low memory foot-print. The aim of the project is that all of the usual functionality that you have in other frameworks (cookie handling, session management, templating etc) will be implemented via loosly coupled components with clean APIs.

## Attributions
This project uses some works from the [Noun Project](http://thenounproject.com/) that require attributions.
* Shield designed by Nate Eul from the Noun Project
* Conversation designed by Gregory Radek from the Noun Project
* Network designed by iconoci from the Noun Project 
* Virus designed by Anu Prasadh Raja from the Noun Project
* Meeting designed by Slava Strizh from the Noun Project
* Cloud designed by Jan-Christoph Borchardt from the Noun Project

The project also makes use of [Twitter Bootstrap](http://getbootstrap.com/)
