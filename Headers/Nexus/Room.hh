#ifndef NEXUS_ROOM_HH_
# define NEXUS_ROOM_HH_

# include "Container.hh"

namespace Nexus
{
	class Room: public Container
	{
		protected:
			friend class Thing;

			Room(json_t *source): Container(source) {}
		public:
			virtual TypeID typeId(void) const { return ROOM; }
			virtual bool isRoom(void) const { return true; }
	};
}

#endif /*!NEXUS_ROOM_HH_*/