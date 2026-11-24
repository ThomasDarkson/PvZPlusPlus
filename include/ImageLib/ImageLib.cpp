#define XMD_H

#include <windows.h>
#include "ImageLib.h"
#include "png/png.h"
#include <math.h>
#include <tchar.h>
#include "../../PakLib/PakInterface.h"

extern "C"
{
#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
}

                              



ImageLib::Image::Image()
{
	mWidth = 0;
	mHeight = 0;
	mBits = NULL;
}

ImageLib::Image::~Image()
{
	delete[] mBits;
}

int	ImageLib::Image::GetWidth()
{
	return mWidth;
}

int	ImageLib::Image::GetHeight()
{
	return mHeight;
}

unsigned long* ImageLib::Image::GetBits()
{
	return mBits;
}

                                                                          
                  

static void png_pak_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_size_t check;

	                                                                        
                                                             
   
	check = (png_size_t)p_fread(data, (png_size_t)1, length,
		(PFILE*)png_ptr->io_ptr);

	if (check != length)
	{
		png_error(png_ptr, "Read Error");
	}
}

ImageLib::Image* GetPNGImage(const SexyString& theFileName)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	PFILE *fp;

	if ((fp = p_fopen(theFileName.c_str(), _S("rb"))) == NULL)
		return NULL;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
	  NULL, NULL, NULL);
	png_set_read_fn(png_ptr, (png_voidp)fp, png_pak_read_data);

	if (png_ptr == NULL)
	{
		p_fclose(fp);
		return NULL;
	}

	                                                                      
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		p_fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return NULL;
	}

                                                                            
                                                                          
                                                                             
      
	if (setjmp(png_ptr->jmpbuf))
	{
		                                                                     
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		p_fclose(fp);
		                                                       
		return NULL;
	}

	                           

	                                  

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       &interlace_type, NULL, NULL);

	                                                                
	png_set_expand(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	                                   
	png_set_palette_to_rgb(png_ptr);
	png_set_gray_to_rgb(png_ptr);
	png_set_bgr(png_ptr);

                                                                    
	unsigned long* aBits = new unsigned long[width*height];
	unsigned long* anAddr = aBits;
	for (int i = 0; i < height; i++)
	{
		png_read_rows(png_ptr, (png_bytepp) &anAddr, NULL, 1);
		anAddr += width;
	}

	                                                                         
	png_read_end(png_ptr, info_ptr);

	                                                                       
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	                    
	p_fclose(fp);

	ImageLib::Image* anImage = new ImageLib::Image();
	anImage->mWidth = width;
	anImage->mHeight = height;
	anImage->mBits = aBits;

	return anImage;
}

ImageLib::Image* GetTGAImage(const SexyString& theFileName)
{
	PFILE* aTGAFile = p_fopen(theFileName.c_str(), _S("rb"));
	if (aTGAFile == NULL)
		return NULL;

	BYTE aHeaderIDLen;
	p_fread(&aHeaderIDLen, sizeof(BYTE), 1, aTGAFile);

	BYTE aColorMapType;
	p_fread(&aColorMapType, sizeof(BYTE), 1, aTGAFile);
	
	BYTE anImageType;
	p_fread(&anImageType, sizeof(BYTE), 1, aTGAFile);

	WORD aFirstEntryIdx;
	p_fread(&aFirstEntryIdx, sizeof(WORD), 1, aTGAFile);

	WORD aColorMapLen;
	p_fread(&aColorMapLen, sizeof(WORD), 1, aTGAFile);

	BYTE aColorMapEntrySize;
	p_fread(&aColorMapEntrySize, sizeof(BYTE), 1, aTGAFile);	

	WORD anXOrigin;
	p_fread(&anXOrigin, sizeof(WORD), 1, aTGAFile);

	WORD aYOrigin;
	p_fread(&aYOrigin, sizeof(WORD), 1, aTGAFile);

	WORD anImageWidth;
	p_fread(&anImageWidth, sizeof(WORD), 1, aTGAFile);	

	WORD anImageHeight;
	p_fread(&anImageHeight, sizeof(WORD), 1, aTGAFile);	

	BYTE aBitCount = 32;
	p_fread(&aBitCount, sizeof(BYTE), 1, aTGAFile);	

	BYTE anImageDescriptor = 8 | (1<<5);
	p_fread(&anImageDescriptor, sizeof(BYTE), 1, aTGAFile);

	if ((aBitCount != 32) ||
		(anImageDescriptor != (8 | (1<<5))))
	{
		p_fclose(aTGAFile);
		return NULL;
	}

	ImageLib::Image* anImage = new ImageLib::Image();

	anImage->mWidth = anImageWidth;
	anImage->mHeight = anImageHeight;
	anImage->mBits = new unsigned long[anImageWidth*anImageHeight];

	p_fread(anImage->mBits, 4, anImage->mWidth*anImage->mHeight, aTGAFile);

	p_fclose(aTGAFile);

	return anImage;
}

int ReadBlobBlock(PFILE* fp, char* data)
{
	unsigned char aCount = 0;
	p_fread(&aCount, sizeof(char), 1, fp);
	p_fread(data, sizeof(char), aCount, fp);
	return aCount;
}

