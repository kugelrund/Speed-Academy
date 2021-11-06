// Generic PNG file loading code

// leave this as first line for PCH reasons...
//
#include "../game/q_shared.h"
#include "../qcommon/qcommon.h"

#include "../zlib/zlib.h"
#include "png.h"
//#include "../qcommon/memory.h"

#define WUFFS_CONFIG__MODULES
#define WUFFS_CONFIG__MODULE__ADLER32
#define WUFFS_CONFIG__MODULE__BASE
#define WUFFS_CONFIG__MODULE__CRC32
#define WUFFS_CONFIG__MODULE__DEFLATE
#define WUFFS_CONFIG__MODULE__PNG
#define WUFFS_CONFIG__MODULE__ZLIB
#include "../wuffs-mirror-release-c/release/c/wuffs-v0.3.c"

// Error returns, some of these (mostly those for loading) are not required anymore
// with the introduction of WUFFS.

#define PNG_ERROR_OK			0
#define PNG_ERROR_DECOMP		1
#define PNG_ERROR_COMP			2
#define PNG_ERROR_MEMORY		3
#define PNG_ERROR_NOSIG	   		4
#define PNG_ERROR_TOO_SMALL	   	5
#define PNG_ERROR_WNP2	   		6
#define PNG_ERROR_HNP2	   		7
#define PNG_ERROR_NOT_TC	   	8
#define PNG_ERROR_INV_FIL	   	9
#define PNG_ERROR_FAILED_CRC	10
#define PNG_ERROR_CREATE_FAIL	11
#define PNG_ERROR_WRITE			12
#define PNG_ERROR_NOT_PALETTE	13
#define PNG_ERROR_NOT8BIT		14
#define PNG_ERROR_TOO_LARGE		15
#define PNG_ERROR_WUFFS         16

static int png_error = PNG_ERROR_OK;

