#ifndef NEXUS_UNIVERSE_HH_
# define NEXUS_UNIVERSE_HH_

# include "WARP/Flux/Object.hh"
# include "WARP/Flux/Array.hh"

# include "Thing.hh"
# include "Limbo.hh"

namespace Nexus
{
	class Parser;
	class Module;

	class Universe: public WARP::Flux::Object
	{
		public:
			static const unsigned UVERSION = 7;
			/* this can be adjusted if needed */
			static const unsigned ID_MAX = 65536;

			class ModuleList: public WARP::Flux::TArray<Module>
			{
				public:
					ModuleList(Universe *universe);
				protected:
					virtual ~ModuleList(void);
					virtual void objectWillBeDestroyed(void);
				public:
					virtual bool activate(void);
					virtual bool enable(const char *name);
					virtual bool disable(const char *name);
					virtual size_t add(Module *mod);
					virtual bool remove(Module *mod);
					Module *locateActiveModule(const char *name) __attribute__ (( warn_unused_result ));;
				private:
					Universe *_universe;
					json_t *_modlist;
					bool _activating;
			};
			friend class ModuleList;

		public:
			Universe(Database *db);
		protected:
			virtual ~Universe();
		protected:
			friend class Thing;

			virtual void acquire(Thing *object);
			virtual void discard(Thing *object);
		protected:
			virtual bool migrateTo(unsigned version);
			void syncObjects(void);
			Thing *createObject(json_t *object, const char *name, bool allocId = false, Container *location = NULL) __attribute__ (( warn_unused_result ));
			json_t *objectTemplate(const char *type) __attribute__ (( warn_unused_result ));
			json_t *thingTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *containerTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *roomTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *zoneTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *portalTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *actorTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *playerTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *robotTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *hologramTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *executableTemplate(void) __attribute__ (( warn_unused_result ));
			json_t *variableTemplate(void) __attribute__ (( warn_unused_result ));
		public:
			virtual unsigned version(void) const;
			virtual bool migrate(void);
			virtual bool sync(void);
			virtual bool checkpoint(void);
			virtual const char *name(void) const;
			virtual bool activateModules(void);
			virtual bool enableModule(const char *name);
			virtual bool disableModule(const char *name);
			virtual ModuleList *modules(void) __attribute__ (( warn_unused_result ));
			virtual Thing *thingFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Thing *thingFromName(const char *type, const char *name) __attribute__ (( warn_unused_result ));
			virtual Limbo *limbo(void) const __attribute__ (( warn_unused_result ));
			virtual Zone *rootZone(void) const __attribute__ (( warn_unused_result ));
			virtual Zone *systemZone(void) const __attribute__ (( warn_unused_result ));
			virtual Thing *systemObjectNamed(const char *name) __attribute__ (( warn_unused_result ));
			virtual Variable *messageObjectNamed(const char *name) __attribute__ (( warn_unused_result ));
			virtual const char *messageNamed(const char *name);
			virtual Container *containerFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Zone *zoneFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Zone *zoneFromName(const char *name) __attribute__ (( warn_unused_result ));
			virtual Room *roomFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Portal *portalFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Actor *actorFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Player *playerFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Player *playerFromName(const char *name) __attribute__ (( warn_unused_result ));
			virtual Robot *robotFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Executable *executableFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Player *newPlayer(const char *name = NULL, bool allocId = false, Container *location = NULL) __attribute__ (( warn_unused_result ));
			virtual Container *newContainer(const char *name = NULL, bool allocId = false, Container *location = NULL) __attribute__ (( warn_unused_result ));
			virtual Room *newRoom(const char *name = NULL, bool allocId = false, Container *location = NULL) __attribute__ (( warn_unused_result ));
			virtual Zone *newZone(const char *name = NULL, bool allocId = false, Container *location = NULL) __attribute__ (( warn_unused_result ));
			virtual Thing *newThing(const char *name = NULL, bool allocId = false, Container *location = NULL) __attribute__ (( warn_unused_result ));
			virtual Portal *newPortal(const char *name = NULL, bool allocId = false, Container *location = NULL) __attribute__ (( warn_unused_result ));
			virtual Variable *newVariable(const char *name = NULL, bool allocId = false, Container *location = NULL) __attribute__ (( warn_unused_result ));
			virtual Parser *parserForCommand(Actor *actor, const char *commandLine) __attribute__ (( warn_unused_result ));
			virtual Parser *builtinsParser(void) __attribute__ (( warn_unused_result ));
		protected:
			Thing **_objects;
			size_t _objectsSize;
			Database *_db;
			Limbo *_limbo;
			Player *_operator;
			Zone *_root;
			Zone *_system;
			json_t *_meta;
			Parser *_builtinsParser;
			Parser *_defaultParser;
			Thing::ID _maxId;
			ModuleList *_modules;
	};
}

#endif /*!NEXUS_UNIVERSE_HH_*/
