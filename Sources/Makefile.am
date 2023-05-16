SUBDIRS = . Modules

lib_LTLIBRARIES = libNexus.la

libNexus_la_SOURCES = \
	Thing.cc Thing-Verbs.cc Thing-IO.cc Thing-Types.cc \
	Thing-Props.cc Thing-Flags.cc Thing-Status.cc Container.cc Actor.cc \
	Player.cc Variable.cc Zone.cc Limbo.cc Database.cc \
	Universe.cc Universe-Schema.cc Universe-Create.cc Universe-Modules.cc \
	Universe-Time.cc Parsers.cc Command.cc Tokens.cc Builtins.cc Module.cc \
	ModuleImplementation.cc \
	Builtins-COMMANDS.cc Builtins-ENABLE.cc Builtins-DISABLE.cc \
	Builtins-MODULES.cc Builtins-QUIT.cc

libNexus_la_LDFLAGS = -avoid-version -no-undefined -rpath ${libdir}
libNexus_la_LIBADD = @JANSSON_LIBS@ @FLUX_LIBS@
