// Filename:-	tr_jpeg_interace.cpp
//

// leave this as first line for PCH reasons...
//
#include "../server/exe_headers.h"



#include "tr_local.h"
#include "tr_jpeg_interface.h"

/*
 * Include file for users of JPEG library.
 * You will need to have included system headers that define at least
 * the typedefs FILE and size_t before you can include jpeglib.h.
 * (stdio.h is sufficient on ANSI-conforming systems.)
 * You may also wish to include "jerror.h".
 */

#define JPEG_INTERNALS
#include "../jpeg/libjpeg-turbo/jpeglib.h"

// JPG decompression now subroutinised so I can call it from the savegame stuff...
//
// This will Z_Malloc the output data buffer that gets fed back into "pic", so Z_Free it yourself later.
//
void Decompress_JPG( const char *filename, byte *pJPGData, unsigned long jpg_input_size, unsigned char **pic, int *width, int *height ) 
{
	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	/* This struct represents a JPEG error handler.  It is declared separately
	* because applications often want to supply a specialized error handler
	* (see the second half of this file for an example).  But here we just
	* take the easy way out and use the standard error handler, which will
	* print a message on stderr and call exit() if compression fails.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct jpeg_error_mgr jerr;
	/* More stuff */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	unsigned char *out;	
	byte  *bbuf;
	
	/* Step 1: allocate and initialize JPEG decompression object */
	
	/* We have to set up the error handler first, in case the initialization
	   * step fails.  (Unlikely, but it could happen if you are out of memory.)
	   * This routine fills in the contents of struct jerr, and returns jerr's
	   * address which we place into the link field in cinfo.
	   */
	cinfo.err = jpeg_std_error(&jerr);
	
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);
	
	/* Step 2: specify data source (eg, a file) */
	
	jpeg_mem_src(&cinfo, pJPGData, jpg_input_size);
	
	/* Step 3: read file parameters with jpeg_read_header() */
	
	(void) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	   *   (a) suspension is not possible with the stdio data source, and
	   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	   * See libjpeg.doc for more info.
	   */
	
	/* Step 4: set parameters for decompression */

	cinfo.out_color_space = JCS_EXT_RGBA;
	
	/* Step 5: Start decompressor */
	
	(void) jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	   * with the stdio data source.
	   */
	
	   /* We may need to do some setup of our own at this point before reading
	   * the data.  After jpeg_start_decompress() we have the correct scaled
	   * output image dimensions available, as well as the output colormap
	   * if we asked for color quantization.
	   * In this example, we need to make an output work buffer of the right size.
	   */ 
	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;
	
	if (cinfo.output_components!=4) {
		VID_Printf(PRINT_WARNING, "JPG %s is unsupported color depth (%d)\n", filename, cinfo.output_components);
	}
	out = (byte *)Z_Malloc(cinfo.output_width*cinfo.output_height*4, TAG_TEMP_JPG, qfalse );
	
	*pic = out;
	*width = cinfo.output_width;
	*height = cinfo.output_height;
	
	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */
	
	/* Here we use the library's state variable cinfo.output_scanline as the
	   * loop counter, so that we don't have to keep track ourselves.
	   */
	while (cinfo.output_scanline < cinfo.output_height) {
	/* jpeg_read_scanlines expects an array of pointers to scanlines.
	* Here the array is only one element long, but you could ask for
	* more than one scanline at a time if that's more convenient.
		*/
		bbuf = ((out+(row_stride*cinfo.output_scanline)));
		buffer = &bbuf;
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
	}
	
	/* Step 7: Finish decompression */
	
	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	   * with the stdio data source.
	   */
	
	/* Step 8: Release JPEG decompression object */
	
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);
	
	/* After finish_decompress, we can close the input file.
	* Here we postpone it until after no more JPEG errors are possible,
	* so as to simplify the setjmp error logic above.  (Actually, I don't
	* think that jpeg_destroy can do an error exit, but why assume anything...)
	*/
	
	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/
	
	/* And we're done! */
}