ImageLib::Image* GetGIFImage(const SexyString& theFileName)
{
	#define BitSet(byte,bit)  (((byte) & (bit)) == (bit))
	#define LSBFirstOrder(x,y)  (((y) << 8) | (x))

	int
		opacity,
		status;

	register int i;

	register unsigned char *p;

	unsigned char
		background,			                                                                                                                                                                   
		c,
		flag,				                                       
		*global_colormap,	                           
		header[1664],
		magick[12];

	unsigned int
		delay,
		dispose,
		global_colors,		                                   
		image_count,
		iterations;

	  
                 
   

	PFILE *fp;

	if ((fp = p_fopen(theFileName.c_str(), _S("rb"))) == NULL)
		return NULL;
	  
                                 
   
	status = p_fread(magick, sizeof(char), 6, fp);                                                                                            

	                                                                                                                                                               
	if (((sexystrncmp((SexyChar*)magick, _S("GIF87"), 5) != 0) && (sexystrncmp((SexyChar*)magick, _S("GIF89"), 5) != 0)))
		return NULL;

	global_colors = 0;
	global_colormap = (unsigned char*)NULL;

	short pw;                     
	short ph;                     

	                                                          
	p_fread(&pw, sizeof(short), 1, fp);                                                 
	p_fread(&ph, sizeof(short), 1, fp);                                                 
	p_fread(&flag, sizeof(char), 1, fp);                             
	p_fread(&background, sizeof(char), 1, fp);                                                                                                                                 
	p_fread(&c, sizeof(char), 1, fp);                                 

	if (BitSet(flag, 0x80))                                             
	{
		  
                          
    
		global_colors = 1 << ((flag & 0x07) + 1);                                                                                                                                                                
		global_colormap = new unsigned char[3 * global_colors];                                                             
		if (global_colormap == (unsigned char*)NULL)
			return NULL;

		p_fread(global_colormap, sizeof(char), 3 * global_colors, fp);                                     
	}

	delay = 0;
	dispose = 0;
	iterations = 1;
	opacity = (-1);
	image_count = 0;

	for (; ; )
	{
		if (p_fread(&c, sizeof(char), 1, fp) == 0)
			break;                                                                                             

		if (c == ';')                                                                     
			break;                  
		if (c == '!')                                                  
		{
			  
                       
     
			p_fread(&c, sizeof(char), 1, fp);                                                 

			switch (c)
			{
			case 0xf9:
			{
				  
                                    
      
				while (ReadBlobBlock(fp, (char*)header) > 0);

				dispose = header[0] >> 2;
				delay = (header[2] << 8) | header[1];
				if ((header[0] & 0x01) == 1)
					opacity = header[3];
				break;
			}
			case 0xfe:
			{
				char* comments;
				int length;

				  
                           
      
				comments = (char*)NULL;
				for (; ; )
				{
					length = ReadBlobBlock(fp, (char*)header);
					if (length <= 0)
						break;
					if (comments == NULL)
					{
						comments = new char[length + 1];
						if (comments != (char*)NULL)
							*comments = '\0';
					}

					header[length] = '\0';
					strcat(comments, (char*)header);
				}
				if (comments == (char*)NULL)
					break;

				delete comments;
				break;
			}
			case 0xff:
			{
				int
					loop;

				  
                                 
      
				loop = false;
				if (ReadBlobBlock(fp, (char*)header) > 0)
					loop = !strncmp((char*)header, "NETSCAPE2.0", 11);
				while (ReadBlobBlock(fp, (char*)header) > 0)
					if (loop)
						iterations = (header[2] << 8) | header[1];
				break;
			}
			default:
			{
				while (ReadBlobBlock(fp, (char*)header) > 0);
				break;
			}
			}
		}

		if (c != ',')                                                     
			continue;

		if (image_count != 0)
		{
			  
                                 
     

			                                      
                                               
    
                        
                                    
    
                     
                                                                   
		}
		image_count++;

		short pagex;
		short pagey;
		short width;
		short height;
		int colors;
		bool interlaced;

		p_fread(&pagex, sizeof(short), 1, fp);                                             
		p_fread(&pagey, sizeof(short), 1, fp);                                            
		p_fread(&width, sizeof(short), 1, fp);                                                  
		p_fread(&height, sizeof(short), 1, fp);                                                   
		p_fread(&flag, sizeof(char), 1, fp);                                             

		colors = !BitSet(flag, 0x80) ? global_colors : 1 << ((flag & 0x07) + 1);                                                                                                                 
		unsigned long* colortable = new unsigned long[colors];                             

		interlaced = BitSet(flag, 0x40);                                                                                                                          

		delay = 0;
		dispose = 0;
		iterations = 1;
		                       
   
                  
                     

                   
                
     
		if ((width == 0) || (height == 0))
			return NULL;
		  
                        
    
		                                                  
                                                                       
           
		if (!BitSet(flag, 0x80))                                             
		{
			  
                       
     
			p = global_colormap;
			for (i = 0; i < (int)colors; i++)
			{
				int r = *p++;
				int g = *p++;
				int b = *p++;

				colortable[i] = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}

			                          
			                                                   
		}
		else
		{
			unsigned char
				* colormap;

			  
                       
     
			colormap = new unsigned char[3 * colors];

			int pos = p_ftell(fp);
			p_fread(colormap, sizeof(char), 3 * colors, fp);

			p = colormap;
			for (i = 0; i < (int)colors; i++)
			{
				int r = *p++;
				int g = *p++;
				int b = *p++;

				colortable[i] = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}
			delete colormap;
		}

		                              
   
                                      
   
                           
                             
                            
   
                          
     
		  
               
    
		                                              

		                                                
		                                              
		if (global_colormap != NULL)
		{
			delete[] global_colormap;
			global_colormap = NULL;
		}

		                                                     
		                            

#define MaxStackSize  4096
#define NullCode  (-1)

		int
			available,
			bits,
			code,
			clear,
			code_mask,
			code_size,
			count,
			end_of_information,
			in_code,
			offset,
			old_code,
			pass,
			y;

		register int
			x;

		register unsigned int
			datum;

		short
			* prefix;

		unsigned char
			data_size,
			first,
			* packet,
			* pixel_stack,
			* suffix,
			* top_stack;

		  
                          
    

		packet = new unsigned char[256];
		prefix = new short[MaxStackSize];
		suffix = new unsigned char[MaxStackSize];
		pixel_stack = new unsigned char[MaxStackSize + 1];

		  
                                     
    
		p_fread(&data_size, sizeof(char), 1, fp);
		clear = 1 << data_size;
		end_of_information = clear + 1;
		available = clear + 2;
		old_code = NullCode;
		code_size = data_size + 1;
		code_mask = (1 << code_size) - 1;
		for (code = 0; code < clear; code++)
		{
			prefix[code] = 0;
			suffix[code] = (unsigned char)code;
		}
		  
                          
    
		datum = 0;
		bits = 0;
		c = 0;
		count = 0;
		first = 0;
		offset = 0;
		pass = 0;
		top_stack = pixel_stack;

		unsigned long* aBits = new unsigned long[width * height];

		register unsigned char* c = NULL;

		for (y = 0; y < (int)height; y++)
		{
			                                           
			                                
			        
			                            

			unsigned long* q = aBits + offset * width;



			for (x = 0; x < (int)width; )
			{
				if (top_stack == pixel_stack)
				{
					if (bits < code_size)
					{
						  
                                                       
        
						if (count == 0)
						{
							  
                             
         
							int pos = p_ftell(fp);

							count = ReadBlobBlock(fp, (char*)packet);
							if (count <= 0)
								break;
							c = packet;
						}
						datum += (*c) << bits;
						bits += 8;
						c++;
						count--;
						continue;
					}
					  
                       
       
					code = datum & code_mask;
					datum >>= code_size;
					bits -= code_size;
					  
                       
       
					if ((code > available) || (code == end_of_information))
						break;
					if (code == clear)
					{
						  
                    
        
						code_size = data_size + 1;
						code_mask = (1 << code_size) - 1;
						available = clear + 2;
						old_code = NullCode;
						continue;
					}
					if (old_code == NullCode)
					{
						*top_stack++ = suffix[code];
						old_code = code;
						first = (unsigned char)code;
						continue;
					}
					in_code = code;
					if (code >= available)
					{
						*top_stack++ = first;
						code = old_code;
					}
					while (code >= clear)
					{
						*top_stack++ = suffix[code];
						code = prefix[code];
					}
					first = suffix[code];
					  
                                          
       
					if (available >= MaxStackSize)
						break;
					*top_stack++ = first;
					prefix[available] = old_code;
					suffix[available] = first;
					available++;
					if (((available & code_mask) == 0) && (available < MaxStackSize))
					{
						code_size++;
						code_mask += available;
					}
					old_code = in_code;
				}
				  
                                    
      
				top_stack--;

				int index = (*top_stack);

				*q = colortable[index];

				if (index == opacity)
					*q = 0;

				x++;
				q++;
			}

			if (!interlaced)
				offset++;
			else
			{
				switch (pass)
				{
				case 0:
				default:
				{
					offset += 8;
					if (offset >= height)
					{
						pass++;
						offset = 4;
					}
					break;
				}
				case 1:
				{
					offset += 8;
					if (offset >= height)
					{
						pass++;
						offset = 2;
					}
					break;
				}
				case 2:
				{
					offset += 4;
					if (offset >= height)
					{
						pass++;
						offset = 1;
					}
					break;
				}
				case 3:
				{
					offset += 2;
					break;
				}
				}
			}

			if (x < width)
				break;

			                                                  
                                  
                                                
		}
		delete pixel_stack;
		delete suffix;
		delete prefix;
		delete packet;

		delete colortable;

		                      
		                

		ImageLib::Image* anImage = new ImageLib::Image();

		anImage->mWidth = width;
		anImage->mHeight = height;
		anImage->mBits = aBits;

		                                 
		p_fclose(fp);
		return anImage;
	}

	p_fclose(fp);

	return NULL;
}

