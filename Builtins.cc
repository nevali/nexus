#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "Nexus/Universe.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Actor.hh"
#include "Nexus/Player.hh"

using namespace Nexus;

DECLARE_BUILTIN(COMMANDS);
DECLARE_BUILTIN(CREATE);
DECLARE_BUILTIN(DESTROY);
DECLARE_BUILTIN(DUMP);
DECLARE_BUILTIN(EXAMINE);
DECLARE_BUILTIN(LIST);
DECLARE_BUILTIN(RENAME);
DECLARE_BUILTIN(QUIT);
DECLARE_BUILTIN(TELEPORT);

BuiltinsParser::BuiltinsParser(Universe *universe):
	Parser(universe)
{
	CommandEntry c[] = {
		{ "BYE", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, QUIT::construct, NULL },

		{ "COMMANDS", CommandEntry::NONE, COMMANDS::construct, "List available commands" },
		{ "CREATE", CommandEntry::NONE, CREATE::construct, "Create a new object" },
		
		{ "DESTROY", CommandEntry::NONE, DESTROY::construct, "Destroy an object" },
		{ "DUMP", CommandEntry::NONE, DUMP::construct, "Display the contents of an object as JSON" },
		
		{ "EXAMINE", CommandEntry::NONE, EXAMINE::construct, "Examine an object" },
		{ "EXIT", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, QUIT::construct, NULL },

		{ "HELP", CommandEntry::NONE, NULL, "Browse the built-in Operator's Manual" },

		{ "LIST", CommandEntry::NONE, LIST::construct, "List the contents of a container" },
		{ "LOGOUT", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, QUIT::construct, NULL },

		{ "RENAME", CommandEntry::NONE, RENAME::construct, "Rename an object" },

		{ "TELEPORT", CommandEntry::NONE, TELEPORT::construct, "Move an object from one container to another" },
		
		{ "QUIT", CommandEntry::UNAMBIGUOUS, QUIT::construct, "End your session" },
		{ NULL, CommandEntry::NONE, NULL, NULL }
	};
	add(c);
}

Command *
BuiltinsParser::parse(Actor *who, const char *commandLine)
{
	char cmdbuf[32];
	const char *start;
	CommandEntry *partial, *exact;
	Command *command;
	bool clash;
	size_t c;

	command = NULL;
	partial = NULL;
	exact = NULL;
	clash = NULL;
	/* All built-in start with an '@' character followed by one or more
	 * alphabetic characters; for our purposes we consider anything from
	 * the '@' to the first whitespace OR forward-slash to the command
	 * name
	 */
	if(!commandLine || *commandLine != '@')
	{
		return NULL;
	}
	start = commandLine;
	commandLine++;
	for(c = 0; *commandLine && c < sizeof(cmdbuf) - 1; commandLine++)
	{
		if(!*commandLine || *commandLine == '/' || isblank(*commandLine) || *commandLine == '\r' || *commandLine == '\n')
		{
			break;
		}
		cmdbuf[c] = *commandLine;
		c++;
	}
	cmdbuf[c] = 0;

	/* commands are partial-matched unless flagged as being full-match-only
	 * an exact match always takes precedence
	 */
	for(c = 0; c < _ncommands; c++)
	{
		if(!strcasecmp(_commands[c].name, cmdbuf))
		{
			exact = &(_commands[c]);
			break;
		}
		if(!(_commands[c].flags & CommandEntry::UNAMBIGUOUS))
		{
			if(!strncasecmp(_commands[c].name, cmdbuf, strlen(cmdbuf)))
			{
				if(partial)
				{
					clash = true;
				}
				else
				{
					partial = &(_commands[c]);
				}
			}
		}
	}
	if(!exact)
	{
		/* if there was more than one partial match, we list the available
		* matches
		*/
		if(clash)
		{
			who->sendf("Multiple command matches for '@%s':\n", cmdbuf);
			return NULL;
		}
		/* if there was exactly one partial match, use it */
		if(partial)
		{
			exact = partial;
		}
		else
		{
			who->sendf("Sorry, the command '@%s' was not recognised.\n", cmdbuf);
			return NULL;
		}
	}
	/* once we've located the matching command entry, invoke its
	 * constructor callback to obtain a Command instance
	 */
	if(!exact->constructor)
	{
		who->sendf("Sorry, the '@%s' command is not implemented.\n", exact->name);
		return NULL;
	}
	command = exact->constructor(this, start);
	if(!command)
	{
		fprintf(stderr, "BuiltinsParser::%s: constructor for command '%s' failed\n", __FUNCTION__, exact->name);
		return NULL;
	}
	/* now we have the Command instance, request it parse its arguments */
	if(!command->parse(who))
	{
		command->release();
		return NULL;
	}
	/* parsing succeeded, return the Command so that it can be executed */
	return command;
}

