#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>

#include <jansson.h>

#include "Nexus/Thing.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Container.hh"

using namespace Nexus;

Thing::ID
Thing::id(void) const
{
	return _id;
}

bool
Thing::setId(Thing::ID id)
{
	if(_id == id)
	{
		return false;
	}
	if(id == ID_INVALID)
	{
		return false;
	}
	if(_id != ID_INVALID)
	{
		fprintf(stderr, "Thing<%p>::%s: refusing to change ID of %s to #%ld\n", this, __FUNCTION__, ident(), id);
		return false;
	}
	if(_universe)
	{
		Thing *thing = _universe->thingFromId(id);
		if(thing)
		{
			fprintf(stderr, "Thing<%p>::%s: attempt to change ID to #%ld which already exists as %s\n", this, __FUNCTION__, id, thing->ident());
			thing->release();
			return false;
		}
	}
	_id = id;
	json_object_set(_obj, "id", json_integer(_id));
	_status |= DIRTY;
	updateIdent();
//	fprintf(stderr, "Thing<%p>::%s: updated ID to %s\n", this, __FUNCTION__, ident());
	if(_universe)
	{
//		fprintf(stderr, "Thing<%p>::%s: will now inform the universe of our new identity\n", this, __FUNCTION__);
		_universe->acquire(this);
	}
	return true;
}

const char *
Thing::ident(void) const
{
	return _ident;
}

void
Thing::updateIdent(void)
{
	snprintf(_ident, sizeof(_ident), "#%ld%c", id(), typeId());
}

const char *
Thing::name(void) const
{
	json_t *value = json_object_get(_obj, "name");

	if(!value)
	{
		return NULL;
	}
	return json_string_value(value);
}

const char *
Thing::displayName(void) const
{
	return _displayName;
}

/* rename an object, or remove its name; note that other object types are much
 * more restrictive about names: no two Players can have the same name, for
 * example
 */
bool
Thing::setName(const char *str)
{
	if(str && !validName(str))
	{
		fprintf(stderr, "Thing<%p>::%s: new name '%s' is not valid for object %s\n", this, __FUNCTION__, str, ident());
		return false;
	}
	if(!nameIsSuitable(str))
	{
		if(str)
		{
			fprintf(stderr, "Thing<%p>::%s: new name '%s' is not valid for object %s\n", this, __FUNCTION__, str, ident());
		}
		else
		{
			fprintf(stderr, "Thing<%p>::%s: object %s must have a name\n", this, __FUNCTION__, ident());
		}
		return false;		
	}
	if(!str)
	{
		json_object_del(_obj, "name");
	}
	else
	{
		json_object_set_new(_obj, "name", json_string(str));
	}
	markDirty();
	updateName();
	return true;
}

void
Thing::updateName(void)
{
	json_t *name;
	const char *str;
	char cstr[Database::MAX_CANON_NAME + 1];

	/* the object's name has been updated; make sure that 'cname'
	 * (the canonicalised name) is set and that _displayName,
	 * is always set to a value even if name and cname are unset
	 */
	name = json_object_get(_obj, "name");
	if(name && (str = json_string_value(name)))
	{
		_displayName = str;
		Database::canonicalise(cstr, sizeof(cstr), str);
		if(cstr[0])
		{
			json_object_set_new(_obj, "cname", json_string(cstr));
		}
		else
		{
			json_object_del(_obj, "cname");
		}
	}
	else
	{
		json_object_del(_obj, "cname");
		_displayName = "<No name>";
	}
}

bool
Thing::isNamed(const char *name) const
{
	char cname[Database::MAX_CANON_NAME + 1];

	Database::canonicalise(cname, sizeof(cname), name);
	return isCanonicallyNamed(cname);
}

const char *
Thing::canonicalName(void) const
{
	json_t *val;

	val = json_object_get(_obj, "cname");
	if(val)
	{
		return json_string_value(val);
	}
	return NULL;
}

bool
Thing::isCanonicallyNamed(const char *cname) const
{
	json_t *nameval;

	nameval = json_object_get(_obj, "cname");
	if(nameval && json_string_value(nameval) && !strcmp(cname, json_string_value(nameval)))
	{
		return true;
	}
	return false;
}

const char *
Thing::description(void) const
{
	json_t *value = json_object_get(_obj, "description");

	if(!value)
	{
		return NULL;
	}
	return json_string_value(value);
}

bool
Thing::setDescription(const char *newDesc)
{
	if(!newDesc)
	{
		json_object_del(_obj, "description");
	}
	else
	{
		json_object_set_new(_obj, "description", json_string(newDesc));
	}
	markDirty();
	return true;
}

Actor *
Thing::owner(void)
{
	json_t *value = json_object_get(_obj, "owner");
	ID id;

	if(!_universe)
	{
		return NULL;
	}
	if(!value)
	{
		return NULL;
	}
	id = json_integer_value(value);
	return _universe->actorFromId(id);
}

bool
Thing::setOwner(Thing *owner)
{
	if(owner && owner->id() != ID_INVALID)
	{
		json_object_set_new(_obj, "owner", json_integer(owner->id()));
	}
	else
	{
		json_object_del(_obj, "owner");
	}
	markDirty();
	return true;
}

Container *
Thing::location(void) const
{
	json_t *value = json_object_get(_obj, "location");
	ID id;

	if(!_universe)
	{
		return NULL;
	}
	if(value)
	{
		id = json_integer_value(value);
	}
	else
	{
		id = ID_LIMBO;
	}
//	fprintf(stderr, "Thing<%p>::%s: location is #%ld\n", this, __FUNCTION__, id);
	return _universe->containerFromId(id);
}

bool
Thing::setLocation(Container *loc)
{
	return setLocation(loc->id());
}

bool
Thing::setLocation(ID newId)
{
	json_t *value = json_object_get(_obj, "location");
	ID locId;
	Container *oldloc;

	if(id() == ID_INVALID)
	{
		fprintf(stderr, "Thing<%p>::%s: not setting location on object with no ID\n", this, __FUNCTION__);
		return false;
	}
	if(_universe)
	{
		if(value)
		{
			locId = json_integer_value(value);
		}
		else
		{
			locId = ID_LIMBO;
		}
		if(newId != locId)
		{
//			fprintf(stderr, "Thing<%p>::%s: old location is #%ld, new location is #%ld\n", this, __FUNCTION__, locId, newId);
			oldloc = location();
			if(oldloc)
			{
				oldloc->remove(this);
				oldloc->release();
			}
		}
	}
	json_object_set(_obj, "location", json_integer(newId));
	markDirty();
	if(_universe)
	{
		Container *newloc;

		newloc = location();
		if(newloc)
		{
			newloc->add(this);
			newloc->release();
		}
	}
	return true;
}

Zone *
Thing::zone(void)
{
	Thing *loc;
	Zone *z;
	
	if(!(loc = location()))
	{
		return NULL;
	}
	if(loc->isZone())
	{
		return loc->asZone();
	}
	z = loc->zone();
	loc->release();
	return z;
}