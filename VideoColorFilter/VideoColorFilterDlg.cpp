
// VideoColorFilterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VideoColorFilter.h"
#include "VideoColorFilterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVideoColorFilterDlg 对话框



CVideoColorFilterDlg::CVideoColorFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoColorFilterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoColorFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_Y1, mSliderY1);
	DDX_Control(pDX, IDC_SLIDER_Y2, mSliderY2);
	DDX_Control(pDX, IDC_SLIDER_U1, mSliderU1);
	DDX_Control(pDX, IDC_SLIDER_U2, mSliderU2);
	DDX_Control(pDX, IDC_SLIDER_V1, mSliderV1);
	DDX_Control(pDX, IDC_SLIDER_V2, mSliderV2);
	DDX_Text(pDX, IDC_FILTER_Y1, mY1);
	DDX_Text(pDX, IDC_FILTER_Y2, mY2);
	DDX_Text(pDX, IDC_FILTER_U1, mU1);
	DDX_Text(pDX, IDC_FILTER_U2, mU2);
	DDX_Text(pDX, IDC_FILTER_V1, mV1);
	DDX_Text(pDX, IDC_FILTER_V2, mV2);
}

BEGIN_MESSAGE_MAP(CVideoColorFilterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_Y1, &CVideoColorFilterDlg::OnNMCustomdrawSliderY1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_Y2, &CVideoColorFilterDlg::OnNMCustomdrawSliderY2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_U1, &CVideoColorFilterDlg::OnNMCustomdrawSliderU1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_U2, &CVideoColorFilterDlg::OnNMCustomdrawSliderU2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_V1, &CVideoColorFilterDlg::OnNMCustomdrawSliderV1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_V2, &CVideoColorFilterDlg::OnNMCustomdrawSliderV2)
END_MESSAGE_MAP()


// CVideoColorFilterDlg 消息处理程序

