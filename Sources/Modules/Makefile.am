pkglib_LTLIBRARIES = Builder.la Comms.la Debug.la Editor.la Guide.la

Builder_la_SOURCES = Builder.hh Builder.cc \
	Builder-CREATE.cc Builder-DESTROY.cc Builder-EXAMINE.cc Builder-FLAG.cc \
	Builder-GO.cc Builder-LIST.cc Builder-LOOK.cc Builder-RENAME.cc \
	Builder-SET.cc Builder-TELEPORT.cc Builder-WHO.cc

Builder_la_LDFLAGS = -avoid-version -module -no-undefined -rpath ${pkglibdir} -export-symbols-regex '^nexusModuleImplementation$$'
Builder_la_LIBADD = ../libNexus.la

Comms_la_SOURCES = Comms.hh Comms.cc \
	Comms-ECHO.cc Comms-EMIT.cc
Comms_la_LDFLAGS = -avoid-version -module -no-undefined -rpath ${pkglibdir} -export-symbols-regex '^nexusModuleImplementation$$'
Comms_la_LIBADD = ../libNexus.la

Debug_la_SOURCES = Debug.hh Debug.cc \
	Debug-DUMP.cc
Debug_la_LDFLAGS = -avoid-version -module -no-undefined -rpath ${pkglibdir} -export-symbols-regex '^nexusModuleImplementation$$'
Debug_la_LIBADD = ../libNexus.la

Editor_la_SOURCES = Editor.hh Editor.cc \
	Editor-EDIT.cc
Editor_la_LDFLAGS = -avoid-version -module -no-undefined -rpath ${pkglibdir} -export-symbols-regex '^nexusModuleImplementation$$'
Editor_la_LIBADD = ../libNexus.la

Guide_la_SOURCES = Guide.hh Guide.cc \
	Guide-HELP.cc
Guide_la_LDFLAGS = -avoid-version -module -no-undefined -rpath ${pkglibdir} -export-symbols-regex '^nexusModuleImplementation$$'
Guide_la_LIBADD = ../libNexus.la
