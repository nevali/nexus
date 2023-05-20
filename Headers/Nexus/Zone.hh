#ifndef NEXUS_ZONE_HH_
# define NEXUS_ZONE_HH_

# include "Container.hh"

namespace Nexus
{
	/* a Zone is a world within the Universe, with its own commands and rules
	 * Zones can be nested, although it is not advisable to nest deeply
	 * every object belongs to exactly one Zone at any given time
	 * Zones are not expected to be seen by Players (other than builders);
	 * and they are where Executable objects should be placed (i.e., they
	 * act as a command scope)
	 * 
	 * when a Room is created it is by default created within the current Zone
	 */
	class Zone: public Container
	{
		protected:
			friend class Thing;

			Zone(json_t *source): Container(source) {}
		public:
			virtual TypeID typeId(void) const { return ZONE; }
			virtual bool isZone(void) const { return true; }
			/* the zone of a Zone is always itself, even if it's contained
			 * by another Zone
			 */
			virtual Zone *zone(void) __attribute__ (( warn_unused_result )) { retain(); return this; }
			virtual bool nameIsSuitable(const char *newName) const;
			/* locate the channel named 'name' in this zone */
			virtual Channel *channelWithName(const char *name) __attribute__ (( warn_unused_result ));
	};
}

#endif /*!NEXUS_ZONE_HH_*/
