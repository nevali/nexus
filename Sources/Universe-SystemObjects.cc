#include "Nexus/Universe.hh"
#include "Nexus/Variable.hh"

using namespace Nexus;

/* Return the root zone */
Zone *
Universe::rootZone(void) const
{
	_root->retain();
	return _root;
}

/* Return the system zone */
Zone *
Universe::systemZone(void) const
{
	_system->retain();
	return _system;
}

/* Return Limbo */
Limbo *
Universe::limbo(void) const
{
	_limbo->retain();
	return _limbo;
}

/* Return the system object named 'name' */
Thing *
Universe::systemObjectNamed(const char *name)
{
	(void) name;

	if(!_system)
	{
		return NULL;
	}
	return _system->firstObjectNamed(name, Thing::ANY);
}

/* Return the message object named 'name' */
Variable *
Universe::messageObjectNamed(const char *name)
{
	Thing *messages, *thing;
	Container *cm;

	thing = NULL;
	if((messages = systemObjectNamed("messages")))
	{
		if((cm = messages->asContainer()))
		{
			thing = cm->firstObjectNamed(name, Thing::VARIABLE);
		}
		messages->release();
	}
	return thing->asVariable();
}

/* Return the message text named 'name' */
const char *
Universe::messageNamed(const char *name)
{
	Variable *object;
	const char *desc;

	desc = NULL;
	if((object = messageObjectNamed(name)))
	{
		desc = object->textValue();
		/* XXX technically this is use-after-free: it assumes that the message
		 * resides within _system, which logically must be true, but if that
		 * somehow ceases to be the case, then desc will be invalid
		 * following this call
		 */
		object->release();
	}
	return desc;
}
