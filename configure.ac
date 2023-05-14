
## Copyright 2023 Mo McRoberts.
##
##  Licensed under the Apache License, Version 2.0 (the "License");
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.

AC_INIT([Nexus],[0.0])
AC_PREFIX_DEFAULT([/opt/nexus])

## Prevent autoconf from supplying undesirable defaults if not already set
: ${CFLAGS=""}
: ${CXXFLAGS=""}

## M4 macros are in a subdirectory named "M4"
AC_CONFIG_MACRO_DIR([M4])
## Build scripts are in a subdirectory named "BuildScripts"
AC_CONFIG_AUX_DIR([BuildScripts])

m4_ifndef([PKG_PREREQ],
	[m4_fatal([must install pkg-config 0.29 or later before running autoconf/autogen])])

AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE

AS_VAR_APPEND([AM_CPPFLAGS],[" -I\${top_srcdir}/PrivateHeaders"])
AS_VAR_APPEND([AM_CPPFLAGS],[" -I\${top_srcdir}/Headers"])

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CXXFLAGS])

AC_PROG_CXX([clang++ g++ c++])

if test "$GXX" = "yes" ; then
	AS_VAR_APPEND([AM_CPPFLAGS],[" -W -Wall -Werror"])
	AS_VAR_APPEND([AM_CXXFLAGS],[" -O0 -g"])
fi


LT_INIT
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([JANSSON],[jansson])

AC_CHECK_FUNCS([dlopen],,[
	AC_CHECK_LIB([dl],[dlopen])
])

AS_VAR_SET([FLUX_CFLAGS],["-I\${top_srcdir}/WARP/Headers"])
AS_VAR_SET([FLUX_LIBS],["\${top_builddir}/WARP/Sources/Flux/libWARP-Flux.la"])
AC_SUBST([FLUX_CFLAGS])
AC_SUBST([FLUX_LIBS])
AC_CONFIG_SUBDIRS([WARP])

AS_VAR_APPEND([AM_CPPFLAGS],[" \${JANSSON_CFLAGS}"])
AS_VAR_APPEND([AM_CPPFLAGS],[" \${FLUX_CFLAGS}"])


AC_CONFIG_FILES([
Makefile
Sources/Makefile
Sources/Modules/Makefile
])

AC_CONFIG_FILES([debug-builder:debug-builder.sh.in],[chmod +x debug-builder])

AC_OUTPUT
