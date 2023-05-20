#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <jansson.h>

#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Database.hh"
#include "Nexus/Thing.hh"
#include "Nexus/Player.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Variable.hh"
#include "Nexus/Room.hh"
#include "Nexus/Channel.hh"

using namespace Nexus;
using namespace WARP::Flux::Diagnostics;

bool
Universe::migrateTo(unsigned newVersion)
{
	if(newVersion == 1)
	{
		::diagf(DIAG_NOTICE, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 2)
	{
		::diagf(DIAG_NOTICE, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);

		if(!_operator)
		{
			Thing *thing = _db->objectFromId(1);

			if(thing)
			{
				if(!thing->isPlayer())
				{
					::diagf(DIAG_CRITICAL, "Universe::%s: object %s was expected to be a Player\n", __FUNCTION__, thing->ident());
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
					::diagf(DIAG_CRITICAL, "Universe::%s: failed to create new 'Operator' Player object\n", __FUNCTION__);
					return false;
				}
				_operator->setId(1);
				if(_operator->id() != 1)
				{
					::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Operator's ID to #1P (is %s)\n", __FUNCTION__, _operator->ident());
					return false;
				}
			}
		}
		if(!_operator->setFlag("system"))
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Operator (%s) as SYSTEM\n", __FUNCTION__, _operator->ident());
			return false;
		}
		if(!_operator->setFlag("immortal"))
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Operator (%s) as IMMORTAL\n", __FUNCTION__, _operator->ident());
			return false;
		}
		_operator->dumpf(stderr);
		if(!_operator->sync())
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to sync Operator\n", __FUNCTION__);
			return false;
		}
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 3)
	{
		::diagf(DIAG_NOTICE, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		if(!json_object_get(_meta, "name"))
		{
			json_object_set_new(_meta, "name", json_string("Unnamed Universe"));
		}
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 4)
	{
		::diagf(DIAG_NOTICE, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		if(!_root)
		{
			Thing *thing = _db->objectFromId(2);

			if(thing)
			{
				if(!thing->isZone())
				{
					::diagf(DIAG_CRITICAL, "Universe::%s: object %s was expected to be a Zone\n", __FUNCTION__, thing->ident());
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
					::diagf(DIAG_CRITICAL, "Universe::%s: failed to create new 'Root' Container object\n", __FUNCTION__);
					return false;
				}
				if(!_root->setId(2))
				{
					::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Root's ID to #2 (is %s)\n", __FUNCTION__, _root->ident());
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
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Root (%s) as SYSTEM\n", __FUNCTION__, _root->ident());
			return false;
		}
		if(!_root->setFlag("fixed"))
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Root (%s) as FIXED\n", __FUNCTION__, _root->ident());
			return false;
		}
		if(!_root->setFlag("hidden"))
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Root (%s) as HIDDEN\n", __FUNCTION__, _root->ident());
			return false;
		}
//		_root->dumpf(stderr);
		if(!_root->sync())
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to sync Root\n", __FUNCTION__);
			return false;
		}
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 5)
	{
		::diagf(DIAG_NOTICE, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);

		if(!_system)
		{		
			_system = newZone("System");
			if(!_system)
			{
				::diagf(DIAG_CRITICAL, "Universe::%s: failed to create a new Zone named 'System'\n", __FUNCTION__);
				return false;
			}
			if(!_system->setId(3))
			{
				::diagf(DIAG_CRITICAL, "Universe::%s: failed to set System's ID to #3 (is %s)\n", __FUNCTION__, _system->ident());
				return false;
			}
			if(!_system->setLocation(_root))
			{
				::diagf(DIAG_CRITICAL, "Universe::%s: failed to set System's (%s) location to Root\n", __FUNCTION__, _system->ident());
			}
		}
		if(!_system->setFlag("system"))
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set System (%s) as SYSTEM\n", __FUNCTION__, _system->ident());
			return false;
		}
		if(!_system->setFlag("fixed"))
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set System (%s) as FIXED\n", __FUNCTION__, _system->ident());
			return false;
		}
		if(!_system->sync())
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to sync System (%s)\n", __FUNCTION__, _system->ident());
			return false;
		}
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 6)
	{
		Thing *thing;

		::diagf(DIAG_NOTICE, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		
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
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Messages (%s) as SYSTEM\n", __FUNCTION__, thing->ident());
			return false;
		}
		if(!thing->setFlag("fixed"))
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to set Messages (%s) as FIXED\n", __FUNCTION__, thing->ident());
			return false;
		}
		if(!thing->sync())
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: failed to sync Messges Container\n", __FUNCTION__);
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

		::diagf(DIAG_NOTICE, "Universe::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		
		thing = systemObjectNamed("messages");
		if(!thing)
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: cannot find 'messages' system object\n", __FUNCTION__);
			return false;
		}
		messages = thing->asContainer();
		if(!messages)
		{
			::diagf(DIAG_CRITICAL, "Universe::%s: 'messages' system object (%s) is not a container\n", __FUNCTION__, thing->ident());
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
	if(newVersion == 8)
	{
		Channel *channel;

		channel = newChannel();
		channel->setId(10);
		channel->setFlag("system");
		channel->setFlag("fixed");
		channel->setName("*GLOBAL");
		channel->setLocation(_system);
		channel->release();
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 9)
	{
		Channel *channel;

		channel = newChannel();
		channel->setId(11);
		channel->setFlag("system");
		channel->setFlag("fixed");
		channel->setName("*DEBUG");
		channel->setLocation(_system);
		channel->release();
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 10)
	{
		Channel *channel;

		channel = newChannel();
		channel->setId(12);
		channel->setFlag("system");
		channel->setFlag("fixed");
		channel->setName("*AUDIT");
		channel->setLocation(_system);
		channel->release();
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	if(newVersion == 11)
	{
		_operator->setFlag("invulnerable");
		_operator->setFlag("builder");
		_operator->setFlag("omnipotent");
		_operator->setFlag("xray");
		_operator->setFlag("debugger");
		_operator->setFlag("auditor");
		_operator->setFlag("dba");
		_operator->setFlag("engineer");
		_operator->sync();
		json_object_set_new(_meta, "version", json_integer(newVersion));
		return true;
	}
	::diagf(DIAG_CRITICAL, "Database::%s: unsupported version %u\n", __FUNCTION__, newVersion);
	return false;
}
