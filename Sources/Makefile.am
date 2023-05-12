noinst_LTLIBRARIES = libNexus.la

libNexus_la_SOURCES = \
	Thing.cc Thing-Verbs.cc Thing-IO.cc Thing-Types.cc \
	Thing-Props.cc Thing-Flags.cc Thing-Status.cc Container.cc Actor.cc Player.cc \
	Variable.cc Zone.cc Limbo.cc Database.cc \
	Universe.cc Universe-Schema.cc Universe-Create.cc \
	Parsers.cc Command.cc Builtins.cc Builtins-COMMANDS.cc \
	Builtins-CREATE.cc Builtins-DESTROY.cc Builtins-DUMP.cc Builtins-ECHO.cc \
	Builtins-EDIT.cc Builtins-EMIT.cc Builtins-EXAMINE.cc Builtins-FLAG.cc \
	Builtins-GO.cc Builtins-HELP.cc Builtins-LIST.cc Builtins-LOOK.cc \
	Builtins-QUIT.cc Builtins-RENAME.cc Builtins-SET.cc Builtins-TELEPORT.cc

libNexus_la_LDFLAGS = -avoid-version
libNexus_la_LIBADD = @JANSSON_LIBS@ @FLUX_LIBS@
