#include "Nexus/Zone.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

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
			fprintf(stderr, "Zone<%p>::%s: a zone named '%s' (%s) already exists\n", this, __FUNCTION__, z->displayName(), z->ident());
			z->release();
			return false;
		}
	}
	return true;
}
