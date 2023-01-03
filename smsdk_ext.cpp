/**
 * vim: set ts=4 sw=4 tw=99 noet:
 * =============================================================================
 * SourceMod Base Extension Code
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
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

#include "smsdk_ext.h"

#ifndef _include_amtl_string_h_
#include <am-string.h>
#endif

#ifdef SMEXT_CONF_METAMOD
#ifndef META_NO_HL2SDK
IServerGameDLL* gamedll;
IVEngineServer* engine;
#endif

ISmmAPI* g_SMAPI;
ISmmPlugin* g_PLAPI;

SourceHook::ISourceHook* g_SHPtr;

PluginId g_PLID;
#endif

IExtension* myself;

IShareSys* g_pShareSys;
IShareSys* sharesys;

ISourceMod* g_pSM;
ISourceMod* smutils;

#ifdef SMEXT_ENABLE_FORWARDSYS
IForwardManager* g_pForwards;
IForwardManager* forwards;
#endif

#ifdef SMEXT_ENABLE_HANDLESYS
IHandleSys* g_pHandleSys;
IHandleSys* handlesys;
#endif

#ifdef SMEXT_ENABLE_PLAYERHELPERS
IPlayerManager *playerhelpers;
#endif
#ifdef SMEXT_ENABLE_DBMANAGER
IDBManager* dbi;
#endif
#ifdef SMEXT_ENABLE_GAMECONF
IGameConfigManager* gameconfs;
#endif
#ifdef SMEXT_ENABLE_MEMUTILS
IMemoryUtils* memutils;
#endif
#ifdef SMEXT_ENABLE_GAMEHELPERS
IGameHelpers* gamehelpers;
#endif
#ifdef SMEXT_ENABLE_TIMERSYS
ITimerSystem* timersys;
#endif
#ifdef SMEXT_ENABLE_THREADER
IThreader* threader;
#endif
#ifdef SMEXT_ENABLE_LIBSYS
ILibrarySys* libsys;
#endif
#ifdef SMEXT_ENABLE_MENUS
IMenuManager* menus;
#endif
#ifdef SMEXT_ENABLE_ADTFACTORY
IADTFactory* adtfactory;
#endif

#ifdef SMEXT_ENABLE_PLUGINSYS
SourceMod::IPluginManager* plsys;
#endif

#ifdef SMEXT_ENABLE_ADMINSYS
IAdminSystem* adminsys;
#endif
#ifdef SMEXT_ENABLE_TEXTPARSERS
ITextParsers* textparsers;
#endif
#ifdef SMEXT_ENABLE_USERMSGS
IUserMessages* usermsgs;
#endif
#ifdef SMEXT_ENABLE_TRANSLATOR
ITranslator* translator;
#endif
#ifdef SMEXT_ENABLE_ROOTCONSOLEMENU
IRootConsole* rootconsole;
#endif

#ifdef SMEXT_CONF_METAMOD
SMM_API void* PL_EXPOSURE(const char *pName, int *pReturnCode)
{
#ifdef METAMOD_PLAPI_VERSION
	if( pName != nullptr && !strcmp( pName, METAMOD_PLAPI_NAME ) )
#else
	if( pName != nullptr && !strcmp( pName, PLAPI_NAME ) )
#endif
	{
		if( pReturnCode )
			*pReturnCode = META_IFACE_OK;
		return static_cast<void*>( g_pExtensionIface );
	}

	if( pReturnCode )
		*pReturnCode = META_IFACE_FAILED;
	return nullptr;
}
#endif

PLATFORM_EXTERN_C IExtensionInterface* GetSMExtAPI() {
	return g_pExtensionIface;
}

SDKExtension::SDKExtension() {
#ifdef SMEXT_CONF_METAMOD
	m_SourceMMLoaded = false;
	m_WeAreUnloaded = false;
	m_WeGotPauseChange = false;
#endif
}

#ifdef SMEXT_CONF_METAMOD
bool SDKExtension::SDK_OnMetamodLoad( ISmmAPI* ismm, char* error, size_t maxlen, bool late ) {
	return true;
}

bool SDKExtension::SDK_OnMetamodPauseChange( bool paused, char* error, size_t maxlen ) {
	return true;
}
#endif

bool SDKExtension::SDK_OnLoad( char* error, size_t maxlength, bool late ) {
	return true;
}

void SDKExtension::SDK_OnAllLoaded() {}
void SDKExtension::SDK_OnPauseChange( bool paused ) {}
void SDKExtension::SDK_OnUnload() {}
void SDKExtension::SDK_OnDependenciesDropped() {}

#ifdef SMEXT_CONF_METAMOD
bool SDKExtension::SDK_OnMetamodUnload( char* error, size_t maxlen ) {
	return true;
}

bool SDKExtension::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

#ifndef META_NO_HL2SDK
#ifndef METAMOD_PLAPI_VERSION
	GET_V_IFACE_ANY(serverFactory, gamedll, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	GET_V_IFACE_CURRENT(engineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
#else
	GET_V_IFACE_ANY(GetServerFactory, gamedll, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
#if SOURCE_ENGINE == SE_SDK2013

	engine = ( IVEngineServer* )ismm->GetEngineFactory()("VEngineServer023", nullptr);
	if( engine == nullptr ) {
		engine = ( IVEngineServer* )ismm->GetEngineFactory()("VEngineServer022", nullptr);
		if( engine == nullptr ) {
			engine = ( IVEngineServer* )ismm->GetEngineFactory()("VEngineServer021", nullptr);
			if( engine == nullptr ) {
				if( error != nullptr && maxlen )
					ismm->Format(error, maxlen, "Could not find interface: VEngineServer023 or VEngineServer022");
				return false;
			}
		}
	}
#else
	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
#endif
#endif
#endif

	m_SourceMMLoaded = true;
	return SDK_OnMetamodLoad( ismm, error, maxlen, late );
}

bool SDKExtension::Unload(char *error, size_t maxlen)
{
	if( !m_WeAreUnloaded ) {
		if( error != nullptr )
			ke::SafeStrcpy( error, maxlen, "This extension must be unloaded by SourceMod." );
		return false;
	}

	return SDK_OnMetamodUnload( error, maxlen );
}

bool SDKExtension::Pause(char *error, size_t maxlen)
{
	if( !m_WeGotPauseChange ) {
		if( error != nullptr )
			ke::SafeStrcpy(error, maxlen, "This extension must be paused by SourceMod.");
		return false;
	}

	m_WeGotPauseChange = false;
	return SDK_OnMetamodPauseChange( true, error, maxlen );
}

bool SDKExtension::Unpause(char *error, size_t maxlen)
{
	if( !m_WeGotPauseChange ) {
		if( error != nullptr )
			ke::SafeStrcpy(error, maxlen, "This extension must be unpaused by SourceMod.");
		return false;
	}

	m_WeGotPauseChange = false;
	return SDK_OnMetamodPauseChange( false, error, maxlen );
}

const char *SDKExtension::GetAuthor() {
	return GetExtensionAuthor();
}

const char *SDKExtension::GetName() {
	return GetExtensionName();
}

const char *SDKExtension::GetDescription() {
	return GetExtensionDescription();
}

const char *SDKExtension::GetURL() {
	return GetExtensionURL();
}

const char *SDKExtension::GetLicense() {
	return SMEXT_CONF_LICENSE;
}

const char *SDKExtension::GetVersion() {
	return GetExtensionVerString();
}

const char *SDKExtension::GetDate() {
	return GetExtensionDateString();
}

const char *SDKExtension::GetLogTag() {
	return GetExtensionTag();
}
#endif

bool SDKExtension::OnExtensionLoad(IExtension *me, IShareSys *sys, char *error, size_t maxlength, bool late)
{
	myself = me;

	g_pShareSys = sharesys = sys;

#ifdef SMEXT_CONF_METAMOD
	m_WeAreUnloaded = true;

	if( !m_SourceMMLoaded ) {
		if( error )
			ke::SafeStrcpy(error, maxlength, "Metamod attach failed");
		return false;
	}
#endif

	SM_GET_IFACE(SOURCEMOD, g_pSM);
	smutils = g_pSM;

#ifdef SMEXT_ENABLE_HANDLESYS
	SM_GET_IFACE(HANDLESYSTEM, g_pHandleSys);
	handlesys = g_pHandleSys;
#endif

#ifdef SMEXT_ENABLE_FORWARDSYS
	SM_GET_IFACE(FORWARDMANAGER, g_pForwards);
	forwards = g_pForwards;
#endif

#ifdef SMEXT_ENABLE_PLAYERHELPERS
	SM_GET_IFACE(PLAYERMANAGER, playerhelpers);
#endif
#ifdef SMEXT_ENABLE_DBMANAGER
	SM_GET_IFACE(DBI, dbi);
#endif
#ifdef SMEXT_ENABLE_GAMECONF
	SM_GET_IFACE(GAMECONFIG, gameconfs);
#endif
#ifdef SMEXT_ENABLE_MEMUTILS
	SM_GET_IFACE(MEMORYUTILS, memutils);
#endif
#ifdef SMEXT_ENABLE_GAMEHELPERS
	SM_GET_IFACE(GAMEHELPERS, gamehelpers);
#endif
#ifdef SMEXT_ENABLE_TIMERSYS
	SM_GET_IFACE(TIMERSYS, timersys);
#endif
#ifdef SMEXT_ENABLE_THREADER
	SM_GET_IFACE(THREADER, threader);
#endif
#ifdef SMEXT_ENABLE_LIBSYS
	SM_GET_IFACE(LIBRARYSYS, libsys);
#endif
#ifdef SMEXT_ENABLE_MENUS
	SM_GET_IFACE(MENUMANAGER, menus);
#endif
#ifdef SMEXT_ENABLE_ADTFACTORY
	SM_GET_IFACE(ADTFACTORY, adtfactory);
#endif
#ifdef SMEXT_ENABLE_PLUGINSYS
	SM_GET_IFACE(PLUGINSYSTEM, plsys);
#endif
#ifdef SMEXT_ENABLE_ADMINSYS
	SM_GET_IFACE(ADMINSYS, adminsys);
#endif
#ifdef SMEXT_ENABLE_TEXTPARSERS
	SM_GET_IFACE(TEXTPARSERS, textparsers);
#endif
#ifdef SMEXT_ENABLE_USERMSGS
	SM_GET_IFACE(USERMSGS, usermsgs);
#endif
#ifdef SMEXT_ENABLE_TRANSLATOR
	SM_GET_IFACE(TRANSLATOR, translator);
#endif
#ifdef SMEXT_ENABLE_ROOTCONSOLEMENU
	SM_GET_IFACE(ROOTCONSOLE, rootconsole);
#endif

	if( SDK_OnLoad( error, maxlength, late ) ) {
#ifdef SMEXT_CONF_METAMOD
		m_WeAreUnloaded = false;
#endif
		return true;
	}

	return false;
}

void SDKExtension::OnExtensionUnload() {
#ifdef SMEXT_CONF_METAMOD
	m_WeAreUnloaded = true;

#endif
	SDK_OnUnload();
}

void SDKExtension::OnExtensionsAllLoaded() {
	SDK_OnAllLoaded();
}

void SDKExtension::OnExtensionPauseChange(bool state)
{
#ifdef SMEXT_CONF_METAMOD
	m_WeGotPauseChange = true;

#endif
	SDK_OnPauseChange( state );
}

bool SDKExtension::IsMetamodExtension() {
#ifdef SMEXT_CONF_METAMOD
	return true;
#else
	return false;
#endif
}

const char *SDKExtension::GetExtensionName() {
	return SMEXT_CONF_NAME;
}

const char *SDKExtension::GetExtensionURL() {
	return SMEXT_CONF_URL;
}

const char *SDKExtension::GetExtensionTag() {
	return SMEXT_CONF_LOGTAG;
}

const char *SDKExtension::GetExtensionAuthor() {
	return SMEXT_CONF_AUTHOR;
}

const char *SDKExtension::GetExtensionVerString() {
	return SMEXT_CONF_VERSION;
}

const char *SDKExtension::GetExtensionDescription() {
	return SMEXT_CONF_DESCRIPTION;
}

const char *SDKExtension::GetExtensionDateString() {
	return SMEXT_CONF_DATESTRING;
}

void SDKExtension::OnDependenciesDropped() {
	SDK_OnDependenciesDropped();
}

#if defined __linux__ || defined __APPLE__
extern "C" void __cxa_pure_virtual(void) {}

void *operator new( size_t size ) {
	return malloc( size );
}

void *operator new[]( size_t size ) {
	return malloc( size );
}

void operator delete( void* ptr ) {
	free( ptr );
}

void operator delete[]( void* ptr ) {
	free( ptr );
}
#endif