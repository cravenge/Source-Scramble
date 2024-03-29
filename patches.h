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

#ifndef _INCLUDE_SOURCEMOD_SRCSCRMBL_PATCHES_H_
#define _INCLUDE_SOURCEMOD_SRCSCRMBL_PATCHES_H_

#include "smsdk_ext.h"

#include <string>
#include <vector>

#include <sm_stringhashmap.h>

class PatchGameConfig : public ITextListener_SMC {
    int m_ParseState;
    unsigned int m_IgnoreLevel;

    std::string m_Patch;
    std::string m_PatchSignature;

    int m_PatchOffset;

    std::vector< uint8_t > m_PatchMatch;
    std::vector< uint8_t > m_PatchPreserve;
    std::vector< uint8_t > m_PatchOverwrite;

    bool m_PatchOneTime;
public:
    void ReadSMC_ParseStart();
    SMCResult ReadSMC_NewSection(const SMCStates *states, const char *name);
    SMCResult ReadSMC_KeyValue(const SMCStates *states, const char *key, const char *value);
    SMCResult ReadSMC_LeavingSection(const SMCStates *states);

    struct PatchConf {
        PatchConf() {}
        PatchConf( std::string &&sigName, int ofst, std::vector< uint8_t > &&mtch, std::vector< uint8_t > &&presv, std::vector< uint8_t > &&ovr, bool ot );

        std::string signatureName;
        int offset;
        std::vector< uint8_t > match;
        std::vector< uint8_t > preserve;
        std::vector< uint8_t > overwrite;
        bool onetime;
    };

    StringHashMap< PatchConf > m_Patches;
};

extern PatchGameConfig g_Patches;

#endif // _INCLUDE_SOURCEMOD_SRCSCRMBL_PATCHES_H_