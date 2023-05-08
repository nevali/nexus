#include <cstdio>
#include <cstring>

#include <jansson.h>

#include "Nexus/Thing.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Actor.hh"

using namespace Nexus;

bool
Thing::dump(Actor *player)
{
	(void) player;

	dumpf(stderr);
	return true;
}

bool
Thing::look(Actor *player)
{
	const char *desc;
	
	if((desc = description()))
	{
		player->sendf("%s\n", desc);
	}
	else
	{
		player->sendf("You are in %s\n", displayName());
	}
	return true;
}

bool
Thing::examine(Actor *player)
{
	Thing *loc;
	Actor *own;
	Zone *z;

	player->send("=======================================================================\n");
	player->sendf("%s (%s) [%s]\n", displayName(), ident(), flagsStr());
	player->send("-----------------------------------------------------------------------\n");
	player->sendf(" %-20s : %s\n", "ID", ident());
	player->sendf(" %-20s : %s\n", "Type", typeStr());
	player->sendf(" %-20s : %s\n", "Flags", flagsStr());

	if((loc = location()))
	{
		player->sendf(" %-20s : %s (%s)\n", "Location", loc->displayName(), loc->ident());
		loc->release();
	}
	if((z = zone()))
	{
		player->sendf(" %-20s : %s (%s)\n", "Zone", z->displayName(), z->ident());
		z->release();
	}
	if((own = owner()))
	{
		player->sendf(" %-20s : %s (%s)\n", "Owner", own->displayName(), own->ident());
		own->release();
	}
	if(name())
	{
		player->sendf(" %-20s : %s [%s]\n", "Name", name(), canonicalName());
	}
	examineSections(player);
	player->send("=======================================================================\n");
	return true;
}

void
Thing::examineSections(Actor *player)
{
	const char *desc;

	if((desc = description()))
	{
		player->send("-[ Description ]-------------------------------------------------------\n");
		player->sendf("%s\n", desc);
	}
}

bool
Thing::list(Actor *who)
{
	who->send("It can't contain anything.\n");
	return true;
}

bool
Thing::teleport(Actor *who, Container *dest)
{
	if(flags() & FIXED)
	{
		who->sendf("%s (%s) is FIXED\n", displayName(), ident());
		return false;
	}
	if(setLocation(dest))
	{
		if(id() != ID_INVALID && id() == who->id())
		{
			/* this should be a trigger */
			dest->look(who);
		}
		return true;
	}
	who->sendf("Cannot teleport %s (%s) to %s (%s)\n", who->displayName(), who->ident(), dest->displayName(), dest->ident());
	return false;
}

bool
Thing::destroy(Actor *who)
{
	bool prev;

	if(flags() & SYSTEM)
	{
		who->sendf("System object %s cannot be @DESTROYed\n", ident());
		return false;
	}
	prev = (flags() & DELETED);
	setLocation(Thing::ID_LIMBO);
	json_object_set_new(_obj, "deleted", json_true());
	markDirty();
	setFlag("deleted");
	sync();
	if(prev)
	{
		who->sendf("%s (%s) has already been destroyed\n", displayName(), ident());
	}
	else
	{
		who->sendf("%s (%s) destroyed\n", displayName(), ident());
	}
	return true;
}

bool
Thing::flag(Actor *who, const char *flag, bool onOff)
{
	if(flags() & SYSTEM)
	{
		who->sendf("System objects' flags cannot be modified\n");
		return false;
	}
	if(!strcasecmp(flag, "deleted"))
	{
		who->sendf("An object cannot be deleted with @FLAG; use @DESTROY instead\n");
		return false;
	}
	if(setFlag(flag, onOff))
	{
		return true;
	}
	who->sendf("Cannot modify the '%s' flag of %s (%s)\n", flag, displayName(), ident());
	return false;
}

bool
Thing::set(Actor *who, const char *name, const char *value)
{
	if(flags() & SYSTEM)
	{
		who->sendf("System objects' properties cannot be modified\n");
		return false;
	}
	if(!name)
	{
		return false;
	}
	/* User properties begin with a $ and are just set on the JSON object */
	if(name[0] == '$')
	{
		char cname[Database::MAX_CANON_NAME + 2];

		cname[0] = '$';
		Database::canonicalise(&(cname[1]), sizeof(cname) - 1, &name[1]);
		if(!cname[1])
		{
			who->sendf("Invalid property name '%s'\n", name);
			return false;
		}
		json_object_set_new(_obj, cname, json_string(value));
		markDirty();
		return true;
	}
	if(!strcasecmp(name, "name"))
	{
		return setName(value);
	}
	if(!strcasecmp(name, "description"))
	{
		return setDescription(value);
	}
	if(!strcasecmp(name, "flags"))
	{
		who->sendf("Flags cannot be set with @SET; use @FLAG instead\n");
		return false;
	}
	if(!strcasecmp(name, "status"))
	{
		who->sendf("Status values cannot be modified with @SET\n");
		return false;
	}
	if(!strcasecmp(name, "deleted"))
	{
		who->sendf("An object cannot be deleted with @SET; use @DESTROY instead\n");
		return false;
	}
	who->sendf("Unknown property '%s' on %s (%s)\n", name, displayName(), ident());
	return false;
}