typedef struct my_error_mgr * my_error_ptr;

struct my_error_mgr
{
  struct jpeg_error_mgr pub;	                     

  jmp_buf setjmp_buffer;	                          
};

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
                                                                            
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

                                   
                                                                  
  (*cinfo->err->output_message) (cinfo);

                                          
  longjmp(myerr->setjmp_buffer, 1);

}

bool ImageLib::WriteJPEGImage(const SexyString& theFileName, ImageLib::Image* theImage)
{
	FILE *fp;

	if ((fp = sexyfopen(theFileName.c_str(), _S("wb"))) == NULL)
		return false;

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		                                                       
                                                                           
     
		jpeg_destroy_compress(&cinfo);
		fclose(fp);
		return false;
	}

	jpeg_create_compress(&cinfo);

	cinfo.image_width = theImage->mWidth;
	cinfo.image_height = theImage->mHeight;
	cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    cinfo.optimize_coding = 1;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 80, TRUE);

	jpeg_stdio_dest(&cinfo, fp);

	jpeg_start_compress(&cinfo, true);

	int row_stride = theImage->GetWidth() * 3;

	unsigned char* aTempBuffer = new unsigned char[row_stride];

	unsigned long* aSrcPtr = theImage->mBits;

	for (int aRow = 0; aRow < theImage->mHeight; aRow++)
	{
		unsigned char* aDest = aTempBuffer;

		for (int aCol = 0; aCol < theImage->mWidth; aCol++)
		{
			unsigned long src = *(aSrcPtr++);

			*aDest++ = (src >> 16) & 0xFF;
			*aDest++ = (src >>  8) & 0xFF;
			*aDest++ = (src      ) & 0xFF;
		}

		jpeg_write_scanlines(&cinfo, &aTempBuffer, 1);
	}

	delete [] aTempBuffer;

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(fp);

	return true;
}

