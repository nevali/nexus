# West of House

Back in 1975, a programmer started putting together the first of a brand new
genre of computer game: [the text adventure](https://wikipedia.org/wiki/Colossal_Cave_Adventure).

A little while later, and some nerds at MIT created what was to become the
most successful and iconic example of the genre, and it was called `DUNGEON`,
right up until the point that MIT got threatened with legal action over the
name, and so (the story goes), even though MIT's lawyers were pretty confident
about their chances, they decided to give it a new name, and so it began…

```
West of House
This is an open field west of a white house, with a boarded front door.
There is a small mailbox here.
A rubber mat saying 'Welcome to Zork!' lies by the door.`
```

Zork had a dungeon! a dragon! a wizard! a thief! treasure! labyrinths!
puzzles! (but it was not an RPG)

But _this_ project is not especially about Zork, nor Infocom—the company
those MIT nerds founded to sell a chaptersied version of their game that
would fit on the tiny (compared to MIT's mainframe) home computers of the
day. There are lots of open source projects out there that re-create the
[Z-Machine](https://wikipedia.org/wiki/Z-machine) that powered the Zork
adventures. This is not one of them.

By 1978, a student at the University of Essex decided to try creating a
new (or old, if you consider table-top RPGs to be at least some of the
inspiration for many of these games) spin on the idea: _multi-player_
dungeon. But that isn't a good acronym and so instead the [MUD](https://en.wikipedia.org/wiki/Multi-user_dungeon),
or Multi-User Dungeon—was born.

As universities were connected to one another, lots of other people over the
years had the idea of running their own MUD, and there have been both lots
of MUD servers, but also lots of different MUD platforms (_codebases_), and
also quite a bit of people sharing the the worlds that they've designed with
one another so that you can play them on _your_ server with your friends.

Generally, MUDs stored the information about the world you and your friends
(and/or enemies) were roaming around—all the locations, the NPCs, the treasures,
and puzzles, and hazards, in a bunch of files on disk in one of an assortment
of poorly-documented but serviceable file formats.

It's important to remember that a lot of MUDs were still being run primarily
out of student accounts on university servers, and so space was at an absolute
premium.

But over time, computers got bigger and faster and felt a little bit roomier,
and by the time the 1990s came along, some branches of the MUD family tree
were going big on flexibility: the more you could do without having to edit
the codebase itself the better, and the holy grail was always to be able to
build an entire world, and define everything about how it functions and players
interact with it "online"—that is, you connect to the server as a player with
sufficient privileges, and you have extra commands available that let you build
what you want. No more messing around with files on disk and having to force
your server to reload them; no more recompiling codebases to get new and
interesting NPC behaviours.

Codebases such as [TinyMUX](https://www.tinymux.org) implemented precisely this
idea, and I—at the time—really liked a great deal about what they were trying
to do, but I could never _quite_ get comfortable with how any of them worked:
either I was so distracted by features missing that I wanted that I'd get lost
in writing code instead of world-building, or I'd really struggle to get to
grips with the world-building commands themselves: bootstrapping was often
quite hard, and naturally a lot of the libraries of "softcode", as it's known
(in contrast to "hardcode", which is compiled-in) would end up being quite
opinionated about how things worked.

And naturally I wanted to build my own from scratch in 1996 or so but none
of my attempts ever made substantial progress, not least because I definitely
wasn't good enough at software architecture to have been able to pull it off:
these things are complex.

But it's now 2023, and although I don't necessarily want to run a MUD, I have
yet to scratch that itch of this _kind_ of world-building environment that
works the way I wanted it to (which is, all credit where it's due, absolutely
inspired by and influenced by TinyMUX: I spent a lot of time building things
on a TinyMUX-based [talker](https://en.wikipedia.org/wiki/Talker) which had
enjoyed a huge amount of collective effort to create what was there and that
I could learn from).

And so this codebase is the beginning of that, put together very simply
because time has moved on and whilst reading JSON files from disk on-demand
would have been pretty unthinkable approximation of a workable "database"
in the mid-1990s, but for a single user doing that on a laptop with modern
kernel caches and SSDs, it's faster than anything but an in-memory database
would have been back then (and probably much faster than that). Also, I don't
need to parse JSON myself. Or invent it.

_Side-note:_ In any event, all of the objects in this implementation are
reference-counted once loaded and provided they lie within a specific
designated "root" object which is pre-loaded at startup, they'll be cached in
memory anyway.

# What is it, and how does it work?

Okay, TL;DR version: there's a hierarchical tree of objects which can be of
several different kinds ranging from fairly self-explanatory ones such as
"room" and "player" to somewhat more interesting things like "executable"
and "timer". There are a set of built-in commands, known as _at-commands_,
because they all begin with a `@` sign. Using these commands, you can create,
modify, and destroy these different kinds of objects.

Make sense? Cool. Here comes the science…

Once you have created a database with the `nexus-createdb` command (which
simply creates a directory and puts some initial JSON files in it), you
can launch `nexus-builder`. This is essentially "god mode": apart from certain
system objects, you can do anything (to the extent that the commands have
been implemented).

`nexus-builder`, as you'll see from the code, is actually pretty trivial,
but all it needs to do is construct a `Universe` and find the `Player`
representing you—which is called `Operator`—and ask it to perform the
commands that you type in. Once there's sufficient world-building capability
implemented that there's something worth _playing_, the plan is to add a
`nexus-player` tool that also operates on the same database, but creates (or
locates) a _different_ `Player` which doesn't, at least not by default,
have the privileges to do the same kind of world-building.

(This could of course just be the same binary inspecting `argv[0]` or having
an "Operator mode" command-line option)

One would be forgiven for envisaging this and wondering how hard it would be
to _make_ it multi-user at this point? Well, comparatively, probably not
very, unless you decided to introduce multithreading or something (don't). We'll
cross that bridge if and when we arrive at it.

# Dependencies, building, etc.

For reasons may become clearer later, this is partly built using a library
called Flux that I am putting together in the aid of implementing something
else entirely, but for _now_ the only actual use is that all objects are
subclasses of `WARP::Flux::Object`, which provides a simple (not thread-safe)
reference-counted object via explicit `retain()` and `release()` calls (and
`protected` virtual destructors and no use of `delete` except in the base's
own `release()` method). Obviously this code could be trivially expropriated
if it becomes expedient to drop the dependency, although I would prefer not
to. It's included as a submodule to ease that, so ensure after cloning you

```sh
git submodule update --init --recursive
```

Until I autotoolsify this project, you'll need to manually run:

```sh
( cd WARP && autoreconf -fvi && ./configure && make )
```

You can then

```sh
make
./nexus-createdb db
./nexus-builder db
```

There's also a `Dockerfile` with some interesting targets that will do some
of this for you. The snag is that `nexus-createdb` requires the directory
it creates does not already exist, which means you have to mount the _parent_
directory into the container. `nexus-createdb` should really just have a
command-line switch to force it to use an existing (empty) directory.

If you manage to get it up and running, `@commands` lists available commands.

At the time of writing, the parser doesn't know about quoted arguments or
escaping so you can only set something's description to a single world. This
is suboptimal.

(You can, however, always edit the `.json` and `.desc` files—don't do that
with the builder running, or else it will overwrite your changes—it doesn't
yet detect that files on disk have changed and managing that might be
quite tricky with the current implementation)

# Bugs and to-do

Vast numbers of both. This is pre-alpha.
