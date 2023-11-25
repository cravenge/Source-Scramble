/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * =============================================================================
 * SourceMod Source Scramble Extension
 * 
 * Copyright (C) 2019 nosoop
 * Copyright (C) 2023 cravenge
 *
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

#ifndef _INCLUDE_SOURCEMOD_SRCSCRMBL_MEMPATCH_H_
#define _INCLUDE_SOURCEMOD_SRCSCRMBL_MEMPATCH_H_

#include "patches.h"

struct MemoryPatch {
    MemoryPatch( void* addr, std::vector< uint8_t > &&mtch, std::vector< uint8_t > &&presv, std::vector< uint8_t > &&ovr, bool ot );
    MemoryPatch( void* addr, const PatchGameConfig::PatchConf &info );
    ~MemoryPatch();

    bool Validate();
    bool Enable();
    bool Disable();

    void* pAddr;

    std::vector< uint8_t > match;
    std::vector< uint8_t > preserve;
    std::vector< uint8_t > overwrite;
    std::vector< uint8_t > original;

    bool onetime;
};

#endif // _INCLUDE_SOURCEMOD_SRCSCRMBL_MEMPATCH_H_