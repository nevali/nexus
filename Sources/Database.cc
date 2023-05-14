#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <cctype>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <jansson.h>

#include "Nexus/Database.hh"
#include "Nexus/Thing.hh"

using namespace Nexus;

Database *
Database::create(const char *path)
{
	if(mkdir(path, 0711) == -1)
	{
		return NULL;
	}
	return new Database(path);
}

Database *
Database::open(const char *path)
{
	Database *db;

	db = new Database(path);
	if(!db->version())
	{
		fprintf(stderr, "Database::%s: path '%s' is not a valid database\n", __FUNCTION__, path);
		db->release();
		errno = ENOENT;
		return NULL;
	}
	return db;
}

size_t
Database::canonicalise(char *buf, size_t buflen, const char *src)
{
	size_t n;

	n = 0;
	if(buflen > MAX_CANON_NAME + 1)
	{
		buflen = MAX_CANON_NAME + 1;
	}
	for(n = 0; n + 1 < buflen && src && *src; src++)
	{
		if(isalnum(*src))
		{
			buf[n] = tolower(*src);
			n++;
		}
	}
	buf[n] = 0;
	return n;
}

Database::Database(const char *path):
	Object(),
	_dirty(true)
{
	fprintf(stderr, "Database::%s: starting with path '%s'\n", __FUNCTION__, path);
	_basePath = strdup(path);
	_pathBufSize = strlen(path) + 64;
	_pathBuf = (char *) calloc(1, _pathBufSize + 1);
	snprintf(_pathBuf, _pathBufSize, "%s/db.json", _basePath);
	fprintf(stderr, "Database::%s: attempting to load metadata from %s\n", __FUNCTION__, _pathBuf);
	_meta = json_load_file(_pathBuf, 0, NULL);
	if(!_meta)
	{
		fprintf(stderr, "Database::%s: WARNING: no metadata available\n", __FUNCTION__);
		_meta = json_object();
		json_object_set_new(_meta, "created", json_integer(time(NULL)));
	}
	snprintf(_pathBuf, _pathBufSize, "%s/players.json", _basePath);
	fprintf(stderr, "Database::%s: attempting to load Players index from %s\n", __FUNCTION__, _pathBuf);
	_playerIndex = json_load_file(_pathBuf, 0, NULL);
	if(!_playerIndex)
	{
		_playerIndex = json_object();
	}

	snprintf(_pathBuf, _pathBufSize, "%s/zones.json", _basePath);
	fprintf(stderr, "Database::%s: attempting to load Zones index from %s\n", __FUNCTION__, _pathBuf);
	_zoneIndex = json_load_file(_pathBuf, 0, NULL);
	if(!_zoneIndex)
	{
		_zoneIndex = json_object();
	}
}

Database::~Database()
{
	fprintf(stderr, "Database::%s: shutting down\n", __FUNCTION__);
	json_decref(_meta);
	json_decref(_playerIndex);
	json_decref(_zoneIndex);
	free(_basePath);
	free(_pathBuf);
}

unsigned
Database::version(void) const
{
	json_t *value;

	value = json_object_get(_meta, "version");
	if(!value)
	{
		return 0;
	}
	return json_integer_value(value);
}

Thing::ID
Database::maxId(void) const
{
	json_t *value;

	value = json_object_get(_meta, "maxId");
	if(!value)
	{
		return 0;
	}
	return json_integer_value(value);
}

Thing::ID
Database::reservedId(void) const
{
	json_t *value;

	value = json_object_get(_meta, "reserve");
	if(!value)
	{
		return 0;
	}
	return json_integer_value(value);
}

Thing::ID
Database::nextId(void) const
{
	Thing::ID n = maxId();

	if(n < 0)
	{
		n = 0;
	}
	if(n < reservedId())
	{
		n = reservedId();
	}
	n++;
	return n;
}

json_t *
Database::settings(void)
{
	json_t *value;

	value = json_object_get(_meta, "settings");
	if(!value)
	{
		fprintf(stderr, "Database::%s: 'settings' key is not present in metadata object\n", __FUNCTION__);
		value = json_object();
		json_object_set_new(_meta, "settings", value);
	}
	return value;
}