bool ImageLib::WritePNGImage(const SexyString& theFileName, ImageLib::Image* theImage)
{
	png_structp png_ptr;
	png_infop info_ptr;

	FILE *fp;

	if ((fp = sexyfopen(theFileName.c_str(), _S("wb"))) == NULL)
		return false;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	  NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
		fclose(fp);
		return false;
	}

	                                                                   
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return false;
	}

                                                                            
                                                                          
                                                                              

	if (setjmp(png_ptr->jmpbuf))
	{
		                                                                  
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		                                                     
		return NULL;
	}

	png_init_io(png_ptr, fp);

	png_color_8 sig_bit;
	sig_bit.red = 8;
	sig_bit.green = 8;
	sig_bit.blue = 8;
	                                            
	sig_bit.alpha = 8;
	png_set_sBIT(png_ptr, info_ptr, &sig_bit);
	png_set_bgr(png_ptr);

	png_set_IHDR(png_ptr, info_ptr, theImage->mWidth, theImage->mHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA,
       PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	                                                             
	                          
	                                                  
	                                   
	                                  
	                               

	png_write_info(png_ptr, info_ptr);

	for (int i = 0; i < theImage->mHeight; i++)
	{
		png_bytep aRowPtr = (png_bytep) (theImage->mBits + i*theImage->mWidth);
		png_write_rows(png_ptr, &aRowPtr, 1);
	}

	                                                                       
	png_write_end(png_ptr, info_ptr);

	                                                                     
	png_destroy_write_struct(&png_ptr, &info_ptr);

	                 
	fclose(fp);

	return true;
}

bool ImageLib::WriteTGAImage(const SexyString& theFileName, ImageLib::Image* theImage)
{
	FILE* aTGAFile = sexyfopen(theFileName.c_str(), _S("wb"));
	if (aTGAFile == NULL)
		return false;

	BYTE aHeaderIDLen = 0;
	fwrite(&aHeaderIDLen, sizeof(BYTE), 1, aTGAFile);

	BYTE aColorMapType = 0;
	fwrite(&aColorMapType, sizeof(BYTE), 1, aTGAFile);
	
	BYTE anImageType = 2;
	fwrite(&anImageType, sizeof(BYTE), 1, aTGAFile);

	WORD aFirstEntryIdx = 0;
	fwrite(&aFirstEntryIdx, sizeof(WORD), 1, aTGAFile);

	WORD aColorMapLen = 0;
	fwrite(&aColorMapLen, sizeof(WORD), 1, aTGAFile);

	BYTE aColorMapEntrySize = 0;
	fwrite(&aColorMapEntrySize, sizeof(BYTE), 1, aTGAFile);	

	WORD anXOrigin = 0;
	fwrite(&anXOrigin, sizeof(WORD), 1, aTGAFile);

	WORD aYOrigin = 0;
	fwrite(&aYOrigin, sizeof(WORD), 1, aTGAFile);

	WORD anImageWidth = theImage->mWidth;
	fwrite(&anImageWidth, sizeof(WORD), 1, aTGAFile);	

	WORD anImageHeight = theImage->mHeight;
	fwrite(&anImageHeight, sizeof(WORD), 1, aTGAFile);	

	BYTE aBitCount = 32;
	fwrite(&aBitCount, sizeof(BYTE), 1, aTGAFile);	

	BYTE anImageDescriptor = 8 | (1<<5);
	fwrite(&anImageDescriptor, sizeof(BYTE), 1, aTGAFile);

	fwrite(theImage->mBits, 4, theImage->mWidth*theImage->mHeight, aTGAFile);

	fclose(aTGAFile);

	return true;
}

