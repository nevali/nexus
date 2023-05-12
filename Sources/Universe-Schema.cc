#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <jansson.h>

#include "Nexus/Database.hh"
#include "Nexus/Thing.hh"
#include "Nexus/Player.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Variable.hh"
#include "Nexus/Room.hh"

using namespace Nexus;

bool
Universe::migrateTo(unsigned newVersion)
{
	if(newVersion == 1)
	{
		fprintf(stderr, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 2)
	{
		fprintf(stderr, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);

		if(!_operator)
		{
			Thing *thing = _db->objectFromId(1);

			if(thing)
			{
				if(!thing->isPlayer())
				{
					fprintf(stderr, "Universe::%s: object %s was expected to be a Player\n", __FUNCTION__, thing->ident());
					thing->release();
					return false;
				}
				_operator = thing->asPlayer();
			}
			else
			{
				_operator = newPlayer("Operator");
				if(!_operator)
				{
					fprintf(stderr, "Universe::%s: failed to create new 'Operator' Player object\n", __FUNCTION__);
					return false;
				}
				_operator->setId(1);
				if(_operator->id() != 1)
				{
					fprintf(stderr, "Universe::%s: failed to set Operator's ID to #1P (is %s)\n", __FUNCTION__, _operator->ident());
					return false;
				}
			}
		}
		if(!_operator->setFlag("system"))
		{
			fprintf(stderr, "Universe::%s: failed to set Operator (%s) as SYSTEM\n", __FUNCTION__, _operator->ident());
			return false;
		}
		if(!_operator->setFlag("immortal"))
		{
			fprintf(stderr, "Universe::%s: failed to set Operator (%s) as IMMORTAL\n", __FUNCTION__, _operator->ident());
			return false;
		}
		_operator->dumpf(stderr);
		if(!_operator->sync())
		{
			fprintf(stderr, "Universe::%s: failed to sync Operator\n", __FUNCTION__);
			return false;
		}
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 3)
	{
		fprintf(stderr, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		if(!json_object_get(_meta, "name"))
		{
			json_object_set_new(_meta, "name", json_string("Unnamed Universe"));
		}
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 4)
	{
		fprintf(stderr, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		if(!_root)
		{
			Thing *thing = _db->objectFromId(2);

			if(thing)
			{
				if(!thing->isZone())
				{
					fprintf(stderr, "Universe::%s: object %s was expected to be a Zone\n", __FUNCTION__, thing->ident());
					thing->release();
					return false;
				}
				_root = thing->asZone();
			}
			else
			{
				_root = newZone("Root");
				if(!_root)
				{
					fprintf(stderr, "Universe::%s: failed to create new 'Root' Container object\n", __FUNCTION__);
					return false;
				}
				if(!_root->setId(2))
				{
					fprintf(stderr, "Universe::%s: failed to set Root's ID to #2 (is %s)\n", __FUNCTION__, _root->ident());
					return false;
				}
				_root->setDescription(
					"You find yourself at the root of this particular universe.\n"
					"\n"
					"Almost every object exists within this one.\n"
					);
			}
		}
		if(!_root->setFlag("system"))
		{
			fprintf(stderr, "Universe::%s: failed to set Root (%s) as SYSTEM\n", __FUNCTION__, _root->ident());
			return false;
		}
		if(!_root->setFlag("fixed"))
		{
			fprintf(stderr, "Universe::%s: failed to set Root (%s) as FIXED\n", __FUNCTION__, _root->ident());
			return false;
		}
		if(!_root->setFlag("hidden"))
		{
			fprintf(stderr, "Universe::%s: failed to set Root (%s) as HIDDEN\n", __FUNCTION__, _root->ident());
			return false;
		}
//		_root->dumpf(stderr);
		if(!_root->sync())
		{
			fprintf(stderr, "Universe::%s: failed to sync Root\n", __FUNCTION__);
			return false;
		}
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 5)
	{
		fprintf(stderr, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);

		if(!_system)
		{		
			_system = newZone("System");
			if(!_system)
			{
				fprintf(stderr, "Universe::%s: failed to create a new Zone named 'System'\n", __FUNCTION__);
				return false;
			}
			if(!_system->setId(3))
			{
				fprintf(stderr, "Universe::%s: failed to set System's ID to #3 (is %s)\n", __FUNCTION__, _system->ident());
				return false;
			}
			if(!_system->setLocation(_root))
			{
				fprintf(stderr, "Universe::%s: failed to set System's (%s) location to Root\n", __FUNCTION__, _system->ident());
			}
		}
		if(!_system->setFlag("system"))
		{
			fprintf(stderr, "Universe::%s: failed to set System (%s) as SYSTEM\n", __FUNCTION__, _system->ident());
			return false;
		}
		if(!_system->setFlag("fixed"))
		{
			fprintf(stderr, "Universe::%s: failed to set System (%s) as FIXED\n", __FUNCTION__, _system->ident());
			return false;
		}
		if(!_system->sync())
		{
			fprintf(stderr, "Universe::%s: failed to sync System (%s)\n", __FUNCTION__, _system->ident());
			return false;
		}
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 6)
	{
		Thing *thing;

		fprintf(stderr, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		
		thing = systemObjectNamed("messages");
		if(!thing)
		{
			thing = newContainer("Messages");
			thing->setId(4);
			thing->setLocation(_system);
			thing->setDescription("This is the Messages container.\n\nThe descriptions of objects within this container are displayed at\ncertain times, such as when a Player begins a new session.\n");
		}
		if(!thing->setFlag("system"))
		{
			fprintf(stderr, "Universe::%s: failed to set Messages (%s) as SYSTEM\n", __FUNCTION__, thing->ident());
			return false;
		}
		if(!thing->setFlag("fixed"))
		{
			fprintf(stderr, "Universe::%s: failed to set Messages (%s) as FIXED\n", __FUNCTION__, thing->ident());
			return false;
		}
		if(!thing->sync())
		{
			fprintf(stderr, "Universe::%s: failed to sync Messges Container\n", __FUNCTION__);
			return false;
		}
		thing->sync();
		thing->release();
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 7)
	{
		Container *messages;
		Thing *thing;
		Variable *var;

		fprintf(stderr, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		
		thing = systemObjectNamed("messages");
		if(!thing)
		{
			fprintf(stderr, "Universe::%s: cannot find 'messages' system object\n", __FUNCTION__);
			return false;
		}
		messages = thing->asContainer();
		if(!messages)
		{
			fprintf(stderr, "Universe::%s: 'messages' system object (%s) is not a container\n", __FUNCTION__, thing->ident());
			thing->release();
			return false;
		}
		var = newVariable("connect", true, messages);
		var->setFlag("system");
		var->setFlag("fixed");
		var->setDescription("This message is displayed when a player connects to the universe");
		var->setText("\n\n  Welcome to ...\n\n\n    U N N A M E D    U N I V E R S E  ! ! !\n\n\n  [Please edit this message to suit your needs]\n");
		var->release();
		var = newVariable("login-open", true, messages);
		var->setFlag("system");
		var->setFlag("fixed");
		var->setDescription("This prompt is displayed to a player when they first connect to the universe, to determine whether they are a returning player or not");
		var->setText("Please enter your name, or 'new' if you are a visitor: ");
		var->release();
		var = newVariable("login-closed", true, messages);
		var->setFlag("system");
		var->setFlag("fixed");
		var->setDescription("This prompt is displayed to a player when they first connect to the universe when only returning players are permitted to connect");
		var->setText("Please enter your name: ");
		var->release();
		messages->release();
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}

	fprintf(stderr, "Database::%s: unsupported version %u\n", __FUNCTION__, newVersion);
	return false;
}
