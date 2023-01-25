# Source Scramble

[:coffee: fund my caffeine addiction :coffee:](https://buymeacoff.ee/nosoop)

A SourceMod extension that provides:
- An easy way for plug-ins to validate and patch platform-specific address locations with a game configuration file
- 2 new handle types for plug-ins to allocate and free their own memory blocks
- Additional memory-related utilities

## Installation

This is the installation process for end-users.

1.  Download the latest package under the Actions tab (`-win-` for Windows, `-nix-` for
older Linux distributions or `linux` for newer ones)
2.  Unpack the downloaded archive into your game/server installation's alias (i.e `cstrike` for CS:S and CS:GO) directory

Patches can only be applied where a plug-in (usually by the patch author) performs the task itself.

It's up to the developer of each patch to provide instructions on how to install them.
Regardless, all patches do require a game configuration file installed in `gamedata/`.

## Origin

This was originally just dedicated to memory patching.  I had a number of gripes with existing
solutions like [Memory Patcher][], [No Thriller Taunt][], and one-off plug-ins for this purpose:

1.  Plug-ins either hardcode the solution, or use some custom conventions in their game config file (solutions I've seen others use / written myself either pollute the `Keys` section or do their own config parsing with `SMCParser`)
2.  There is no verification on bytes to be patched (what if a game update modifies the
function?)
3.  Reverting the patch would have to be manually implemented.  Some plug-ins neglect to do this.

Writing it as an extension allows it to:

1.  Leverage SourceMod's game configuration parsing logic.  No need to reparse the file, and
this provides a uniform convention for patches.
2.  Use the SourceMod handle system; this allows for managed cleanup and provides a relatively
nice API for developers to work with.

[Memory Patcher]: https://forums.alliedmods.net/showthread.php?p=2617543
[No Thriller Taunt]: https://forums.alliedmods.net/showthread.php?t=171343

## Developer usage

There are a number of things provided with this extension:

- Memory blocks, which provide a Handle type to allocate memory.
- Memory patches, which are a game config-dedicated section that allows a plug-in to safely
overwrite and restore the contents of a memory location.
- Get*Address natives.

Developing with this extension is intended for power users that are already getting their hands
dirty with server internals.  Most developers do not need this kind of flexibility.

### Memory patches

Memory patches allow developers to declare a byte payload to be written to memory.

#### Creating new patches

Since patches generally operate on machine code, you'll need to be familiar with that to write
patches.

A new `Patches` section is added at the same level of `Addresses`, `Offsets`, and
`Signatures` in the game configuration file.  An example of a section is below:

```
"Patches"
{
	// this patch makes buildings solid to TFBots by forcing certain code paths
	"CTraceFilterObject::ShouldHitEntity()::TFBotCollideWithBuildings"
	{
		"signature" 	"CTraceFilterObject::ShouldHitEntity()"
		"linux"
		{
			"offset"	"1A6h"
			"match"		"\x75"
			"replace"	"\x70"
		}
		"windows"
		{
			"offset"	"9Ah"
			"match"		"\x74"
			"replace"	"\x71"
		}
	}
}
```

A few things are present:

- A subsection.  The name of the section will be the name used when getting a `MemoryPatch`
handle with `MemoryPatch.FromConf()`.
- A function `signature` name referencing the name of a signature in a `Signatures` section
somewhere else in the game config file.
- The `offset` to patch.  Hexadecimal notation is supported with the `h` suffix, for easy
referencing in IDA or similar.
- `replace` (required) and `match` (optional) Hex strings (`\x01\x02\x03`) indicating the byte payload and a signature to match against at the previously mentioned offset.
	- `verify` signatures can use `\x2A` to indicate wildcards, same as SourceMod.
- An optional `preserve` hex string indicating which bits from the original location should be
copied to the patch.  (New in 0.7.x.)
	- For example, if you want to copy the high 4 bits in a byte from the original memory,
	that would be represented in binary as `0b11110000`, and you would use `\xF0`.

Any values written on top of an applied patch will be reverted back when the patch is removed.

#### Applying patches

For more complex cases (e.g., scoped hook memory patches or potentially dynamic patch
modifications), you'll have to write your own plug-in to patch / unpatch the memory as desired.

This should be fairly self-explanatory:

```sourcepawn
// GameData hGameConf = new GameData(...);

// as mentioned, patches are cleaned up when the handle is deleted
MemoryPatch patch = MemoryPatch.FromConf(hGameConf, "CTraceFilterObject::ShouldHitEntity()::TFBotCollideWithBuildings");

if (!patch.Validate()) {
	ThrowError("Failed to verify patch.");
} else if (patch.Enable()) {
	LogMessage("Enabled patch.");
}

// ...

// restore the bytes that were in place when the patch was enabled
// any writes on top of the patched area are also wiped
patch.Disable();
```

### Memory blocks

A `MemoryBlock` is a `calloc`-allocated chunk of memory that can be accessed with
`StoreToAddress` and `LoadFromAddress` (indirectly via wrapped helper methods).

Some patches I've dealt with operate on fixed locations in memory (e.g., floating point load
operations that don't take immediate values), so with this I can point to the `MemoryBlock`
address space and put in whatever I need.

It's also a useful way of allocating structures for things like `SDKCall`s.

Basic use of the API:

```sourcepawn
// allocates and zero-initializes 4 bytes of memory
MemoryBlock block = new MemoryBlock(4);

block.Set(0, view_as<int>(0.75), NumberType_Int32);

Address pFloatBlock = block.Address;

// frees the 4 bytes that was allocated
delete block;
```

### Get*Address natives

New to Source Scramble 0.6.x, this allows a plug-in to get the address of one of its own
variables (`cell_t` or `char[]`).

This replaces certain use cases of memory blocks; you can now point to an existing variable in
the plug-in's memory space in cases when you need to read a float value or more granular control
in things like DHooks to send a fixed buffer.

```sourcepawn
float g_flValue;

Address pFloatLocation = GetCellAddress(g_flValue);
// patch an indirect load or whatever with the address of that float value

g_flValue = 0.75; // changes are reflected instantly wherever this memory location is referenced
```

Of course, this is all use-at-your-own-risk.
