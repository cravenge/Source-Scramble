/**
 * vim: set ts=4 sw=4 tw=99 noet :
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

#ifndef _INCLUDE_SOURCEMOD_SRCSCRMBL_MEMPATCH_H_
#define _INCLUDE_SOURCEMOD_SRCSCRMBL_MEMPATCH_H_

#include <sourcehook.h>
#include <sh_memory.h>

#include "patches.h"

struct MemoryPatch {
    MemoryPatch( uintptr_t addr, const PatchGameConfig::PatchConf& info ) {
        this->pAddr = addr;

        for( auto vrfy : info.verify ) {
            this->verify.push_back( vrfy );
        }

        for( auto presv : info.preserve ) {
            this->preserve.push_back( presv );
        }

        for( auto repl : info.replace ) {
            this->replace.push_back( repl );
        }
    }

    ~MemoryPatch() {
        if( this->original.size() ) {
            size_t i = 0;
            while( i < this->original.size() ) {
                *( ( uint8_t* )this->pAddr + i ) = this->original[i];

                i++;
            }

            this->original.clear();
        }
    }

    bool Validate() {
        size_t i = 0;
        
        auto addr = ( uint8_t* )this->pAddr;
        while( i < this->verify.size() ) {
            if( this->verify[i] != '*' && this->verify[i] != addr[i] ) {
                return false;
            }

            i++;
        }

        return true;
    }

    bool Enable() {
        if( this->original.size() ) {
            return false;
        }
        
        size_t i = 0;
        while( i < this->replace.size() ) {
            this->original.push_back( *( ( uint8_t* )this->pAddr + i ) );

            i++;
        }

        SourceHook::SetMemAccess(( void* )this->pAddr, this->replace.size() * sizeof( uint8_t ), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);

        i = 0;

        uint8_t presvBits;
        while( i < this->replace.size() ) {
            presvBits = 0;
            if( this->preserve.size() > i )
                presvBits = this->preserve[i];
            
            *( ( uint8_t* )this->pAddr + i ) = ( this->replace[i] & ~presvBits ) | ( this->original[i] & presvBits );

            i++;
        }

        return true;
    }

    bool Disable() {
        if( !this->original.size() ) {
            return false;
        }
        
        size_t i = 0;
        while( i < this->original.size() ) {
            *( ( uint8_t* )this->pAddr + i ) = this->original[i];

            i++;
        }
        
        this->original.clear();
        return true;
    }

    uintptr_t pAddr;
    std::vector<uint8_t> verify;
    std::vector<uint8_t> preserve;
    std::vector<uint8_t> replace;
    std::vector<uint8_t> original;
};

#endif // _INCLUDE_SOURCEMOD_SRCSCRMBL_MEMPATCH_H_