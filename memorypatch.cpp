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

#include "memorypatch.h"

#include <sourcehook.h>
#include <sh_memory.h>

MemoryPatch::MemoryPatch( void* addr, const PatchGameConfig::PatchConf& info ) {
    this->pAddr = reinterpret_cast< void* >( reinterpret_cast< uint8_t* >( addr ) + info.offset );

    for( auto mtch : info.match ) {
        this->match.emplace_back( mtch );
    }

    for( auto presv : info.preserve ) {
        this->preserve.emplace_back( presv );
    }

    for( auto repl : info.replace ) {
        this->replace.emplace_back( repl );
    }
}

MemoryPatch::~MemoryPatch() {
    if( this->original.size() ) {
        size_t i = 0;
        while( i < this->original.size() ) {
            *( reinterpret_cast< uint8_t* >( this->pAddr ) + i ) = this->original[i];

            i++;
        }

        this->original.clear();
    }
}

bool MemoryPatch::Validate() {
    if( !this->match.size() ) {
        return true;
    }

    size_t i = 0;

    auto addr = reinterpret_cast< uint8_t* >( this->pAddr );
    while( i < this->match.size() ) {
        if( this->match[i] != '*' && this->match[i] != addr[i] ) {
            return false;
        }

        i++;
    }
    return true;
}

bool MemoryPatch::Enable() {
    if( this->original.size() ) {
        return false;
    }

    size_t i = 0;
    while( i < this->replace.size() ) {
        this->original.emplace_back( *( reinterpret_cast< uint8_t* >( this->pAddr ) + i ) );

        i++;
    }

    SourceHook::SetMemAccess(this->pAddr, this->replace.size() * sizeof( uint8_t ), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);

    i = 0;

    uint8_t presvBits;
    while( i < this->replace.size() ) {
        presvBits = 0;
        if( this->preserve.size() > i )
            presvBits = this->preserve[i];

        *( reinterpret_cast< uint8_t* >( this->pAddr ) + i ) = ( this->replace[i] & ~presvBits ) | ( this->original[i] & presvBits );

        i++;
    }
    return true;
}

bool MemoryPatch::Disable() {
    if( !this->original.size() ) {
        return false;
    }

    size_t i = 0;
    while( i < this->original.size() ) {
        *( reinterpret_cast< uint8_t* >( this->pAddr ) + i ) = this->original[i];

        i++;
    }

    this->original.clear();
    return true;
}