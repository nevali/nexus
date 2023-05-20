#include <cstring>

#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Zone.hh"
#include "Nexus/Thing.hh"
#include "Nexus/Portal.hh"
#include "Nexus/Room.hh"
#include "Nexus/Variable.hh"

#include "Builder.hh"

using namespace Nexus::Modules::Builder;

bool
CREATE::execute(ExecutionContext *ctx)
{
	const char *name;

	if(argc() < 2 || argc() > 3 || argv(1)[0] != '/')
	{
		ctx->who->send("Usage: @CREATE /TYPE [NAME]\n");
		return false;
	}
	if(argc() > 2)
	{
		name = argv(2);
	}
	else
	{
		name = NULL;
	}
	if(!strcasecmp(argv(1), "/thing"))
	{
		return createThing(ctx, name);
	}
	if(!strcasecmp(argv(1), "/zone"))
	{
		return createZone(ctx, name);
	}
	if(!strcasecmp(argv(1), "/room"))
	{
		return createRoom(ctx, name);
	}
	if(!strcasecmp(argv(1), "/portal"))
	{
		return createPortal(ctx, name);
	}
	ctx->who->sendf("Sorry, I don't know how to create a '%s'\n", argv(1) + 1);
	return false;
}

bool
CREATE::createZone(ExecutionContext *ctx, const char *name)
{
	Zone *newZone, *parent = ctx->who->zone();
	Room *entrance;
	Variable *settings;
	json_t *data;

	if(!parent)
	{
		parent = _universe->rootZone();
	}
	if(!parent)
	{
		parent = _universe->limbo();
	}
	if(!parent)
	{
		ctx->who->send("Cannot create a new zone because you do are not in a zone\n");
		return false;
	}
	if(parent->id() == Thing::ID_LIMBO)
	{
		/* if the actor is currently in Limbo, create the new zone in the root instead */
		newZone = _universe->rootZone();
		if(newZone)
		{
			parent->release();
			parent = newZone;
		}
	}
	newZone = _universe->newZone(name, true, parent);
	if(!newZone)
	{
		ctx->who->sendf("Unable to create new Zone within %s (%s)\n", parent->displayName(), parent->ident());
		parent->release();
		return false;
	}
	newZone->setOwner(ctx->who);
	newZone->setDescription("This is a brand new zone which can be customised to your needs.\n");
	ctx->who->sendf("New Zone %s (%s) created within %s (%s)\n", newZone->displayName(), newZone->ident(), parent->displayName(), parent->ident());
	entrance = _universe->newRoom("Entrance", true, newZone);
	if(!entrance)
	{
		ctx->who->sendf("Failed to create entrance to new Zone %s (%s)\n", newZone->displayName(), newZone->ident());
	}
	else
	{
		entrance->setOwner(ctx->who);
		entrance->setDescription(
			"Greetings, traveller! Welcome to this brand new entrance room, in\n"
			"your brand new zone!\n"
			"\n"
			"There's not a speck of dust, mainly because there isn't, well,\n"
			"anything: it's extremely dull in here, but at least the paint smells\n"
			"fresh.\n"
			"\n"
			"You should edit this description as soon as you can, lest you expire\n"
			"from boredom... or paint fumes.\n"
		);
		ctx->who->sendf("New Room %s (%s) created within %s %s\n", entrance->displayName(), entrance->ident(), newZone->displayName(), newZone->ident());
	}
	settings = _universe->newVariable("Settings", true, newZone);
	if(!settings)
	{
		ctx->who->sendf("Failed to create Settings object in new Zone %s (%s)\n", newZone->displayName(), newZone->ident());
	}
	else
	{
		settings->setOwner(ctx->who);
		data = json_object();
		if(entrance)
		{
			json_object_set_new(data, "entrance", json_integer(entrance->id()));
		}
		settings->setValue(data);
	}
	/* Clean up */
	if(settings)
	{
		settings->release();
	}
	if(entrance)
	{
		entrance->release();
	}
	newZone->release();
	parent->release();
	return true;
}

bool
CREATE::createRoom(ExecutionContext *ctx, const char *name)
{
	Zone *parent = ctx->who->zone();
	Room *newRoom;

	if(!parent)
	{
		ctx->who->send("Cannot create a new room because you do are not in a zone\n");
		return false;
	}
	newRoom = _universe->newRoom(name, true, parent);
	if(!newRoom)
	{
		ctx->who->sendf("Unable to create new Room within %s (%s)\n", parent->displayName(), parent->ident());
		parent->release();
		return false;
	}
	newRoom->setOwner(ctx->who);
	newRoom->setDescription("This is a brand new room, ready for decorating!\n");
	ctx->who->sendf("New Room %s (%s) created within %s (%s)\n", newRoom->displayName(), newRoom->ident(), parent->displayName(), parent->ident());
	newRoom->release();
	parent->release();
	return true;			
}

bool
CREATE::createPortal(ExecutionContext *ctx, const char *name)
{
	Container *parent = ctx->who->location();
	Portal *newPortal;

	if(!parent)
	{
		ctx->who->send("Cannot create a new portal because you do not seem to be anywhere\n");
		return false;
	}
	newPortal = _universe->newPortal(name, true, parent);
	if(!newPortal)
	{
		ctx->who->sendf("Unable to create new Portal within %s (%s)\n", parent->displayName(), parent->ident());
		parent->release();
		return false;
	}
	newPortal->setOwner(ctx->who);
	ctx->who->sendf("New Portal %s (%s) created within %s (%s)\n", newPortal->displayName(), newPortal->ident(), parent->displayName(), parent->ident());
	newPortal->release();
	parent->release();
	return true;
}

bool
CREATE::createThing(ExecutionContext *ctx, const char *name)
{
	Thing *newThing;

	newThing = _universe->newThing(name, true, ctx->who);
	if(!newThing)
	{
		ctx->who->sendf("Unable to create new Thing within %s (%s)\n", ctx->who->displayName(), ctx->who->ident());
		return false;
	}
	newThing->setOwner(ctx->who);
	ctx->who->sendf("New Thing %s (%s) created within %s (%s)\n", newThing->displayName(), newThing->ident(), ctx->who->displayName(), ctx->who->ident());
	newThing->release();
	return true;
}