int LoadJPG( const char *filename, unsigned char **pic, int *width, int *height ) 
{	
	*pic = NULL;	
	
	fileHandle_t	h;
	const int len = FS_FOpenFileRead( filename, &h, qfalse );
	if(!h)
	{
		return 0;
	}
	byte *pJPGData = (byte *)Z_Malloc(len, TAG_TEMP_JPG, qfalse);
	FS_Read(pJPGData, len, h);
	FS_FCloseFile( h );

	Decompress_JPG( filename, pJPGData, len, pic, width, height );

	Z_Free (pJPGData);
	return len;
}


/* Expanded data destination object for stdio output */

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  byte* outfile;		/* target stream */
  int	size;
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;


/*
 * Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 */

void init_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  dest->pub.next_output_byte = dest->outfile;
  dest->pub.free_in_buffer = dest->size;
}


/*
 * Empty the output buffer --- called whenever buffer fills up.
 *
 * In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 */

boolean empty_output_buffer (j_compress_ptr cinfo)
{
  return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

static int hackSize;

void term_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
  size_t datacount = dest->size - dest->pub.free_in_buffer;
  hackSize = datacount;
}


/*
 * Prepare for output to a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing compression.
 */

void jpegDest (j_compress_ptr cinfo, byte* outfile, int size)
{
  my_dest_ptr dest;

  /* The destination object is made permanent so that multiple JPEG images
   * can be written to the same file without re-executing jpeg_stdio_dest.
   * This makes it dangerous to use this manager and a different destination
   * manager serially with the same JPEG object, because their private object
   * sizes may be different.  Caveat programmer.
   */
  if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  sizeof(my_destination_mgr));
  }

  dest = (my_dest_ptr) cinfo->dest;
  dest->pub.init_destination = init_destination;
  dest->pub.empty_output_buffer = empty_output_buffer;
  dest->pub.term_destination = term_destination;
  dest->outfile = outfile;
  dest->size = size;
}

// returns a Z_Malloc'd piece of mem that you should free up yourself
//
byte *Compress_JPG(int *pOutputSize, int quality, int image_width, int image_height, byte *image_buffer, qboolean bInvertDuringCompression)
{
  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
  int row_stride;		/* physical row width in image buffer */  

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */
	byte *out =	//  (unsigned char *)ri.Hunk_AllocateTempMemory(image_width*image_height*4);
					(unsigned char *)Z_Malloc(image_width*image_height*4, TAG_TEMP_JPG, qfalse);  	

  jpegDest(&cinfo, out, image_width*image_height*4);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = image_width; 	/* image width and height, in pixels */
  cinfo.image_height = image_height;
  cinfo.input_components = 4;		/* # of color components per pixel */
  cinfo.in_color_space = JCS_EXT_RGBA; 	/* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = image_width * 4;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
	 if (bInvertDuringCompression)
	 { 
		row_pointer[0] = & image_buffer[((cinfo.image_height-1)*row_stride)-cinfo.next_scanline * row_stride];
	 }
	 else
	 {
		row_pointer[0] = & image_buffer[									cinfo.next_scanline * row_stride];		 
	 }

     jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  
  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */

  *pOutputSize = hackSize;
  return out;
}

void SaveJPG(const char * filename, int quality, int image_width, int image_height, unsigned char *image_buffer) 
{
	int iOutputSize = 0;

	byte *pbOut = Compress_JPG(&iOutputSize, quality, image_width, image_height, image_buffer, qtrue);

	FS_WriteFile( filename, pbOut, iOutputSize );

	Z_Free(pbOut);
}



void JPG_ErrorThrow(LPCSTR message)
{
	Com_Error( ERR_FATAL, "JPG: %s\n", message );
}

void JPG_MessageOut(LPCSTR message)
{
	VID_Printf(PRINT_ALL, "%s\n", message);
}

//////////////// eof ////////////

