#    Copyright (C) 2011 Raphters authors,
#    
#    This file is part of Raphters.
#    
#    Raphters is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Raphters is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(NCDC2014)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/webapp" "/usr/local/include" "/usr/include")
SET(RaphtersSources dispatcher request response raphters)
SET(WebAppSources webapp/webapp.c webapp/utils.c)
LINK_DIRECTORIES("/usr/local/lib" "/usr/include")

ADD_LIBRARY(raphters_dynamic SHARED ${RaphtersSources})
SET_TARGET_PROPERTIES(raphters_dynamic PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(raphters_dynamic uriparser fcgi)

ADD_LIBRARY(raphters_static STATIC ${RaphtersSources})
SET_TARGET_PROPERTIES(raphters_static PROPERTIES LINKER_LANGUAGE C)

ADD_EXECUTABLE(webapp ${WebAppSources})
TARGET_LINK_LIBRARIES(webapp cgi mysqlclient raphters_dynamic)
