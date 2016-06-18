
// ExampleDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExampleDemo.h"
#include "ExampleDemoDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <cmath>
#include "facepp/faceppapi.h"
#define IDC_NEXT_BUTTON 10000
#define IDC_GROUP_BUTTON 8000

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExampleDemoDlg dialog

CExampleDemoDlg::CExampleDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExampleDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pOrignImage = NULL;
	m_pEffectImage = NULL;

	m_bShowEffect = false;
	m_rtShowRect.X = m_rtShowRect.Y = m_rtShowRect.Width = m_rtShowRect.Height = 0;
	m_fZoom = 1.0f;
	m_imagepercent = 0.4;
	m_templatepercent = 0.1;
	m_classifyid = -1;
	facepp_init();
}

CExampleDemoDlg::~CExampleDemoDlg()
{
	SAFE_DELETE(m_pOrignImage);
	SAFE_DELETE(m_pEffectImage);
}

void CExampleDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CExampleDemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_OPEN, &CExampleDemoDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, &CExampleDemoDlg::OnBnClickedBtnNext)

END_MESSAGE_MAP()


// CExampleDemoDlg message handlers

BOOL CExampleDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// 设置窗口为屏幕宽高
	MoveWindow(0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
	SetDlgItemInt(ID_MSG, 0);
	m_tempplate=LoadTemplateImg("template");



//1、动态创建进入下一张图片按钮
	CRect rcBtn(20, 0, 120, 15);   //按钮高度为15，间隔高度为10  
	DWORD dwStyle = WS_CHILD | BS_PUSHBUTTON;
	m_nextimageb = new CButton();
	m_nextimageb->Create("下一张", dwStyle, rcBtn, this, IDC_NEXT_BUTTON);

//2、动态创建单选框按钮
	
	for (int i = 0; i < m_tempplate.size(); i++)
	{
		DWORD dwStyle = WS_CHILD | BS_AUTORADIOBUTTON;
		rcBtn.top += 25;
		rcBtn.bottom += 25;
		CButton *pButton = new CButton();
		if (0 == i)
		{
			dwStyle |= WS_GROUP;    //第一个按钮时添加分组风格  
		}
		CString index;
		index.Format("%d", i);
		pButton->Create(index, dwStyle, rcBtn, this, IDC_GROUP_BUTTON + i);

		pButton->SetFont(GetFont()); //设置为父窗口的字体  
		m_radioList.push_back(pButton);
		

	}







	::SetWindowPos(this->m_hWnd, HWND_BOTTOM, 200, 0, 1400, 1000, SWP_NOZORDER);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
vector<Bitmap*> CExampleDemoDlg::LoadTemplateImg(string filesrc)
{
	vector<Bitmap*> tempplate;
	vector<string>filepathname;
	ScanDirectory(filesrc, ".jpg", filepathname);//filesrc是文件夹名字
	//std::sort(filepathname.begin(),filepathname.end());
	for (int i = 0; i < filepathname.size(); i++)
	{
		string image = filesrc+"\\"+ filepathname[i];
		std::wstring widestr = std::wstring(image.begin(), image.end());
		Bitmap* pInImage = Bitmap::FromFile(widestr.c_str());
		if (pInImage->GetLastStatus() == -1)
		{
			SAFE_DELETE(pInImage);
			continue;
		}

		Bitmap*  img = pInImage->Clone(0, 0, pInImage->GetWidth(), pInImage->GetHeight(), PixelFormat32bppARGB);
		tempplate.push_back(img);
	}
	return tempplate;
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExampleDemoDlg::OnPaint()
{
	CDialogEx::OnPaint();
	//DrawRects();
	if (m_pOrignImage == NULL)
	{
		return;
	}

	Graphics gsClient(this->GetSafeHwnd());
	gsClient.SetSmoothingMode(SmoothingModeHighQuality);
	gsClient.SetInterpolationMode(InterpolationModeNearestNeighbor);


//1、创建可以绘制的rect区域
	CRect dialog_rect;
	GetClientRect(dialog_rect);
	int top = 50;
	int bottom = 50;
	int left = 50;
	int right = 50;
	float drawwidth = dialog_rect.Width() - left - right;
	float drawheight = dialog_rect.Height()- top - bottom;
	Rect draw_rect(left,top,drawwidth,drawheight);


//2、根据m_imagepercent计算图片绘制区域
	int mx = draw_rect.X + draw_rect.Width*(1 - m_imagepercent)*0.5;
	Rect imgrect(mx, draw_rect.Y+50, m_imagepercent*draw_rect.Width, m_imagepercent*draw_rect.Width);
	int tiaoshiw = m_pOrignImage->GetWidth();
	int tiaoshih = m_pOrignImage->GetHeight();
	gsClient.DrawImage(m_pOrignImage, imgrect, 0, 0, m_pOrignImage->GetWidth(), m_pOrignImage->GetHeight(), UnitPixel);


//3、左侧模板绘制
	int button_w = 100;
	int button_h = 30;
	float scalewh = 0.6667;
	int ne = 5;



	//绘制左侧第一排

	int tw_left1 = imgrect.Width*0.33;
	int th_left1 = tw_left1*scalewh;
	int tx_left1 = imgrect.X - 20 - 2 * tw_left1;
	Rect tempplate_rect_left1(tx_left1, draw_rect.Y, tw_left1, th_left1);

	for (int i = 0; i < ne ; i++)
	{
		int iy = (i - 0)*(tempplate_rect_left1.Height + 45);
		Rect tempplate_recti(tempplate_rect_left1.X, tempplate_rect_left1.Y + iy, tempplate_rect_left1.Width, tempplate_rect_left1.Height);
		gsClient.DrawImage(m_tempplate[i], tempplate_recti, 0, 0, m_tempplate[i]->GetWidth(), m_tempplate[i]->GetHeight(), UnitPixel);

		//ResizeBitmap(&m_tempplate[i], tempplate_recti.Width, tempplate_recti.Height);


		int newy = tempplate_recti.Y + tempplate_recti.Height + 7;
		int newx = tempplate_recti.X + tempplate_recti.Width / 3.;

		CButton *pButton = m_radioList[i];
		((CButton *)pButton)->MoveWindow(newx, newy, button_w, button_h, 1);
		((CButton *)pButton)->ShowWindow(SW_SHOW);
	}

//左侧第二排
	int tw_left = imgrect.Width*0.33;
	int th_left = tw_left*scalewh;
	int tx_left = imgrect.X-10- tw_left;
	Rect tempplate_rect_left(tx_left, draw_rect.Y, tw_left, th_left);
	
	for (int i = ne; i < ne * 2; i++)
	{
		int iy = (i - ne)*(tempplate_rect_left1.Height + 45);
		Rect tempplate_recti(tempplate_rect_left.X, tempplate_rect_left.Y+iy, tempplate_rect_left.Width, tempplate_rect_left.Height);
		gsClient.DrawImage(m_tempplate[i], tempplate_recti, 0, 0, m_tempplate[i]->GetWidth(), m_tempplate[i]->GetHeight(), UnitPixel);

		//ResizeBitmap(&m_tempplate[i], tempplate_recti.Width, tempplate_recti.Height);


		int newy = tempplate_recti.Y + tempplate_recti.Height+7;
		int newx = tempplate_recti.X+ tempplate_recti.Width/3.;

		CButton *pButton = m_radioList[i];
		((CButton *)pButton)->MoveWindow(newx, newy, button_w, button_h, 1);
		((CButton *)pButton)->ShowWindow(SW_SHOW);
	}








//4、绘制右侧模板

	int tw_right = imgrect.Width*0.33;
	int th_right = tw_right*scalewh;
	int tx_right = imgrect.X + imgrect.Width+10;
	Rect tempplate_rect_right(tx_right, draw_rect.Y, tw_right, th_right);
	int nright = 5;
	for (int i = ne*2; i <ne*3; i++)
	{
		if (i>m_tempplate.size()-1)
		{
			break;
		}
		int iy = (i- ne * 2)*(tempplate_rect_right.Height + 45);
		Rect tempplate_recti(tempplate_rect_right.X, tempplate_rect_right.Y + iy, tempplate_rect_right.Width, tempplate_rect_right.Height);
		gsClient.DrawImage(m_tempplate[i], tempplate_recti, 0, 0, m_tempplate[i]->GetWidth(), m_tempplate[i]->GetHeight(), UnitPixel);

		//ResizeBitmap(&m_tempplate[i], tempplate_recti.Width, tempplate_recti.Height);


		int newy = tempplate_recti.Y + tempplate_recti.Height + 7;
		int newx = tempplate_recti.X + tempplate_recti.Width / 3.;

		CButton *pButton = m_radioList[i];
		((CButton *)pButton)->MoveWindow(newx, newy, button_w, button_h, 1);
		((CButton *)pButton)->ShowWindow(SW_SHOW);
	}


	int tw_right1 = imgrect.Width*0.33;
	int th_right1 = tw_right1*scalewh;
	int tx_right1 = imgrect.X + (imgrect.Width + 10)+10+ tw_right1;
	Rect tempplate_rect_right1(tx_right1, draw_rect.Y, tw_right1, th_right1);
	for (int i = ne * 3; i < ne * 4; i++)
	{
		if (i > m_tempplate.size() - 1)
		{
			break;
		}
		int iy = (i - ne * 3)*(tempplate_rect_right1.Height + 45);
		Rect tempplate_recti(tempplate_rect_right1.X, tempplate_rect_right1.Y + iy, tempplate_rect_right1.Width, tempplate_rect_right1.Height);
		gsClient.DrawImage(m_tempplate[i], tempplate_recti, 0, 0, m_tempplate[i]->GetWidth(), m_tempplate[i]->GetHeight(), UnitPixel);

		//ResizeBitmap(&m_tempplate[i], tempplate_recti.Width, tempplate_recti.Height);


		int newy = tempplate_recti.Y + tempplate_recti.Height + 7;
		int newx = tempplate_recti.X + tempplate_recti.Width / 3.;

		CButton *pButton = m_radioList[i];
		((CButton *)pButton)->MoveWindow(newx, newy, button_w, button_h, 1);
		((CButton *)pButton)->ShowWindow(SW_SHOW);
	}









//5、绘制底侧"下一张"按钮
	int bbnx = imgrect.X + imgrect.Width/2.2;
	int bbny= imgrect.Y+ imgrect.Height+30;
	((CButton *)m_nextimageb)->MoveWindow(bbnx, bbny, button_w, button_h, 1);
	((CButton *)m_nextimageb)->ShowWindow(SW_SHOW);

}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CExampleDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CExampleDemoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, 1200, 1000);

	this->Invalidate(TRUE);
}


