#include <cstdio>
#include <cstring>
#include <cerrno>

#include "Nexus/Database.hh"

int
main(int argc, char **argv)
{
	Nexus::Database *db;
	bool r;
	
	if(argc != 2 || argv[1][0] == '-')
	{
		fprintf(stderr, "Usage: %s PATH\n", argv[0]);
		fprintf(stderr, "\nMigrates an existing database to the latest version.\n");
		return 1;
	}
	db = Nexus::Database::open(argv[1]);
	if(!db)
	{
		fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
		return 2;
	}
	fprintf(stderr, "%s: %s: database is version %u, current is %u\n", argv[0], argv[1], db->version(), Nexus::Database::VERSION);
	r = db->migrate();
	db->release();
	if(!r)
	{
		fprintf(stderr, "%s: %s: failed to migrate to latest version\n", argv[0], argv[1]);
		return 3;
	}
	fprintf(stderr, "%s: %s: migrated successfully\n", argv[0], argv[1]);
	return 0;
}