BOOL CVideoColorFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CWnd::SetWindowPos(NULL,0,0,1020,850,SWP_NOZORDER);
	GetDlgItem(IDC_VIDEO)->SetWindowPos( NULL,320,1,360,640,SWP_NOZORDER  );
	GetDlgItem(IDC_SLIDER_Y1)->SetWindowPos( NULL,50,650,900,20,SWP_NOZORDER  );
	GetDlgItem(IDC_SLIDER_Y2)->SetWindowPos( NULL,50,675,900,20,SWP_NOZORDER  );
	GetDlgItem(IDC_SLIDER_U1)->SetWindowPos( NULL,50,700,900,20,SWP_NOZORDER  );
	GetDlgItem(IDC_SLIDER_U2)->SetWindowPos( NULL,50,725,900,20,SWP_NOZORDER  );
	GetDlgItem(IDC_SLIDER_V1)->SetWindowPos( NULL,50,750,900,20,SWP_NOZORDER  );
	GetDlgItem(IDC_SLIDER_V2)->SetWindowPos( NULL,50,775,900,20,SWP_NOZORDER  );
	GetDlgItem(IDC_TEXT_Y1)->SetWindowPos( NULL,30,650,20,20,SWP_NOZORDER  );
	GetDlgItem(IDC_TEXT_Y2)->SetWindowPos( NULL,30,675,20,20,SWP_NOZORDER  );
	GetDlgItem(IDC_TEXT_U1)->SetWindowPos( NULL,30,700,20,20,SWP_NOZORDER  );
	GetDlgItem(IDC_TEXT_U2)->SetWindowPos( NULL,30,725,20,20,SWP_NOZORDER  );
	GetDlgItem(IDC_TEXT_V1)->SetWindowPos( NULL,30,750,20,20,SWP_NOZORDER  );
	GetDlgItem(IDC_TEXT_V2)->SetWindowPos( NULL,30,775,20,20,SWP_NOZORDER  );
	GetDlgItem(IDC_FILTER_Y1)->SetWindowPos( NULL,950,650,50,20,SWP_NOZORDER  );
	GetDlgItem(IDC_FILTER_Y2)->SetWindowPos( NULL,950,675,50,20,SWP_NOZORDER  );
	GetDlgItem(IDC_FILTER_U1)->SetWindowPos( NULL,950,700,50,20,SWP_NOZORDER  );
	GetDlgItem(IDC_FILTER_U2)->SetWindowPos( NULL,950,725,50,20,SWP_NOZORDER  );
	GetDlgItem(IDC_FILTER_V1)->SetWindowPos( NULL,950,750,50,20,SWP_NOZORDER  );
	GetDlgItem(IDC_FILTER_V2)->SetWindowPos( NULL,950,775,50,20,SWP_NOZORDER  );


	if(__argv[2]==nullptr||sizeof(__argv[2])==0){
		sprintf_s(configSaveFileName,"%s",__argv[2]);
	}else{
		sprintf_s(configSaveFileName,"%s","yuvset.ini");		
	}


	CFile mFile;
	if(mFile.Open(configSaveFileName, CFile::modeRead) == 0){
		mY1=0x96;
		mY2=0x7F;
		mU1=0x15;
		mU2=0x6A;
		mV1=0x2C;
		mV2=0x4C;
		TRACE("open failed!\n");
	}else{
		TRACE("open ok!\n");
		int size = mFile.GetLength();
		char sRead[1024];   
		memset(sRead,0,1024);
		mFile.Read(sRead,size); 
		CString y1,y2,u1,u2,v1,v2;
		AfxExtractSubString(y1, sRead, 0, ',');
		AfxExtractSubString(y2, sRead, 1, ',');
		AfxExtractSubString(u1, sRead, 2, ',');
		AfxExtractSubString(u2, sRead, 3, ',');
		AfxExtractSubString(v1, sRead, 4, ',');
		AfxExtractSubString(v2, sRead, 5, ',');
		mY1 = atoi(y1);
		mY2 = atoi(y2);
		mU1 = atoi(u1);
		mU2 = atoi(u2);
		mV1 = atoi(v1);
		mV2 = atoi(v2);
		mFile.Close();
	}
	UpdateData(FALSE);

	mSliderY1.SetRange(0,0xFF);
	mSliderY2.SetRange(0,0x7F);
	mSliderU1.SetRange(0,0xFF);
	mSliderU2.SetRange(0,0x7F);
	mSliderV1.SetRange(0,0xFF);
	mSliderV2.SetRange(0,0x7F);	

	mSliderY1.SetPos(mY1);
	mSliderY2.SetPos(mY2);
	mSliderU1.SetPos(mU1);
	mSliderU2.SetPos(mU2);
	mSliderV1.SetPos(mV1);
	mSliderV2.SetPos(mV2);

	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		TRACE("WSAStartup error !\n");
	}

	GetDlgItem(IDC_VIDEO)->SetFocus();


	//mController = new Controller();
	//mSoundService = new SoundService();
	mVideoService = new VideoService();
	mVideoService->setPlayUrl(__argv[1]);
	mSDLWindow = new SDLWindow();
	mSDLWindow->createWindow(this);	
	mVideoService->start(mSDLWindow);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoColorFilterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoColorFilterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVideoColorFilterDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	mVideoService->stop();
	mSDLWindow->destory();
	CDialogEx::OnCancel();
}


void CVideoColorFilterDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	delete mVideoService;
	delete mSDLWindow;
}


void CVideoColorFilterDlg::OnNMCustomdrawSliderY1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	mY1 = mSliderY1.GetPos();
	upYuvFilter();
}


void CVideoColorFilterDlg::OnNMCustomdrawSliderY2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	mY2 = mSliderY2.GetPos();
	upYuvFilter();
}


void CVideoColorFilterDlg::OnNMCustomdrawSliderU1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	mU1 = mSliderU1.GetPos();
	upYuvFilter();
}


void CVideoColorFilterDlg::OnNMCustomdrawSliderU2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	mU2 = mSliderU2.GetPos();
	upYuvFilter();
}


void CVideoColorFilterDlg::OnNMCustomdrawSliderV1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	mV1 = mSliderV1.GetPos();
	upYuvFilter();
}


void CVideoColorFilterDlg::OnNMCustomdrawSliderV2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	mV2 = mSliderV2.GetPos();	
	upYuvFilter();
}

void CVideoColorFilterDlg::upYuvFilter()
{
	UpdateData(FALSE);
	yuvFilter[0] = mY1;
	yuvFilter[1] = mY2;
	yuvFilter[2] = mU1;
	yuvFilter[3] = mU2;
	yuvFilter[4] = mV1;
	yuvFilter[5] = mV2;
	mVideoService->setMapFilter(yuvFilter);


	//对文件进行写操作    
	CFile mFile(configSaveFileName, CFile::modeWrite|CFile::modeCreate);  
	char temp[1024];
	sprintf_s(temp,"%d,%d,%d,%d,%d,%d",mY1,mY2,mU1,mU2,mV1,mV2);
	mFile.Write(temp,strlen(temp));    
	mFile.Flush();    
	mFile.Close(); 
}
