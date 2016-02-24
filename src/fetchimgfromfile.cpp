#include <fetchimgfromfile.h>
#include <stdio.h>
#include <macro.h>

static char s_png_header[] = {	0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,
								0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52 };
static char s_png_endian[] = { 	0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82 };

static const size_t s_png_header_size = sizeof(s_png_header)/sizeof(s_png_header[0]);
static const size_t s_png_endian_size = sizeof(s_png_endian)/sizeof(s_png_endian[0]);

static char s_jpg_header[] = { 	0xff,0xd8,0xff,0xe0,0x00,0x10,0x4a,0x46 };
//								0x49,0x46,0x00,0x01,0x01,0x01,0x00,0x48,
//								0x00,0x48 };
static char s_jpg_endian[] = { 	0xff, 0xd9 };

static const size_t s_jpg_header_size = sizeof(s_jpg_header)/sizeof(s_jpg_header[0]);
static const size_t s_jpg_endian_size = sizeof(s_jpg_endian)/sizeof(s_jpg_endian[0]);

static char s_tif_header[] = { 0x49,0x49,0x2a,0x00 };
static char s_tif_endian[] = { 0xff,0xd9 };

static const size_t s_tif_header_size = sizeof(s_tif_header)/sizeof(s_tif_header[0]);
static const size_t s_tif_endian_size = sizeof(s_tif_endian)/sizeof(s_tif_endian[0]);

FetchImgFromFile::FetchImgFromFile(){
	src_fp = NULL;
	cur_checked_size = 0;
	src_txt = NULL;
	png_count = 0;
	jpg_count = 0;
	tif_count = 0;
	swf_count = 0;
	memset( output_dir, 0x00, sizeof(output_dir) );
}

bool FetchImgFromFile::fetch( const char* src, const char* outputdir ){
	tAssertc( src, return false );
	tAssertc( outputdir, return false );

	strcpy( output_dir, outputdir );

	cur_checked_size = 0;
	png_count = 0;
	jpg_count = 0;
	tif_count = 0;
	swf_count = 0;

	if( src_fp ){
		fclose( src_fp );
		src_fp = NULL;
	}
	src_fp = fopen( src, "rb" );
	tAssertcm( src_fp, return false, "file %s", src );

	fseek( src_fp, 0, SEEK_END );
	size_t file_size = ftell(src_fp);
	tLogSystem( "file %s size %lu", src, file_size );
	fseek( src_fp, 0, SEEK_SET );

	for( cur_checked_size = 0; cur_checked_size < file_size; ){
		fseek( src_fp, cur_checked_size, SEEK_SET );

		size_t size = file_size - cur_checked_size;
		size_t img_size = fetchImg( src_fp, cur_checked_size, size );
		if( img_size > 0 ){
			cur_checked_size += img_size;
		}else{
			++cur_checked_size;
			//tLogDebug( "checked size %010lu %010lu", cur_checked_size, size );
		}
	}

	fclose( src_fp );
	src_fp = NULL;
	return true;
}


size_t FetchImgFromFile::fetchImg( FILE* fp, const size_t start, const size_t size ){
	tAssertc( fp, return 0 );

#if 0
	size_t img_size = fetchPNG( fp, start, size );
	if( img_size ) return img_size;

	fseek( fp, start, SEEK_SET );

	img_size = fetchJPG( fp, start, size );
	if( img_size ) return img_size;
#else
	size_t img_size = fetchImgByStartEndTag( fp, start, size, 
			s_png_header, s_png_header_size,
			s_png_endian, s_png_endian_size, "png", png_count );
	if( img_size ) return img_size;

	fseek( fp, start, SEEK_SET );
	img_size = fetchImgByStartEndTag( fp, start, size, 
			s_jpg_header, s_jpg_header_size,
			s_jpg_endian, s_jpg_endian_size, "jpg", jpg_count );
	if( img_size ) return img_size;
/*
	fseek( fp, start, SEEK_SET );
	img_size = fetchImgByStartEndTag( fp, start, size, 
			s_tif_header, s_tif_header_size,
			s_tif_endian, s_tif_endian_size, "tiff", tif_count );
	if( img_size ) return img_size;
*/

	fseek( fp, start, SEEK_SET );
	img_size = fetchSWF( fp, start, size );
	if( img_size ) return img_size;
#endif

	return img_size;
}