bool ImageLib::WriteBMPImage(const SexyString& theFileName, ImageLib::Image* theImage)
{
	FILE* aFile = sexyfopen(theFileName.c_str(), _S("wb"));
	if (aFile == NULL)
		return false;

	BITMAPFILEHEADER aFileHeader;
	BITMAPINFOHEADER aHeader;

	memset(&aFileHeader,0,sizeof(aFileHeader));
	memset(&aHeader,0,sizeof(aHeader));

	int aNumBytes = theImage->mWidth*theImage->mHeight*4;

	aFileHeader.bfType = ('M'<<8) | 'B';
	aFileHeader.bfSize = sizeof(aFileHeader) + sizeof(aHeader) + aNumBytes;
	aFileHeader.bfOffBits = sizeof(aHeader); 

	aHeader.biSize = sizeof(aHeader);
	aHeader.biWidth = theImage->mWidth;
	aHeader.biHeight = theImage->mHeight;
	aHeader.biPlanes = 1;
	aHeader.biBitCount = 32;
	aHeader.biCompression = BI_RGB;

	fwrite(&aFileHeader,sizeof(aFileHeader),1,aFile);
	fwrite(&aHeader,sizeof(aHeader),1,aFile);
	DWORD *aRow = theImage->mBits + (theImage->mHeight-1)*theImage->mWidth;
	int aRowSize = theImage->mWidth*4;
	for (int i=0; i<theImage->mHeight; i++, aRow-=theImage->mWidth)
		fwrite(aRow,4,theImage->mWidth,aFile);

	fclose(aFile);
	return true;
}

                                                                           
                  

typedef struct {
	struct jpeg_source_mgr pub;	                   

	PFILE * infile;		                   
	JOCTET * buffer;		                     
	boolean start_of_file;	                                  
} pak_source_mgr;

typedef pak_source_mgr * pak_src_ptr;

#define INPUT_BUF_SIZE 4096

METHODDEF(void) init_source (j_decompress_ptr cinfo)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;
	src->start_of_file = TRUE;
}

METHODDEF(boolean) fill_input_buffer (j_decompress_ptr cinfo)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;
	size_t nbytes;

	nbytes = p_fread(src->buffer, 1, INPUT_BUF_SIZE, src->infile);
	                                                                            

	if (nbytes <= 0) {
		if (src->start_of_file)	                                           
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		                              
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

METHODDEF(void) skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) fill_input_buffer(cinfo);
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

METHODDEF(void) term_source (j_decompress_ptr cinfo)
{
	                            
}

void jpeg_pak_src (j_decompress_ptr cinfo, PFILE* infile)
{
	pak_src_ptr src;

	                                                                         
                                                                          
                                                                        
                                                          
                                                                  
                                                                  
   
	if (cinfo->src == NULL) {	                                      
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof(pak_source_mgr));
		src = (pak_src_ptr) cinfo->src;
		src->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			INPUT_BUF_SIZE * sizeof(JOCTET));
	}

	src = (pak_src_ptr) cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart;                         
	src->pub.term_source = term_source;
	src->infile = infile;
	src->pub.bytes_in_buffer = 0;                                             
	src->pub.next_input_byte = NULL;                          
}


ImageLib::Image* GetJPEGImage(const SexyString& theFileName)
{
	PFILE *fp;

	if ((fp = p_fopen(theFileName.c_str(), _S("rb"))) == NULL)
		return NULL;

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		                                                       
                                                                           
     
		jpeg_destroy_decompress(&cinfo);
		p_fclose(fp);
		return 0;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_pak_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	int row_stride = cinfo.output_width * cinfo.output_components;

	unsigned char** buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	unsigned long* aBits = new unsigned long[cinfo.output_width*cinfo.output_height];
	unsigned long* q = aBits;

	if (cinfo.output_components==1)
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);

			unsigned char* p = *buffer;
			for (int i = 0; i < cinfo.output_width; i++)
			{
				int r = *p++;
				*q++ = 0xFF000000 | (r << 16) | (r << 8) | (r);
			}
		}
	}
	else
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);

			unsigned char* p = *buffer;
			for (int i = 0; i < cinfo.output_width; i++)
			{
				int r = *p++;
				int g = *p++;
				int b = *p++;

				*q++ = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}
		}
	}

	ImageLib::Image* anImage = new ImageLib::Image();
	anImage->mWidth = cinfo.output_width;
	anImage->mHeight = cinfo.output_height;
	anImage->mBits = aBits;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	p_fclose(fp);

	return anImage;
}

