/*
MIT License

Copyright (c) 2017-2021 Mark E Sowden <hogsy@oldtimes-software.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "package_private.h"

/* Outwars FF package format */

PLPackage *PlLoadFfPackage( const char *path ) {
	FunctionStart();

	PLFile *fp = PlOpenFile( path, false );
	if ( fp == NULL ) {
		return NULL;
	}

	uint32_t num_indices;
	if ( PlReadFile( fp, &num_indices, sizeof( uint32_t ), 1 ) != 1 ) {
		PlCloseFile( fp );
		return NULL;
	}

	typedef struct OW_FFIndex {
		uint32_t offset;
		char name[ 40 ];
	} OW_FFIndex;
	OW_FFIndex *indices = pl_malloc( sizeof( OW_FFIndex ) * num_indices );
	unsigned int *sizes = pl_malloc( sizeof( unsigned int ) * num_indices ); /* aren't stored in index data, so we'll calc these */
	if ( num_indices > 0 ) {
		if ( PlReadFile( fp, indices, sizeof( OW_FFIndex ), num_indices ) == num_indices ) {
			for ( unsigned int i = 0; i < ( num_indices - 1 ); ++i ) {
				sizes[ i ] = indices[ i + 1 ].offset - indices[ i ].offset;
			}
		} else {
			PlReportErrorF( PL_RESULT_FILEREAD, "failed to read indices" );
		}
	} else {
		PlReportErrorF( PL_RESULT_FILEREAD, "invalid number of indices in package" );
	}

	PlCloseFile( fp );

	if ( PlGetFunctionResult() != PL_RESULT_SUCCESS ) {
		pl_free( indices );
		pl_free( sizes );
		return NULL;
	}

	PLPackage *package = PlCreatePackageHandle( path, num_indices - 1, NULL );
	if ( ( package->table = pl_calloc( package->table_size, sizeof( struct PLPackageIndex ) ) ) != NULL ) {
		for ( unsigned int i = 0; i < package->table_size; ++i ) {
			PLPackageIndex *index = &package->table[ i ];
			index->offset = indices[ i ].offset;
			index->fileSize = sizes[ i ];
			strncpy( index->fileName, indices[ i ].name, sizeof( index->fileName ) );
		}
	} else {
		PlDestroyPackage( package );
		package = NULL;
	}

	pl_free( indices );
	pl_free( sizes );

	return package;
}
