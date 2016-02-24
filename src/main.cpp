#include <stdio.h>
#include <fetchimgfromfile.h>
#include <macro.h>

int main( int argc, char** argv){

#if 1
//	o.fetch( "../data/a.png", "../out" );
	tAssertcm( argc > 2, return -1, "need cmd: file_name out_dir" );
	tLogDebug( "%s %s", argv[1], argv[2] );

	FetchImgFromFile o;
	char* src_file = NULL;
	char* out_dir = NULL;
	src_file = argv[1];
	out_dir = argv[2];
	o.fetch( src_file, out_dir );
#else
	FetchImgFromFile o;
	o.fetch( "/media/数据/chrome.DMP", "../out" );
#endif
	return 0;
}
