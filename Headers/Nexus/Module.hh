#ifndef NEXUS_MODULE_HH_
# define NEXUS_MODULE_HH_

# include <cstddef>

# include <WARP/Flux/Object.hh>

namespace Nexus
{
	class Universe;
	class ModuleImplementation;
	struct CommandEntry;

	class Module: public WARP::Flux::Object
	{
		public:
			/* maximum length of a canonicalised name */
			static const size_t MAX_CANON_NAME = 31;

			static Module *load(Universe *universe, const char *name);
			static bool nameIsValid(const char *name);
			/* canonicalise a name (strip down to [a-z0-9-], limited to MAX_CANON_NAME characters) */
			static size_t canonicalise(char *buf, size_t buflen, const char *name);
		private:
			Module(Universe *universe, const char *name);
		protected:
			virtual ~Module();
		public:
			/* returns the module's name as supplied */
			virtual const char *name(void) const;
			/* returns the module's canonical name */
			virtual const char *canonicalName(void) const;
			/* returns the module's pathname */
			virtual const char *path(void) const;
		private:
			Universe *_universe;
			void *_handle;
			char *_name;
			char _cname[MAX_CANON_NAME + 1];
			char *_path;
			ModuleImplementation *_impl;
	};

	class ModuleImplementation: public WARP::Flux::Object
	{
		protected:
			ModuleImplementation(Universe *universe, Module *module);
			virtual ~ModuleImplementation();
		public:
			virtual const char *name(void) const;			
			virtual const char *canonicalName(void) const;
			virtual const char *path(void) const;
		protected:
			virtual bool addBuiltinCommands(CommandEntry *entries);
			virtual bool removeBuiltinCommands(CommandEntry *entries);
		protected:
			/* These are not retained and will be valid for the entire
			 * lifetime of the module
			 */
			Universe *_universe;
			Module *_module;
	};
}

# define PROVIDE_MODULE(className) \
	extern "C" Nexus::ModuleImplementation *nexusModuleImplementation(Nexus::Universe *universe, Nexus::Module *module) \
	{ \
		return new className(universe, module); \
	}

#endif /*!NEXUS_MODULE_HH_*/
