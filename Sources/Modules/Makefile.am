pkglib_LTLIBRARIES = Builder.la

Builder_la_SOURCES = Builder.cc Builder-CREATE.cc Builder-DESTROY.cc
Builder_la_LDFLAGS = -avoid-version -module -no-undefined -rpath ${pkglibdir} -export-symbols-regex '^nexusModuleImplementation$$'
Builder_la_LIBADD = ../libNexus.la
