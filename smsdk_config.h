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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_

#define SMEXT_CONF_NAME		"Source Scramble"
#define SMEXT_CONF_DESCRIPTION	"Presents more tools for developers to work with memory reallocations"
#define SMEXT_CONF_VERSION	"0.10.0.0"
#define SMEXT_CONF_AUTHOR	"nosoop | Updated by cravenge"
#define SMEXT_CONF_URL		"https://github.com/cravenge/Source-Scramble"
#define SMEXT_CONF_LOGTAG	"SRCSCRMBL"
#define SMEXT_CONF_LICENSE	"GPL"
#define SMEXT_CONF_DATESTRING	__DATE__ " " __TIME__

#define SMEXT_LINK(name)	SDKExtension* g_pExtensionIface = name;

//#define SMEXT_CONF_METAMOD

//#define SMEXT_ENABLE_FORWARDSYS
#define SMEXT_ENABLE_HANDLESYS
//#define SMEXT_ENABLE_PLAYERHELPERS
//#define SMEXT_ENABLE_DBMANAGER
#define SMEXT_ENABLE_GAMECONF
//#define SMEXT_ENABLE_MEMUTILS
//#define SMEXT_ENABLE_GAMEHELPERS
//#define SMEXT_ENABLE_TIMERSYS
//#define SMEXT_ENABLE_THREADER
//#define SMEXT_ENABLE_LIBSYS
//#define SMEXT_ENABLE_MENUS
//#define SMEXT_ENABLE_ADTFACTORY
//#define SMEXT_ENABLE_PLUGINSYS
//#define SMEXT_ENABLE_ADMINSYS
//#define SMEXT_ENABLE_TEXTPARSERS
//#define SMEXT_ENABLE_USERMSGS
//#define SMEXT_ENABLE_TRANSLATOR
#define SMEXT_ENABLE_ROOTCONSOLEMENU

#endif // _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_