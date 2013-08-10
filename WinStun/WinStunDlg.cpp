
#include "stdafx.h"
#include "WinStun.h"
#include "WinStunDlg.h"

#include <cassert>

#include "../stun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog {
public:
    // CAboutDlg();

    // Dialog Data
    //enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};
/*
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
*/

// CWinStunDlg dialog



CWinStunDlg::CWinStunDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CWinStunDlg::IDD, pParent)
    , mServerName(_T("")) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinStunDlg::DoDataExchange(CDataExchange* pDX) {
    mServerName = _T("stun.qq.com");
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SERVER, mServerName);
    DDX_Text(pDX, IDC_RESULT, CString(_T("准备就绪..")));
    DDV_MaxChars(pDX, mServerName, 128);
}

BEGIN_MESSAGE_MAP(CWinStunDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_TEST, OnBnClickedTest)
END_MESSAGE_MAP()


// CWinStunDlg message handlers

BOOL CWinStunDlg::OnInitDialog() {
    CDialog::OnInitDialog();
    // Add "About..." menu item to system menu.
    // IDM_ABOUTBOX must be in the system command range.
    /*
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);
    CMenu* pSysMenu = GetSystemMenu(FALSE);

    if(pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);

        if(!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    */
    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon
    // TODO: Add extra initialization here
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWinStunDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    ///if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    //    CAboutDlg dlgAbout;
    //    dlgAbout.DoModal();
    // } else {
    CDialog::OnSysCommand(nID, lParam);
    //}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWinStunDlg::OnPaint() {
    if(IsIconic()) {
        CPaintDC dc(this); // device context for painting
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWinStunDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CWinStunDlg::OnBnClickedTest() {
    // TODO: Add your control notification handler code here
    CEdit* display = (CEdit*)GetDlgItem(IDC_RESULT);
    display->SetWindowText(CString(_T("正在获取信息...\r\n")));
    display->UpdateWindow();
    //获取服务器地址(可以是Ip:port形式)
    CString& server = mServerName;
    GetDlgItem(IDC_SERVER)->GetWindowText(server);
    char* s = (char*)(LPCTSTR)server;

    StunAddress4 stunServerAddr;

    stunParseServerName(s, stunServerAddr);

    bool verbose = false;
    StunAddress4 sAddr;
    sAddr.port = 0;
    sAddr.addr = 0;
    bool preservePort;
    bool hairpin;
    int port = 0;
    NatType stype = stunNatType(stunServerAddr, verbose, &preservePort, &hairpin, port, &sAddr);
    display->SetWindowText(CString(_T("测试中...\r\n")));
    display->UpdateWindow();

    CString text;

    switch(stype) {
        case StunTypeOpen:
            text = _T("没有检测到NAT设备,您是直接连接到外网 - p2p可以工作.");
            break;

        case StunTypeConeNat:
            text = _T("完全锥形NAT(Full Cone NAT) - p2p通过STUN可以工作.");
            break;

        case StunTypeRestrictedNat:
            text = _T("地址受限制锥形NAT(Address restricted NAT) - p2p通过STUN可以工作.");
            break;

        case StunTypePortRestrictedNat:
            text = _T("端口受限制锥形NAT(Port restricted NAT) - p2p通过STUN可以工作.");
            break;

        case StunTypeSymNat:
            text = _T("对称型NAT(Symetric NAT) - p2p无法直接工作.");
            break;

        case StunTypeSymFirewall:
            text = _T("对称型NAT,有防火墙(Symetric firewall) - p2p无法直接工作.");
            break;

        case StunTypeBlocked:
            text = _T("不能连接到服务器,请检测服务器名.");
            break;

        default:
            text = _T("未知NAT类型");
            break;
    }

    text += _T("\r\n\r\n");

    /*
    if(preservePort) {
        text += _T("端口号将保持\r\n");
    } else {
        text += _T("端口号无法保持\r\n");
    }
    */
    if(hairpin) {
        text += _T("支持应聘中国在线面试");
    } else {
        text += _T("不支持应聘中国在线面试");
    }

    text += _T("\r\n\r\n");
    CString strAddr;
    strAddr.Format(_T("公网IP地址: %d.%d.%d.%d ,端口: %d"),
                   (sAddr.addr >> 24) & 0xFF, (sAddr.addr >> 16) & 0xFF, (sAddr.addr >> 8) & 0xFF, (sAddr.addr >> 0) & 0xFF, sAddr.port);
    text += strAddr;
    display->SetWindowText(text + CString(_T("\r\n")));
}
