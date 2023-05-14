#include <cstdio>
#include <cstring>
#include <cerrno>

#include "Nexus/Database.hh"

int
main(int argc, char **argv)
{
	Nexus::Database *db;

	if(argc != 2 || argv[1][0] == '-')
	{
		fprintf(stderr, "Usage: %s PATH\n", argv[0]);
		fprintf(stderr, "\nCreates a new database. PATH must not already exist\n");
		return 1;
	}
	db = Nexus::Database::create(argv[1]);
	if(!db)
	{
		if(errno == EEXIST)
		{
			fprintf(stderr, "%s: %s: database path already exists, cannot create new database\n", argv[0], argv[1]);
			return 2;
		}
		fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
		return 3;
	}
	db->migrate();
	db->release();
	return 0;
}
