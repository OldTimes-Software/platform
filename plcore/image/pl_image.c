/**
 * Hei Platform Library
 * Copyright (C) 2017-2021 Mark E Sowden <hogsy@oldtimes-software.com>
 * This software is licensed under MIT. See LICENSE for more details.
 */

#include <plcore/pl_filesystem.h>
#include <plcore/pl_image.h>

#include <errno.h>

#include "filesystem_private.h"
#include "image_private.h"

#define STBI_MALLOC( sz ) pl_malloc( sz )
#define STBI_REALLOC( p, newsz ) pl_realloc( p, newsz )
#define STBI_FREE( p ) pl_free( p )

#define STBIW_MALLOC( sz ) pl_malloc( sz )
#define STBIW_REALLOC( p, newsz ) pl_realloc( p, newsz )
#define STBIW_FREE( p ) pl_free( p )

#define STB_IMAGE_WRITE_IMPLEMENTATION
#if defined( STB_IMAGE_WRITE_IMPLEMENTATION )
#include "stb_image_write.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#if defined( STB_IMAGE_IMPLEMENTATION )
#include "stb_image.h"

static PLImage *LoadStbImage( const char *path ) {
	PLFile *file = PlOpenFile( path, true );
	if ( file == NULL ) {
		return NULL;
	}

	int x, y, component;
	unsigned char *data = stbi_load_from_memory( file->data, ( int ) file->size, &x, &y, &component, 4 );

	PlCloseFile( file );

	if ( data == NULL ) {
		PlReportErrorF( PL_RESULT_FILEREAD, "failed to read in image (%s)", stbi_failure_reason() );
		return NULL;
	}

	PLImage *image = pl_calloc( 1, sizeof( PLImage ) );
	image->colour_format = PL_COLOURFORMAT_RGBA;
	image->format = PL_IMAGEFORMAT_RGBA8;
	image->width = ( unsigned int ) x;
	image->height = ( unsigned int ) y;
	image->size = PlGetImageSize( image->format, image->width, image->height );
	image->levels = 1;
	image->data = pl_calloc( image->levels, sizeof( uint8_t * ) );
	image->data[ 0 ] = data;

	return image;
}

#endif

#define MAX_IMAGE_LOADERS 4096

typedef struct PLImageLoader {
	const char *extension;
	PLImage *( *LoadImage )( const char *path );
} PLImageLoader;

static PLImageLoader imageLoaders[ MAX_IMAGE_LOADERS ];
static unsigned int numImageLoaders = 0;

void PlRegisterImageLoader( const char *extension, PLImage *( *LoadImage )( const char *path ) ) {
	if ( numImageLoaders >= MAX_IMAGE_LOADERS ) {
		PlReportBasicError( PL_RESULT_MEMORY_EOA );
		return;
	}

	imageLoaders[ numImageLoaders ].extension = extension;
	imageLoaders[ numImageLoaders ].LoadImage = LoadImage;

	numImageLoaders++;
}

void PlRegisterStandardImageLoaders( unsigned int flags ) {
	typedef struct SImageLoader {
		unsigned int flag;
		const char *extension;
		PLImage *( *LoadFunction )( const char *path );
	} SImageLoader;

	static const SImageLoader loaderList[] = {
	        { PL_IMAGE_FILEFORMAT_TGA, "tga", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_PNG, "png", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_JPG, "jpg", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_BMP, "bmp", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_PSD, "psd", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_GIF, "gif", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_HDR, "hdr", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_PIC, "pic", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_PNM, "pnm", LoadStbImage },
	        { PL_IMAGE_FILEFORMAT_FTX, "ftx", PlLoadFtxImage },
	        { PL_IMAGE_FILEFORMAT_3DF, "3df", PlLoad3dfImage },
	        { PL_IMAGE_FILEFORMAT_TIM, "tim", PlLoadTimImage },
	        { PL_IMAGE_FILEFORMAT_SWL, "swl", PlLoadSwlImage },
	};

	for ( unsigned int i = 0; i < plArrayElements( loaderList ); ++i ) {
		if ( flags != PL_IMAGE_FILEFORMAT_ALL && !( flags & loaderList[ i ].flag ) ) {
			continue;
		}

		PlRegisterImageLoader( loaderList[ i ].extension, loaderList[ i ].LoadFunction );
	}
}

void PlClearImageLoaders( void ) {
	numImageLoaders = 0;
}

