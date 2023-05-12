#include <cstdio>
#include <cstring>

#include <jansson.h>

#include "Nexus/Thing.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

void
Thing::send(const char *str)
{
	fprintf(stderr, "[[ To #%ld%c ]] %s", id(), typeChar(), str);
}

void
Thing::emit(const char *str)
{
	fprintf(stderr, "[[ From #%ld%c ]] %s", id(), typeChar(), str);
}

void
Thing::vsendf(const char *format, va_list ap)
{
	static char buffer[512];

	vsnprintf(buffer, sizeof(buffer) - 1, format, ap);
	send(buffer);
}

void
Thing::vemitf(const char *format, va_list ap)
{
	static char buffer[512];

	vsnprintf(buffer, sizeof(buffer) - 1, format, ap);
	emit(buffer);
}

void
Thing::sendf(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsendf(format, ap);
	va_end(ap);
}

void
Thing::emitf(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vemitf(format, ap);
	va_end(ap);
}