#if 0
ImageLib::Image* GetJPEG2000Image(const std::string& theFileName)
{
	DWORD aTick = GetTickCount();

	static bool inited = false;
	if (!inited)
	{
		inited = true;
		jas_init();
	}

	jas_stream_t* aStream = jas_stream_fopen(theFileName.c_str(),"rb");
	if (!aStream)
		return NULL;

	                              
	int aFormat = jas_image_getfmt(aStream);
	if (aFormat < 0)
	{
		jas_stream_close(aStream);
		return NULL;
	}

	jas_image_t* aJasImage = jas_image_decode(aStream,aFormat,NULL);
	if (!aJasImage)
	{
		jas_stream_close(aStream);
		return NULL;
	}

	DWORD aDecodeTime = GetTickCount() - aTick;

	int aNumComponents = jas_image_numcmpts(aJasImage);
	if (aNumComponents<1 || aNumComponents>4)
	{
		jas_stream_close(aStream);
		jas_image_destroy(aJasImage);
		return NULL;
	}

	int i;
	int aMaxWidth = 0;
	int aMaxHeight = 0;
	for (i=0; i<aNumComponents; i++)
	{
		int hstep = jas_image_cmpthstep(aJasImage,i);
		int numHSteps = jas_image_cmptwidth(aJasImage,i);

		int vstep = jas_image_cmptvstep(aJasImage,i);
		int numVSteps = jas_image_cmptheight(aJasImage,i);

		int aWidth = jas_image_cmpttlx(aJasImage,i) + hstep*numHSteps;
		int aHeight = jas_image_cmpttly(aJasImage,i) + vstep*numVSteps;

		if (aWidth > aMaxWidth)
			aMaxWidth = aWidth;

		if (aHeight > aMaxHeight)
			aMaxHeight = aHeight;
	}

	                            
	ImageLib::Image *anImage = new ImageLib::Image;
	anImage->mWidth = aMaxWidth;
	anImage->mHeight = aMaxHeight;
	anImage->mBits = new unsigned long[aMaxWidth * aMaxHeight];
	memset(anImage->mBits,0,aMaxWidth * aMaxHeight*4);

	int aColorModel = jas_image_clrspc(aJasImage);

	for (i=0; i<aNumComponents; i++)
	{
		int hstep = jas_image_cmpthstep(aJasImage,i);
		int vstep = jas_image_cmptvstep(aJasImage,i);
		int numHSteps = jas_image_cmptwidth(aJasImage,i);
		int numVSteps = jas_image_cmptheight(aJasImage,i);
		int xorig = jas_image_cmpttlx(aJasImage,i);
		int yorig = jas_image_cmpttly(aJasImage,i);
		bool sign = jas_image_cmptsgnd(aJasImage,i)?true:false;

		jas_matrix_t* aMatrix = jas_matrix_create(1,numHSteps);
		if (!aMatrix)
		{
			delete anImage;
			jas_image_destroy(aJasImage);
			jas_stream_close(aStream);

			return NULL;
		}

		int aShift = 8 - jas_image_cmptprec(aJasImage,i);
		if (aShift<0)
		{
			delete anImage;
			jas_matrix_destroy(aMatrix);
			jas_image_destroy(aJasImage);
			jas_stream_close(aStream);

			return NULL;
		}

		unsigned long* destRow = anImage->mBits + yorig*numVSteps*anImage->GetWidth() + xorig*numHSteps;

		              
		int aComponentType = jas_image_cmpttype(aJasImage,i);
		int aColorType = JAS_IMAGE_CT_COLOR(aComponentType);

		switch (aColorType)
		{
			case JAS_IMAGE_CT_RGB_R: aShift += 16; break;
			case JAS_IMAGE_CT_RGB_G: aShift += 8; break;
			case JAS_IMAGE_CT_RGB_B: break;
			default: aShift += 24; break; 
		}

		for (int y=0; y<numVSteps; y++)
		{
			if (jas_image_readcmpt(aJasImage,i,0,y,numHSteps,1,aMatrix) )
			{
				delete anImage;
				jas_matrix_destroy(aMatrix);
				jas_image_destroy(aJasImage);
				jas_stream_close(aStream);

				return NULL;			
			}

			unsigned long* dest = destRow;
			for (int x=0; x<numHSteps; x++)
			{
				int aVal = jas_matrix_getv(aMatrix,x);
				if (sign)
					aVal = (unsigned char)(aVal + 128);

				aVal <<= aShift;

				unsigned long *destRowWriter = dest;
				for (int j=0; j<vstep; j++)
				{
					unsigned long *destWriter = destRowWriter;
					for (int k=0; k<hstep; k++)
						*destWriter++ |= aVal;

					destRowWriter += anImage->GetWidth();
				}

				dest += hstep;
			}
			destRow += vstep*anImage->GetWidth();
		}

		                          
		jas_matrix_destroy(aMatrix);
	}

	DWORD aReadTime = GetTickCount() - aTick;
	char aBuf[512];
	sprintf(aBuf,"%d %d\n",aDecodeTime,aReadTime);
	OutputDebugString(aBuf);


	if (aNumComponents < 4)                 
	{
		int aSize = anImage->GetWidth()*anImage->GetHeight();
		unsigned long *dest = anImage->mBits;
		for (i=0; i<aSize; i++)
			*dest++ |= 0xff000000;
	}

	jas_image_destroy(aJasImage);
	jas_stream_close(aStream);

	return anImage;
}
#else

#include "j2k-codec/j2k-codec.h"

HMODULE gJ2KCodec = NULL;
SexyString gJ2KCodecKey = _S("Your registration here");