size_t FetchImgFromFile::fetchPNG( FILE* fp, const size_t start, const size_t size ){
	tAssertc( fp, return 0 );
	if( size < s_png_header_size ) return 0;
	if( !isPNGHeader( fp ) ) return 0;

	size_t png_size = 0;
	size_t size2 = size - s_png_header_size;
	for( size_t i=0; i < size2; ++i ){
		size_t rare_size = size2 - i;
		if( rare_size < s_png_endian_size ) {
			return 0;
		}
		size_t cur_pos = start + s_png_header_size + i;
		fseek( fp, cur_pos, SEEK_SET );

		if( isPNGEndian( fp ) ){
			png_size = i + s_png_header_size + s_png_endian_size;

			//seek to start
			fseek( fp, start, SEEK_SET );
			char* buff = new char[png_size];
			fread( buff, png_size, 1, fp );

			char file_name[256]; memset(file_name, 0x00, sizeof(file_name) );
			sprintf( file_name, "%s/%05lu.png", output_dir, png_count );
			tLogSystem( "save %s %lu => %lu, size %lu", file_name, cur_pos, cur_pos+png_size, png_size );
			FILE* fp_png = fopen( file_name, "wb" );
			//write png_size
			tAssertm( fwrite( buff, png_size, 1, fp_png ) > 0, "save png %s", file_name );
			fclose( fp_png );
			fp_png = NULL;
			delete [] buff;
			buff = NULL;

			png_count++;
			break;
		}
	}

	return png_size;
}

bool FetchImgFromFile::isPNGHeader( FILE* fp ){
	tAssertc( fp, return false );

	char buff[256]; memset(buff, 0x00, sizeof(buff) );
	fread( buff, s_png_header_size, 1, fp );
	if( 0 == memcmp( buff, s_png_header, s_png_header_size ) ){
		return true;
	}

	return false;
}

bool FetchImgFromFile::isPNGEndian( FILE* fp ){
	tAssertc( fp, return false );

	char buff[256]; memset(buff, 0x00, sizeof(buff) );
	fread( buff, s_png_endian_size, 1, fp );
	if( 0 == memcmp( buff, s_png_endian, s_png_endian_size ) ){
		return true;
	}

	return false;
}

size_t FetchImgFromFile::fetchJPG( FILE* fp, const size_t start, const size_t size ){
	tAssertc( fp, return 0 );
	if( size < s_jpg_header_size ) return 0;
	if( !isJPGHeader( fp ) ) return 0;

	size_t jpg_size = 0;
	size_t size2 = size - s_jpg_header_size;
	for( size_t i=0; i < size2; ++i ){
		size_t rare_size = size2 - i;
		if( rare_size < s_jpg_endian_size ) {
			return 0;
		}
		size_t cur_pos = start + s_jpg_header_size + i;
		fseek( fp, cur_pos, SEEK_SET );

		if( isJPGEndian( fp ) ){
			jpg_size = i + s_jpg_header_size + s_jpg_endian_size;

			//seek to start
			fseek( fp, start, SEEK_SET );
			char* buff = new char[jpg_size];
			fread( buff, jpg_size, 1, fp );

			char file_name[256]; memset(file_name, 0x00, sizeof(file_name) );
			sprintf( file_name, "%s/%05lu.jpg", output_dir, jpg_count );
			tLogSystem( "save %s %lu => %lu, size %lu", file_name, cur_pos, cur_pos+jpg_size, jpg_size );
			FILE* fp_jpg = fopen( file_name, "wb" );
			//write jpg_size
			tAssertm( fwrite( buff, jpg_size, 1, fp_jpg ) > 0, "save jpg %s", file_name );
			fclose( fp_jpg );
			fp_jpg = NULL;
			delete [] buff;
			buff = NULL;

			jpg_count++;
			break;
		}
	}

	return jpg_size;
}

bool FetchImgFromFile::isJPGHeader( FILE* fp ){
	tAssertc( fp, return false );

	char buff[256]; memset(buff, 0x00, sizeof(buff) );
	fread( buff, s_jpg_header_size, 1, fp );
	if( 0 == memcmp( buff, s_jpg_header, s_jpg_header_size ) ){
		return true;
	}
	return false;
}

