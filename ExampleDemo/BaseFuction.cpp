
#include "BaseFuction.h"


/*调用示例：
vector<string>filepathname;
ScanDirectory(filesrc,".txt",filepathname);//filesrc是文件夹名字
*/

bool   CBaseFuction::ScanDirectory(const string &src_dir_path, const const string &file_extension, vector<string>& src_files)
{
	if (-1 == _access(src_dir_path.c_str(), 0))
	{
		return false;
	}

	_finddata_t fileInfo;
	intptr_t hFile;

	string dirName_filter = src_dir_path + string("\\") + string("*.*");

	if ((hFile = _findfirst(dirName_filter.c_str(), &fileInfo)) != -1)
	{
		do
		{

			if (!(fileInfo.attrib & _A_SUBDIR))
			{

				if (file_extension == string(".*") || getFileType(fileInfo.name) == file_extension)
				{
					string filename = string(fileInfo.name);
					src_files.push_back(filename);


				}
			}

		} while (_findnext(hFile, &fileInfo) == 0);
		_findclose(hFile);
	}
	return true;
}
const string CBaseFuction::getFileType(const string &filename)
{

	string::size_type pos = filename.find_last_of('.');

	if (pos != string::npos)
	{

		return string(filename, pos, string::npos);
	}
	else
	{
		// return null string  
		return "";
	}

}
//输入原图片指针的指针，及新图片的新宽高
//示例：ResizeBitmap(&m_pOrignImageS,width,height);
BOOL CBaseFuction::ResizeBitmap(Bitmap **ppImg, int m_NewWidth, int m_NewHeight)
{
	if (ppImg == NULL || *ppImg == NULL)
	{
		return FALSE;
	}

	Bitmap *m_NewImage = new Bitmap(m_NewWidth, m_NewHeight);

	ColorMatrix colorMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	ImageAttributes imageAtt;
	imageAtt.SetColorMatrix(&colorMatrix);
	imageAtt.SetWrapMode(WrapModeTileFlipXY);

	Graphics gs(m_NewImage);
	gs.SetSmoothingMode(SmoothingModeAntiAlias);
	gs.SetInterpolationMode(InterpolationModeHighQuality);
	gs.DrawImage(
		(*ppImg),
		Rect(0, 0, m_NewWidth, m_NewHeight),  // Destination rectangle
		0,								   // Source rectangle temp->x 
		0,								   // Source rectangle temp->y
		(*ppImg)->GetWidth(),     // Source rectangle width
		(*ppImg)->GetHeight(),    // Source rectangle height
		UnitPixel,
		&imageAtt);

	//交换指针
	delete (*ppImg);
	(*ppImg) = m_NewImage;
	return TRUE;
}

