
// ExampleDemoDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include <list>
using namespace std;

// CExampleDemoDlg dialog
class CExampleDemoDlg : public CDialogEx
{
// Construction
public:
	CExampleDemoDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CExampleDemoDlg();

// Dialog Data
	enum { IDD = IDD_EXAMPLEDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	Bitmap* m_pOrignImage;
	Bitmap* m_pEffectImage;

	bool m_bShowEffect;
	Rect m_rtShowRect;
	float m_fZoom;

private:



// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnNext();
	vector<Bitmap*> LoadTemplateImg(string filesrc);
	vector<Bitmap*> m_tempplate;
	bool   ScanDirectory(const string &src_dir_path, const const string &file_extension, vector<string>& src_files);
	const string getFileType(const string &filename);
private:
	std::vector<CButton*> m_radioList;
	float m_imagepercent;
	float m_buttionsize;
	float m_templatepercent;
	BOOL ResizeBitmap(Bitmap **ppImg, int m_NewWidth, int m_NewHeight);
	CButton* m_nextimageb;
	vector<string>m_imagefilepathname;
	vector<string>m_imagefilename;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	int m_classifyid;

	string m_lastimgpathname;
	string m_lastimgname;
	Bitmap* GetFaceRect(Bitmap* pImageS);
	Bitmap* GetRect(CRect r, Bitmap* pImageSori);

};
