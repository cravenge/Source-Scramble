/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * =============================================================================
 * Source Scramble (C)2019 nosoop.  All rights reserved
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2", the
 * "Source Engine", the "SourcePawn JIT" and any Game MODs that run on software
 * by the Valve Corporation. You must obey the GNU General Public License in
 * all respects for all other code used. Additionally, AlliedModders LLC grants
 * this exception to all derivative works. AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>
 *
 * Version: $Id$
 */

#if defined _srcscramble_included
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

	// Disowns a block
	//
	// @error               Block not owned by extension anymore
	public native void Disown();

	// Retrieves up to 4 bytes from a block
	//
	// @param index         Index in the block
	// @param size          How many bytes should be read
	//                      If retrieving a floating-point value, use NumberType_Int32
	// @return              Value read
	// @error               index + GetNumberTypeByteCount(size) > this.Size
	public int GetData(int index, NumberType size) {
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
	public void SetData(int index, any value, NumberType size) {
		if( index < 0 || index + GetNumberTypeByteCount(size) > this.Size ) {
			ThrowError("Invalid index %d (count: %d)", index, this.Size);
		}
		
		StoreToAddress(this.Address + view_as<Address>(index), value, size);
	}

	// Retrieves the size of the block
	property int Size {
		public native get();
	}

	// Retrieves the address of the block
	property Address Address {
		public native get();
	}
};

methodmap MemoryPatch < Handle
{
	// Finds a section in a GameConfig file and makes a memory patch out of the
	// obtained information
	//
	// @param gameconf      GameConfig Handle
	// @param name          Name of the section to find
	// @return              A handle to the memory patch or null on failure
	public static native MemoryPatch FromConf(Handle gameconf, const char[] name);

	// Validates a patch
	//
	// @return              True if check passes, false if not
	public native bool Validate();

	// Returns whether or not a patch is active
	public native bool IsActive();

	// Enables a patch
	//
	// @note Use Validate() first if the information from FromConf() has a "match"
	//       key specified
	//
	// @return              True on success, false otherwise
	public native bool Enable();

	// Disables a patch
	//
	// @return              True on success, false otherwise
	public native bool Disable();

	// Retrieves the size of a patch's data type
	//
	// @note Valid data types are "match", "preserve", "overwrite" and "original"
	//
	// @param type          Type of patch data
	// @return              The size of a patch's data type
	// @error               Invalid patch data type
	public native int GetSize(const char[] type);

	// Sets the size of a patch's data type. If the size is smaller than the
	// current size, the patch's data type is truncated. If the size is larger
	// than the current size, the data at the additional indexes will be
	// initialized to zero
	//
	// @note Valid data types are "match", "preserve", "overwrite" and "original"
	//
	// @param type          Type of patch data
	// @param newsize       New size
	// @error               Invalid patch data type
	public native void SetSize(const char[] type, int newsize);

	// Retrieves a value from a patch's data type
	//
	// @note Valid data types are "match", "preserve", "overwrite" and "original"
	//
	// @param type          Type of patch data
	// @param index         Index in the patch data type
	// @return              Value read
	// @error               Invalid patch data type or index
	public native int GetData(const char[] type, int index);

	// Sets a value in a patch's data type
	//
	// @note Valid data types are "match", "preserve", "overwrite" and "original"
	// @note Accepted values can only be within the range of -255 to 255
	//
	// @param type          Type of patch data
	// @param index         Index in the patch data type
	// @param value         Value to set
	// @error               Invalid patch data type, index or value
	public native void SetData(const char[] type, int index, int value);

	// Discards a patch
	//
	// @error               Patch not tended to by extension anymore
	public native void Discard();

	// Retrieves the address of the patch
	property Address Address {
		public native get();
	}
}

/**
 * Returns how many bytes there are
 *
 * @param size              Representation of the bytes
 * @return                  The amount of bytes
 * @error					Invalid size
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
 * Disowns a block
 *
 * @param block             Block Handle
 * @error                   Invalid Handle or block not owned by extension anymore
 */
native void DisownMemoryBlock(Handle block);

/**
 * Retrieves up to 4 bytes from a block
 *
 * @param block             Block Handle
 * @param index             Index in the block
 * @param size              How many bytes should be read
 *                          If retrieving a floating-point value, use NumberType_Int32
 * @return                  Value read
 * @error                   Invalid Handle or index + GetNumberTypeByteCount(size) > GetMemoryBlockSize()
 */
stock int GetMemoryBlockData(Handle block, int index, NumberType size)
{
	int blocksize = GetMemoryBlockSize(block);
	if (index < 0 || index + GetNumberTypeByteCount(size) > blocksize)
	{
		ThrowError("Invalid index %d (count: %d)", index, blocksize);
	}
	
	return LoadFromAddress(GetMemoryBlockAddress(block) + view_as<Address>(index), size);
}

/**
 * Sets up to 4 bytes in a block
 *
 * @param block             Block Handle
 * @param index             Index in the block
 * @param size              How many bytes should be written
 *                          If setting a floating-point value, use NumberType_Int32
 * @error                   Invalid Handle or index + GetNumberTypeByteCount(size) > GetMemoryBlockSize()
 */
stock void SetMemoryBlockData(Handle block, int index, any value, NumberType size)
{
	int blocksize = GetMemoryBlockSize(block);
	if (index < 0 || index + GetNumberTypeByteCount(size) > blocksize)
	{
		ThrowError("Invalid index %d (count: %d)", index, blocksize);
	}
	
	StoreToAddress(GetMemoryBlockAddress(block) + view_as<Address>(index), value, size);
}