PLImage *PlCreateImage( uint8_t *buf, unsigned int w, unsigned int h, PLColourFormat col, PLImageFormat dat ) {
	PLImage *image = pl_malloc( sizeof( PLImage ) );
	if ( image == NULL ) {
		return NULL;
	}

	image->width = w;
	image->height = h;
	image->colour_format = col;
	image->format = dat;
	image->size = PlGetImageSize( image->format, image->width, image->height );
	image->levels = 1;

	image->data = pl_calloc( image->levels, sizeof( uint8_t * ) );
	if ( image->data == NULL ) {
		PlDestroyImage( image );
		return NULL;
	}

	image->data[ 0 ] = pl_calloc( image->size, sizeof( uint8_t ) );
	if ( image->data[ 0 ] == NULL ) {
		PlDestroyImage( image );
		return NULL;
	}

	if ( buf != NULL ) {
		memcpy( image->data[ 0 ], buf, image->size );
	}

	return image;
}

void PlDestroyImage( PLImage *image ) {
	if ( image == NULL ) {
		return;
	}

	PlFreeImage( image );
	pl_free( image );
}

PLImage *PlLoadImage( const char *path ) {
	if ( !PlFileExists( path ) ) {
		PlReportBasicError( PL_RESULT_FILEPATH );
		return NULL;
	}

	const char *extension = PlGetFileExtension( path );
	for ( unsigned int i = 0; i < numImageLoaders; ++i ) {
		if ( pl_strcasecmp( extension, imageLoaders[ i ].extension ) == 0 ) {
			PLImage *image = imageLoaders[ i ].LoadImage( path );
			if ( image != NULL ) {
				strncpy( image->path, path, sizeof( image->path ) );
				return image;
			}
		}
	}

	PlReportBasicError( PL_RESULT_UNSUPPORTED );

	return NULL;
}

bool PlWriteImage( const PLImage *image, const char *path ) {
	if ( plIsEmptyString( path ) ) {
		PlReportErrorF( PL_RESULT_FILEPATH, PlGetResultString( PL_RESULT_FILEPATH ) );
		return false;
	}

	int comp = ( int ) PlGetNumberOfColourChannels( image->colour_format );
	if ( comp == 0 ) {
		PlReportErrorF( PL_RESULT_IMAGEFORMAT, "invalid colour format" );
		return false;
	}

	const char *extension = PlGetFileExtension( path );
	if ( !plIsEmptyString( extension ) ) {
		if ( !pl_strncasecmp( extension, "bmp", 3 ) ) {
			if ( stbi_write_bmp( path, ( int ) image->width, ( int ) image->height, comp, image->data[ 0 ] ) == 1 ) {
				return true;
			}
		} else if ( !pl_strncasecmp( extension, "png", 3 ) ) {
			if ( stbi_write_png( path, ( int ) image->width, ( int ) image->height, comp, image->data[ 0 ], 0 ) == 1 ) {
				return true;
			}
		} else if ( !pl_strncasecmp( extension, "tga", 3 ) ) {
			if ( stbi_write_tga( path, ( int ) image->width, ( int ) image->height, comp, image->data[ 0 ] ) == 1 ) {
				return true;
			}
		} else if ( !pl_strncasecmp( extension, "jpg", 3 ) || !pl_strncasecmp( extension, "jpeg", 3 ) ) {
			if ( stbi_write_jpg( path, ( int ) image->width, ( int ) image->height, comp, image->data[ 0 ], 90 ) == 1 ) {
				return true;
			}
		}
	}

	PlReportErrorF( PL_RESULT_FILETYPE, PlGetResultString( PL_RESULT_FILETYPE ) );
	return false;
}

// Returns the number of samples per-pixel depending on the colour format.
unsigned int PlGetNumberOfColourChannels( PLColourFormat format ) {
	switch ( format ) {
		case PL_COLOURFORMAT_ABGR:
		case PL_COLOURFORMAT_ARGB:
		case PL_COLOURFORMAT_BGRA:
		case PL_COLOURFORMAT_RGBA: {
			return 4;
		}

		case PL_COLOURFORMAT_BGR:
		case PL_COLOURFORMAT_RGB: {
			return 3;
		}
	}

	return 0;
}

static bool RGB8toRGBA8( PLImage *image ) {
	size_t size = PlGetImageSize( PL_IMAGEFORMAT_RGBA8, image->width, image->height );
	if ( size == 0 ) {
		return false;
	}

	typedef struct RGB8 {
		uint8_t r, g, b;
	} RGB8;
	typedef struct RGBA8 {
		uint8_t r, g, b, a;
	} RGBA8;
	size_t num_pixels = image->size / 3;

	RGB8 *src = ( RGB8 * ) image->data[ 0 ];
	RGBA8 *dst = pl_malloc( size );
	for ( size_t i = 0; i < num_pixels; ++i ) {
		dst->r = src->r;
		dst->g = src->g;
		dst->b = src->b;
		dst->a = 255;
		dst++;
		src++;
	}

	pl_free( image->data[ 0 ] );

	image->data[ 0 ] = ( uint8_t * ) ( &dst[ 0 ] );
	image->size = size;
	image->format = PL_IMAGEFORMAT_RGBA8;
	image->colour_format = PL_COLOURFORMAT_RGBA;

	return true;
}

