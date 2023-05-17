#include <cstdio>
#include <cstring>
#include <cerrno>

#include <sys/time.h>
#include <unistd.h>

#include <jansson.h>

#include "WARP/Flux/Flux.h"

#include "Nexus/Player.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"

namespace Nexus
{
	namespace Tools
	{
		class Builder: public WARP::Flux::Tool
		{
			public:
				Builder(int argc, char **argv);
				virtual ~Builder();
				virtual void runLoopIsStarting(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop);
				virtual void runLoopEnded(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop);
				virtual void runLoopPass(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop);
				virtual bool shouldRunLoopTerminate(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop);
			protected:
				virtual bool processArgs(void);
				virtual int initialise(void);
			private:
				Nexus::Universe *_universe;
				Nexus::Player *_player;
		};
	}
}

using namespace Nexus::Tools;

Builder::Builder(int argc, char **argv):
	Tool(argc, argv),
	_universe(NULL),
	_player(NULL)
{
}

Builder::~Builder()
{
	if(_player)
	{
		_player->release();
		_player = NULL;
	}
	if(_universe)
	{
		_universe->release();
		_universe = NULL;
	}
}

int
Builder::initialise(void)
{
	int r;

	if((r = Tool::initialise()))
	{
		return r;
	}
	if(!_universe->migrate())
	{
		fprintf(stderr, "%s: %s: failed to migrate universe from version %u to version %u\n", argv(0), argv(1), _universe->version(), Nexus::Universe::UVERSION);
		return 4;
	}
	_player = _universe->playerFromName("operator");
	if(!_player)
	{
		fprintf(stderr, "failed to load initial player\n");
		return 10;
	}
	if(!_universe->activateModules())
	{
		fprintf(stderr, "%s: %s: warning: failed to activate one or more modules\n", argv(0), argv(1));
	}
	return 0;
}


void
Builder::runLoopIsStarting(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop)
{
	WARP::Flux::Channel *input;

	(void) sender;
	(void) loop;

	_universe->resume();
	_universe->checkpoint();
	input = new WARP::Flux::Channel(NULL, 0);
	add(input);
	_player->connect(input);
	input->release();
}

void
Builder::runLoopEnded(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop)
{
	(void) sender;
	(void) loop;

	_universe->suspend();
	_universe->checkpoint();
}

void
Builder::runLoopPass(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop)
{
	(void) sender;
	(void) loop;
	_universe->tick();
}

bool
Builder::shouldRunLoopTerminate(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop)
{
	(void) sender;
	(void) loop;

	if(_player)
	{
		return !_player->connected();
	}
	return true;
}

bool
Builder::processArgs(void)
{
	Nexus::Database *db;

	if(argc() != 2)
	{
		fprintf(stderr, "Usage: %s DB-PATH\n", argv(0));
		return false;
	}
	db = Nexus::Database::open(argv(1));
	if(!db)
	{
		fprintf(stderr, "%s: %s: %s\n", argv(0), argv(1), strerror(errno));
		return false;
	}
	if(db->version() < Nexus::Database::DBVERSION)
	{
		fprintf(stderr, "%s: %s: database is an old version (%u) and must be migrated to version %u\nUse 'nexus-migratedb %s'\n", argv(0), argv(1), db->version(), Nexus::Database::DBVERSION, argv(1));
		return false;
	}
	_universe = new Nexus::Universe(db);
	db->release();
	return true;
}

int
main(int argc, char **argv)
{
	Builder builder(argc, argv);

	return builder.run();
}
