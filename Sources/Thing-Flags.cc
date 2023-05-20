#include <cstdio>
#include <cstring>

#include <jansson.h>

#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Thing.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace WARP::Flux::Diagnostics;

unsigned
Thing::flags(void) const
{
	return _flags;
}

const char *
Thing::flagsStr(void) const
{
	return _flagsStr;
}

void
Thing::updateFlags(void)
{
	json_t *flags, *value;
	const char *key;
	size_t c;

	if(!(flags = json_object_get(_obj, "flags")) || !json_is_object(flags))
	{
		::debugf("Thing<%p>::%s: creating new flags object\n", this, __FUNCTION__);
		flags = json_object();
		json_object_set_new(_obj, "flags", flags);
	}
	_flags = NONE;
	json_object_foreach(flags, key, value)
	{
		applyFlag(key, json_is_true(value));
	}
	if(json_boolean_value(json_object_get(_obj, "deleted")))
	{
		_flags |= DELETED;
	}
	c = 0;
	if(_flags & SYSTEM)
	{
		_flagsStr[c] = 'S';
		c++;
	}
	if(_flags & DELETED)
	{
		_flagsStr[c] = '!';
		c++;
	}
	else
	{
		if(_flags & HIDDEN)
		{
			_flagsStr[c] = 'H';
			c++;
		}
		if(_flags & FIXED)
		{
			_flagsStr[c] = 'F';
			c++;
		}
		if(_flags & IMMORTAL)
		{
			_flagsStr[c] = 'I';
			c++;
		}
	}
	_flagsStr[c] = 0;
}

bool
Thing::setFlag(const char *flag, bool set)
{
	const char *flags[] = { "system", "hidden", "fixed", "deleted", NULL };
	size_t c;

	for(c = 0; flags[c]; c++)
	{
		if(!strcasecmp(flags[c], flag))
		{
			json_object_set_new(json_object_get(_obj, "flags"), flags[c], json_boolean(set));
			updateFlags();
			return true;
		}
	}
	::debugf("Thing<%p>::%s: unknown flag: '%s'\n", this, __FUNCTION__, flag);
	return false;
}

void
Thing::applyFlag(const char *flag, bool set)
{
	if(!strcmp(flag, "system"))
	{
		if(set)
		{
			_flags = _flags | SYSTEM;
		}
		else
		{
			_flags = _flags & (~SYSTEM);
		}
		return;
	}
	if(!strcmp(flag, "hidden"))
	{
		if(set)
		{
			_flags = _flags | HIDDEN;
		}
		else
		{
			_flags = _flags & (~HIDDEN);
		}
		return;
	}
	if(!strcmp(flag, "fixed"))
	{
		if(set)
		{
			_flags = _flags | FIXED;
		}
		else
		{
			_flags = _flags & (~FIXED);
		}
		return;
	}
	if(!strcmp(flag, "deleted"))
	{
		if(set)
		{
			_flags = _flags | DELETED;
		}
		else
		{
			_flags = _flags & (~DELETED);
		}
		return;
	}
	::debugf("Thing<%p>::%s: unknown flag: '%s'\n", this, __FUNCTION__, flag);
}
