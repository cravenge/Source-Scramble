/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * =============================================================================
 * Source Scramble (C)2019 nosoop.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#ifdef _srcscramble_included
 #endinput
#endif

#define _srcscramble_included

methodmap MemoryBlock < Handle
{
	// Creates a static global block
	//
	// The "size" determines how many entries the block has; it cannot be
	// changed after creation.
	//
	// @param size          The number of entries the block can hold
	// @return              A handle to the memory block or null on failure
	public native MemoryBlock(int size);

	// Get the current size of the allocated memory block
	property int Size {
		public native get();
	}

	// Get the current address of the allocated memory block
	property Address Address {
		public native get();
	}

	// Retrieve up to 4 bytes from a block
	//
	// @param index         Index in the block
	// @param size          How many bytes should be read
	//                      If retrieving a floating-point value, use NumberType_Int32
	// @return              Value(s) read
	// @error               index + GetNumberTypeByteCount(size) > this.Size
	public int Get(int index, NumberType size) {
		if( index < 0 || index + GetNumberTypeByteCount(size) > this.Size ) {
			ThrowError("Invalid index %d (count: %d)", index, this.Size);
		}
		
		return LoadFromAddress(this.Address + view_as<Address>(index), size);
	}

	// Sets up to 4 bytes in a block
	//
	// @param index         Index in the block
	// @param value         Value to set
	// @param size          How many bytes should be written
	//                      If setting a floating-point value, use NumberType_Int32
	// @error               index + GetNumberTypeByteCount(size) > this.Size
	public void Set(int index, any data, NumberType size) {
		if( index < 0 || index + GetNumberTypeByteCount(size) > this.Size ) {
			ThrowError("Invalid index %d (count: %d)", index, this.Size);
		}
		
		StoreToAddress(this.Address + view_as<Address>(index), data, size);
	}
};

methodmap MemoryPatch < Handle
{
	// Finds an address in a GameConfig file and makes a memory patch out of the
	// obtained information
	//
	// @param gameconf      GameConfig Handle
	// @param name          Name of the property to find
	// @return              A handle to the memory patch or null on failure
	public static native MemoryPatch FromConf(Handle gameconf, const char[] name);

	// Validates a patch
	//
	// @return              True if check passes, false if not
	public native bool Validate();

	// Enables a patch
	//
	// Note: Use Validate() first if the information from FromConf()
	//       has a "match" key specified
	//
	// @return              True on success, false otherwise
	public native bool Enable();

	// Disables a patch
	//
	// @return              True on success, false otherwise
	public native bool Disable();

	// Get the current address of the memory patch
	property Address Address {
		public native get();
	}

	// Get the current size of the memory patch
	property int Size {
		public native get();
	}
}

/**
 * Creates a static global block
 *
 * The "size" determines how many entries the block has; it cannot be
 * changed after creation.
 *
 * @param size              The number of entries the block can hold
 * @return                  A handle to the memory block or null on failure
 */
native MemoryBlock CreateMemoryBlock(int size);

/**
 * Get current size of the allocated memory block
 *
 * @param block             Block Handle
 * @return                  The size of the block
 * @error					Invalid Handle
 */
native int GetMemoryBlockSize(Handle block);

/**
 * Get current address of the allocated memory block
 *
 * @param block             Block Handle
 * @return                  The address of the block
 * @error					Invalid Handle
 */
native Address GetMemoryBlockAddress(Handle block);

/**
 * Retrieve up to 4 bytes from a block
 *
 * @param block             Block Handle
 * @param index             Index in the block
 * @param size              How many bytes should be read
 *                          If retrieving a floating-point value, use NumberType_Int32
 * @return                  Value(s) read
 * @error                   Invalid Handle or index + GetNumberTypeByteCount(size) > GetBlockSize()
 */
stock int GetBlockEntry(Handle block, int index, NumberType size)
{
	int blocksize = GetBlockSize(block);
	if (index < 0 || index + GetNumberTypeByteCount(size) > blocksize)
	{
		ThrowError("Invalid index %d (count: %d)", index, blocksize);
	}
	
	return LoadFromAddress(GetBlockAddress(block) + view_as<Address>(index), size);
}

/**
 * Sets up to 4 bytes in a block
 *
 * @param block             Block Handle
 * @param index             Index in the block
 * @param size              How many bytes should be written
 *                          If setting a floating-point value, use NumberType_Int32
 * @error                   Invalid Handle or index + GetNumberTypeByteCount(size) > GetBlockSize()
 */
