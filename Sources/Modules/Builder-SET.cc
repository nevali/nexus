#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
SET::execute(ExecutionContext *ctx)
{
	Thing *thing;
	const char *property, *newValue;

	if(argc() < 3 || argc() > 4)
	{
		ctx->who->send("Usage: @SET TARGET PROPERTY [NEW-VALUE]\n");
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
	if(!(thing = ctx->who->resolveIdOrBuiltin(argv(1))))
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	if(thing->set(ctx->who, property, newValue))
	{
		thing->release();
		return true;
	}
	if(newValue)
	{
		ctx->who->sendf("Sorry, I can't set the %s of %s (%s) to '%s'\n", property, thing->displayName(), thing->ident(), newValue);
	}
	else
	{
		ctx->who->sendf("Sorry, I can't remove the %s of %s (%s)\n", property, thing->displayName(), thing->ident());
	}
	thing->release();
	return false;
}
