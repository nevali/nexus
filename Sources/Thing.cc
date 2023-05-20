#include <cstdio>
#include <cstring>
#include <cctype>

#include <jansson.h>

#include "WARP/Flux/Diagnostics.hh"

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
using namespace WARP::Flux::Diagnostics;

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
	return Thing::typeId(str);
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
	if((_flags & SYSTEM) && newName && newName[0] == '*')
	{
		newName++;
	}
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
//		::debugf("Thing<%p>::%s: instantiated anonymous object\n", this, __FUNCTION__);
	}
	else
	{
//		::debugf("Thing<%p>::%s: instantiated #%ld%c\n", this, __FUNCTION__, _id, (char) _typeId);
	}
}

Thing::~Thing()
{
	if(_id == ID_INVALID)
	{
		::debugf("Thing<%p>::%s: destroying anonymous object\n", this, __FUNCTION__);
	}
	else
	{
		::debugf("Thing<%p>::%s: destroying object #%ld%c\n", this, __FUNCTION__, _id, (char) _typeId);
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
//		::debugf("Thing<%p>::%s: syncing changes to %s\n", this, __FUNCTION__, ident());
		if(!_db->storeObject(_obj))
		{
			return false;
		}
		_status &= ~DIRTY;
		return true;
	}
	::debugf("Thing<%p>::%s: cannot sync changes to %s because no database is available\n", this, __FUNCTION__, ident());
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
