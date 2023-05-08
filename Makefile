OUT = nexus-builder
OBJ = builder.o

MKDBOUT = nexus-createdb
MKDBOBJ = createdb.o

MIGRATEOUT = nexus-migratedb
MIGRATEOBJ = migratedb.o

LIBOUT = libnexus.a
LIBOBJ = Thing.o Thing-Verbs.o Thing-IO.o Thing-Types.o \
	Thing-Props.o Thing-Flags.o Thing-Status.o Container.o Actor.o Player.o \
	Variable.o Zone.o Limbo.o Database.o \
	Universe.o Universe-Schema.o Universe-Create.o \
	Parsers.o Command.o Builtins.o Builtins-COMMANDS.o \
	Builtins-CREATE.o Builtins-DESTROY.o Builtins-DUMP.o Builtins-EDIT.o \
	Builtins-EXAMINE.o Builtins-FLAG.o Builtins-GO.o Builtins-HELP.o \
	Builtins-LIST.o Builtins-LOOK.o Builtins-QUIT.o Builtins-RENAME.o \
	Builtins-SET.o Builtins-TELEPORT.o

CPPFLAGS = -O0 -g -W -Wall -IHeaders -IWARP/Headers `pkg-config --cflags jansson`
LIBS = WARP/Sources/Flux/.libs/libWARP-Flux.a `pkg-config --libs jansson`

all: $(LIBOUT) $(MKDBOUT) $(MIGRATEOUT) $(OUT)

$(OUT): $(OBJ) $(LIBOUT)
	$(CXX) -o $(OUT) $(OBJ) $(LIBOUT) $(LIBS)

$(MKDBOUT): $(MKDBOBJ) $(LIBOUT)
	$(CXX) -o $(MKDBOUT) $(MKDBOBJ) $(LIBOUT) $(LIBS)

$(MIGRATEOUT): $(MIGRATEOBJ) $(LIBOUT)
	$(CXX) -o $(MIGRATEOUT) $(MIGRATEOBJ) $(LIBOUT) $(LIBS)

$(LIBOUT): $(LIBOBJ)
	$(AR) rcs $@ $+

clean:
	rm -f $(OUT) $(OBJ)
	rm -f $(MKDBOUT) $(MKDBOBJ)
	rm -f $(MIGRATEOUT) $(MIGRATEOBJ)
	rm -f $(LIBOUT) $(LIBOBJ)
