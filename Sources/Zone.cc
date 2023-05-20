#include <cstring>

#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Zone.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Database.hh"

using namespace Nexus;
using namespace WARP::Flux::Diagnostics;

bool
Zone::nameIsSuitable(const char *newName) const
{
	Zone *z;

	/* a Zone need not have a name, but any name it has must be (canonically)
	 * unique
	 */
	if(newName)
	{
		if(!validName(newName))
		{
			return false;
		}
		if(_universe && (z = _universe->zoneFromName(newName)))
		{
			if(z == this)
			{
				z->release();
				return true;
			}
			::debugf("Zone<%p>::%s: a zone named '%s' (%s) already exists\n", this, __FUNCTION__, z->displayName(), z->ident());
			z->release();
			return false;
		}
	}
	return true;
}

Channel *
Zone::channelWithName(const char *name)
{
	char cname[Database::MAX_CANON_NAME + 1];
	const char *p;
	size_t c;

	Database::canonicalise(cname, sizeof(cname), name);
	if(!cname[0])
	{
		return NULL;
	}
	for(c = 0; c < _ncontents; c++)
	{
		if(_contents[c] &&
			_contents[c]->isChannel())
		{
			/* XXX Thing::nameMatches() ?? */
			if((p = _contents[c]->canonicalName()) && !strcmp(p, cname))
			{
				_contents[c]->retain();
				return _contents[c]->asChannel();
			}
		}
	}
	return NULL;
}