void ImageLib::InitJPEG2000()
{
	gJ2KCodec = ::LoadLibrary(_T(_S("j2k-codec.dll")));
}

void ImageLib::CloseJPEG2000()
{
	if (gJ2KCodec != NULL)
	{
		::FreeLibrary(gJ2KCodec);
		gJ2KCodec = NULL;
	}
}

void ImageLib::SetJ2KCodecKey(const SexyString& theKey)
{
	gJ2KCodecKey = theKey;
}

int __stdcall Pak_seek(void *data_source, int offset)
{
	return p_fseek((PFILE*) data_source, offset, SEEK_SET);
}

int __stdcall Pak_read(void *ptr, int size, void *data_source)
{
	return p_fread(ptr, 1, size, (PFILE*) data_source);
}

void __stdcall Pak_close(void *data_source)
{	
}

ImageLib::Image* GetJPEG2000Image(const SexyString& theFileName)
{
	if (gJ2KCodec != NULL)
	{
		PFILE* aFP = p_fopen(theFileName.c_str(), _S("rb"));
		if (aFP == NULL)
			return NULL;

		static int (__stdcall *fJ2K_getVersion)() = NULL;
		static void (__stdcall *fJ2K_Unlock)(const SexyChar*) = NULL;
		static void* (__stdcall *fJ2K_OpenCustom)(void*, J2K_Callbacks*) = NULL;
		static void* (__stdcall *fJ2K_OpenFile)(const SexyChar*) = NULL;
		static void (__stdcall *fJ2K_Close)(void*) = NULL;
		static int (__stdcall *fJ2K_GetInfo)(void*, int*, int*, int*) = NULL;
		static int (__stdcall *fJ2K_GetResolutionDimensions)(void*, int, int*, int*) = NULL;
		static int (__stdcall *fJ2K_Decode)(void*, unsigned char**, int*, SexyChar*, int*) = NULL;
		static int (__stdcall *fJ2K_getLastError)() = NULL;
		static const SexyChar* (__stdcall *fJ2K_getErrorStr)(int) = NULL;
		static bool loadFuncs = true;

		if (loadFuncs)
		{
			loadFuncs = false;
			*((void**)&fJ2K_getVersion) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getVersion@0");
			*((void**)&fJ2K_Unlock) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Unlock@4");
			*((void**)&fJ2K_OpenCustom) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_OpenCustom@8");
			*((void**)&fJ2K_OpenFile) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_OpenFile@4");
			*((void**)&fJ2K_Close) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Close@4");
			*((void**)&fJ2K_GetInfo) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_GetInfo@16");
			*((void**)&fJ2K_GetResolutionDimensions) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_GetResolutionDimensions@16");
			*((void**)&fJ2K_Decode) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Decode@20");
			*((void**)&fJ2K_getLastError) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getLastError@0");
			*((void**)&fJ2K_getErrorStr) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getErrorStr@4");

			                                                                                                                         

			if (!(fJ2K_getVersion != NULL && 
				  fJ2K_Unlock != NULL && 
				  fJ2K_OpenCustom != NULL && 
				  fJ2K_OpenFile != NULL && 
				  fJ2K_Close != NULL && 
				  fJ2K_GetInfo != NULL && 
				  fJ2K_GetResolutionDimensions != NULL && 
				  fJ2K_Decode != NULL &&
				  fJ2K_getLastError != NULL &&
				  fJ2K_getErrorStr != NULL))
			{
				ImageLib::CloseJPEG2000();
				return NULL;
			}

			int aJ2kVer = (*fJ2K_getVersion)();
			if (aJ2kVer < 0x120000) 
			{
				ImageLib::CloseJPEG2000();
				return NULL;
			}

			(*fJ2K_Unlock)(gJ2KCodecKey.c_str()); 
		}

		J2K_Callbacks aCallbacks;
		aCallbacks.read = Pak_read;
		aCallbacks.seek = Pak_seek;
		aCallbacks.close = Pak_close;

		                     
		void* aJ2KImage = (*fJ2K_OpenCustom)(aFP, &aCallbacks);
		if (aJ2KImage == NULL)
		{
			int anErrNum = (*fJ2K_getLastError)();
			SexyString anErrorMessage = (*fJ2K_getErrorStr)(anErrNum);
			return NULL;
		}

		int aWidth, aHeight, aComponents;
		if ((*fJ2K_GetInfo)(aJ2KImage, &aWidth, &aHeight, &aComponents) != J2KERR_SUCCESS) 
		{
			(*fJ2K_Close)(aJ2KImage);
			return NULL;
		}

		(*fJ2K_GetResolutionDimensions)(aJ2KImage, 0, &aWidth, &aHeight);
		
		unsigned long* aBuffer = new unsigned long[aWidth*aHeight];
		if (aBuffer == NULL)
		{
			(*fJ2K_Close)(aJ2KImage);
			return NULL;
		}

		SexyChar anOptsBuffer[32];
		sexystrcpy(anOptsBuffer, _S("bpp=4,rl=0"));

		int aSize = aWidth*aHeight*4;
		int aPitch = aWidth*4;
		if ((*fJ2K_Decode)(aJ2KImage, (unsigned char**)&aBuffer, &aSize, anOptsBuffer, &aPitch) != J2KERR_SUCCESS)
		{
			(*fJ2K_Close)(aJ2KImage);
			delete[] aBuffer;
			return NULL;
		}
		(*fJ2K_Close)(aJ2KImage);

		ImageLib::Image* anImage = new ImageLib::Image;
		anImage->mBits = aBuffer;
		anImage->mWidth = aWidth;
		anImage->mHeight = aHeight;
		if (ImageLib::gIgnoreJPEG2000Alpha)
		{
			DWORD *aPtr = anImage->mBits;
			DWORD *anEnd = aPtr+anImage->mWidth*anImage->mHeight;
			for (; aPtr!=anEnd; ++aPtr)
				*aPtr |= 0xFF000000;
		}

		p_fclose(aFP);

		return anImage;
	}
	return NULL;
}

