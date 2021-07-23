
// VideoColorFilterDlg.h : ͷ�ļ�
//
#include "VideoService.h"
#include "SDLWindow.h"

#pragma once


// CVideoColorFilterDlg �Ի���
class CVideoColorFilterDlg : public CDialogEx
{
// ����
public:
	CVideoColorFilterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_VIDEOCOLORFILTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	VideoService *mVideoService;
	SDLWindow *mSDLWindow;
	virtual void OnCancel();

	u_char yuvFilter[6];
	void upYuvFilter();
public:
	afx_msg void OnDestroy();
	
	CSliderCtrl mSliderY1;
	CSliderCtrl mSliderY2;
	CSliderCtrl mSliderU1;
	CSliderCtrl mSliderU2;
	CSliderCtrl mSliderV1;
	CSliderCtrl mSliderV2;
	int mY1;
	int mY2;
	int mU1;
	int mU2;
	int mV1;
	int mV2;
	afx_msg void OnNMCustomdrawSliderY1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderY2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderU1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderU2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderV1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderV2(NMHDR *pNMHDR, LRESULT *pResult);

	char configSaveFileName[1024];
};