void CExampleDemoDlg::OnBnClickedBtnOpen()
{


/*
	CFileDialog fileDialog(true, NULL, NULL, NULL, TEXT("所有支持的图片格式|*.jpg;*.jpeg;*.bmp;*.png|Jpeg图片(*.jpg;*.jpeg;)|*.jpg;*.jpeg|Windows位图(*.bmp)|*.bmp|PNG 格式(*.png)|*.png||"));	
	if (fileDialog.DoModal() != IDOK)
	{
		return;
	}
	CString strFoo =fileDialog.GetPathName();
	WCHAR wstr[256];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, strFoo.GetBuffer(0), -1, wstr, 256);

	Bitmap* pInImage = Bitmap::FromFile(wstr);
	if (pInImage->GetLastStatus() == -1)
	{
		SAFE_DELETE(pInImage);
		return;
	}
	SAFE_DELETE(m_pOrignImage);
	m_pOrignImage =  pInImage;

	this->Invalidate(true);*/

	char szPath[MAX_PATH];     //存放选择的目录路径 
	CString str;

	ZeroMemory(szPath, sizeof(szPath));

	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;

	bi.pszDisplayName = szPath;
	bi.lpszTitle = "请选择需要打包的目录：";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	//弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);
	
	if (lp && SHGetPathFromIDList(lp, szPath))
	{
		m_imagefilepathname.clear();
		m_imagefilename.clear();
		ScanDirectory(szPath, ".jpg", m_imagefilename);//filesrc是文件夹名字
		for (int i=0;i<m_imagefilename.size();i++)
		{
			m_imagefilepathname.push_back(szPath + string("\\") + m_imagefilename[i]);
		}
		
		OnBnClickedBtnNext();

	}
	else
		AfxMessageBox("无效的目录，请重新选择");
}
BOOL CExampleDemoDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class  
	UINT   nID, nMode;
	nID = LOWORD(wParam);
	nMode = HIWORD(wParam);
	if (nMode == BN_CLICKED)
	{
		if (nID < (IDC_GROUP_BUTTON + m_radioList.size()) && nID >= IDC_GROUP_BUTTON)
		{

			m_classifyid=nID - IDC_GROUP_BUTTON;

		}
	}

	return CDialog::OnCommand(wParam, lParam);
}
#include <sstream>
void CExampleDemoDlg::OnBnClickedBtnNext()
{
	SAFE_DELETE(m_pOrignImage);
//当要按下个按钮的时候，保存上一张图片的选择结果到相应的文件夹中,移动新的图片，需要等到delete后，才能移动，否则无法访问

	if (m_classifyid>=0)
	{
		CString classify_file;
		classify_file.Format("%d", m_classifyid);
		classify_file =classify_file+"\\"+ CString(m_imagefilename.back().c_str());
		CString imgpath = m_imagefilepathname.back().c_str();
		MoveFile(imgpath, classify_file);
		//MoveFile("0.jpg", "t.jpg");
		m_imagefilepathname.pop_back();
		m_imagefilename.pop_back();
		((CButton *)GetDlgItem(IDC_GROUP_BUTTON+ m_classifyid))->SetCheck(FALSE);
		m_classifyid = -1;
	}


	if (m_imagefilepathname.empty())
	{
		AfxMessageBox("当前文件已挑选完毕，请重新选择新的文件夹");
		m_classifyid = -1;
		return;
	}
	string strFoo = m_imagefilepathname.back();
	m_pOrignImage = ReadBitmap(strFoo);
	if (!GetFaceRect(m_pOrignImage))
	{	
		m_classifyid = 19;
		SAFE_DELETE(m_pOrignImage);
		OnBnClickedBtnNext();
		//remove(szName);

		//m_imagefilepathname.pop_back();
		//m_imagefilename.pop_back();

	}



	//SAFE_DELETE(pInImage);
	this->Invalidate(FALSE);
}