#endif


int ImageLib::gAlphaComposeColor = 0xFFFFFF;
bool ImageLib::gAutoLoadAlpha = true;
bool ImageLib::gIgnoreJPEG2000Alpha = true;

ImageLib::Image* ImageLib::GetImage(const SexyString& theFilename, bool lookForAlphaImage)
{
	if (!gAutoLoadAlpha)
		lookForAlphaImage = false;

	int aLastDotPos = theFilename.rfind(_S('.'));
	int aLastSlashPos = max((int)theFilename.rfind(_S('\\')), (int)theFilename.rfind(_S('/')));

	SexyString anExt;
	SexyString aFilename;

	                                  
  
                                                                              
                                                 
  
       
	aFilename = theFilename;

	ImageLib::Image* anImage = NULL;

	if ((anImage == NULL) && ((sexystricmp(anExt.c_str(), _S(".png")) == 0) || (anExt.length() == 0)))
		anImage = GetPNGImage(aFilename + _S(".png"));

	if ((anImage == NULL) && ((sexystricmp(anExt.c_str(), _S(".tga")) == 0) || (anExt.length() == 0)))
		 anImage = GetTGAImage(aFilename + _S(".tga"));

	if ((anImage == NULL) && ((sexystricmp(anExt.c_str(), _S(".jpg")) == 0) || (anExt.length() == 0)))
		 anImage = GetJPEGImage(aFilename + _S(".jpg"));

	if ((anImage == NULL) && ((sexystricmp(anExt.c_str(), _S(".gif")) == 0) || (anExt.length() == 0)))
		 anImage = GetGIFImage(aFilename + _S(".gif"));

	if (anImage == NULL && (sexystricmp(anExt.c_str(), _S(".j2k")) == 0 || anExt.length() == 0))
		anImage = GetJPEG2000Image(aFilename + _S(".j2k"));
	if (anImage == NULL && (sexystricmp(anExt.c_str(), _S(".jp2")) == 0 || anExt.length() == 0))
		anImage = GetJPEG2000Image(aFilename + _S(".jp2"));


	                         
	ImageLib::Image* anAlphaImage = NULL;
	if(lookForAlphaImage)
	{
		                   
		anAlphaImage = GetImage(theFilename.substr(0, aLastSlashPos+1) + _S("_") +
			theFilename.substr(aLastSlashPos+1, theFilename.length() - aLastSlashPos - 1), false);

		                   
		if(anAlphaImage==NULL)
			anAlphaImage = GetImage(theFilename + _S("_"), false);
	}



	                                   
	if (anAlphaImage != NULL) 
	{
		if (anImage != NULL)
		{
			if ((anImage->mWidth == anAlphaImage->mWidth) &&
				(anImage->mHeight == anAlphaImage->mHeight))
			{
				                                         
				                                              
				                                               

				                                 
     
                                                                 
              
              
       

				for (int y = 0; y < anImage->mHeight; y++)
				{
					unsigned long* dest = anImage->mBits + y * anImage->mWidth;
					unsigned long* src = anAlphaImage->mBits + y * anAlphaImage->mWidth;

					for (int x = 0; x < anImage->mWidth; x++)
					{
						dest[x] = (dest[x] & 0x00FFFFFF) | ((src[x] & 0xFF) << 24);
					}
				}

			}

			delete anAlphaImage;
		}
		else if (gAlphaComposeColor==0xFFFFFF)
		{
			anImage = anAlphaImage;

			                                         

                                                
                                  
    
                                                      
             
      

			for (int y = 0; y < anImage->mHeight; y++)
			{
				unsigned long* dest = anImage->mBits + y * anImage->mWidth;

				for (int x = 0; x < anImage->mWidth; x++)
				{
					dest[x] = (0xFFFFFF) | ((dest[x] & 0xFF) << 24);
				}
			}
		}
		else
		{
			const int aColor = gAlphaComposeColor;
			anImage = anAlphaImage;

			                                         

                                                
                                  
    
                                                
             
      

			for (int y = 0; y < anImage->mHeight; y++)
			{
				unsigned long* dest = anImage->mBits + y * anImage->mWidth;

				for (int x = 0; x < anImage->mWidth; x++)
				{
					dest[x] = aColor | ((dest[x] & 0xFF) << 24);
				}
			}
		}
	}

	return anImage;
}