static const byte png_signature[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
static const char png_copyright[] = "Copyright\0Raven Software Inc. 2001";
static const char *png_errors[] = 
{
	"OK.",
	"Error decompressing image data.",
	"Error compressing image data.",
	"Error allocating memory.",
	"PNG signature not found.",
	"Image is too small to load.",
	"Width is not a power of two.",
	"Height is not a power of two.",
	"Image is not 24 or 32 bit.",
	"Invalid filter or compression type.",
	"Failed CRC check.",
	"Could not create file.",
	"Error writing to file.",
	"Image is not indexed colour.",
	"Image does not have 8 bits per sample.",
	"Image is too large",
	"",  // to be filled with wuffs message
};

// Gets the error string for a failed PNG operation

const char *PNG_GetError(void)
{
	return(png_errors[png_error]);
}

// Create a header chunk

void PNG_CreateHeader(png_ihdr_t *header, int width, int height, int bytedepth)
{
	header->width = BigLong(width);
	header->height = BigLong(height);
	header->bitdepth = 8;

	if(bytedepth == 3)
	{
		header->colortype = 2;
	}
	if(bytedepth == 4)
	{
		header->colortype = 6;
	}
	header->compression = 0;
	header->filter = 0;
	header->interlace = 0;
}

// Filter a row of data

void PNG_Filter(byte *out, byte filter, const byte *in, const byte *lastline, ulong rowbytes, ulong bpp)
{
	ulong		i;

	switch(filter)
	{
	case PNG_FILTER_VALUE_NONE:
		memcpy(out, in, rowbytes);
		break;
	case PNG_FILTER_VALUE_SUB:
		for(i = 0; i < bpp; i++)
		{
			*out++ = *in++;
		}
		for(i = bpp; i < rowbytes; i++)
		{
			*out++ = *in - *(in - bpp);
			in++;
		}
		break;
	case PNG_FILTER_VALUE_UP:
		for(i = 0; i < rowbytes; i++)
		{
			if(lastline)
			{
				*out++ = *in++ - *lastline++;
			}
			else
			{
				*out++ = *in++;
			}
		}
		break;
	case PNG_FILTER_VALUE_AVG:
		for(i = 0; i < bpp; i++)
		{
			if(lastline)
			{
				*out++ = *in++ - (*lastline++ >> 1);
			}
			else
			{
				*out++ = *in++;
			}
		}
		for(i = bpp; i < rowbytes; i++)
		{
			if(lastline)
			{
				*out++ = *in - ((*lastline++ + *(in - bpp)) >> 1);
			}
			else
			{
				*out++ = *in - (*(in - bpp) >> 1);
			}
			in++;
		}
		break;
	case PNG_FILTER_VALUE_PAETH:
		int			a, b, c;
		int			pa, pb, pc, p;

		for(i = 0; i < bpp; i++)
		{
			if(lastline)
			{
				*out++ = *in++ - *lastline++;
			}
			else
			{
				*out++ = *in++;
			}
		}
		for(i = bpp; i < rowbytes; i++)
		{
			a = *(in - bpp);
			c = 0;
			b = 0;
			if(lastline)
			{
				c = *(lastline - bpp);
				b = *lastline++;
			}

			p = b - c;
			pc = a - c;

			pa = p < 0 ? -p : p;
			pb = pc < 0 ? -pc : pc;
			pc = (p + pc) < 0 ? -(p + pc) : p + pc;

			p = (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;

			*out++ = *in++ - p;
		}
		break;
	}
}

// Pack up the image data line by line

bool PNG_Pack(byte *out, ulong *size, ulong maxsize, byte *data, int width, int height, int bytedepth)
{
	z_stream		zdata;
	ulong			rowbytes;
	ulong			y;
	const byte		*lastline, *source;
	// Storage for filter type and filtered row
	byte			workline[(MAX_PNG_WIDTH * MAX_PNG_DEPTH) + 1];

	// Number of bytes per row
	rowbytes = width * bytedepth;

	memset(&zdata, 0, sizeof(z_stream));
	if(deflateInit(&zdata, Z_DEFAULT_COMPRESSION) != Z_OK)
	{
		png_error = PNG_ERROR_COMP;
		return(false);
	}

	zdata.next_out = out;
	zdata.avail_out = maxsize;

	lastline = NULL;
	source = data + ((height - 1) * rowbytes);
	for(y = 0; y < height; y++)
	{
		// Refilter using the most compressable filter algo
		// Assume paeth to speed things up
		workline[0] = (byte)PNG_FILTER_VALUE_PAETH;
		PNG_Filter(workline + 1, (byte)PNG_FILTER_VALUE_PAETH, source, lastline, rowbytes, bytedepth);

		zdata.next_in = workline;
		zdata.avail_in = rowbytes + 1;
		if(deflate(&zdata, Z_SYNC_FLUSH) != Z_OK)
		{
			deflateEnd(&zdata);
			png_error = PNG_ERROR_COMP;
			return(false);
		}
		lastline = source;
		source -= rowbytes;
	}
	if(deflate(&zdata, Z_FINISH) != Z_STREAM_END)
	{
		png_error = PNG_ERROR_COMP;
		return(false);
	}
	*size = zdata.total_out;
	deflateEnd(&zdata);
	return(true);
}

static bool PNG_error_Wuffs(const wuffs_base__status& status)
{
	png_error = PNG_ERROR_WUFFS;
	png_errors[PNG_ERROR_WUFFS] = status.message();
	return false;
}

bool PNG_Load(const byte *data, ulong datasize, png_image_t *image, uint32_t wuffs_pixfmt_repr)
{
	png_error = PNG_ERROR_OK;

	const auto src_slice = wuffs_base__make_slice_u8((uint8_t*)data, datasize);
	const auto src_io_buffer_meta = wuffs_base__make_io_buffer_meta(datasize, 0, 0, false);
	auto src_io_buffer = wuffs_base__make_io_buffer(src_slice, src_io_buffer_meta);

	const auto decoder = (wuffs_png__decoder*)Z_Malloc(sizeof__wuffs_png__decoder(), TAG_TEMP_PNG, qfalse);
	wuffs_base__status status = decoder->initialize(
		sizeof__wuffs_png__decoder(), WUFFS_VERSION, WUFFS_INITIALIZE__DEFAULT_OPTIONS);
	if (!status.is_ok()) {
		Z_Free(decoder);
		return PNG_error_Wuffs(status);
	}

	auto image_config = wuffs_base__null_image_config();
	status = decoder->decode_image_config(&image_config, &src_io_buffer);
	if (!status.is_ok()) {
		Z_Free(decoder);
		return PNG_error_Wuffs(status);
	}

	auto frame_config = wuffs_base__null_frame_config();
	status = decoder->decode_frame_config(&frame_config, &src_io_buffer);
	if (!status.is_ok()) {
		Z_Free(decoder);
		return PNG_error_Wuffs(status);
	}

	auto pixel_config = wuffs_base__null_pixel_config();
	pixel_config.set(wuffs_pixfmt_repr, WUFFS_BASE__PIXEL_SUBSAMPLING__NONE, frame_config.width(), frame_config.height());

	image->width = pixel_config.width();
	image->height = pixel_config.height();
	image->bytedepth = pixel_config.pixel_format().bits_per_pixel() / 8;
	image->data = (byte*)Z_Malloc(image->width * image->height * image->bytedepth, TAG_TEMP_PNG, qfalse);

	const auto pixel_buffer_memory_slice = wuffs_base__make_slice_u8(image->data, image->width * image->height * image->bytedepth);
	auto pixel_buffer = wuffs_base__null_pixel_buffer();
	pixel_buffer.set_from_slice(&pixel_config, pixel_buffer_memory_slice);
	wuffs_base__pixel_blend pixel_blend = WUFFS_BASE__PIXEL_BLEND__SRC;
	auto work_buffer = (uint8_t*)Z_Malloc(decoder->workbuf_len().max_incl, TAG_TEMP_PNG, qfalse);
	wuffs_base__slice_u8 work_buffer_slice = wuffs_base__make_slice_u8(work_buffer, decoder->workbuf_len().max_incl);
	status = decoder->decode_frame(&pixel_buffer, &src_io_buffer, pixel_blend, work_buffer_slice, nullptr);
	Z_Free(work_buffer);
	Z_Free(decoder);

	if (!status.is_ok()) {
		return PNG_error_Wuffs(status);
	}
	return true;
}

// Outputs a crc'd chunk of PNG data

bool PNG_OutputChunk(fileHandle_t fp, ulong type, byte *data, ulong size)
{
	ulong	crc, little, outcount;

	// Output a standard PNG chunk - length, type, data, crc
	little = BigLong(size);
	outcount = FS_Write(&little, sizeof(little), fp);

	little = BigLong(type);
	crc = crc32(0, (byte *)&little, sizeof(little));
	outcount += FS_Write(&little, sizeof(little), fp);

	if(size)
	{
		crc = crc32(crc, data, size);
		outcount += FS_Write(data, size, fp);
	}

	little = BigLong(crc);
	outcount += FS_Write(&little, sizeof(little), fp);

	if(outcount != (size + 12))
	{
		png_error = PNG_ERROR_WRITE;
		return(false);
	}
	return(true);
}

// Saves a PNG format compressed image

bool PNG_Save(const char *name, byte *data, int width, int height, int bytedepth)
{
	byte			*work;
	fileHandle_t	fp;
	int				maxsize;
	ulong			size, outcount;
	png_ihdr_t		png_header;

	png_error = PNG_ERROR_OK;

	// Create the file
	fp = FS_FOpenFileWrite(name);
	if(!fp)
	{
		png_error = PNG_ERROR_CREATE_FAIL;
		return(false);
	}
	// Write out the PNG signature
	outcount = FS_Write(png_signature, sizeof(png_signature), fp);
	if(outcount != sizeof(png_signature))
	{
		FS_FCloseFile(fp);
		png_error = PNG_ERROR_WRITE;
		return(false);
	}
	// Create and output a valid header
	PNG_CreateHeader(&png_header, width, height, bytedepth);
	if(!PNG_OutputChunk(fp, PNG_IHDR, (byte *)&png_header, sizeof(png_header)))
	{
		FS_FCloseFile(fp);
		return(false);
	}
	// Create and output the copyright info
 	if(!PNG_OutputChunk(fp, PNG_tEXt, (byte *)png_copyright, sizeof(png_copyright)))
	{
		FS_FCloseFile(fp);
		return(false);
	}
	// Max size of compressed image (source size + 0.1% + 12)
	maxsize = (width * height * bytedepth) + 4096;
	work = (byte *)Z_Malloc(maxsize, TAG_TEMP_PNG, qtrue);	// fixme: optimise to qfalse sometime - ok?
  
	// Pack up the image data
	if(!PNG_Pack(work, &size, maxsize, data, width, height, bytedepth))
	{
		Z_Free(work);
		FS_FCloseFile(fp);
		return(false);
	}
	// Write out the compressed image data
	if(!PNG_OutputChunk(fp, PNG_IDAT, (byte *)work, size))
	{
		Z_Free(work);
		FS_FCloseFile(fp);
		return(false);
	}
	Z_Free(work);
	// Output terminating chunk
	if(!PNG_OutputChunk(fp, PNG_IEND, NULL, 0))
	{
		FS_FCloseFile(fp);
		return(false);
	}
	FS_FCloseFile(fp);
	return(true);
}

/*
=============
LoadPNG32
=============
*/
bool LoadPNG32 (char *name, byte **pixels, int *width, int *height, int *bytedepth)
{
	byte		*buffer;
	byte		**bufferptr = &buffer;
	int			nLen;
	png_image_t	png_image;

	if(!pixels)
	{
		bufferptr = NULL;
	}
	nLen = FS_ReadFile ( ( char * ) name, (void **)bufferptr);
	if (nLen == -1) 
	{
		if(pixels)
		{
			*pixels = NULL;
		}
		return(false);
	}
	if(!pixels)
	{
		return(true);
	}
	*pixels = NULL;
	png_image.isimage = true;
	if(!PNG_Load(buffer, nLen, &png_image, WUFFS_BASE__PIXEL_FORMAT__RGBA_NONPREMUL))
	{
		Com_Printf ("Error parsing %s: %s\n", name, PNG_GetError());
		return(false);
	}
	*pixels = png_image.data;
	if(width)
	{
		*width = png_image.width;
	}
	if(height)
	{
		*height = png_image.height;
	}
	if(bytedepth)
	{
		*bytedepth = png_image.bytedepth;
	}
	FS_FreeFile(buffer);
	return(true);
}

/*
=============
LoadPNG8
=============
*/
bool LoadPNG8 (char *name, byte **pixels, int *width, int *height)
{
	byte		*buffer;
	byte		**bufferptr = &buffer;
	int			nLen;
	png_image_t	png_image;

	if(!pixels)
	{
		bufferptr = NULL;
	}
	nLen = FS_ReadFile ( ( char * ) name, (void **)bufferptr);
	if (nLen == -1) 
	{
		if(pixels)
		{
			*pixels = NULL;
		}
		return(false);
	}
	if(!pixels)
	{
		return(true);
	}
	*pixels = NULL;
	png_image.isimage = false;
	if(!PNG_Load(buffer, nLen, &png_image, WUFFS_BASE__PIXEL_FORMAT__A))
	{
		Com_Printf ("Error parsing %s: %s\n", name, PNG_GetError());
		return(false);
	}
	*pixels = png_image.data;
	if(width)
	{
		*width = png_image.width;
	}
	if(height)
	{
		*height = png_image.height;
	}
	FS_FreeFile(buffer);
	return(true);
}

// end