AMLOCAL_AMFLAGS = -I M4

SUBDIRS = @subdirs@ Sources .

bin_PROGRAMS = nexus-builder nexus-createdb nexus-migratedb

nexus_builder_SOURCES = builder.cc
nexus_builder_LDADD = Sources/libNexus.la
nexus_createdb_SOURCES = createdb.cc
nexus_createdb_LDADD = Sources/libNexus.la
nexus_migratedb_SOURCES = migratedb.cc
nexus_migratedb_LDADD = Sources/libNexus.la
