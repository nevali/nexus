#include <cstdio>
#include <cstring>

#include <jansson.h>

#include "Nexus/Thing.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Actor.hh"

using namespace Nexus;

void
Thing::dump(Actor *player)
{
	(void) player;

	dumpf(stderr);
}

void
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
}

void
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

void
Thing::list(Actor *who)
{
	who->send("It can't contain anything.\n");
}

void
Thing::teleport(Actor *who, Container *dest)
{
	if(setLocation(dest))
	{
		if(id() != ID_INVALID && id() == who->id())
		{
			dest->look(who);
		}
		return;
	}

}

void
Thing::destroy(Actor *who)
{
	bool prev;

	if(flags() & SYSTEM)
	{
		who->sendf("Sorry, system object %s cannot be @DESTROYed\n", ident());
		return;
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
}

void
Thing::flag(Actor *who, const char *flag, bool onOff)
{
	if(setFlag(flag, onOff))
	{
		who->send("Flags updated\n");
	}
}

