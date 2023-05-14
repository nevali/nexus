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
	_modules(NULL)
{
	fprintf(stderr, "Universe::%s: universe is starting up\n", __FUNCTION__);
	db->retain();
	_meta = db->universe();
	json_incref(_meta);
	fprintf(stderr, "Universe::%s: universe version %u (current is %u)\n", __FUNCTION__, version(), UVERSION);
	fprintf(stderr, "Universe::%s: creating Limbo\n", __FUNCTION__);
	_limbo = new Limbo();
	_limbo->setUniverse(this);
	fprintf(stderr, "Universe::%s: pre-loading Operator\n", __FUNCTION__);
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

/* Obtain a thing (of any kind) with the given ID, adding it to the list of
 * active objects
 */
Thing *
Universe::thingFromId(Thing::ID id)
{
	Thing *thing;

	if(id < 0 || id > ID_MAX)
	{
		return NULL;
	}
	if(id == Limbo::ID_LIMBO)
	{
		_limbo->retain();
		return _limbo;
	}
	if((size_t) id < _objectsSize)
	{
		/* this approach won't work well with sparsely-numbered objects */
		if(_objects[id])
		{
//			fprintf(stderr, "Universe::%s: returning cached Thing<%p> #%ld%c\n", __FUNCTION__, _objects[id], id, _objects[id]->typeChar());
			_objects[id]->retain();
			return _objects[id];
		}
	}
	thing = _db->objectFromId(id);
	if(!thing)
	{
		return NULL;
	}
	acquire(thing);
	return thing;
}

Thing *
Universe::thingFromName(const char *type, const char *name)
{
	Thing::ID id;

	id = _db->idFromName(type, name);
	if(id == Thing::ID_INVALID)
	{
		return NULL;
	}
	return thingFromId(id);
}


/* Store a reference to a newly-loaded object which will be re-used until
 * it is destroyed
 */
void
Universe::acquire(Thing *thing)
{
	Thing::ID id;
	size_t newSize;
	Thing **p;

	id = thing->id();
	if(id < 1 || id > ID_MAX)
	{
		return;
	}
	if(id > _maxId)
	{
		_maxId = id;
	}
	newSize = id + 1;

	if(newSize > _objectsSize)
	{
//		fprintf(stderr, "Universe::%s: resizing object cache from %lu to %lu\n", __FUNCTION__, _objectsSize, newSize);
		p = (Thing **) realloc(_objects, newSize * sizeof(Thing *));
		if(!p)
		{
			abort();
		}
		_objects = p;
		p = &(_objects[_objectsSize]);
		memset(p, 0, sizeof(Thing *) * (newSize - _objectsSize));
		_objectsSize = newSize;
	}
	if(_objects[id])
	{
		fprintf(stderr, "Universe::%s: WARNING: discarding old object #%lu (Thing<%p>) in favour of Thing<%p> %s\n",
			__FUNCTION__, id, _objects[id], thing, thing->ident());
	}
	/* this is a weak reference -- see Universe::discard() which is invoked by
	 * Thing::~Thing()
	 */
	_objects[id] = thing;
//	fprintf(stderr, "Universe::%s: acquired Thing<%p> %s\n", __FUNCTION__, thing, thing->ident());
	thing->setUniverse(this);
}

void
Universe::discard(Thing *thing)
{
	Thing::ID id = thing->id();

	if((size_t) id < _objectsSize)
	{
		if(_objects[id])
		{
//			fprintf(stderr, "Universe::%s: discarding object %s\n", __FUNCTION__, thing->ident());
			_objects[id] = NULL;
		}
	}
}

/* Return the root zone */
Zone *
Universe::rootZone(void) const
{
	_root->retain();
	return _root;
}

/* Return the system zone */
Zone *
Universe::systemZone(void) const
{
	_system->retain();
	return _system;
}

/* Return Limbo */
Limbo *
Universe::limbo(void) const
{
	_limbo->retain();
	return _limbo;
}

/* Return the system object named 'name' */
Thing *
Universe::systemObjectNamed(const char *name)
{
	(void) name;

	if(!_system)
	{
		return NULL;
	}
	return _system->firstObjectNamed(name, Thing::ANY);
}

/* Return the message object named 'name' */
Variable *
Universe::messageObjectNamed(const char *name)
{
	Thing *messages, *thing;
	Container *cm;

	thing = NULL;
	if((messages = systemObjectNamed("messages")))
	{
		if((cm = messages->asContainer()))
		{
			thing = cm->firstObjectNamed(name, Thing::VARIABLE);
		}
		messages->release();
	}
	return thing->asVariable();
}

/* Return the message text named 'name' */
const char *
Universe::messageNamed(const char *name)
{
	Variable *object;
	const char *desc;

	desc = NULL;
	if((object = messageObjectNamed(name)))
	{
		desc = object->textValue();
		/* XXX technically this is use-after-free: it assumes that the message
		 * resides within _system, which logically must be true, but if that
		 * somehow ceases to be the case, then desc will be invalid
		 * following this call
		 */
		object->release();
	}
	return desc;
}

Container *
Universe::containerFromId(Thing::ID id)
{
	Thing *thing;
	Container *ret;

	thing = thingFromId(id);
	if(thing)
	{
		if((ret = thing->asContainer()))
		{
			return ret;
		}
		thing->release();
	}
	return NULL;
}

Zone *
Universe::zoneFromId(Thing::ID id)
{
	Thing *thing;
	Zone *ret;

	thing = thingFromId(id);
	if(thing)
	{
		if((ret = thing->asZone()))
		{
			return ret;
		}
		thing->release();
	}
	return NULL;
}

Zone *
Universe::zoneFromName(const char *name)
{
	Thing *thing;

	thing = thingFromName("zone", name);
	if(thing)
	{
		return thing->asZone();
	}
	return NULL;
}

Room *
Universe::roomFromId(Thing::ID id)
{
	Thing *thing;
	Room *ret;

	thing = thingFromId(id);
	if(thing)
	{
		if((ret = thing->asRoom()))
		{
			return ret;
		}
		thing->release();
	}
	return NULL;
}

Portal *
Universe::portalFromId(Thing::ID id)
{
	Thing *thing;
	Portal *ret;

	thing = thingFromId(id);
	if(thing)
	{
		if((ret = thing->asPortal()))
		{
			return ret;
		}
		thing->release();
	}
	return NULL;
}

Actor *
Universe::actorFromId(Thing::ID id)
{
	Thing *thing;
	Actor *ret;

	thing = thingFromId(id);
	if(thing)
	{
		if((ret = thing->asActor()))
		{
			return ret;
		}
		thing->release();
	}
	return NULL;
}

Player *
Universe::playerFromId(Thing::ID id)
{
	Thing *thing;
	Player *ret;

	thing = thingFromId(id);
	if(thing)
	{
		if((ret = thing->asPlayer()))
		{
			return ret;
		}
		thing->release();
	}
	return NULL;
}

Player *
Universe::playerFromName(const char *name)
{
	Thing *thing;

	thing = thingFromName("player", name);
	if(thing)
	{
		return thing->asPlayer();
	}
	return NULL;
}

Robot *
Universe::robotFromId(Thing::ID id)
{
	Thing *thing;
	Robot *ret;

	thing = thingFromId(id);
	if(thing)
	{
		if((ret = thing->asRobot()))
		{
			return ret;
		}
		thing->release();
	}
	return NULL;
}

Executable *
Universe::executableFromId(Thing::ID id)
{
	Thing *thing;
	Executable *ret;

	thing = thingFromId(id);
	if(thing)
	{
		if((ret = thing->asExecutable()))
		{
			return ret;
		}
		thing->release();
	}
	return NULL;
}

unsigned
Universe::version(void) const
{
	json_t *value;

	if(_meta)
	{
		value = json_object_get(_meta, "version");
		if(value)
		{
			return json_integer_value(value);
		}
	}
	return 0;
}

bool
Universe::migrate(void)
{
	for(unsigned currentVersion = version(); currentVersion < UVERSION; currentVersion = version())
	{
		if(!migrateTo(currentVersion + 1))
		{
			fprintf(stderr, "Universe::%s: migration to version %u failed\n", __FUNCTION__, currentVersion + 1);
			return false;
		}
		if(version() <= currentVersion)
		{
			fprintf(stderr, "Universe::%s: post-migration version is %u (expected at least %u)\n", __FUNCTION__, version(), currentVersion + 1);
			return false;
		}
		sync();
	}
	fprintf(stderr, "Universe::%s: universe is up to date with version %u\n", __FUNCTION__, version());
	return true;
}

bool
Universe::checkpoint(void)
{
	if(_db)
	{
		syncObjects();
		return _db->checkpoint();
	}
	return true;
}

bool
Universe::sync(void)
{
	if(_db)
	{
		syncObjects();
		return _db->sync();
	}
	return true;
}

void
Universe::syncObjects(void)
{
//	fprintf(stderr, "Universe::%s: synchronising changes\n", __FUNCTION__);
	for(size_t n = 0; n < _objectsSize; n++)
	{
		if(_objects[n])
		{
			_objects[n]->sync();
		}
	}
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
