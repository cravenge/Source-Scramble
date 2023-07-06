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

#include "extension.h"

#ifdef PLATFORM_X64
# ifdef PLATFORM_LINUX
#  define _INTTYPES_H	1

# endif
#include "PseudoAddrManager.h"

#endif
cell_t CreateMemoryBlock(IPluginContext* pContext, const cell_t* params)
{
    cell_t size = params[1];
    if( size <= 0 )
        return pContext->ThrowNativeError("Invalid size (must be > 0)");

    MemoryBlock* pMemoryBlock = new MemoryBlock( size );
    if( pMemoryBlock == nullptr )
        return 0;

    if( pMemoryBlock->pBlock == nullptr ) {
        delete pMemoryBlock;
        return 0;
    }

    Handle_t hndl = handlesys->CreateHandle(g_MemoryBlock, pMemoryBlock, pContext->GetIdentity(), myself->GetIdentity(), nullptr);
    if( !hndl )
        delete pMemoryBlock;
    return static_cast< cell_t >( hndl );
}

cell_t DisownMemoryBlock(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryBlock* pMemoryBlock;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryBlock, &sec, reinterpret_cast< void** >( &pMemoryBlock )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    if( !pMemoryBlock->stored )
        return pContext->ThrowNativeError("Memory block is already disowned");

    pMemoryBlock->stored = false;
    return 0;
}

cell_t GetMemoryBlockSize(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryBlock* pMemoryBlock;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryBlock, &sec, reinterpret_cast< void** >( &pMemoryBlock )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    return static_cast< cell_t >( pMemoryBlock->size );
}

cell_t GetMemoryBlockAddress(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryBlock* pMemoryBlock;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryBlock, &sec, reinterpret_cast< void** >( &pMemoryBlock )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

#ifdef PLATFORM_X64
    return static_cast< cell_t >( pseudoAddr.ToPseudoAddress( pMemoryBlock->pBlock ) );
#else
    return static_cast< cell_t >( reinterpret_cast< uintptr_t >( pMemoryBlock->pBlock ) );
#endif
}

cell_t CreateMemoryPatchFromConf(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;

    IGameConfig* gc = gameconfs->ReadHandle(hndl, pContext->GetIdentity(), &err);
    if( gc == nullptr )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    char* key;
    pContext->LocalToString(params[2], &key);

    StringHashMap< PatchGameConfig::PatchConf >::Result r = g_Patches.m_Patches.find(key);
    if( !r.found() )
        return pContext->ThrowNativeError("Unable to find \"%s\"", key);

    const PatchGameConfig::PatchConf &patConf = r->value;

    void* addr;
    if( !gc->GetMemSig(patConf.signatureName.c_str(), reinterpret_cast< void** >( &addr )) || addr == nullptr )
        return pContext->ThrowNativeError("Unable to find \"%s\" signature from \"%s\"", patConf.signatureName.c_str(), key);

    MemoryPatch* pMemoryPatch = new MemoryPatch( addr, patConf );
    if( pMemoryPatch == nullptr )
        return 0;

    hndl = handlesys->CreateHandle(g_MemoryPatch, pMemoryPatch, pContext->GetIdentity(), myself->GetIdentity(), nullptr);
    if( !hndl )
        delete pMemoryPatch;
    return static_cast< cell_t >( hndl );
}

cell_t ValidateMemoryPatch(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    return pMemoryPatch->Validate();
}

cell_t IsMemoryPatchActive(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    return pMemoryPatch->IsActive();
}

cell_t EnableMemoryPatch(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None ) {
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);
    } else if( reinterpret_cast< uintptr_t >( pMemoryPatch->pAddr ) < 0x10000 ) {
        return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory", pMemoryPatch->pAddr);
    }

    return pMemoryPatch->Enable();
}

cell_t DisableMemoryPatch(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    return pMemoryPatch->Disable();
}

cell_t GetMemoryPatchSize(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    char* key;
    pContext->LocalToString(params[2], &key);
    if( !strcmp( key, "match" ) ) {
        return static_cast< cell_t >( pMemoryPatch->match.size() );
    } else if( !strcmp( key, "preserve" ) ) {
        return static_cast< cell_t >( pMemoryPatch->preserve.size() );
    } else if( !strcmp( key, "overwrite" ) ) {
        return static_cast< cell_t >( pMemoryPatch->overwrite.size() );
    } else if( !strcmp( key, "original" ) ) {
        return static_cast< cell_t >( pMemoryPatch->original.size() );
    }

    return pContext->ThrowNativeError("Invalid patch data type \"%s\"", key);
}

