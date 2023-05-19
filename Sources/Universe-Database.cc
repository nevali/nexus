#include <cstring>

#include "Nexus/Universe.hh"
#include "Nexus/Database.hh"

using namespace Nexus;

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
	tick();
	updateClocks();
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
	tick();
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
