#pragma once


// CAnimationDialog dialog

class CAnimationDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAnimationDialog)

public:
	CAnimationDialog(CWnd* pParent = NULL, LPCTSTR pImagesName = _T(""), LPCTSTR pShowTips = _T(""));   // standard constructor
	virtual ~CAnimationDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANIMATION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	HDC m_hRenderDC;//�豸����
	int m_nIndex;//ͼƬ��������
	int m_nCount;//ͼƬ��������
	int m_nElapse;//����ʱʱ��
	RECT m_rect;//���ڴ�С
	std::wstring m_wstrImagesName;//ͼƬ·��ǰ׺
	std::wstring m_wstrShowTips;//��ʾ��ʾ��Ϣ

public:
	void StartAnimation();
	void StartAnimation(int nElapse, RECT * pRect);
	void CloseAnimation();
};
