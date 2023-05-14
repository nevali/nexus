#include <cstdio>
#include <cstring>
#include <cctype>

#include <jansson.h>

#include "Nexus/Thing.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Container.hh"
#include "Nexus/Room.hh"
#include "Nexus/Portal.hh"
#include "Nexus/Actor.hh"
#include "Nexus/Player.hh"
#include "Nexus/Robot.hh"
#include "Nexus/Executable.hh"
#include "Nexus/Variable.hh"

using namespace Nexus;

static inline Thing::ID
idFromJSON(json_t *source)
{
	json_t *value = json_object_get(source, "id");

	if(!value)
	{
		return Thing::ID_INVALID;
	}
	return (Thing::ID) json_integer_value(value);
}

static inline Thing::TypeID
typeIdFromJSON(json_t *source)
{
	json_t *value = json_object_get(source, "type");
	const char *str;

	if(!value)
	{
		return Thing::INVALID;
	}
	str = json_string_value(value);
	if(!str)
	{
		return Thing::INVALID;
	}
	if(!strcmp(str, "thing"))
	{
		return Thing::THING;
	}
	if(!strcmp(str, "container"))
	{
		return Thing::CONTAINER;
	}
	if(!strcmp(str, "zone"))
	{
		return Thing::ZONE;
	}
	if(!strcmp(str, "room"))
	{
		return Thing::ROOM;
	}
	if(!strcmp(str, "portal"))
	{
		return Thing::PORTAL;
	}
	if(!strcmp(str, "actor"))
	{
		return Thing::ACTOR;
	}
	if(!strcmp(str, "player"))
	{
		return Thing::PLAYER;
	}
	if(!strcmp(str, "robot"))
	{
		return Thing::ROBOT;
	}
	if(!strcmp(str, "executable"))
	{
		return Thing::EXECUTABLE;
	}
	if(!strcmp(str, "variable"))
	{
		return Thing::VARIABLE;
	}
	if(!strcmp(str, "switch"))
	{
		return Thing::SWITCH;
	}
	if(!strcmp(str, "timer"))
	{
		return Thing::TIMER;
	}
	return Thing::INVALID;
}

Thing *
Thing::objectFromJSON(json_t *source, bool isNew)
{
	Thing::ID newId;
	Thing::TypeID newTypeId;

	newId = idFromJSON(source);
	newTypeId = typeIdFromJSON(source);
	if((!isNew && (newId == Thing::ID_INVALID)) ||
		(isNew && (newId != Thing::ID_INVALID)))
	{
		fprintf(stderr, "Thing::%s: object has an invalid ID: typeId = %c\n", __FUNCTION__, newTypeId);
		json_dumpf(source, stderr, JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ENCODE_ANY);
		fprintf(stderr, "\n");
		return NULL;
	}
	switch(newTypeId)
	{
		case THING:
			return new Thing(source);
		case CONTAINER:
			return new Container(source);
		case ZONE:
			return new Zone(source);
		case ROOM:
			return new Room(source);
		case PORTAL:
			return new Portal(source);
		case ACTOR:
			return new Actor(source);
		case PLAYER:
			return new Player(source);
		case ROBOT:
			return new Robot(source);
		case EXECUTABLE:
			return new Executable(source);
		case VARIABLE:
			return new Variable(source);
		default:
			fprintf(stderr, "Thing::%s: object has a missing or unsupported typeId; ID = %ld, typeId = %c\n", __FUNCTION__, newId, newTypeId);
			json_dumpf(source, stderr, JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ENCODE_ANY);
			fprintf(stderr, "\n");
	}
	return NULL;
}

bool
Thing::validName(const char *name)
{
	if(!name || strlen(name) < 1 || strlen(name) > 64)
	{
		return false;
	}
	if(!isalpha(name[0]))
	{
		return false;
	}
	for(; *name; name++)
	{
		if(!isprint(*name))
		{
			return false;
		}
	}
	return true;
}

bool
Thing::nameIsSuitable(const char *newName) const
{
	if(!validName(newName))
	{
		return false;
	}
	return true;
}

Thing::Thing(json_t *source):
	Object(),
	_obj(NULL),
	_universe(NULL),
	_db(NULL),
	_flags(NONE),
	_status(DEFAULT),
	_displayName(NULL)
{
	if(source)
	{
		json_incref(source);
		_obj = source;
	}
	else
	{
		_obj = json_object();
	}
	_id = idFromJSON(_obj);
	_typeId = typeIdFromJSON(_obj);
	if(_id == ID_INVALID)
	{
//		fprintf(stderr, "Thing<%p>::%s: instantiated anonymous object\n", this, __FUNCTION__);
	}
	else
	{
//		fprintf(stderr, "Thing<%p>::%s: instantiated #%ld%c\n", this, __FUNCTION__, _id, (char) _typeId);
	}
}

Thing::~Thing()
{
	if(_id == ID_INVALID)
	{
		fprintf(stderr, "Thing<%p>::%s: destroying anonymous object\n", this, __FUNCTION__);
	}
	else
	{
		fprintf(stderr, "Thing<%p>::%s: destroying object #%ld%c\n", this, __FUNCTION__, _id, (char) _typeId);
	}
	if(_universe)
	{
		_universe->discard(this);
	}
	json_decref(_obj);
	_obj = NULL;
}

void
Thing::unpack(void)
{
}

void
Thing::dumpf(FILE *out)
{
	json_dumpf(_obj, out, JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ENCODE_ANY);
	fprintf(out, "\n");
	fflush(out);
}

void
Thing::setUniverse(Universe *universe)
{
	if(_universe == universe)
	{
		return;
	}
	_universe = universe;
	unpack();
}

void
Thing::setDatabase(Database *db)
{
	if(_db == db)
	{
		return;
	}
	_db = db;
	updateIdent();
	updateFlags();
	updateName();
}

void
Thing::markDirty(void)
{
	_status |= DIRTY;
}

bool
Thing::sync(void)
{
	if(!(_status & DIRTY))
	{
		return true;
	}
	if(_db)
	{
//		fprintf(stderr, "Thing<%p>::%s: syncing changes to %s\n", this, __FUNCTION__, ident());
		if(!_db->storeObject(_obj))
		{
			return false;
		}
		_status &= ~DIRTY;
		return true;
	}
	fprintf(stderr, "Thing<%p>::%s: cannot sync changes to %s because no database is available\n", this, __FUNCTION__, ident());
	return false;
}

Universe *
Thing::universe(void) const
{
	return _universe;
}

Database *
Thing::database(void) const
{
	return _db;
}

/* resolve a target relative to this object */
Thing *
Thing::resolveTarget(const char *target)
{
	if(!target || !*target)
	{
		return NULL;
	}
	if(!strcasecmp(target, "here"))
	{
		return location();
	}
	if(!strcasecmp(target, "me"))
	{
		retain();
		return this;
	}
	if(target[0] == '#')
	{
		ID id;

		id = ID_INVALID;
		if(_universe && sscanf(target, "#%ld", &id) == 1)
		{
			return _universe->thingFromId(id);
		}
	}
	return NULL;
}
