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

#include "patches.h"
#include "util.h"

#define PSTATE_GAMEDEFS_PATCHES			1
#define PSTATE_GAMEDEFS_PATCHES_PATCH		2
#define PSTATE_GAMEDEFS_PATCHES_PATCH_MATCH 	3

#ifdef PLATFORM_X64
#define PLATFORM_ARCH_SUFFIX			"64"
#else
#define PLATFORM_ARCH_SUFFIX			""
#endif

#if defined PLATFORM_WINDOWS
#define PLATFORM_NAME				"windows" PLATFORM_ARCH_SUFFIX
#elif defined PLATFORM_LINUX
#define PLATFORM_NAME				"linux" PLATFORM_ARCH_SUFFIX
#elif defined PLATFORM_APPLE
#define PLATFORM_NAME				"mac" PLATFORM_ARCH_SUFFIX
#endif

PatchGameConfig g_Patches;

static inline bool DoesPlatformMatch( const char* platform ) {
    return !strcmp( platform, PLATFORM_NAME );
}

void PatchGameConfig::ReadSMC_ParseStart() {
    m_ParseState = PSTATE_GAMEDEFS_PATCHES;
    m_IgnoreLevel = 0;

    m_PatchOffset = 0;
    m_PatchOneTime = false;
}

SMCResult PatchGameConfig::ReadSMC_NewSection(const SMCStates *states, const char *name)
{
    if( ( m_IgnoreLevel ) || ( !*name ) ) {
        ++m_IgnoreLevel;
        return SMCResult_Continue;
    }

    if( m_ParseState == PSTATE_GAMEDEFS_PATCHES ) {
        m_Patch = name;

        StringHashMap< PatchConf >::Result r = m_Patches.find(name);
        if( r.found() ) {
            PatchConf &patConf = r->value;

            m_PatchSignature = std::move( patConf.signatureName );
            m_PatchOffset = patConf.offset;
            m_PatchMatch = std::move( patConf.match );
            m_PatchPreserve = std::move( patConf.preserve );
            m_PatchOverwrite = std::move( patConf.overwrite );
            m_PatchOneTime = patConf.onetime;
        }

        m_ParseState = PSTATE_GAMEDEFS_PATCHES_PATCH;
    } else if( m_ParseState == PSTATE_GAMEDEFS_PATCHES_PATCH ) {
        if( DoesPlatformMatch( name ) ) {
            m_ParseState = PSTATE_GAMEDEFS_PATCHES_PATCH_MATCH;
            return SMCResult_Continue;
        }

        if( strcmp( name, "linux" ) && strcmp( name, "windows" ) && strcmp( name, "mac" ) &&
            strcmp( name, "linux64") && strcmp( name, "windows64" ) && strcmp( name, "mac64" ) ) {
            smutils->LogError(myself, "Error while parsing Patch section for \"%s\":", m_Patch.c_str());
            smutils->LogError(myself, "Unrecognized platform \"%s\"", name);
        }

        m_IgnoreLevel = 1;
    } else {
        ++m_IgnoreLevel;
    }
    return SMCResult_Continue;
}

SMCResult PatchGameConfig::ReadSMC_KeyValue(const SMCStates *states, const char *key, const char *value)
{
    if( ( m_IgnoreLevel ) || ( ( m_ParseState != PSTATE_GAMEDEFS_PATCHES_PATCH ) && ( m_ParseState != PSTATE_GAMEDEFS_PATCHES_PATCH_MATCH ) ) || ( !*key ) ||
        ( !*value ) ) {
        return SMCResult_Continue;
    }

    if( !strcmp( key, "one-time" ) ) {
        if( !strcmp( value, "yes" ) )
            m_PatchOneTime = true;
    } else if( !strcmp( key, "overwrite" ) ) {
        m_PatchOverwrite = EscapedHexToByteVector( value );
    } else if( !strcmp( key, "preserve" ) ) {
        m_PatchPreserve = EscapedHexToByteVector( value );
    } else if( !strcmp( key, "match" ) ) {
        m_PatchMatch = EscapedHexToByteVector( value );
    } else if( !strcmp( key, "offset" ) ) {
        if( value[strlen( value ) - 1] == 'h' ) {
            m_PatchOffset = static_cast< int >( strtol( value, nullptr, 16 ) );
        } else {
            m_PatchOffset = static_cast< int >( strtol( value, nullptr, 0 ) );
        }
    } else if( !strcmp( key, "signature" ) ) {
        m_PatchSignature = value;
    }
    return SMCResult_Continue;
}

SMCResult PatchGameConfig::ReadSMC_LeavingSection(const SMCStates *states)
{
    if( m_IgnoreLevel ) {
        --m_IgnoreLevel;
        return SMCResult_Continue;
    }

    if( m_ParseState == PSTATE_GAMEDEFS_PATCHES_PATCH ) {
        PatchConf patConf( std::move( m_PatchSignature ), m_PatchOffset, std::move( m_PatchMatch ), std::move( m_PatchPreserve ), std::move( m_PatchOverwrite ), m_PatchOneTime );
        m_Patches.replace(m_Patch.c_str(), patConf);

        if( m_PatchOneTime )
            m_PatchOneTime = false;
        m_PatchOverwrite.clear();
        m_PatchPreserve.clear();
        m_PatchMatch.clear();
        if( m_PatchOffset )
            m_PatchOffset = 0;
        m_PatchSignature.clear();

        m_ParseState = PSTATE_GAMEDEFS_PATCHES;
    } else if( m_ParseState == PSTATE_GAMEDEFS_PATCHES_PATCH_MATCH ) {
        m_ParseState = PSTATE_GAMEDEFS_PATCHES_PATCH;
    }
    return SMCResult_Continue;
}

PatchGameConfig::PatchConf::PatchConf( std::string &&sigName, int ofst, std::vector< uint8_t > &&mtch, std::vector< uint8_t > &&presv, std::vector< uint8_t > &&ovr, bool ot ) {
    this->signatureName = std::move( sigName );
    this->offset = ofst;
    this->match = std::move( mtch );
    this->preserve = std::move( presv );
    this->overwrite = std::move( ovr );
    this->onetime = ot;
}