#define scale_5to8( i ) ( ( ( ( double ) ( i ) ) / 31 ) * 255 )

static uint8_t *ImageDataRGB5A1toRGBA8( const uint8_t *src, size_t n_pixels ) {
	uint8_t *dst = pl_malloc( n_pixels * 4 );
	if ( dst == NULL ) {
		return NULL;
	}

	uint8_t *dp = dst;

	for ( size_t i = 0; i < n_pixels; ++i ) {
		/* Red */
		*( dp++ ) = scale_5to8( ( src[ 0 ] & 0xF8 ) >> 3 );

		/* Green */
		*( dp++ ) = scale_5to8( ( ( src[ 0 ] & 0x07 ) << 2 ) | ( ( src[ 1 ] & 0xC0 ) >> 6 ) );

		/* Blue */
		*( dp++ ) = scale_5to8( ( src[ 1 ] & 0x3E ) >> 1 );

		/* Alpha */
		*( dp++ ) = ( src[ 1 ] & 0x01 ) ? 255 : 0;

		src += 2;
	}

	return dst;
}

bool PlConvertPixelFormat( PLImage *image, PLImageFormat new_format ) {
	if ( image->format == new_format ) {
		return true;
	}

	switch ( image->format ) {
		case PL_IMAGEFORMAT_RGB8: {
			if ( new_format == PL_IMAGEFORMAT_RGBA8 ) { return RGB8toRGBA8( image ); }
		}

		case PL_IMAGEFORMAT_RGB5A1: {
			if ( new_format == PL_IMAGEFORMAT_RGBA8 ) {
				uint8_t **levels = pl_calloc( image->levels, sizeof( uint8_t * ) );

				/* Make a new copy of each detail level in the new format. */

				unsigned int lw = image->width;
				unsigned int lh = image->height;

				for ( unsigned int l = 0; l < image->levels; ++l ) {
					levels[ l ] = ImageDataRGB5A1toRGBA8( image->data[ l ], lw * lh );
					if ( levels[ l ] == NULL ) {
						/* Memory allocation failed, ditch any buffers we've created so far. */
						for ( unsigned int m = 0; m < l; ++m ) {
							pl_free( levels[ m ] );
						}

						pl_free( levels );

						PlReportErrorF( PL_RESULT_MEMORY_ALLOCATION, "couldn't allocate memory for image data" );
						return false;
					}

					lw /= 2;
					lh /= 2;
				}

				/* Now that all levels have been converted, free and replace the old buffers. */

				for ( unsigned int l = 0; l < image->levels; ++l ) {
					pl_free( image->data[ l ] );
				}

				pl_free( image->data );
				image->data = levels;

				image->format = new_format;
				/* TODO: Update colour_format */

				return true;
			}
		} break;

		default:
			break;
	}

	PlReportErrorF( PL_RESULT_IMAGEFORMAT, "unsupported image format conversion" );
	return false;
}

unsigned int PlGetImageSize( PLImageFormat format, unsigned int width, unsigned int height ) {
	switch ( format ) {
		case PL_IMAGEFORMAT_RGB_DXT1:
			return ( width * height ) >> 1;
		default: {
			unsigned int bytes = PlImageBytesPerPixel( format );
			return width * height * bytes;
		}
	}
}

/* Returns the number of BYTES per pixel for the given PLImageFormat.
 *
 * If the format doesn't have a predictable size or the size isn't a multiple
 * of one byte, returns ZERO. */
unsigned int PlImageBytesPerPixel( PLImageFormat format ) {
	switch ( format ) {
		case PL_IMAGEFORMAT_RGBA4:
		case PL_IMAGEFORMAT_RGB5A1:
		case PL_IMAGEFORMAT_RGB565:
			return 2;
		case PL_IMAGEFORMAT_RGB8:
			return 3;
		case PL_IMAGEFORMAT_RGBA_DXT1:
		case PL_IMAGEFORMAT_RGBA8:
			return 4;
		case PL_IMAGEFORMAT_RGBA12:
			return 6;
		case PL_IMAGEFORMAT_RGBA16:
		case PL_IMAGEFORMAT_RGBA16F:
			return 8;
		default:
			return 0;
	}
}

