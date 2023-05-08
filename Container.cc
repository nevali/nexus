#include <cstdio>
#include <cstring>

#include "Nexus/Container.hh"
#include "Nexus/Actor.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

Container::Container(json_t *obj):
	Thing(obj),
	_contents(NULL),
	_ncontents(0)
{
}

Container::~Container()
{
	for(size_t n = 0; n < _ncontents; n++)
	{
		if(_contents[n])
		{
			fprintf(stderr, "Container<%p>::%s: releasing contained #%ld%c\n", this, __FUNCTION__, _contents[n]->id(), _contents[n]->typeChar());
			_contents[n]->release();
		}
	}
	free(_contents);
}

/* Return the first object of the specified type (which may be ANY) whose
 * canonical name exactly matches that given
 *
 * This method is only useful in circumstances where the best course of action
 * in the event of a naming clash is to just pick the first match -- this is
 * virtually only the case with certain system objects and is generally not
 * useful behaviour elsewhere
 */
Thing *
Container::firstObjectNamed(const char *name, Thing::TypeID type)
{
	char cname[Database::MAX_CANON_NAME + 1];

	Database::canonicalise(cname, sizeof(cname), name);
	for(size_t n = 0; n < _ncontents; n++)
	{
		if(_contents[n])
		{
			if(type == Thing::ANY || _contents[n]->typeId() == type)
			{
				if(_contents[n]->isCanonicallyNamed(cname))
				{
					_contents[n]->retain();
					return _contents[n];
				}
			}
		}
	}
	return NULL;
}

void
Container::list(Actor *player)
{
	player->send("=[ Contents ]==========================================================\n");
	player->send("   ID         Name\n");
	player->send("-----------------------------------------------------------------------\n");
	for(size_t c = 0; c < _ncontents; c++)
	{
		if(_contents[c])
		{
			player->sendf("  %-10s  %s\n", _contents[c]->ident(), _contents[c]->displayName());
		}
	}
	player->send("=======================================================================\n");
}

void
Container::examineSections(Actor *player)
{
	Thing::examineSections(player);
	if(_ncontents)
	{
		bool first;

		player->send("-[ Contents ]---------------------------------------------------------\n");
		first = true;
		for(size_t c = 0; c < _ncontents; c++)
		{
			if(_contents[c])
			{
				player->sendf("%s%s (%s)", (first ? "" : ", "), _contents[c]->displayName(), _contents[c]->ident());
				first = false;
			}
		}
		player->send("\n");
	}
}

void
Container::unpack(void)
{
	json_t *contents, *value;
	size_t index;

	updateIdent();
	updateFlags();
//	fprintf(stderr, "Container<%p>::%s: unpacking %s (%s)\n", this, __FUNCTION__, name(), ident());
	contents = json_object_get(_obj, "contents");
	if(!contents)
	{
		fprintf(stderr, "- object has no contents\n");
		return;
	}
	if(!json_is_array(contents))
	{
		fprintf(stderr, "- object.contents is not an array\n");
		return;
	}
	expandContents(json_array_size(contents));
	json_array_foreach(contents, index, value)
	{
		ID id;
		Thing *thing;

		if(!json_is_integer(value))
		{
			fprintf(stderr, "- entry #%lu is not an integer\n", index);
			continue;
		}
		id = (ID) json_integer_value(value);
		thing = _universe->thingFromId(id);
		if(!thing)
		{
			fprintf(stderr, "- failed to load thing ID #%ld\n", id);
			continue;
		}
		/* invokes old-location->remove(thing) and this->add(thing) */
		thing->setLocation(this);
		fprintf(stderr, "Container<%p>::%s: unpacked %s (%s)\n", this, __FUNCTION__, thing->name(), thing->ident());
		thing->release();
	}
}

void
Container::add(Thing *thing)
{
	size_t c;
	json_t *carray, *cvalue;
	bool found;

	if(thing->id() == ID_INVALID)
	{
		return;
	}
//	fprintf(stderr, "Container<%p>::%s: adding %s (#%ld%c)\n", this, __FUNCTION__, thing->name(), thing->id(), thing->typeChar());
	if(!(carray = json_object_get(_obj, "contents")))
	{
		carray = json_array();
		json_object_set_new(_obj, "contents", carray);
	}
	found = false;
	json_array_foreach(carray, c, cvalue)
	{
		if(json_integer_value(cvalue) == thing->id())
		{
			found = true;
			break;
		}
	}
	if(!found)
	{
		json_array_append_new(carray, json_integer(thing->id()));
	}
	for(c = 0; c < _ncontents; c++)
	{
		if(_contents[c] == thing)
		{
//			fprintf(stderr, "- already part of this object's contents\n");
			return;
		}
	}
	for(c = 0; c < _ncontents; c++)
	{
		if(!_contents[c])
		{
//			fprintf(stderr, "- adding at position %lu\n", c);
			thing->retain();
			_contents[c] = thing;
			_status |= DIRTY;
			return;
		}
	}
	expandContents(_ncontents + 8);
	if(c >= _ncontents || _contents[c])
	{
		abort();
	}
//	fprintf(stderr, "- adding at final position %lu\n", c);
	thing->retain();
	_contents[c] = thing;
	_status |= DIRTY;
	return;
}

void
Container::remove(Thing *thing)
{
	json_t *carray, *cvalue;
	size_t index;
	bool found;

//	fprintf(stderr, "Container<%p>::%s: removing %s (#%ld%c)\n", this, __FUNCTION__, thing->name(), thing->id(), thing->typeChar());
	if((carray = json_object_get(_obj, "contents")))
	{
		found = false;
		json_array_foreach(carray, index, cvalue)
		{
			if(json_integer_value(cvalue) == thing->id())
			{
				found = true;
				break;
			}
		}
		if(found)
		{
			json_array_remove(carray, index);
		}
	}
	for(size_t c = 0; c < _ncontents; c++)
	{
		if(_contents[c] == thing)
		{
//			fprintf(stderr, "- removing from position %lu\n", c);
			_contents[c] = NULL;
			_status |= DIRTY;
			thing->release();
			return;
		}
	}
}

void
Container::expandContents(size_t newSize)
{
	Thing **p;

	if(_ncontents >= newSize)
	{
		return;
	}
//	fprintf(stderr, "Container<%p>::%s: expanding contents to %lu\n", this, __FUNCTION__, newSize);
	p = (Thing **) realloc(_contents, sizeof(Thing *) * newSize);
	if(!p)
	{
		abort();
	}
	_contents = p;
	p = &(_contents[_ncontents]);
	memset(p, 0, sizeof(Thing *) * (newSize - _ncontents));
	_ncontents = newSize;
}