json_t *
Database::universe(void)
{
	json_t *value;

	value = json_object_get(_meta, "universe");
	if(!value)
	{
		fprintf(stderr, "Database::%s: 'universe' key is not present in metadata object\n", __FUNCTION__);
		value = json_object();
		json_object_set_new(_meta, "universe", value);
	}
	return value;
}

/* Load an object with the ID id and return it as a json_t JSON object */
json_t *
Database::jsonFromId(Thing::ID id)
{
	json_t *obj;
	char *p;

	snprintf(_pathBuf, _pathBufSize, "%s/%ld/%ld.json", _basePath, id, id);
//	fprintf(stderr, "Database::%s: attempting to load %s\n", __FUNCTION__, _pathBuf);
	obj = json_load_file(_pathBuf, 0, NULL);
	if(!obj)
	{
		if(errno != ENOENT)
		{
			fprintf(stderr, "Database::%s: failed to load %s: %s\n", __FUNCTION__, _pathBuf, strerror(errno));
		}
		return NULL;
	}
	snprintf(_pathBuf, _pathBufSize, "%s/%ld/%ld.desc", _basePath, id, id);
	p = textFromFile(_pathBuf);
	if(p)
	{
		json_object_set_new(obj, "description", json_string(p));
		free(p);
	}
	snprintf(_pathBuf, _pathBufSize, "%s/%ld/%ld.text", _basePath, id, id);
	p = textFromFile(_pathBuf);
	if(p)
	{
		json_object_set_new(obj, "text", json_string(p));
		free(p);
	}
	return obj;
}

/* Load the contents of the text file pathname into a new buffer and return it */
char *
Database::textFromFile(const char *path)
{
	size_t pos, size, newSize;
	ssize_t r;
	int fd;
	char *buffer, *p;

	fd = ::open(path, O_RDONLY);
	if(fd < 0)
	{
		if(errno != ENOENT)
		{
			fprintf(stderr, "Database::%s: %s: %s\n", __FUNCTION__, path, strerror(errno));
		}
		return NULL;
	}
	pos = 0;
	size = 0;
	newSize = 0;
	buffer = NULL;
	p = NULL;
	for(;;)
	{
		if(pos >= newSize)
		{
			/* read in half-kilobyte chunks */
			newSize += 512;
		}
		if(newSize > size)
		{
			p = (char *) realloc(buffer, newSize + 2);
			if(!p)
			{
				fprintf(stderr, "Database::%s: %s: failed to realloc buffer to %lu bytes: %s\n", __FUNCTION__, path, newSize, strerror(errno));
				free(buffer);
				close(fd);
				return NULL;
			}
			buffer = p;
			p = &(buffer[size]);
			memset(p, 0, newSize - size + 2);
			size = newSize;
//			fprintf(stderr, "Database::%s: buffer is now %lu\n", __FUNCTION__, size);
		}
		r = read(fd, &(buffer[pos]), size - pos);
		if(r == 0)
		{
			/* EOF */
			break;
		}
		if(r < 0)
		{
			if(errno == EINTR || errno == EAGAIN)
			{
				continue;
			}
			fprintf(stderr, "Database::%s: %s: read failed: %s\n", __FUNCTION__, path, strerror(errno));
			free(buffer);
			close(fd);
			return NULL;
		}
		pos += r;
		buffer[pos] = 0;
	}
	close(fd);
//	fprintf(stderr, "Database::%s: %s: read %lu bytes\n", __FUNCTION__, path, pos);
	return buffer;
}

Thing *
Database::objectFromId(Thing::ID id)
{
	json_t *obj;
	Thing *thing;

	if(id < 1)
	{
		return NULL;
	}
	obj = jsonFromId(id);
	if(!obj)
	{
		return NULL;
	}
	thing = Thing::objectFromJSON(obj);
	if(!thing)
	{
		fprintf(stderr, "Database:%s: failed to deserialise object %ld\n", __FUNCTION__, id);
		json_dumpf(obj, stderr, JSON_INDENT(2)|JSON_ENSURE_ASCII|JSON_PRESERVE_ORDER);
		fprintf(stderr, "\n");
		json_decref(obj);
		return NULL;
	}
	json_decref(obj);
	if(thing->id() != id)
	{
		fprintf(stderr, "Database:%s: loaded object's ID (#%ld%c) is not expected #%ld\n", __FUNCTION__, thing->id(), thing->typeChar(), id);
		thing->release();
		return NULL;
	}
	thing->setDatabase(this);
	return thing;
}

