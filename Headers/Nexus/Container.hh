#ifndef NEXUS_CONTAINER_HH_
# define NEXUS_CONTAINER_HH_

# include "Thing.hh"

namespace Nexus
{
	class Container: public Thing
	{
		protected:
			friend class Thing;
			friend class Player;
			
			Container(json_t *source);
			virtual ~Container();
		public:
			virtual TypeID typeId(void) const { return CONTAINER; }
			virtual bool isContainer(void) const { return true; }
			virtual bool list(Actor *who);
			virtual Thing *firstObjectNamed(const char *name, Thing::TypeID type) __attribute__ (( warn_unused_result ));
		protected:
			virtual void unpack(void);
			virtual void add(Thing *thing);
			virtual void remove(Thing *thing);
			virtual void expandContents(size_t newSize);
			virtual void examineSections(Actor *who);
		protected:
			Thing **_contents;
			size_t _ncontents;
	};
}

#endif /*!NEXUS_CONTAINER_HH_*/