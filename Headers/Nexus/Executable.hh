#ifndef NEXUS_EXECUTABLE_HH_
# define NEXUS_EXECUTABLE_HH_

# include "Thing.hh"

namespace Nexus
{
	/* an Executable represents a script object within the universe
	 *
	 * depending on its name, an Executable may be:
	 *
	 * name (alphabetic):
	 * - a command which can be invoked by Actors
	 *   commands must be either held by the Actor, in their current location,
	 *   or be located in their Zone tree
	 *
	 * !name:
	 * - an event which is triggered 
	 *   for example, a script may be executed whenever somebody enters a Room
	 *
	 * $name: defines a library function, which is searched from the calling
	 *   Executable's own location (rather than the invoking Actor's)
	 */
	class Executable: public Thing
	{
		protected:
			friend class Thing;

			Executable(json_t *source): Thing(source) {}
		public:
			virtual TypeID typeId(void) const { return EXECUTABLE; }
			virtual bool isExecutable(void) const { return true; }
	};
}

#endif /*!NEXUS_EXECUTABLE_HH_*/
