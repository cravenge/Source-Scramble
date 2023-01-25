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

#ifdef PLATFORM_X64
#ifdef PLATFORM_LINUX
# define _INTTYPES_H	1
#endif

#include "PseudoAddrManager.h"
#endif

cell_t CreateSourceMemoryBlock(IPluginContext* pContext, const cell_t* params)
{
    cell_t size = params[1];
    if( size <= 0 ) {
        return pContext->ThrowNativeError("Cannot allocate %d bytes of memory", size);
    }

    MemoryBlock* pMemoryBlock = new MemoryBlock( size );
    if( pMemoryBlock == nullptr ) {
        return BAD_HANDLE;
    }

    if( pMemoryBlock->pBlock == nullptr ) {
        delete pMemoryBlock;
        return BAD_HANDLE;
    }

    return static_cast< cell_t >( handlesys->CreateHandle(g_MemoryBlock, 
            pMemoryBlock, 
            pContext->GetIdentity(), 
            myself->GetIdentity(), 
            nullptr) );
}

cell_t GetMemoryBlockSize(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryBlock* pMemoryBlock;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryBlock, &sec, ( void** )&pMemoryBlock) )
          != HandleError_None ) {
        return pContext->ThrowNativeError("Invalid MemoryBlock handle %x (error %d)", hndl, err);
    }

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

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryBlock, &sec, ( void** )&pMemoryBlock) )
          != HandleError_None ) {
        return pContext->ThrowNativeError("Invalid MemoryBlock handle %x (error %d)", hndl, err);
    }

#ifdef PLATFORM_X64
    return static_cast< cell_t >( pseudoAddr.ToPseudoAddress( pMemoryBlock->pBlock ) );
#else
    return static_cast< cell_t >( reinterpret_cast< uintptr_t >( pMemoryBlock->pBlock ) );
#endif
}

cell_t SourceMemoryPatch_FromConf(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;

    IGameConfig* gc = gameconfs->ReadHandle(hndl, pContext->GetIdentity(), &err);
    if( gc == nullptr ) {
        return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);
    }

    char* key;
    pContext->LocalToString(params[2], &key);

    StringHashMap< PatchGameConfig::PatchConf >::Result r = g_Patches.m_Patches.find(key);
    if( !r.found() ) {
        return pContext->ThrowNativeError("Cannot find patch name \"%s\"", key);
    }

    const PatchGameConfig::PatchConf& patConf = r->value;

    void* addr;
    if( !gc->GetMemSig(patConf.signatureName.c_str(), &addr) || addr == nullptr ) {
        return pContext->ThrowNativeError("Cannot find \"%s\" signature from \"%s\"", patConf.signatureName.c_str(), key);
    }

    MemoryPatch* pMemoryPatch = new MemoryPatch( addr, patConf );
    if( pMemoryPatch == nullptr ) {
        return BAD_HANDLE;
    }

    return static_cast< cell_t >( handlesys->CreateHandle(g_MemoryPatch,
            pMemoryPatch, 
            pContext->GetIdentity(), 
            myself->GetIdentity(),
            nullptr) );
}

cell_t ValidateMemoryPatch(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, ( void** )&pMemoryPatch) )
          != HandleError_None ) {
        return pContext->ThrowNativeError("Invalid MemoryPatch handle %x (error %d)", hndl, err);
    }

    return pMemoryPatch->Validate();
}

cell_t EnableMemoryPatch(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, ( void** )&pMemoryPatch) )
          != HandleError_None ) {
        return pContext->ThrowNativeError("Invalid MemoryPatch handle %x (error %d)", hndl, err);
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

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, ( void** )&pMemoryPatch) )
          != HandleError_None ) {
        return pContext->ThrowNativeError("Invalid MemoryPatch handle %x (error %d)", hndl, err);
    }

    return pMemoryPatch->Disable();
}

cell_t GetMemoryPatchAddress(IPluginContext* pContext, const cell_t* params)
{
    Handle_t hndl = static_cast< Handle_t >( params[1] );

    HandleError err;
    HandleSecurity sec;

    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    MemoryPatch* pMemoryPatch;

    if( ( err = handlesys->ReadHandle(hndl, g_MemoryPatch, &sec, ( void** )&pMemoryPatch) )
          != HandleError_None ) {
        return pContext->ThrowNativeError("Invalid MemoryPatch handle %x (error %d)", hndl, err);
    }

#ifdef PLATFORM_X64
    return static_cast< cell_t >( pseudoAddr.ToPseudoAddress( pMemoryPatch->pAddr ) );
#else
    return static_cast< cell_t >( reinterpret_cast< uintptr_t >( pMemoryPatch->pAddr ) );
#endif
}

cell_t GetCellAddress(IPluginContext* pContext, const cell_t* params)
{
    cell_t* value;
    if( pContext->LocalToPhysAddr(params[1], &value) != SP_ERROR_NONE ) {
        return pContext->ThrowNativeError("Error encountered while converting cell reference to a physical address");
    }

#ifdef PLATFORM_X64
    return static_cast< cell_t >( pseudoAddr.ToPseudoAddress( value ) );
#else
    return static_cast< cell_t >( reinterpret_cast< intptr_t >( value ) );
#endif
}

cell_t GetStringAddress(IPluginContext* pContext, const cell_t* params)
{
    char* buffer;
    if( pContext->LocalToString(params[1], &buffer) != SP_ERROR_NONE ) {
        return pContext->ThrowNativeError("Error encountered while converting string reference to a physical address");
    }

#ifdef PLATFORM_X64
    return static_cast< cell_t >( pseudoAddr.ToPseudoAddress( buffer ) );
#else
    return static_cast< cell_t >( reinterpret_cast< intptr_t >( buffer ) );
#endif
}

sp_nativeinfo_t g_SrcScrambleNatives[] = {
    { "CreateMemoryBlock",           CreateSourceMemoryBlock },
    { "GetMemoryBlockSize",          GetMemoryBlockSize },
    { "GetMemoryBlockAddress",       GetMemoryBlockAddress },
    { "MemoryPatch_FromConf",        SourceMemoryPatch_FromConf },
    { "ValidateMemoryPatch",         ValidateMemoryPatch },
    { "EnableMemoryPatch",           EnableMemoryPatch },
    { "DisableMemoryPatch",          DisableMemoryPatch },
    { "GetMemoryPatchAddress",       GetMemoryPatchAddress },

    { "GetCellAddress",              GetCellAddress },
    { "GetStringAddress",            GetStringAddress },

    { "MemoryBlock.MemoryBlock",     CreateSourceMemoryBlock },
    { "MemoryBlock.Size.get",        GetMemoryBlockSize },
    { "MemoryBlock.Address.get",     GetMemoryBlockAddress },
    { "MemoryPatch.FromConf",        SourceMemoryPatch_FromConf },
    { "MemoryPatch.Validate",        ValidateMemoryPatch },
    { "MemoryPatch.Enable",          EnableMemoryPatch },
    { "MemoryPatch.Disable",         DisableMemoryPatch },
    { "MemoryPatch.Address.get",     GetMemoryPatchAddress },

    { nullptr,                       nullptr },
};