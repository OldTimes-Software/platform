/* Copyright (C) 2020 Mark E Sowden <markelswo@gmail.com> */

#include "plugin.h"

static PLPluginDescription pluginDesc = {
        .description = "Galleon file format support.",
        .pluginVersion = { 0, 0, 1 },
        .interfaceVersion = PL_PLUGIN_INTERFACE_VERSION,
};

const PLPluginExportTable *gInterface = NULL;

PL_EXPORT const PLPluginDescription *PLQueryPlugin( unsigned int interfaceVersion ) {
	return &pluginDesc;
}

PLPackage *PKG_LoadFile( const char *path );

PL_EXPORT void PLInitializePlugin( const PLPluginExportTable *functionTable ) {
	gInterface = functionTable;
	gInterface->RegisterPackageLoader( "pkg", PKG_LoadFile );
}
