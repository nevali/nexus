#ifndef NEXUS_DATABASE_HH_
# define NEXUS_DATABASE_HH_

# include <cstdint>

# include "WARP/Flux/Object.hh"

# include "Thing.hh"

namespace Nexus
{
	class Database: public WARP::Flux::Object
	{
		public:
			static const unsigned VERSION = 1;
			/* maximum length of a canonicalised name */
			static const size_t MAX_CANON_NAME = 31;

			/* create a new database (path must not exist) */
			static Database *create(const char *path) __attribute__ (( warn_unused_result ));
			/* open an existing database (path must exist) */
			static Database *open(const char *path) __attribute__ (( warn_unused_result ));
			/* canonicalise a name (strip down to all-lowercase alphanumeric-only, limited to MAX_CANON_NAME characters) */
			static size_t canonicalise(char *buf, size_t buflen, const char *name);
		public:
			virtual unsigned version(void) const;
			virtual bool migrate(void);
			virtual bool sync(void);
			virtual bool checkpoint(void);
			virtual json_t *settings(void);
			virtual json_t *universe(void);
			virtual Thing *objectFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual Thing::ID idFromName(const char *type, const char *name);
			virtual Thing::ID maxId(void) const;
			virtual Thing::ID reservedId(void) const;
			virtual Thing::ID nextId(void) const;
		protected:
			friend class Thing;

			Database(const char *path);
			virtual ~Database(void);
			virtual json_t *jsonFromId(Thing::ID id) __attribute__ (( warn_unused_result ));
			virtual json_t *indexForType(const char *type);
			virtual bool migrateTo(unsigned newVersion);
			virtual bool storeObject(json_t *obj);
			virtual bool indexObject(json_t *obj);
		private:
			char *textFromFile(const char *path);
			bool writeToFile(const char *path, const char *text);
		protected:
			char *_basePath;
			char *_pathBuf;
			size_t _pathBufSize;
			json_t *_meta;
			bool _dirty;
			json_t *_playerIndex;
			json_t *_zoneIndex;
	};
}

#endif /*!NEXUS_DATABASE_HH_*/