cell_t SetMemoryPatchSize(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    if( params[3] < 0 )
        return pContext->ThrowNativeError("Invalid patch size: %d", params[3]);

    char* key;
    pContext->LocalToString(params[2], &key);
    if( !strcmp( key, "match" ) ) {
        pMemoryPatch->match.resize( params[3], static_cast< uint8_t >( 0x00 ) );
        return 0;
    } else if( !strcmp( key, "preserve" ) ) {
        pMemoryPatch->preserve.resize( params[3], static_cast< uint8_t >( 0x00 ) );
        return 0;
    } else if( !strcmp( key, "overwrite" ) ) {
        pMemoryPatch->overwrite.resize( params[3], static_cast< uint8_t >( 0x00 ) );
        return 0;
    } else if( !strcmp( key, "original" ) ) {
        pMemoryPatch->original.resize( params[3], static_cast< uint8_t >( 0x00 ) );
        return 0;
    }

    return pContext->ThrowNativeError("Invalid patch data type \"%s\"", key);
}

cell_t GetMemoryPatchData(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    cell_t idx = params[3];
    if( idx < 0 )
        return pContext->ThrowNativeError("Invalid patch data index %d", idx);

    char* key;
    pContext->LocalToString(params[2], &key);
    if( !strcmp( key, "match" ) ) {
        if( !pMemoryPatch->match.size() ) {
            return pContext->ThrowNativeError("No data in \"match\" is found");
        } else if( idx >= pMemoryPatch->match.size() ) {
            return pContext->ThrowNativeError("Invalid index %d (count: %d)", idx, pMemoryPatch->match.size());
        }

        return static_cast< cell_t >( pMemoryPatch->match[idx] );
    } else if( !strcmp( key, "preserve" ) ) {
        if( !pMemoryPatch->preserve.size() ) {
            return pContext->ThrowNativeError("No data in \"preserve\" is found");
        } else if( idx >= pMemoryPatch->preserve.size() ) {
            return pContext->ThrowNativeError("Invalid index %d (count: %d)", idx, pMemoryPatch->preserve.size());
        }

        return static_cast< cell_t >( pMemoryPatch->preserve[idx] );
    } else if( !strcmp( key, "overwrite" ) ) {
        if( !pMemoryPatch->overwrite.size() ) {
            return pContext->ThrowNativeError("No data in \"overwrite\" is found");
        } else if( idx >= pMemoryPatch->overwrite.size() ) {
            return pContext->ThrowNativeError("Invalid index %d (count: %d)", idx, pMemoryPatch->overwrite.size());
        }

        return static_cast< cell_t >( pMemoryPatch->overwrite[idx] );
    } else if( !strcmp( key, "original" ) ) {
        if( !pMemoryPatch->original.size() ) {
            return pContext->ThrowNativeError("No data in \"original\" is found");
        } else if( idx >= pMemoryPatch->original.size() ) {
            return pContext->ThrowNativeError("Invalid index %d (count: %d)", idx, pMemoryPatch->original.size());
        }

        return static_cast< cell_t >( pMemoryPatch->original[idx] );
    }
    
    return pContext->ThrowNativeError("Invalid patch data type \"%s\"", key);
}

cell_t SetMemoryPatchData(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    cell_t idx = params[3];
    if( idx < 0 )
        return pContext->ThrowNativeError("Invalid patch data index %d", idx);

    cell_t val = params[4];
    if( val < -255 || val > 255 )
        return pContext->ThrowNativeError("Invalid patch data value %d", val);

    if( val <= -129 )
        val *= ( -1 );

    char* key;
    pContext->LocalToString(params[2], &key);
    if( !strcmp( key, "match" ) ) {
        if( !pMemoryPatch->match.size() ) {
            return pContext->ThrowNativeError("No data in \"match\" is found");
        } else if( idx >= pMemoryPatch->match.size() ) {
            return pContext->ThrowNativeError("Invalid index %d (count: %d)", idx, pMemoryPatch->match.size());
        }

        if( val >= 128 )
            pMemoryPatch->match[idx] = val;
        else
            pMemoryPatch->match[idx] = static_cast< uint8_t >( val );
        return 0;
    } else if( !strcmp( key, "preserve" ) ) {
        if( !pMemoryPatch->preserve.size() ) {
            return pContext->ThrowNativeError("No data in \"preserve\" is found");
        } else if( idx >= pMemoryPatch->preserve.size() ) {
            return pContext->ThrowNativeError("Invalid index %d (count: %d)", idx, pMemoryPatch->preserve.size());
        }

        if( val >= 128 )
            pMemoryPatch->preserve[idx] = val;
        else
            pMemoryPatch->preserve[idx] = static_cast< uint8_t >( val );
        return 0;
    } else if( !strcmp( key, "overwrite" ) ) {
        if( !pMemoryPatch->overwrite.size() ) {
            return pContext->ThrowNativeError("No data in \"overwrite\" is found");
        } else if( idx >= pMemoryPatch->overwrite.size() ) {
            return pContext->ThrowNativeError("Invalid index %d (count: %d)", idx, pMemoryPatch->overwrite.size());
        }

        if( val >= 128 )
            pMemoryPatch->overwrite[idx] = val;
        else
            pMemoryPatch->overwrite[idx] = static_cast< uint8_t >( val );
        return 0;
    } else if( !strcmp( key, "original" ) ) {
        if( !pMemoryPatch->original.size() ) {
            return pContext->ThrowNativeError("No data in \"original\" is found");
        } else if( idx >= pMemoryPatch->original.size() ) {
            return pContext->ThrowNativeError("Invalid index %d (count: %d)", idx, pMemoryPatch->original.size());
        }

        if( val >= 128 )
            pMemoryPatch->original[idx] = val;
        else
            pMemoryPatch->original[idx] = static_cast< uint8_t >( val );
        return 0;
    }

    return pContext->ThrowNativeError("Invalid patch data type \"%s\"", key);
}