/**
 * Retrieves the size of the block
 *
 * @param block             Block Handle
 * @return                  The size of the block
 * @error					Invalid Handle
 */
native int GetMemoryBlockSize(Handle block);

/**
 * Retrieves the address of the block
 *
 * @param block             Block Handle
 * @return                  The address of the block
 * @error					Invalid Handle
 */
native Address GetMemoryBlockAddress(Handle block);

/**
 * Finds a section in a GameConfig file and makes a memory patch out of the
 * obtained information
 *
 * @param gameconf          GameConfig Handle
 * @param name              Name of the section to find
 * @return                  A handle to the memory patch or null on failure
 * @error                   Invalid Handle or section name
 */
native MemoryPatch CreateMemoryPatchFromConf(Handle gameconf, const char[] name);

/**
 * Validates a patch
 *
 * @param patch             Patch Handle
 * @return                  True if check passes, false if not
 * @error                   Invalid Handle
 */
native bool ValidateMemoryPatch(Handle patch);

/**
 * Returns whether or not a patch is active
 *
 * @param patch             Patch Handle
 * @return                  True if active patch, false if not
 * @error                   Invalid Handle
 */
native bool IsMemoryPatchActive(Handle patch);

/**
 * Enables a patch
 *
 * @note Use ValidateMemoryPatch() first if the information from CreateMemoryPatchFromConf()
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
 * Retrieves the size of a patch's data type
 *
 * @note Valid patch data types are "match", "preserve", "overwrite" and "original"
 *
 * @param patch             Patch Handle
 * @param type              Type of patch data
 * @return                  The size of a patch's data type
 * @error                   Invalid Handle or patch data type
 */
native int GetMemoryPatchSize(Handle patch, const char[] type);

/**
 * Sets the size of a patch's data type. If the size is smaller than the current size, 
 * patch's data type is truncated. If the size is larger than the current size, the data at
 * the additional indexes will be initialized to zero
 *
 * @note Valid patch data types are "match", "preserve", "overwrite" and "original"
 *
 * @param patch             Patch Handle
 * @param type              Type of patch data
 * @param newsize           New size
 * @error                   Invalid Handle or patch data type
 */
native void SetMemoryPatchSize(Handle patch, const char[] type, int newsize);

/**
 * Retrieves a value in a patch's data type
 *
 * @note Valid patch data types are "match", "preserve", "overwrite" and "original"
 *
 * @param patch             Patch Handle
 * @param type              Type of patch data
 * @param index             Index in the patch data type
 * @return                  Value read
 * @error                   Invalid Handle, patch data type or index
 */
native int GetMemoryPatchData(Handle patch, const char[] type, int index);

/**
 * Sets a value in a patch's data type
 *
 * @note Valid patch data types are "match", "preserve", "overwrite" and "original"
 * @note Accepted values can only be within the range of -255 to 255
 *
 * @param patch             Patch Handle
 * @param type              Type of patch data
 * @param index             Index in the patch data type
 * @param value             Value to set
 * @error                   Invalid Handle, patch data type, index or value
 */
native void SetMemoryPatchData(Handle patch, const char[] type, int index, int value);

/**
 * Discards a patch
 *
 * @param patch             Patch Handle
 * @error                   Invalid Handle or patch not tended to by extension anymore
 */
native void DiscardMemoryPatch(Handle patch);

/**
 * Retrieves the address of the patch
 *
 * @param patch             Patch Handle
 * @return                  The address of the patch
 * @error                   Invalid Handle
 */
native Address GetMemoryPatchAddress(Handle patch);

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
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public void __ext_srcscrmbl_SetNTVOptional()
{
	MarkNativeAsOptional("CreateMemoryBlock");
	MarkNativeAsOptional("DisownMemoryBlock");
	MarkNativeAsOptional("GetMemoryBlockSize");
	MarkNativeAsOptional("GetMemoryBlockAddress");
	MarkNativeAsOptional("CreateMemoryPatchFromConf");
	MarkNativeAsOptional("ValidateMemoryPatch");
	MarkNativeAsOptional("IsMemoryPatchActive");
	MarkNativeAsOptional("EnableMemoryPatch");
	MarkNativeAsOptional("DisableMemoryPatch");
	MarkNativeAsOptional("GetMemoryPatchSize");
	MarkNativeAsOptional("SetMemoryPatchSize");
	MarkNativeAsOptional("GetMemoryPatchData");
	MarkNativeAsOptional("SetMemoryPatchData");
	MarkNativeAsOptional("DiscardMemoryPatch");
	MarkNativeAsOptional("GetMemoryPatchAddress");
	
	MarkNativeAsOptional("GetCellAddress");
	MarkNativeAsOptional("GetStringAddress");
	
	MarkNativeAsOptional("MemoryBlock.MemoryBlock");
	MarkNativeAsOptional("MemoryBlock.Disown");
	MarkNativeAsOptional("MemoryBlock.Size.get");
	MarkNativeAsOptional("MemoryBlock.Address.get");
	MarkNativeAsOptional("MemoryPatch.FromConf");
	MarkNativeAsOptional("MemoryPatch.Validate");
	MarkNativeAsOptional("MemoryPatch.IsActive");
	MarkNativeAsOptional("MemoryPatch.Enable");
	MarkNativeAsOptional("MemoryPatch.Disable");
	MarkNativeAsOptional("MemoryPatch.GetSize");
	MarkNativeAsOptional("MemoryPatch.SetSize");
	MarkNativeAsOptional("MemoryPatch.GetData");
	MarkNativeAsOptional("MemoryPatch.SetData");
	MarkNativeAsOptional("MemoryPatch.Discard");
	MarkNativeAsOptional("MemoryPatch.Address.get");
}

#endif