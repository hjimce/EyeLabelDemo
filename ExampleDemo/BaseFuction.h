#pragma once
#include "stdafx.h"
#include <string>  
#include  <io.h>  
#include <vector>
using namespace std;


class CBaseFuction
{
public:
	static bool ScanDirectory(const string &src_dir_path, const const string &file_extension, vector<string>& src_files);
	static const string getFileType(const string &filename);
	static BOOL ResizeBitmap(Bitmap **ppImg, int m_NewWidth, int m_NewHeight);

};

