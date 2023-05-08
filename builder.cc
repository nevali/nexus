#include <cstdio>
#include <cstring>
#include <cerrno>

#include <jansson.h>

#include "Nexus/Player.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"

int
main(int argc, char **argv)
{
	char cmdbuf[256];
	Nexus::Database *db;
	Nexus::Universe *universe;
	Nexus::Player *player;

	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s DB-PATH\n", argv[0]);
		return 1;
	}
	db = Nexus::Database::open(argv[1]);
	if(!db)
	{
		fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
		return 2;
	}
	if(db->version() < Nexus::Database::VERSION)
	{
		fprintf(stderr, "%s: %s: database is an old version (%u) and must be migrated to version %u\nUse 'nexus-migratedb %s'\n", argv[0], argv[1], db->version(), Nexus::Database::VERSION, argv[1]);
		return 3;
	}
	universe = new Nexus::Universe(db);
	db->release();
	db = NULL;

	if(!universe->migrate())
	{
		fprintf(stderr, "%s: %s: failed to migrate universe from version %u to version %u\n", argv[0], argv[1], universe->version(), Nexus::Universe::VERSION);
		universe->release();
		return 4;
	}
	player = universe->playerFromName("operator");
	if(!player)
	{
		fprintf(stderr, "failed to load initial player\n");
		universe->release();
		return 10;
	}
	player->connect();
	universe->checkpoint();
	while(player->connected())
	{
		printf("%s> ", universe->name());
		fflush(stdout);
		if(NULL == fgets(cmdbuf, sizeof(cmdbuf) - 1, stdin))
		{
			break;
		}
		player->perform(cmdbuf);
		/* this ensures that all changes are written to disk immediately */
		universe->sync();
	}
	player->release();
	universe->checkpoint();
	universe->release();
	return 0;
}
