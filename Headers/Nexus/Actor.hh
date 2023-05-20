#ifndef NEXUS_ACTOR_HH_
# define NEXUS_ACTOR_HH_

# include "Container.hh"

namespace Nexus
{
	struct ExecutionContext;

	class Actor: public Container
	{
		protected:
			friend class Thing;

			Actor(json_t *source): Container(source) {}
			virtual bool activate(void);
			virtual bool deactivate(void);
			virtual void activated(void);
			virtual void deactivated(void);
			virtual void connectChannels(void);
			virtual void disconnectChannels(void);
			virtual bool connectToChannel(Channel *channel);
			virtual bool disconnectFromChannel(Channel *channel);
		public:
			virtual TypeID typeId(void) const { return ACTOR; }
			virtual bool isActor(void) const { return true; }
			virtual bool setFlag(const char *flag, bool set = true);
			virtual bool perform(const char *command, size_t len = -1);
			virtual bool perform(ExecutionContext *context, const char *command, size_t cmdlen);
		public:
			/* the following methods provide the logic for finding different
			 * types of object within the Actor's current scope (which may
			 * be nowhere); they return NULL if nothing was found, or a
			 * retained reference if there was a match
			 *
			 * where ambiguity is permitted, this is controlled via
			 * parameters
			 *
			 * these methods are used by all of the commands that have
			 * parameters specifying an object
			 */

			/* XXX all of these will need an execution scope to handle CHOWN */
			
			/* 'me', 'here', '#nnn', etc. */
			virtual Thing *resolveIdOrBuiltin(const char *str);
			/* find a location by its name, used when manipiulating rooms */
			virtual Container *resolveLocation(const char *str);
			/* similar to resolveLocation() but used when travelling */
			virtual Container *resolveDestination(const char *str);
			/* find a command or verb in scope */
			virtual Executable *resolveExecutable(const char *name);
			/* is it the name of an object being carried */
			virtual Thing *resolveCarried(const char *name);
			/* is it the name of an object lying nearby */
			virtual Thing *resolveNearby(const char *name);
			/* is it the name of an addressable Actor */
			virtual Actor *resolveActor(const char *name);
			/* is it the name of another online Player */
			virtual Player *resolvePlayer(const char *name);
			/* is it the name of *any* player - only useful for commands which
			 * are only relevant to players 
			 */
			virtual Player *resolveAnyPlayer(const char *name);
			/* is it the name of a channel '#channel' */
			virtual Channel *resolveChannel(const char *name);
			/* is it the name of a zone '!system' */
			virtual Zone *resolveZone(const char *name);
			/* resolve an object using the 'standard' rules for its type */
			virtual Thing *resolveByType(TypeID type, const char *name);
		protected:
			virtual void applyFlag(const char *flag, bool set);
	};
}

#endif /*!NEXUS_ACTOR_HH_*/
