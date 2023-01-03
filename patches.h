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

#ifndef _INCLUDE_SOURCEMOD_SRCSCRMBL_PATCHES_H_
#define _INCLUDE_SOURCEMOD_SRCSCRMBL_PATCHES_H_

#include "smsdk_ext.h"

#ifndef _GLIBCXX_STRING
#include <string>
#endif
#ifndef _GLIBCXX_VECTOR
#include <vector>
#endif

#include <sm_stringhashmap.h>

class PatchGameConfig : public ITextListener_SMC {
public:
    void ReadSMC_ParseStart();
    SMCResult ReadSMC_NewSection(const SMCStates *states, const char *name);
	SMCResult ReadSMC_KeyValue(const SMCStates *states, const char *key, const char *value);
	SMCResult ReadSMC_LeavingSection(const SMCStates *states);
private:
    int m_ParseState;
    unsigned int m_IgnoreLevel;
public:
    struct PatchConf {
        std::string signatureName;
        int offset;
        std::vector<uint8_t> verify;
        std::vector<uint8_t> preserve;
        std::vector<uint8_t> replace;

        PatchConf( std::string&& sigName, int ofst, std::vector<uint8_t>&& vrfy, std::vector<uint8_t>&& presv, std::vector<uint8_t>&& repl );
        
        PatchConf() {}
    };
private:
    std::string m_Patch;
    std::string m_PatchSignature;
    int m_PatchOffset;
    std::vector<uint8_t> m_PatchVerify;
    std::vector<uint8_t> m_PatchPreserve;
    std::vector<uint8_t> m_PatchReplace;
public:
    StringHashMap<PatchConf> m_Patches;
};

extern PatchGameConfig g_Patches;

#endif // _INCLUDE_SOURCEMOD_SRCSCRMBL_PATCHES_H_