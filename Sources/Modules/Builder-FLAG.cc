#include <cstring>

#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
FLAG::execute(ExecutionContext *ctx)
{
	Thing *thing;
	const char *flag;
	bool onOff;

	if(argc() < 3 || argc() > 4)
	{
		ctx->who->send("Usage: @FLAG TARGET FLAG [on|off]\n");
		return false;
	}
	flag = argv(2);
	if(argc() > 3)
	{
		if(!strcasecmp(argv(3), "on") || !strcasecmp(argv(3), "true"))
		{
			onOff = true;
		}
		else if(!strcasecmp(argv(3), "off") || !strcasecmp(argv(3), "false"))
		{
			onOff = false;
		}
		else
		{
			ctx->who->sendf("Sorry, I don't understand '%s'\n", argv(3));
			return false;
		}
	}
	else
	{
		onOff = true;
	}
	if(!(thing = ctx->who->resolveIdOrBuiltin(argv(1))))
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	if(thing->flag(ctx->who, flag, onOff))
	{
		thing->release();
		return true;
	}
	ctx->who->sendf("Sorry, I can't set the %s flag of %s (%s)\n", flag, thing->displayName(), thing->ident());
	thing->release();
	return false;
}