stock void SetBlockEntry(Handle block, int index, any data, NumberType size)
{
	int blocksize = GetBlockSize(block);
	if (index < 0 || index + GetNumberTypeByteCount(size) > blocksize)
	{
		ThrowError("Invalid index %d (count: %d)", index, blocksize);
	}
	
	StoreToAddress(GetBlockAddress(block) + view_as<Address>(index), data, size);
}

/**
 * Returns how many byte(s) there is/are
 *
 * @param size              Representation of the bytes
 *
 * @return                  The amount of byte(s)
 */
stock static int GetNumberTypeByteCount(NumberType size)
{
	switch (size)
	{
		case NumberType_Int8: return 1;
		case NumberType_Int16: return 2;
		case NumberType_Int32: return 4;
	}
	
	ThrowError("Invalid number type %d", size);
	return 0;
}

/**
 * Finds an address in a GameConfig file and makes a memory patch out of the
 * obtained information
 *
 * @param gameconf          GameConfig Handle
 * @param name              Name of the property to find
 * @return                  A handle to the memory patch or null on failure
 * @error                   Invalid Handle or property name
 */
native MemoryPatch MemoryPatch_FromConf(Handle gameconf, const char[] name);

/**
 * Validates a patch
 *
 * @param patch             Patch Handle
 * @return                  True if check passes, false if not
 * @error                   Invalid Handle
 */
native bool ValidateMemoryPatch(Handle patch);

/**
 * Enables a patch
 *
 * Note: Use ValidateMemoryPatch() first if the information from MemoryPatch_FromConf()
 *       has a "match" key specified
 *
 * @param patch             Patch Handle
 * @return                  True on success, false otherwise
 * @error                   Invalid Handle
 */
native bool EnableMemoryPatch(Handle patch);

/**
 * Disables a patch
 *
 * @param patch             Patch Handle
 * @return                  True on success, false otherwise
 * @error                   Invalid Handle
 */
native bool DisableMemoryPatch(Handle patch);

/**
 * Get current address of the memory patch
 *
 * @param patch             Patch Handle
 * @return                  The address of the patch
 * @error                   Invalid Handle
 */
native Address GetMemoryPatchAddress(Handle patch);

/**
 * Get current size of the memory patch
 *
 * @param patch             Patch Handle
 *
 *
native int GetMemoryPatchSize(Handle patch);

/**
 * Returns an address calculated from a cell reference
 *
 * @param cell              Cell reference to calculate
 *
 * @return                  An address calculated
 * @error                   Calculation returned something else other than SP_ERROR_NONE
 */
native Address GetCellAddress(any &cell);

/**
 * Returns an address calculated from a string
 *
 * @param str               String to calculate
 *
 * @return                  An address calculated
 * @error                   Calculation returned something else other than SP_ERROR_NONE
 */
native Address GetStringAddress(char[] str);

public Extension __ext_srcscrmbl =
{
	name = "Source Scramble",
	file = "srcscramble.ext",
#ifdef AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#ifdef REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#ifndef REQUIRE_EXTENSIONS
public void __ext_srcscrmbl_SetNTVOptional()
{
	MarkNativeAsOptional("CreateMemoryBlock");
	MarkNativeAsOptional("GetMemoryBlockSize");
	MarkNativeAsOptional("GetMemoryBlockAddress");
	MarkNativeAsOptional("MemoryPatch_FromConf");
	MarkNativeAsOptional("ValidateMemoryPatch");
	MarkNativeAsOptional("EnableMemoryPatch");
	MarkNativeAsOptional("DisableMemoryPatch");
	MarkNativeAsOptional("GetMemoryPatchAddress");
	MarkNativeAsOptional("GetMemoryPatchSize");
	
	MarkNativeAsOptional("GetCellAddress");
	MarkNativeAsOptional("GetStringAddress");
	
	MarkNativeAsOptional("MemoryBlock.MemoryBlock");
	MarkNativeAsOptional("MemoryBlock.Size.get");
	MarkNativeAsOptional("MemoryBlock.Address.get");
	MarkNativeAsOptional("MemoryPatch.FromConf");
	MarkNativeAsOptional("MemoryPatch.Validate");
	MarkNativeAsOptional("MemoryPatch.Enable");
	MarkNativeAsOptional("MemoryPatch.Disable");
	MarkNativeAsOptional("MemoryPatch.Address.get");
	MarkNativeAsOptional("MemoryPatch.Size.get");
}
#endif
