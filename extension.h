/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * =============================================================================
 * SourceMod Source Scramble Extension
 * 
 * Copyright (C) 2019 nosoop
 * Copyright (C) 2023 cravenge
 * All rights reserved
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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include "smsdk_ext.h"

#include "memoryblock.h"
#include "memorypatch.h"

class SrcScramble : public SDKExtension {
public:
# ifdef SMEXT_CONF_METAMOD
    /**
     * @brief Called when Metamod is attached, before the extension version is called.
     *
     * @param error			Error buffer.
     * @param maxlen		Maximum size of error buffer.
     * @param late			Whether or not Metamod considers this a late load.
     * @return				True to succeed, false to fail.
     */
    //virtual bool SDK_OnMetamodLoad( ISmmAPI* ismm, char* error, size_t maxlen, bool late );

    /**
     * @brief Called when Metamod's pause state is changing.
     * NOTE: By default this is blocked unless sent from SourceMod.
     *
     * @param paused		Pause state being set.
     * @param error			Error buffer.
     * @param maxlen		Maximum size of error buffer.
     * @return				True to succeed, false to fail.
     */
    //virtual bool SDK_OnMetamodPauseChange( bool paused, char* error, size_t maxlen );

# endif
    /**
     * @brief This is called after the initial loading sequence has been processed.
     *
     * @param error		Error message buffer.
     * @param maxlength	Size of error message buffer.
     * @param late		Whether or not the module was loaded after map load.
     * @return			True to succeed loading, false to fail.
     */
    virtual bool SDK_OnLoad( char* error, size_t maxlength, bool late );

    /**
     * @brief This is called once all known extensions have been loaded.
     */
    //virtual void SDK_OnAllLoaded();

    /**
     * @brief Called when the pause state is changed.
     */
    //virtual void SDK_OnPauseChange( bool paused );
	
    /**
     * @brief This is called once the extension unloading process begins.
     */
    virtual void SDK_OnUnload();

    /**
     * @brief Called after SDK_OnUnload, once all dependencies have been
     * removed, and the extension is about to be removed from memory.
     */
    //virtual void SDK_OnDependenciesDropped();
# ifdef SMEXT_CONF_METAMOD

    /**
     * @brief Called when Metamod is detaching, after the extension version is called.
     * NOTE: By default this is blocked unless sent from SourceMod.
     *
     * @param error			Error buffer.
     * @param maxlen		Maximum size of error buffer.
     * @return				True to succeed, false to fail.
     */
    //virtual bool SDK_OnMetamodUnload( char* error, size_t maxlen );
# endif
};

class MemoryBlockHandler : public IHandleTypeDispatch {
public:
    void OnHandleDestroy(HandleType_t type, void *object);
    bool GetHandleApproxSize(HandleType_t type, void *object, unsigned int *pSize);
};

class MemoryPatchHandler : public IHandleTypeDispatch {
public:
    void OnHandleDestroy(HandleType_t type, void *object);
};

extern Handle_t g_MemoryBlock;
extern Handle_t g_MemoryPatch;

extern sp_nativeinfo_t g_SrcScrambleNatives[];

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_