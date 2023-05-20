#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Universe.hh"
#include "Nexus/Variable.hh"
#include "Nexus/Channel.hh"

using namespace WARP::Flux::Diagnostics;
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

Channel *
Universe::systemChannelWithName(const char *name)
{
	if(_system)
	{
		return _system->channelWithName(name);
	}
	return NULL;
}


Channel *
Universe::debugChannel(void)
{
	if(_debugChannel)
	{
		_debugChannel->retain();
		return _debugChannel;
	}
	return NULL;
}

Channel *
Universe::auditChannel(void)
{
	if(_auditChannel)
	{
		_auditChannel->retain();
		return _auditChannel;
	}
	return NULL;
}

Channel *
Universe::globalChannel(void)
{
	if(_globalChannel)
	{
		_globalChannel->retain();
		return _globalChannel;
	}
	return NULL;
}

void
Universe::auditf(const char *format, ...)
{
	va_list ap;

	if(_auditChannel)
	{
		va_start(ap, format);
		_auditChannel->vemitf(format, ap);
		va_end(ap);
	}
}

void
Universe::debugf(const char *format, ...)
{
	va_list ap;

	if(_debugChannel)
	{
		va_start(ap, format);
		_debugChannel->vemitf(format, ap);
		va_end(ap);
	}
}

void
Universe::diagf(DiagnosticLevel level, const char *format, ...)
{
	va_list ap;

	(void) level;

	if(_debugChannel)
	{
		va_start(ap, format);
		_debugChannel->vemitf(format, ap);
		va_end(ap);
	}
}

void
Universe::diagnosticCallback(const Diagnostic *diag, void *context)
{
	Universe *self = static_cast<Universe *>(context);

	if(diag->level() < DIAG_DEBUG || diag->level() < diagnosticLevel())
	{
		return;
	}
	if(self->_debugChannel)
	{
		self->_debugChannel->emit(diag->message());
	}
	if(diag->level() > DIAG_NOTICE)
	{
		fprintf(stderr, "%s", diag->message());
		fflush(stderr);
	}
}