bool
Database::storeObject(json_t *obj)
{
	Thing::ID id;
	json_t *data, *sub;

	if((id = json_integer_value(json_object_get(obj, "id"))) <= 0)
	{
		fprintf(stderr, "Database::%s: refusing to write object with invalid ID\n", __FUNCTION__);
		return false;
	}
	if(id > maxId())
	{
		json_object_set_new(_meta, "maxId", json_integer(id));
	}
	snprintf(_pathBuf, _pathBufSize, "%s/%ld", _basePath, id);
	if(mkdir(_pathBuf, 0711) < 0 && errno != EEXIST)
	{
		fprintf(stderr, "Database::%s: failed to create %s: %s\n", __FUNCTION__, _pathBuf, strerror(errno));
		return false;
	}
	snprintf(_pathBuf, _pathBufSize, "%s/%ld/%ld.json", _basePath, id, id);
//	fprintf(stderr, "Database::%s: saving to %s\n", __FUNCTION__, _pathBuf);
	/* this is a shallow copy, because we only want to remove keys */
	data = json_copy(obj);
	json_object_del(data, "description");
	json_object_del(data, "text");
	if(json_dump_file(data, _pathBuf, JSON_INDENT(2)|JSON_ENSURE_ASCII|JSON_SORT_KEYS) < 0)
	{
		fprintf(stderr, "Database::%s: failed to write to %s: %s\n", __FUNCTION__, _pathBuf, strerror(errno));
		json_decref(data);
		return false;
	}
	_dirty = true;
	json_decref(data);
	snprintf(_pathBuf, _pathBufSize, "%s/%ld/%ld.desc", _basePath, id, id);
	if((sub = json_object_get(obj, "description")))
	{	
		writeToFile(_pathBuf, json_string_value(sub));
	}
	else
	{
		::unlink(_pathBuf);
	}
	snprintf(_pathBuf, _pathBufSize, "%s/%ld/%ld.text", _basePath, id, id);
	if((sub = json_object_get(obj, "text")))
	{	
		writeToFile(_pathBuf, json_string_value(sub));
	}
	else
	{
		::unlink(_pathBuf);
	}
	indexObject(obj);
	sync();
	return true;
}

bool
Database::writeToFile(const char *pathname, const char *string)
{
	FILE *f;

	f = fopen(pathname, "wb");
	if(!f)
	{
		fprintf(stderr, "Database::%s: failed to open %s for writing: %s\n", __FUNCTION__, pathname, strerror(errno));
		return false;
	}
	fputs(string, f);
	fclose(f);
	return true;
}

json_t *
Database::indexForType(const char *type)
{
	if(!strcmp(type, "player"))
	{
		return _playerIndex;
	}
	if(!strcmp(type, "robot"))
	{
		return _playerIndex;
	}
	if(!strcmp(type, "zone"))
	{
		return _zoneIndex;
	}
	return NULL;
}

bool
Database::indexObject(json_t *obj)
{
	Thing::ID id;
	const char *type;
	const char *name;
	char cname[MAX_CANON_NAME + 1];
	json_t *index;

	bool deleted = json_boolean_value(json_object_get(obj, "deleted"));

	if((id = json_integer_value(json_object_get(obj, "id"))) <= 0)
	{
		return false;
	}
	if(id > maxId())
	{
		json_object_set_new(_meta, "maxId", json_integer(id));
		_dirty = true;
	}
	if(!(type = json_string_value(json_object_get(obj, "type"))))
	{
		return false;
	}
	if(!(name = json_string_value(json_object_get(obj, "cname"))))
	{
		return false;
	}
	canonicalise(cname, sizeof(cname), name);
	index = indexForType(type);
	if(!index)
	{
		return false;
	}
	if(deleted)
	{
		json_t *entry;

		entry = json_object_get(index, cname);
		if(entry && json_integer_value(entry) == id)
		{
			json_object_del(index, cname);
		}
	}
	else
	{
		json_object_set_new(index, cname, json_integer(id));
	}
//	fprintf(stderr, "Database::%s: indexing %s '%s' => #%ld\n", __FUNCTION__, type, cname, id);
	return true;
}

