#ifndef FETCHIMGFROMFILE_H
#define FETCHIMGFROMFILE_H

#include <stdio.h>

class FetchImgFromFile{
public:
	FetchImgFromFile();
	bool fetch( const char* src, const char* outputdir );
private:
	size_t fetchImg( FILE* fp, const size_t start, const size_t size );

private:
	size_t fetchPNG( FILE* fp, const size_t start, const size_t size );
	bool isPNGHeader( FILE* fp );
	bool isPNGEndian( FILE* fp );
private:
	size_t fetchJPG( FILE* fp, const size_t start, const size_t size );
	bool isJPGHeader( FILE* fp );
	bool isJPGEndian( FILE* fp );

private:
	size_t fetchSWF( FILE* fp, const size_t start, const size_t size );
	size_t isSWFHeader( FILE* fp );
	
private:
	size_t fetchImgByStartEndTag( FILE* fp, const size_t start, const size_t filesize, 
			const char* starttag, const size_t startsize, 
			const char* endtag, const size_t endsize, const char* filefmg, size_t& fetchcount );
private:
	FILE* src_fp;
	char* src_txt;
	char output_dir[256];
	size_t cur_checked_size;
	size_t png_count;
	size_t jpg_count;
	size_t tif_count;
	size_t swf_count;
};

#endif//FETCHIMGFROMFILE_H
