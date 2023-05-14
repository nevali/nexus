#ifndef NEXUS_THING_HH_
# define NEXUS_THING_HH_

# include <cstdarg>

# include <jansson.h>

# include "WARP/Flux/Object.hh"

namespace Nexus
{

	class Database;
	class Universe;
	class Container;
	class Zone;
	class Room;
	class Portal;
	class Actor;
	class Player;
	class Robot;
	class Executable;
	class Variable;
	
	class Thing: public WARP::Flux::Object
	{
		public:
			typedef long ID;
			typedef enum
			{
				INVALID =    '!',
				ANY =        '*',
				THING =      'T',
				CONTAINER =  'C',
				ZONE =       'Z',
				ACTOR =      'A',
				ROOM =       'R',
				PORTAL =     'E',
				PLAYER =     'P',
				ROBOT =      'B',
				EXECUTABLE = 'X',
				VARIABLE =   'V',
				SWITCH =     'S',
				TIMER =      'F'
			} TypeID;

			typedef enum
			{
				NONE = 0,
				SYSTEM = (1<<1),
				HIDDEN = (1<<2),
				FIXED = (1<<3),
				IMMORTAL = (1<<4), /* Actors */
				LOCKED = (1<<5),  /* Containers */
				DELETED = (1<<6),
				HOLOGRAM = (1<<7)
			} Flags;

			typedef enum
			{
				DEFAULT = 0,
				INVISIBLE = (1<<1),
				CONNECTING = (1<<2),
				CONNECTED = (1<<3),
				DISCONNECTING = (1<<4),
				ACTIVE = (1<<5),
				DIRTY = (1<<6),
				DESTROYED = (1<<7)
			} Status;

			static const ID ID_INVALID = -1;
			static const ID ID_LIMBO = 0;
		public:
			static Thing *objectFromJSON(json_t *source, bool isNew = false);
			static bool validName(const char *name);
			static const char *typeName(TypeID typeId);
		protected:
			Thing(json_t *source = NULL);
			virtual ~Thing();
		protected:
			friend class Database;
			friend class Universe;
			friend class Player;

			virtual bool setId(ID id);
			virtual void setUniverse(Universe *universe);
			virtual void setDatabase(Database *db);
		protected:
			/* internal methods */
			virtual void unpack(void);
			virtual void updateIdent(void);
			virtual void updateFlags(void);
			virtual void updateName(void);
			virtual void applyFlag(const char *name, bool value);
			virtual void dumpf(FILE *out);
			virtual void markDirty(void);
			virtual bool sync(void);
			virtual void examineSections(Actor *who);
		public:
			/* polymorphism/rudimentary RTTI */
			virtual bool isContainer(void) const { return false; };
			virtual bool isZone(void) const { return false; };
			virtual bool isRoom(void) const { return false; };
			virtual bool isPortal(void) const { return false; };
			virtual bool isActor(void) const { return false; };
			virtual bool isPlayer(void) const { return false; };
			virtual bool isRobot(void) const { return false; };
			virtual bool isExecutable(void) const { return false; };
			virtual bool isVariable(void) const { return false; };
			virtual Container *asContainer(void) const;
			virtual Zone *asZone(void) const;
			virtual Room *asRoom(void) const;
			virtual Portal *asPortal(void) const;
			virtual Actor *asActor(void) const;
			virtual Player *asPlayer(void) const;
			virtual Robot *asRobot(void) const;
			virtual Executable *asExecutable(void) const;
			virtual Variable *asVariable(void) const;
		public:
			/* object properties */
			virtual Universe *universe(void) const;
			virtual Database *database(void) const;
			virtual ID id(void) const;
			virtual TypeID typeId(void) const { return THING; }
			virtual char typeChar(void) const;
			virtual const char *typeStr(void) const;
			virtual unsigned flags(void) const;
			virtual unsigned status(void) const;
			virtual const char *flagsStr(void) const;
			virtual bool setFlag(const char *flag, bool set = true);
			virtual const char *ident(void) const;
			virtual const char *name(void) const;
			virtual const char *displayName(void) const;
			virtual const char *canonicalName(void) const;
			virtual bool setName(const char *newName);
			virtual bool nameIsSuitable(const char *newName) const;
			virtual bool isNamed(const char *name) const;
			virtual bool isCanonicallyNamed(const char *name) const;
			virtual const char *description(void) const;
			virtual bool setDescription(const char *newDesc);
			virtual Container *location(void) const __attribute__ (( warn_unused_result ));
			virtual bool setLocation(ID loc);
			virtual bool setLocation(Container *loc);
			virtual Actor *owner(void) __attribute__ (( warn_unused_result ));
			virtual bool setOwner(Thing *owner);
			virtual Zone *zone(void) __attribute__ (( warn_unused_result ));
			virtual Thing *resolveTarget(const char *target) __attribute__ (( warn_unused_result ));

		public:
			/* send: directs the supplied string to the object */
			virtual void send(const char *string);
			virtual void sendf(const char *format, ...) __attribute__ (( format(printf, 2, 3) ));
			virtual void vsendf(const char *format, va_list ap);

			/* emit: directs the supplied string to everything in the object's location */
			virtual void emit(const char *string);
			virtual void emitf(const char *format, ...) __attribute__ (( format(printf, 2, 3) ));
			virtual void vemitf(const char *format, va_list ap);

		public:
			/* built-in actions (verbs) that can be carried out by an Actor
			 * these can all be overidden in subclasses
			 */
			virtual bool destroy(Actor *who);
			virtual bool dump(Actor *who);
			virtual bool look(Actor *who);
			virtual bool examine(Actor *who);
			virtual bool list(Actor *who);
			virtual bool teleport(Actor *who, Container *destination);
			virtual bool flag(Actor *who, const char *flagName, bool onOff);
			virtual bool set(Actor *who, const char *property, const char *value);
		protected:
			json_t *_obj;
			Universe *_universe;
			Database *_db;
			ID _id;
			TypeID _typeId;
			char _ident[32];
			unsigned _flags;
			char _flagsStr[8];
			unsigned _status;
			const char *_displayName;
	};
}

#endif /*!NEXUS_THING_HH_*/