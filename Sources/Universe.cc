#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <jansson.h>

#include "Nexus/Database.hh"
#include "Nexus/Thing.hh"
#include "Nexus/Player.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Variable.hh"
#include "Nexus/Room.hh"
#include "Nexus/Module.hh"

using namespace Nexus;

Universe::Universe(Database *db):
	Object(),
	_objects(NULL),
	_objectsSize(0),
	_db(db),
	_limbo(NULL),
	_operator(NULL),
	_root(NULL),
	_meta(NULL),
	_builtinsParser(NULL),
	_defaultParser(NULL),
	_maxId(0),
	_modules(NULL),
	_now(0),
	_launchTime(0),
	_ageAtLaunch(0),
	_age(0),
	_running(true)
{
	gettimeofday(&_wallClock, NULL);
	fprintf(stderr, "Universe::%s: universe is starting up\n", __FUNCTION__);
	db->retain();
	_meta = db->universe();
	json_incref(_meta);
	fprintf(stderr, "Universe::%s: universe version %u (current is %u)\n", __FUNCTION__, version(), UVERSION);
	_ageAtLaunch = _age = json_integer_value(json_object_get(_meta, "age"));
	updateClocks();
	_launchTime = _now;
	fprintf(stderr, "Universe::%s: creating Limbo\n", __FUNCTION__);
	_limbo = new Limbo();
	_limbo->setUniverse(this);
	fprintf(stderr, "Universe::%s: pre-loading Operator\n", __FUNCTION__);
	_actors = new WARP::Flux::TArray<Actor>();
	_operator = playerFromName("operator");
	if(!_operator)
	{
		fprintf(stderr, "Universe::%s: failed to get Player 'operator'\n", __FUNCTION__);
	}

	fprintf(stderr, "Universe::%s: pre-loading system Zones\n", __FUNCTION__);
	_root = zoneFromName("root");
	if(!_root)
	{
		fprintf(stderr, "Universe::%s: failed to get Zone 'root'\n", __FUNCTION__);
	}
	_system = zoneFromName("system");
	if(!_system)
	{
		fprintf(stderr, "Universe::%s: failed to get Zone 'system'\n", __FUNCTION__);
	}
	fprintf(stderr, "Universe::%s: creating parsers\n", __FUNCTION__);
	_builtinsParser = new BuiltinsParser(this);
	fprintf(stderr, "Universe::%s: initialisation complete\n", __FUNCTION__);
	_modules = new Universe::ModuleList(this);
}

Universe::~Universe()
{
	size_t c, count;

	fprintf(stderr, "Universe::%s: shutting down...\n", __FUNCTION__);
	if(_actors)
	{
		fprintf(stderr, "- releasing actors\n");
		_actors->release();
		_actors = NULL;
	}
	if(_modules)
	{
		fprintf(stderr, "- releasing modules\n");
		_modules->release();
	}
	fprintf(stderr, "- releasing parsers\n");
	if(_builtinsParser)
	{
		_builtinsParser->release();
	}
	if(_defaultParser)
	{
		_defaultParser->release();
	}
	fprintf(stderr, "- syncing database\n");
	sync();
	json_decref(_meta);
	if(_root)
	{
		fprintf(stderr, "- releasing root Zone\n");
		_root->release();
	}
	if(_system)
	{
		fprintf(stderr, "- releasing system Zone\n");
		_system->release();
	}
	if(_operator)
	{
		fprintf(stderr, "- releasing Operator\n");
		_operator->release();
	}
	if(_limbo)
	{
		fprintf(stderr, "- releasing Limbo\n");
		_limbo->release();
	}
	count = 0;
	for(c = 0; c < _objectsSize; c++)
	{
		if(_objects[c])
		{
			count++;
		}
	}
	free(_objects);
	fprintf(stderr, "- %lu latent cached objects\n", count);
	fprintf(stderr, "- releasing Database\n");
	_db->release();
}

const char *
Universe::name(void) const
{
	return json_string_value(json_object_get(_meta, "name"));
}

Parser *
Universe::builtinsParser(void)
{
	if(_builtinsParser)
	{
		_builtinsParser->retain();
		return _builtinsParser;
	}
	return NULL;
}

Parser *
Universe::parserForCommand(Actor *who, const char *commandLine)
{
	if(!commandLine || !*commandLine)
	{
		return NULL;
	}
	/* only Players can access the builtins */
	if(who->isPlayer() && _builtinsParser)
	{
		if(*commandLine == '@')
		{
			_builtinsParser->retain();
			return _builtinsParser;
		};
	}
	/* if alternative command modes are required, e.g., slash-commands, they
	 * could be implemented here
	 */
	if(_defaultParser)
	{
		_defaultParser->retain();
		return _defaultParser;
	}
	return NULL;
}
