#include "WARP/Flux/Array.hh"

#include "Nexus/Channel.hh"
#include "Nexus/Actor.hh"

using namespace Nexus;

Channel::Channel(json_t *source):
	Thing(source),
	_currentMembers(NULL)
{
	_currentMembers = new WARP::Flux::TArray<Actor>();
}

Channel::~Channel()
{
	if(_currentMembers)
	{
		_currentMembers->release();
		_currentMembers = NULL;
	}
}

bool
Channel::connect(Actor *actor)
{
	_currentMembers->add(actor);
	return true;
}

bool
Channel::disconnect(Actor *actor)
{
	_currentMembers->remove(actor);
	return true;
}

void
Channel::send(const char *str)
{
	emit(str);
}

void
Channel::emit(const char *str)
{
	fprintf(stderr, "[%s] %s", displayName(), str);
}
