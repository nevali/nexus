# Reference

## Commands

Commands beginning with `@` are built-ins

| Command                       | Description
|-------------------------------|--------------------------|
| `@bye`, `@quit`, or `@logout` | End session
| `@create TYPE [NAME]`         | Make a new object of `TYPE`
| `@destroy`                    | Destroy an object
| `@teleport TARGET DEST`       | Teleport `TARGET` to `DEST`
| `@name TARGET NEWNAME`        | Rename `TARGET` to `NEWNAME`
| `@desc TARGET NEWDESC`        | Set `TARGET`'s description to `NEWDESC`
| `@examine [TARGET]`           | Examine `TARGET` (defaults to yourself)
| `@dump [TARGET]`              | Dumps `TARGET` (defaults to yourself) as JSON
| `@flag TARGET FLAG [on/off]`  | Sets (or clears) `FLAG` on `TARGET`
| `@echo TEXT`                  | Echoes `TEXT` back to you, equivalent to `@send me TEXT`
| `@emit TEXT`                  | Echoes `TEXT` to all (other) objects in your current location, equivalent to `@oemit me TEXT`
| `@send TARGET TEXT`           | As `@echo` but echoes to `TARGET` instead of you
| `@oemit TARGET TEXT`          | As `@emit`, but emits from `TARGET` instead of you
| `@broadcast TARGET TEXT`      | As `@oemit`, but broadcasts to all objects within `TARGET` recursively
| `@zbroadcast TARGET TEXT`     | As `@broadcast`, but broadcasts to all objects within `TARGET`'s zone



`TARGET` (or `DEST`) may be:

* `me` for yourself
* `@name` for the Player called `name`
* `here` for your current location
* `#nnn` for the object with ID `nnn`
* for `@name`, `universe`, to rename the universe

Teleporting to a portal teleports the target to the portal's
destination.

## Flags

On all objects:

| Flag             | Description
|------------------|-----------------------
| `[S]` - `SYSTEM` | Cannot be `@destroy`ed; this flag cannot be set or cleared
| `[H]`- `HIDDEN`  | Not visible to Actors
| `[F]`- `FIXED`   | Cannot be `@teleport`ed


On Containers:

| Flag             | Description
|------------------|-----------------------------------------
| `[L]`- `LOCKED`         | Nothing can be placed inside this object/Actors cannot enter it

On Actors

| Flag                     | Description
|--------------------------|-----------------------
| `[I]` - `IMMORTAL`       | Takes no damage/cannot be killed

# Quick start

Create a database with `nexus-createdb`:

```
$ ./nexus-createdb test
```

then you can start building and exploring with `nexus-builder`:

```
$ ./nexus-builder test

Greetings, Operator!

You find yourself in Limbo, a place which does not exist.
Zero> @teleport me #2
You find yourself at the root of this particular universe.

Almost every object exists within this one.

Zero> @list
=======================================================================
   ID         Name
-----------------------------------------------------------------------
  #10Z        System
  #1P         Operator
=======================================================================
```