cell_t DiscardMemoryPatch(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

    if( !pMemoryPatch->retained )
        return pContext->ThrowNativeError("Memory patch is already discarded");

    pMemoryPatch->retained = false;
    return 0;
}

cell_t GetMemoryPatchAddress(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, reinterpret_cast< void** >( &pMemoryPatch )) )
          != HandleError_None )
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);

#ifdef PLATFORM_X64
    return static_cast< cell_t >( pseudoAddr.ToPseudoAddress( pMemoryPatch->pAddr ) );
#else
    return static_cast< cell_t >( reinterpret_cast< uintptr_t >( pMemoryPatch->pAddr ) );
#endif
}

cell_t GetCellAddress(IPluginContext* pContext, const cell_t* params)
{
    cell_t* value;
    if( pContext->LocalToPhysAddr(params[1], &value) != SP_ERROR_NONE )
        return pContext->ThrowNativeError("Failed to convert cell reference to a physical address");

#ifdef PLATFORM_X64
    return static_cast< cell_t >( pseudoAddr.ToPseudoAddress( static_cast< void* >( value ) ) );
#else
    return static_cast< cell_t >( reinterpret_cast< intptr_t >( value ) );
#endif
}

cell_t GetStringAddress(IPluginContext* pContext, const cell_t* params)
{
    char* buffer;
    if( pContext->LocalToString(params[1], &buffer) != SP_ERROR_NONE )
        return pContext->ThrowNativeError("Failed to convert string reference to a physical address");

#ifdef PLATFORM_X64
    return static_cast< cell_t >( pseudoAddr.ToPseudoAddress( static_cast< void* >( buffer ) ) );
#else
    return static_cast< cell_t >( reinterpret_cast< intptr_t >( buffer ) );
#endif
}

sp_nativeinfo_t g_SrcScrambleNatives[] = {
    { "CreateMemoryBlock",           CreateMemoryBlock },
    { "DisownMemoryBlock",           DisownMemoryBlock },
    { "GetMemoryBlockSize",          GetMemoryBlockSize },
    { "GetMemoryBlockAddress",       GetMemoryBlockAddress },
    { "CreateMemoryPatchFromConf",   CreateMemoryPatchFromConf },
    { "ValidateMemoryPatch",         ValidateMemoryPatch },
    { "IsMemoryPatchActive",         IsMemoryPatchActive },
    { "EnableMemoryPatch",           EnableMemoryPatch },
    { "DisableMemoryPatch",          DisableMemoryPatch },
    { "GetMemoryPatchSize",          GetMemoryPatchSize },
    { "SetMemoryPatchSize",          SetMemoryPatchSize },
    { "GetMemoryPatchData",          GetMemoryPatchData },
    { "SetMemoryPatchData",          SetMemoryPatchData },
    { "DiscardMemoryPatch",          DiscardMemoryPatch },
    { "GetMemoryPatchAddress",       GetMemoryPatchAddress },

    { "GetCellAddress",              GetCellAddress },
    { "GetStringAddress",            GetStringAddress },

    { "MemoryBlock.MemoryBlock",     CreateMemoryBlock },
    { "MemoryBlock.Disown",          DisownMemoryBlock },
    { "MemoryBlock.Size.get",        GetMemoryBlockSize },
    { "MemoryBlock.Address.get",     GetMemoryBlockAddress },
    { "MemoryPatch.FromConf",        CreateMemoryPatchFromConf },
    { "MemoryPatch.Validate",        ValidateMemoryPatch },
    { "MemoryPatch.IsActive",        IsMemoryPatchActive },
    { "MemoryPatch.Enable",          EnableMemoryPatch },
    { "MemoryPatch.Disable",         DisableMemoryPatch },
    { "MemoryPatch.GetSize",         GetMemoryPatchSize },
    { "MemoryPatch.SetSize",         SetMemoryPatchSize },
    { "MemoryPatch.GetData",         GetMemoryPatchData },
    { "MemoryPatch.SetData",         SetMemoryPatchData },
    { "MemoryPatch.Discard",         DiscardMemoryPatch },
    { "MemoryPatch.Address.get",     GetMemoryPatchAddress },

    { nullptr,                       nullptr },
};