#include <cstdio>
#include <cstring>

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
#include "Nexus/Hologram.hh"
#include "Nexus/Executable.hh"
#include "Nexus/Variable.hh"

using namespace Nexus;

const char *
Thing::typeName(Thing::TypeID typeId)
{
	switch(typeId)
	{
		case INVALID:
			return "(invalid)";
		case ANY:
			return "(any)";
		case THING:
			return "thing";
		case CONTAINER:
			return "container";
		case ZONE:
			return "zone";
		case ROOM:
			return "room";
		case PORTAL:
			return "portal";
		case ACTOR:
			return "actor";
		case PLAYER:
			return "player";
		case ROBOT:
			return "robot";
		case HOLOGRAM:
			return "hologram";
		case EXECUTABLE:
			return "executable";
		case VARIABLE:
			return "variable";
		case SWITCH:
			return "switch";
		case TIMER:
			return "timer";
	}
	return "(unknown)";
}

const char *
Thing::typeStr(void) const
{
	return typeName(typeId());
}

char
Thing::typeChar(void) const
{
	return (char) typeId();
}

Actor *
Thing::asActor(void) const
{
	if(isActor())
	{
		return static_cast<Actor *>((Thing *) this);
	}
	return NULL;
}

Player *
Thing::asPlayer(void) const
{
	if(isPlayer())
	{
		return static_cast<Player *>((Thing *) this);
	}
	return NULL;
}

Robot *
Thing::asRobot(void) const
{
	if(isRobot())
	{
		return static_cast<Robot *>((Thing *) this);
	}
	return NULL;
}

Hologram *
Thing::asHologram(void) const
{
	if(isHologram())
	{
		return static_cast<Hologram *>((Thing *) this);
	}
	return NULL;
}

Portal *
Thing::asPortal(void) const
{
	if(isPortal())
	{
		return static_cast<Portal *>((Thing *) this);
	}
	return NULL;
}

Room *
Thing::asRoom(void) const
{
	if(isRoom())
	{
		return static_cast<Room *>((Thing *) this);
	}
	return NULL;
}

Container *
Thing::asContainer(void) const
{
	if(isContainer())
	{
		return static_cast<Container *>((Thing *) this);
	}
	return NULL;
}

Zone *
Thing::asZone(void) const
{
	if(isZone())
	{
		return static_cast<Zone *>((Thing *) this);
	}
	return NULL;
}

Executable *
Thing::asExecutable(void) const
{
	if(isExecutable())
	{
		return static_cast<Executable *>((Thing *) this);
	}
	return NULL;
}

Variable *
Thing::asVariable(void) const
{
	if(isVariable())
	{
		return static_cast<Variable *>((Thing *) this);
	}
	return NULL;
}

