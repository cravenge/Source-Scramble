/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Source Scramble Extension
 * Copyright (C) 2019 nosoop.  All rights reserved.
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

#include "extension.h"

Handle_t g_MemoryBlock;
MemoryBlockHandler g_MemoryBlockHandler;

Handle_t g_MemoryPatch;
MemoryPatchHandler g_MemoryPatchHandler;

SrcScramble g_SrcScramble;
SMEXT_LINK(&g_SrcScramble);

bool SrcScramble::SDK_OnLoad( char* error, size_t maxlength, bool late ) {
    sharesys->AddNatives(myself, g_SrcScrambleNatives);
    sharesys->RegisterLibrary(myself, "srcscramble");

    gameconfs->AddUserConfigHook("Patches", &g_Patches);

    g_MemoryBlock = handlesys->CreateType("MemoryBlock", 
        &g_MemoryBlockHandler, 
        0, 
        nullptr, 
        nullptr, 
        myself->GetIdentity(), 
        nullptr);

    g_MemoryPatch = handlesys->CreateType("MemoryPatch", 
        &g_MemoryPatchHandler, 
        0, 
        nullptr, 
        nullptr, 
        myself->GetIdentity(), 
        nullptr);

    rootconsole->ConsolePrint("[" SMEXT_CONF_LOGTAG "] Loaded successfully!");
    return true;
}

void SrcScramble::SDK_OnUnload() {
    rootconsole->ConsolePrint("[" SMEXT_CONF_LOGTAG "] Unloading...");

    handlesys->RemoveType(g_MemoryPatch, myself->GetIdentity());
    handlesys->RemoveType(g_MemoryBlock, myself->GetIdentity());

    gameconfs->RemoveUserConfigHook("Patches", &g_Patches);
}

void MemoryBlockHandler::OnHandleDestroy(HandleType_t type, void *object)
{
    delete static_cast< MemoryBlock* >( object );
}

bool MemoryBlockHandler::GetHandleApproxSize(HandleType_t type, void *object, unsigned int *pSize)
{
    *pSize = static_cast< unsigned int >( ( static_cast< MemoryBlock* >( object ) )->size );
    return true;
}

void MemoryPatchHandler::OnHandleDestroy(HandleType_t type, void *object)
{
    delete static_cast< MemoryPatch* >( object );
}