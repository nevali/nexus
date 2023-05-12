#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <jansson.h>

#include "Nexus/Database.hh"
#include "Nexus/Thing.hh"
#include "Nexus/Container.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Player.hh"

using namespace Nexus;

/* Methods for the creation of new objects in the Universe */

Thing *
Universe::createObject(json_t *object, const char *name, bool allocId, Container *location)
{
	Thing *thing;

	thing = Thing::objectFromJSON(object, true);
	if(!thing)
	{
		fprintf(stderr, "Universe::%s: failed to create object from JSON\n", __FUNCTION__);
		return NULL;
	}
	thing->setDatabase(this->_db);
	thing->setUniverse(this);
	if(name)
	{
		if(!thing->setName(name))
		{
			fprintf(stderr, "Universe::%s: failed to set name of new object to '%s'\n", __FUNCTION__, name);
			thing->release();
			return NULL;
		}
	}
	if(allocId)
	{
		Thing::ID id;

		id = _db->nextId();
//		fprintf(stderr, "Universe::%s: Database-provided next ID is #%ld, local maximum is #%ld\n", __FUNCTION__, id, _maxId);
		if(id <= _maxId)
		{
			/* make sure multiple yet-to-be-synced objects don't end up with clashing IDs */
			_maxId++;
			id = _maxId;
		}
		if(!thing->setId(id))
		{
			fprintf(stderr, "Universe::%s: cannot assign ID to newly-created object\n", __FUNCTION__);
			thing->release();
			return NULL;
		}
	}
	if(location)
	{
		if(!thing->setLocation(location))
		{
			fprintf(stderr, "Universe::%s: cannot set location on newly-created object\n",  __FUNCTION__);
			thing->release();
			return NULL;
		}
	}
	return thing;
}

/* Return a JSON object "template" for a new Thing, which optionally has a
 * name. This is used by newThing() to construct the object which will be
 * created. Each of the different object types has a similar template-building
 * method below.
 */

json_t *
Universe::objectTemplate(const char *type)
{
	json_t *obj;

	/* XXX check type via some static method on Thing */
	obj = json_object();
	json_object_set_new(obj, "type", json_string(type));
	json_object_set_new(obj, "flags", json_object());
	json_object_set_new(obj, "contents", json_array());
	return obj;
}

json_t *
Universe::thingTemplate(void)
{
	return objectTemplate("thing");
}

json_t *
Universe::containerTemplate(void)
{
	return objectTemplate("container");
}

json_t *
Universe::zoneTemplate(void)
{
	return objectTemplate("zone");
}

json_t *
Universe::roomTemplate(void)
{
	return objectTemplate("room");
}

json_t *
Universe::portalTemplate(void)
{
	return objectTemplate("portal");
}

json_t *
Universe::actorTemplate(void)
{
	return objectTemplate("actor");
}

json_t *
Universe::playerTemplate(void)
{
	return objectTemplate("player");
}

json_t *
Universe::robotTemplate(void)
{
	return objectTemplate("robot");
}

json_t *
Universe::hologramTemplate(void)
{
	return objectTemplate("hologram");
}

json_t *
Universe::executableTemplate(void)
{
	return objectTemplate("executable");
}

json_t *
Universe::variableTemplate(void)
{
	return objectTemplate("variable");
}


Player *
Universe::newPlayer(const char *name, bool allocId, Container *location)
{
	json_t *obj;
	Player *p;
	Thing *thing;

	if(name)
	{
		p = playerFromName(name);
		if(p)
		{
			fprintf(stderr, "Universe::%s: a Player named '%s' already exists\n", __FUNCTION__, p->displayName());
			p->release();
			return NULL;
		}
	}
//	fprintf(stderr, "Universe::%s: creating new Player '%s'\n", __FUNCTION__, name);
	if(!(obj = playerTemplate()))
	{
		return NULL;
	}
	thing = createObject(obj, name, allocId, location);
	json_decref(obj);
	if(!thing)
	{
		return NULL;
	}
	return thing->asPlayer();
}

Container *
Universe::newContainer(const char *name, bool allocId, Container *location)
{
	json_t *obj;
	Thing *thing;

//	fprintf(stderr, "Universe::%s: creating new Container '%s'\n", __FUNCTION__, name);
	if(!(obj = containerTemplate()))
	{
		return NULL;
	}
	thing = createObject(obj, name, allocId, location);
	json_decref(obj);
	if(!thing)
	{
		return NULL;
	}
	return thing->asContainer();
}

Zone *
Universe::newZone(const char *name, bool allocId, Container *location)
{
	json_t *obj;
	Thing *thing;

	if(name)
	{
		thing = zoneFromName(name);
		if(thing)
		{
			fprintf(stderr, "Universe::%s: a Zone named '%s' already exists\n", __FUNCTION__, thing->displayName());
			thing->release();
			return NULL;
		}
	}
//	fprintf(stderr, "Universe::%s: creating new Zone '%s'\n", __FUNCTION__, name);
	if(!(obj = zoneTemplate()))
	{
		return NULL;
	}
	thing = createObject(obj, name, allocId, location);
	json_decref(obj);
	if(!thing)
	{
		return NULL;
	}
	return thing->asZone();
}


Room *
Universe::newRoom(const char *name, bool allocId, Container *location)
{
	json_t *obj;
	Thing *thing;

//	fprintf(stderr, "Universe::%s: creating new Room '%s'\n", __FUNCTION__, name);
	if(!(obj = roomTemplate()))
	{
		return NULL;
	}
	thing = createObject(obj, name, allocId, location);
	json_decref(obj);
	if(!thing)
	{
		return NULL;
	}
	return thing->asRoom();
}


Thing *
Universe::newThing(const char *name, bool allocId, Container *location)
{
	json_t *obj;
	Thing *thing;

//	fprintf(stderr, "Universe::%s: creating new Thing '%s'\n", __FUNCTION__, name);
	if(!(obj = thingTemplate()))
	{
		return NULL;
	}
	thing = createObject(obj, name, allocId, location);
	json_decref(obj);
	if(!thing)
	{
		return NULL;
	}
	return thing;
}

Variable *
Universe::newVariable(const char *name, bool allocId, Container *location)
{
	json_t *obj;
	Thing *thing;

//	fprintf(stderr, "Universe::%s: creating new Variable '%s'\n", __FUNCTION__, name);
	if(!(obj = variableTemplate()))
	{
		return NULL;
	}
	thing = createObject(obj, name, allocId, location);
	json_decref(obj);
	if(!thing)
	{
		return NULL;
	}
	return thing->asVariable();
}

Portal *
Universe::newPortal(const char *name, bool allocId, Container *location)
{
	json_t *obj;
	Thing *thing;

//	fprintf(stderr, "Universe::%s: creating new Portal '%s'\n", __FUNCTION__, name);
	if(!(obj = portalTemplate()))
	{
		return NULL;
	}
	thing = createObject(obj, name, allocId, location);
	json_decref(obj);
	if(!thing)
	{
		return NULL;
	}
	return thing->asPortal();
}