bool
COMMANDS::execute(Actor *actor)
{
	actor->send("=[ Built-in Commands ]=================================================\n");
	for(size_t c = 0 ; c < _parser->commandCount(); c++)
	{
		CommandEntry *entry = _parser->commandAtIndex(c);
		if(!entry || (entry->flags & CommandEntry::HIDDEN))
		{
			continue;
		}
		actor->sendf("  @%-20s  %s\n", entry->name, (entry->desc ? entry->desc : ""));
	}	
	actor->send("=======================================================================\n");
	return true;
}


bool
CREATE::execute(Actor *actor)
{
	const char *name;

	if(_argc < 2 || _argc > 3 || _argv[1][0] != '/')
	{
		actor->send("Usage: @CREATE /TYPE [NAME]\n");
		return false;
	}
	if(_argc > 2)
	{
		name = _argv[2];
	}
	else
	{
		name = NULL;
	}
	if(!strcasecmp(_argv[1], "/zone"))
	{
		Zone *newZone, *parent = actor->zone();

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
			actor->send("Cannot create a new zone because you do are not in a zone\n");
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
		if(newZone)
		{
			newZone->setOwner(actor);
			actor->sendf("New Zone %s (%s) created within %s (%s)\n", newZone->displayName(), newZone->ident(), parent->displayName(), parent->ident());
			newZone->release();
			parent->release();
			return true;			
		}
		parent->release();
		return false;
	}
	actor->sendf("Sorry, I don't know how to create a '%s'\n", _argv[1] + 1);
	return false;
}

bool
QUIT::execute(Actor *actor)
{
	actor->send("Goodbye!\n");
	if(actor->isPlayer())
	{
		actor->asPlayer()->disconnect();
	}
	return true;
}

bool
EXAMINE::execute(Actor *actor)
{
	Thing *thing;
	bool r;

	if(_argc > 2)
	{
		actor->send("Usage: @EXAMINE [WHAT]\n");
		return false;
	}
	if(_argc < 2)
	{
		actor->examine(actor);
		return true;
	}
	thing = actor->resolveTarget(_argv[1]);
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	r = true;
	thing->examine(actor);
	thing->release();
	return r;
}

bool
DESTROY::execute(Actor *actor)
{
	Thing *thing;
	bool r;

	if(_argc != 2)
	{
		actor->send("Usage: @DESTROY WHAT\n");
		return false;
	}
	thing = actor->resolveTarget(_argv[1]);
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	r = true;
	thing->destroy(actor);
	thing->release();
	return r;
}

bool
DUMP::execute(Actor *actor)
{
	Thing *thing;

	if(_argc > 2)
	{
		actor->send("Usage: @EXAMINE [WHAT]\n");
		return false;
	}
	if(_argc < 2)
	{
		actor->dump(actor);
		return true;
	}
	thing = actor->resolveTarget(_argv[1]);
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	thing->dump(actor);
	thing->release();
	return true;
}

bool
LIST::execute(Actor *actor)
{
	Thing *thing;

	if(_argc > 2)
	{
		actor->send("Usage: @EXAMINE [WHAT]\n");
		return false;
	}
	if(_argc < 2)
	{
		thing = actor->location();
		thing->list(actor);
		thing->release();
		return true;
	}
	thing = actor->resolveTarget(_argv[1]);
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	thing->list(actor);
	thing->release();
	return true;
}

bool
RENAME::execute(Actor *actor)
{
	Thing *thing;
	const char *newName;

	if(_argc < 2 || _argc > 3)
	{
		actor->send("Usage: @RENAME WHAT [NEW-NAME]\n");
		return false;
	}
	if(_argc > 2)
	{
		newName = _argv[2];
		if(!newName[0])
		{
			newName = NULL;
		}
	}
	else
	{
		newName = NULL;
	}
	if(!(thing = actor->resolveTarget(_argv[1])))
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	if(thing->setName(newName))
	{
		thing->release();
		return true;
	}
	if(newName)
	{
		actor->sendf("Sorry, I can't rename %s (%s) to '%s'\n", thing->displayName(), thing->ident(), newName);
	}
	else
	{
		actor->sendf("Sorry, I can't remove the name of %s (%s)\n", thing->displayName(), thing->ident());
	}
	return true;
}

bool
TELEPORT::execute(Actor *actor)
{
	Thing *thing, *dest;
	Container *container;

	if(_argc != 3)
	{
		actor->send("Usage: @TELEPORT WHAT WHERE\n");
		return false;
	}
	if(!(thing = actor->resolveTarget(_argv[1])))
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	if(!(dest = actor->resolveTarget(_argv[2])))
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[2]);
		thing->release();
		return false;
	}
	if(!dest->isContainer())
	{
		actor->sendf("%s (%s) is not a container\n", dest->displayName(), dest->ident());
		dest->release();
		thing->release();
	}
	container = dest->asContainer();
	dest = NULL;
	thing->teleport(actor, container);
	container->release();
	thing->release();
	return true;
}