Thing::ID
Database::idFromName(const char *type, const char *name)
{
	json_t *index, *value;
	char cname[MAX_CANON_NAME + 1];

	index = indexForType(type);
	canonicalise(cname, sizeof(cname), name);
//	fprintf(stderr, "Database::%s: cname='%s'\n", __FUNCTION__, cname);
	if(index)
	{
		value = json_object_get(index, cname);
		if(value && json_integer_value(value) > 0)
		{
//			fprintf(stderr, "Database::%s: %s '%s' has the ID #%lld\n", __FUNCTION__, type, name, json_integer_value(value));
			return json_integer_value(value);
		}
	}
	return Thing::ID_INVALID;
}

bool
Database::checkpoint(void)
{
	snprintf(_pathBuf, _pathBufSize, "%s/db.json", _basePath);
	json_object_set_new(_meta, "lastUpdate", json_integer(time(NULL)));
//	fprintf(stderr, "Database::%s: saving metadata to %s\n", __FUNCTION__, _pathBuf);
	if(json_dump_file(_meta, _pathBuf, JSON_INDENT(2)|JSON_ENSURE_ASCII|JSON_SORT_KEYS) < 0)
	{
		fprintf(stderr, "Database::%s: failed to write to %s: %s\n", __FUNCTION__, _pathBuf, strerror(errno));
		return false;
	}
	snprintf(_pathBuf, _pathBufSize, "%s/players.json", _basePath);
	if(json_dump_file(_playerIndex, _pathBuf, 0) < 0)
	{
		fprintf(stderr, "Database::%s: failed to write to %s: %s\n", __FUNCTION__, _pathBuf, strerror(errno));
		return false;
	}
	snprintf(_pathBuf, _pathBufSize, "%s/zones.json", _basePath);
	if(json_dump_file(_zoneIndex, _pathBuf, 0) < 0)
	{
		fprintf(stderr, "Database::%s: failed to write to %s: %s\n", __FUNCTION__, _pathBuf, strerror(errno));
		return false;
	}
	_dirty = false;
	return true;
}

bool
Database::sync(void)
{
	if(!_dirty)
	{
		return true;
	}
	return checkpoint();
}

bool
Database::migrate(void)
{
	for(unsigned currentVersion = version(); currentVersion < DBVERSION; currentVersion = version())
	{
		if(!migrateTo(currentVersion + 1))
		{
			fprintf(stderr, "Database::%s: migration to version %u failed\n", __FUNCTION__, currentVersion + 1);
			return false;
		}
		if(version() <= currentVersion)
		{
			fprintf(stderr, "Database::%s: post-migration version is %u (expected at least %u)\n", __FUNCTION__, version(), currentVersion + 1);
			return false;
		}
		checkpoint();
	}
	return true;
}

bool
Database::migrateTo(unsigned newVersion)
{
	if(newVersion == 1)
	{
		fprintf(stderr, "Database::%s: migrating to version %u\n", __FUNCTION__, newVersion);
		if(!json_object_get(_meta, "universe"))
		{
			json_object_set_new(_meta, "universe", json_object());
		}
		if(!json_object_get(_meta, "settings"))
		{
			json_object_set_new(_meta, "settings", json_object());
		}
		if(!json_object_get(_meta, "maxId"))
		{
			json_object_set_new(_meta, "maxId", json_integer(0));
		}
		if(!json_object_get(_meta, "reserve"))
		{
			/* IDs < 100 are used by the system */
			json_object_set_new(_meta, "reserve", json_integer(100));
		}
		json_object_set(_meta, "version", json_integer(newVersion));
		return true;
	}
	fprintf(stderr, "Database::%s: unsupported version %u\n", __FUNCTION__, newVersion);
	return false;
}

