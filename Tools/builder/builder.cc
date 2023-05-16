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
		class Builder: public WARP::Flux::RunLoopDelegate, public WARP::Flux::BufferDelegate
		{
			public:
				Builder(int argc, char **argv);
				virtual ~Builder();
				virtual int run(void);
				virtual bool shouldRunLoopTerminate(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop);
				virtual void bufferFilled(WARP::Flux::Object *sender, WARP::Flux::Buffer *buffer, char *base, size_t *nbytes);
			private:
				bool processArgs(void);
				int initialise(void);
			private:
				int _argc;
				char **_argv;
				Nexus::Universe *_universe;
				Nexus::Player *_player;
				WARP::Flux::RunLoop *_runLoop;
				WARP::Flux::Channel *_input;
				WARP::Flux::Buffer *_inputBuffer;
				bool _showPrompt;
		};
	}
}

using namespace Nexus::Tools;

Builder::Builder(int argc, char **argv):
	_argc(argc), _argv(argv),
	_universe(NULL),
	_player(NULL),
	_runLoop(NULL),
	_input(NULL),
	_inputBuffer(NULL),
	_showPrompt(false)	
{
	_runLoop = new WARP::Flux::RunLoop(this);
	_inputBuffer = new WARP::Flux::Buffer(this);
	_input = new WARP::Flux::Channel(_inputBuffer, 0);
	_runLoop->add(_input);
}

Builder::~Builder()
{
	if(_player)
	{
		_player->release();
	}
	if(_universe)
	{
		_universe->checkpoint();
		_universe->release();
	}
	_inputBuffer->release();
	_input->release();
	_runLoop->release();
}

int
Builder::run(void)
{
	int r;
	struct timeval tv;

	if(!processArgs())
	{
		return 1;
	}
	if((r = initialise()))
	{
		return r;
	}
	_showPrompt = true;
	while(!_runLoop->terminated())
	{
		if(_showPrompt && isatty(0))
		{
			_showPrompt = false;
			printf("%s> ", _universe->name());
			fflush(stdout);
		}
		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		_runLoop->processEventsWithTimeout(&tv);
		_universe->tick();
	}
	_universe->suspend();
	return 0;
}

void
Builder::bufferFilled(WARP::Flux::Object *sender, WARP::Flux::Buffer *buffer, char *base, size_t *nbytes)
{
	(void) sender;
	(void) buffer;
	(void) base;

	WARP::Flux::debugf("Builder::%s() %ld bytes available in buffer\n", __FUNCTION__, *nbytes);
	/* assume for now there's an EOL */
	_player->perform(base);
	_universe->sync();
	_showPrompt = true;
}

bool
Builder::processArgs(void)
{
	if(_argc != 2)
	{
		fprintf(stderr, "Usage: %s DB-PATH\n", _argv[0]);
		return false;
	}
	return true;
}

int
Builder::initialise(void)
{
	Nexus::Database *db;

	db = Nexus::Database::open(_argv[1]);
	if(!db)
	{
		fprintf(stderr, "%s: %s: %s\n", _argv[0], _argv[1], strerror(errno));
		return 2;
	}
	if(db->version() < Nexus::Database::DBVERSION)
	{
		fprintf(stderr, "%s: %s: database is an old version (%u) and must be migrated to version %u\nUse 'nexus-migratedb %s'\n", _argv[0], _argv[1], db->version(), Nexus::Database::DBVERSION, _argv[1]);
		return 3;
	}
	_universe = new Nexus::Universe(db);
	db->release();
	db = NULL;
	if(!_universe->migrate())
	{
		fprintf(stderr, "%s: %s: failed to migrate universe from version %u to version %u\n", _argv[0], _argv[1], _universe->version(), Nexus::Universe::UVERSION);
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
		fprintf(stderr, "%s: %s: warning: failed to activate one or more modules\n", _argv[0], _argv[1]);
	}
	_player->connect();
	_universe->checkpoint();
	_universe->resume();
	return 0;
}

bool
Builder::shouldRunLoopTerminate(WARP::Flux::Object *sender, WARP::Flux::RunLoop *loop)
{
	(void) sender;
	(void) loop;

	if(_input->closed())
	{
		_player->disconnect();
	}
	return !_player->connected();
}

int
main(int argc, char **argv)
{
	Builder builder(argc, argv);

	return builder.run();
}