void PlInvertImageColour( PLImage *image ) {
	unsigned int num_colours = PlGetNumberOfColourChannels( image->colour_format );
	switch ( image->format ) {
		case PL_IMAGEFORMAT_RGB8:
		case PL_IMAGEFORMAT_RGBA8: {
			for ( size_t i = 0; i < image->size; i += num_colours ) {
				uint8_t *pixel = &image->data[ 0 ][ i ];
				pixel[ 0 ] = ~pixel[ 0 ];
				pixel[ 1 ] = ~pixel[ 1 ];
				pixel[ 2 ] = ~pixel[ 2 ];
			}
		} break;

		default:
			break;
	}

	PlReportErrorF( PL_RESULT_IMAGEFORMAT, "unsupported image format" );
}

/* utility function */
void PlGenerateStipplePattern( PLImage *image, unsigned int depth ) {
#if 0
    unsigned int p = 0;
    unsigned int num_colours = plGetSamplesPerPixel(image->colour_format);
    switch(image->format) {
        case PL_IMAGEFORMAT_RGB8: break;
        case PL_IMAGEFORMAT_RGBA8: {
            for(unsigned int i = 0; i < image->size; i += num_colours) {
                uint8_t *pixel = &image->data[0][i];
                if(num_colours == 4) {
                    //if()
                }
            }
        } break;

        default:break;
    }

    ReportError(PL_RESULT_IMAGEFORMAT, "unsupported image format");
#else
	/* todo */
#endif
}

void PlReplaceImageColour( PLImage *image, PLColour target, PLColour dest ) {
	unsigned int num_colours = PlGetNumberOfColourChannels( image->colour_format );
	switch ( image->format ) {
		case PL_IMAGEFORMAT_RGB8:
		case PL_IMAGEFORMAT_RGBA8: {
			for ( unsigned int i = 0; i < image->size; i += num_colours ) {
				uint8_t *pixel = &image->data[ 0 ][ i ];
				if ( num_colours == 4 ) {
					if ( PlCompareColour( PLColour( pixel[ 0 ], pixel[ 1 ], pixel[ 2 ], pixel[ 3 ] ), target ) ) {
						pixel[ 0 ] = dest.r;
						pixel[ 1 ] = dest.g;
						pixel[ 2 ] = dest.b;
						pixel[ 3 ] = dest.a;
					}
				} else {
					if ( PlCompareColour( PLColourRGB( pixel[ 0 ], pixel[ 1 ], pixel[ 2 ] ), target ) ) {
						pixel[ 0 ] = dest.r;
						pixel[ 1 ] = dest.g;
						pixel[ 2 ] = dest.b;
					}
				}
			}
		} break;

		default:
			break;
	}

	PlReportErrorF( PL_RESULT_IMAGEFORMAT, "unsupported image format" );
}

void PlFreeImage( PLImage *image ) {
	FunctionStart();

	if ( image == NULL || image->data == NULL ) {
		return;
	}

	for ( unsigned int levels = 0; levels < image->levels; ++levels ) {
		pl_free( image->data[ levels ] );
	}

	pl_free( image->data );
}

bool PlImageIsPowerOfTwo( const PLImage *image ) {
	if ( ( ( image->width == 0 ) || ( image->height == 0 ) ) || ( !PlIsPowerOfTwo( image->width ) || !PlIsPowerOfTwo( image->height ) ) ) {
		return false;
	}

	return true;
}

bool PlFlipImageVertical( PLImage *image ) {
	unsigned int width = image->width;
	unsigned int height = image->height;

	unsigned int bytes_per_pixel = PlImageBytesPerPixel( image->format );
	if ( bytes_per_pixel == 0 ) {
		PlReportErrorF( PL_RESULT_IMAGEFORMAT, "cannot flip images in this format" );
		return false;
	}

	unsigned int bytes_per_row = width * bytes_per_pixel;

	unsigned char *swap = pl_malloc( bytes_per_row );
	if ( swap == NULL ) {
		return false;
	}

	for ( unsigned int l = 0; l < image->levels; ++l ) {
		for ( unsigned int r = 0; r < height / 2; ++r ) {
			unsigned char *tr = image->data[ l ] + ( r * bytes_per_row );
			unsigned char *br = image->data[ l ] + ( ( ( height - 1 ) - r ) * bytes_per_row );

			memcpy( swap, tr, bytes_per_row );
			memcpy( tr, br, bytes_per_row );
			memcpy( br, swap, bytes_per_row );
		}

		bytes_per_row /= 2;
		height /= 2;
	}

	pl_free( swap );

	return true;
}

/**
 * Returns a list of file extensions representing all
 * the formats supported by the image loader.
 */
const char **PlGetSupportedImageFormats( unsigned int *numElements ) {
	static const char *imageFormats[ MAX_IMAGE_LOADERS ];
	for ( unsigned int i = 0; i < numImageLoaders; ++i ) {
		imageFormats[ i ] = imageLoaders[ i ].extension;
	}

	*numElements = numImageLoaders;

	return imageFormats;
}