bool FetchImgFromFile::isJPGEndian( FILE* fp ){
	tAssertc( fp, return false );

	char buff[256]; memset(buff, 0x00, sizeof(buff) );
	fread( buff, s_jpg_endian_size, 1, fp );
	if( 0 == memcmp( buff, s_jpg_endian, s_jpg_endian_size ) ){
		return true;
	}

	return false;
}

size_t FetchImgFromFile::fetchImgByStartEndTag( 
							FILE* fp, const size_t start, const size_t filesize, 
							const char* starttag, const size_t startsize, 
							const char* endtag, const size_t endsize, 
							const char* filefmt, size_t& fetchcount ){
	tAssertc( fp, return 0 );
	tAssertc( starttag, return 0 );
	tAssertc( endtag, return 0 );
	tAssertc( filefmt, return 0 );

	if( filesize < startsize ) return 0;
	char buff_header[256]; memset(buff_header, 0x00, sizeof(buff_header) );
	fread( buff_header, startsize, 1, fp );
	if( 0 != memcmp( buff_header, starttag, startsize ) ){
		return 0;
	}

	size_t img_size = 0;
	size_t size2 = filesize - startsize;
	for( size_t i=0; i < size2; ++i ){
		size_t rare_size = size2 - i;
		if( rare_size < endsize ) {
			return 0;
		}
		size_t cur_pos = start + startsize + i;
		fseek( fp, cur_pos, SEEK_SET );

		//check endian
		char buff_endian[256]; memset(buff_endian, 0x00, sizeof(buff_endian) );
		fread( buff_endian, endsize, 1, fp );
		if( 0 == memcmp( buff_endian, endtag, endsize ) ){

			img_size = i + startsize + endsize;

			//seek to start
			fseek( fp, start, SEEK_SET );
			char* buff = new char[img_size];
			fread( buff, img_size, 1, fp );

			char file_name[256]; memset(file_name, 0x00, sizeof(file_name) );
			sprintf( file_name, "%s/%05ld.%s", output_dir, fetchcount, filefmt );
			tLogSystem( "save %s %lu => %lu, size %lu", file_name, cur_pos, cur_pos+img_size, img_size );
			FILE* fp_img = fopen( file_name, "wb" );
			//write img_size
			tAssertm( fwrite( buff, img_size, 1, fp_img ) > 0, "save img %s", file_name );
			fclose( fp_img );
			fp_img = NULL;
			delete [] buff;
			buff = NULL;

			fetchcount++;
			break;
		}
	}

	return img_size;
}

size_t FetchImgFromFile::fetchSWF( FILE* fp, const size_t start, const size_t size ){
	size_t swf_length = isSWFHeader( fp );
	if( swf_length == 0 ) return 0;
	if( size < swf_length ) return 0;

	char* buff = new char[swf_length];
	fread( buff, swf_length, 1, fp );

	char file_name[256]; memset(file_name, 0x00, sizeof(file_name) );
	sprintf( file_name, "%s/%05lu.swf", output_dir, swf_count );
	tLogDebug( "save %s %lu => %lu, size %lu", file_name, start, start+swf_length, swf_length );
	FILE* fp_swf = fopen( file_name, "wb" );
	//write png_size
	tAssertm( fwrite( buff, swf_length, 1, fp_swf ) > 0, "save %s", file_name );
	fclose( fp_swf );
	fp_swf = NULL;
	delete [] buff;
	buff = NULL;

	swf_count++;
	return swf_length;
}

struct SWFHeader {
	unsigned char file_type[3];
	unsigned char version;
	unsigned int file_length;
	//	SWFRect frame_rect;
	//	uint16 frame_rate;
	//	uint16 frame_count;
};

size_t FetchImgFromFile::isSWFHeader( FILE* fp ){

	SWFHeader header;
	size_t fpos = ftell( fp );
	fread( &header, sizeof(header), 1, fp );

	size_t length = 0;
	const int swf = 0x465753;
	const char* p = (const char*)&swf;
	bool is_swf = (	header.file_type[2] == p[0] 
					&& header.file_type[1] == p[1] 
					&& header.file_type[0] == p[2] );
	if( is_swf){
		if( header.version < 20 ){
			length = header.file_length;
		}
	}

	fseek( fp, fpos, SEEK_SET );
	return length;
}
