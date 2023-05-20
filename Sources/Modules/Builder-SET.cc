#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
SET::execute(Actor *actor)
{
	Thing *thing;
	const char *property, *newValue;

	if(argc() < 3 || argc() > 4)
	{
		actor->send("Usage: @SET TARGET PROPERTY [NEW-VALUE]\n");
		return false;
	}
	property = argv(2);
	if(argc() > 3)
	{
		newValue = argv(3);
	}
	else
	{
		newValue = NULL;
	}
	if(!(thing = actor->resolveIdOrBuiltin(argv(1))))
	{
		actor->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	if(thing->set(actor, property, newValue))
	{
		thing->release();
		return true;
	}
	if(newValue)
	{
		actor->sendf("Sorry, I can't set the %s of %s (%s) to '%s'\n", property, thing->displayName(), thing->ident(), newValue);
	}
	else
	{
		actor->sendf("Sorry, I can't remove the %s of %s (%s)\n", property, thing->displayName(), thing->ident());
	}
	thing->release();
	return false;
}