Bitmap* CExampleDemoDlg::ReadBitmap(string path)
{
	
	const char* szName = path.c_str();


	WCHAR wstr[1000];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szName, -1, wstr, 1000);

	Bitmap* pInImage = Bitmap::FromFile(wstr);
	if (pInImage->GetLastStatus() == -1)
	{
		SAFE_DELETE(pInImage);
		return NULL;
	}
	return pInImage;

}



/*调用示例：
vector<string>filepathname;
ScanDirectory(filesrc,".txt",filepathname);//filesrc是文件夹名字
*/
#include <string>  
#include  <io.h>  
bool   CExampleDemoDlg::ScanDirectory(const string &src_dir_path, const const string &file_extension, vector<string>& src_files)
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
const string CExampleDemoDlg::getFileType(const string &filename)
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
BOOL CExampleDemoDlg::ResizeBitmap(Bitmap **ppImg, int m_NewWidth, int m_NewHeight)
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


#include "facepp/PupilGUI.h"
bool CExampleDemoDlg::GetFaceRect(Bitmap* pImageSori)
{
	//_sleep(100);
	Bitmap* pImageS = pImageSori->Clone(0, 0, pImageSori->GetWidth(), pImageSori->GetHeight(), PixelFormat32bppARGB);
	// TODO:  在此添加您专用的创建代码
	float scalew = 1.;
	if (pImageS->GetWidth() > 480 || pImageS->GetHeight() > 640)
	{

		scalew=min(480.f / pImageS->GetWidth(), 640.f / pImageS->GetHeight());

		int width = pImageS->GetWidth()*scalew;
		int height = pImageS->GetHeight()*scalew;
		ResizeBitmap(&pImageS, width, height);
	}


	Gdiplus::BitmapData TempBitmapData;
	Gdiplus::Rect rc(0, 0, pImageS->GetWidth(), pImageS->GetHeight());
	pImageS->LockBits(&rc, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &TempBitmapData);


	CPupilGUI cpi = CPupilGUI((BYTE*)TempBitmapData.Scan0, TempBitmapData.Width, TempBitmapData.Height);
	pImageS->UnlockBits(&TempBitmapData);
	SAFE_DELETE(pImageS);
	if (cpi.isface == true)
	{
		int x0=cpi.m_face_detection.rFace.left;
		int y0 = cpi.m_face_detection.rFace.top;
		int width = (cpi.m_face_detection.rFace.right-x0)/scalew;
		int height = (cpi.m_face_detection.rFace.bottom-y0)/scalew;
		if (height+width<700)
		{
			return false;
		}
		else
		{
			return true;
		}
		//return GetRect(CRect(x0/scalew, y0/scalew, width/scalew, height/scalew), pImageSori);

	}
	else
	{
		return false;
	}



}
Bitmap* CExampleDemoDlg::GetRect(CRect r,Bitmap* pImageSori)
{
	int a = r.top + r.left;
	int b = r.Height();
	int c = r.Width();
	Gdiplus::BitmapData TempBitmapData_ori;
	Gdiplus::Rect rcori(0, 0, pImageSori->GetWidth(), pImageSori->GetHeight());
	pImageSori->LockBits(&rcori, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &TempBitmapData_ori);
	BYTE*pori= (BYTE*)TempBitmapData_ori.Scan0;



	Bitmap* pImageS = pImageSori->Clone(0, 0, pImageSori->GetWidth(), pImageSori->GetHeight(), PixelFormat32bppARGB);
	ResizeBitmap(&pImageS, r.Width(), r.Height());
	Gdiplus::BitmapData TempBitmapData_scale;
	Gdiplus::Rect rcscale(0, 0, pImageS->GetWidth(), pImageS->GetHeight());
	pImageS->LockBits(&rcscale, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &TempBitmapData_scale);
	BYTE*pscale = (BYTE*)TempBitmapData_scale.Scan0;

	for (int i=0;i<r.Height();i++)
	{
		for (int j = 0; j < r.Width(); j++)
		{
			int posx = r.left+ j;
			int posy = r.top + i;
			int indexori = (posy*TempBitmapData_ori.Width + posx) * 4;
			for (int k=0;k<4;k++)
			{
				pscale[(i*r.Width() + j) * 4+k] = pori[indexori+k];
			}

		}
	}
	pImageS->UnlockBits(&TempBitmapData_scale);
	pImageSori->UnlockBits(&TempBitmapData_ori);

	return  pImageS;
}