#include <cstdio>
#include <cstring>

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
#include "Nexus/Channel.hh"

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
		::debugf("Thing::%s: object has an invalid ID: typeId = %c\n", __FUNCTION__, newTypeId);
		json_dumpf(source, stderr, JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ENCODE_ANY);
		::debugf("\n");
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
		case CHANNEL:
			return new Channel(source);
		default:
			::debugf("Thing::%s: object has a missing or unsupported typeId; ID = %ld, typeId = %c\n", __FUNCTION__, newId, newTypeId);
			json_dumpf(source, stderr, JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ENCODE_ANY);
			::debugf("\n");
	}
	return NULL;
}

Thing::TypeID
Thing::typeId(const char *str)
{
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
	if(!strcmp(str, "channel"))
	{
		return Thing::CHANNEL;
	}
	return Thing::INVALID;
}

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
		case EXECUTABLE:
			return "executable";
		case VARIABLE:
			return "variable";
		case SWITCH:
			return "switch";
		case TIMER:
			return "timer";
		case CHANNEL:
			return "channel";
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

Channel *
Thing::asChannel(void) const
{
	if(isChannel())
	{
		return static_cast<Channel *>((Thing *) this);
	}
	return NULL;
}

