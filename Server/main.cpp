
#define _CRT_SECURE_NO_WARNINGS


#include <WinSock2.h>
#include <windows.h>
#include <GdiPlus.h>
#include <string>

#include "resource.h"

#pragma comment (lib, "Gdiplus.lib")
#pragma comment (lib, "WS2_32.lib")

using namespace Gdiplus;
using namespace std;

#define WM_SOCKET      WM_USER + 1
#define WM_LABELUPDATE WM_USER + 2
#define MAX_SOCKET     16

WNDPROC OrginProc;
HINSTANCE hApp;
HWND      hMainWindow;

HWND hMinButton;
HWND hMaxButton;
HWND hCloseButton;
HWND hSettingButton;
HWND hChatButton;
HWND hPortEdit;
HWND hPortLabel;
HWND hPortConfirm;

HWND hChatContent;
HWND hChatClearButton;

HWND hStateLabel;

HWND hUDPButton;


int isUDP = 0;
int isMin = 0;
int isMax = 0;
int isCloseHover = 0;
int isChat = 1;
int isListen = 0;
LPWSTR labelText;

int port;
int clientNumber;

char szHost[256];
char* szClassName = "MainWindow";
SOCKET serverSocket;
SOCKET clientSocks[MAX_SOCKET];

BOOL CreateAndListen(int nPort);

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyMinButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyMaxButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyCloseButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MySettingButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyChatButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MyPortEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyPortLabelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyPortConfirmProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MyChatClearButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyChatContentProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MyStateLabelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MyUDPButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CreateAndListen(int nPort);


void setLabelText(HWND hwnd, LPWSTR text, int bgWidth, int bgHeight, float fontX, float fontY);
LPWSTR stringToWString(const char* szString);
void CloseAllSocket();
void RemoveClient(SOCKET s);
BOOL AddClient(SOCKET s);
long onSocket(WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{

	Gdiplus::GdiplusStartupInput gdiInput;
	ULONG_PTR gdiplusStartupToken;
    Gdiplus::GdiplusStartup(&gdiplusStartupToken,&gdiInput,NULL);

	
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	WSAStartup(sockVersion, &wsaData);

	
	WNDCLASSEX wndclass;
	wndclass.cbSize        = sizeof(wndclass);
	wndclass.style         = CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc   = WinProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = LoadIcon(hInstance, (LPCSTR)IDI_ICON1);
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = szClassName;
	wndclass.hIconSm       = NULL;
	
	RegisterClassEx(&wndclass);
	
	HWND hwnd = ::CreateWindowEx(
		0,
		szClassName,
		"My First Window",
		WS_POPUP  ,
		100,
		100,
		460,
		600,
		NULL,
		NULL,
		hInstance,
		NULL);
	
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Error in Create Window", "Error", MB_OK);
		return -1;
	}

	hApp = hInstance;
	hMainWindow = hwnd;
	
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	serverSocket = INVALID_SOCKET;
	
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusStartupToken);

	
	return msg.wParam;	
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	
	switch(message)
	{
	case WM_CREATE:
		{
			hMinButton = CreateWindow(TEXT("Button"),
			                          "min",
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  0,
									  0,
									  30,
									  25,
									  hWnd,
									  (HMENU)IDB_MIN,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDB_MIN), GWL_WNDPROC, (LONG)MyMinButtonProc);

			hMaxButton = CreateWindow(TEXT("Button"),
			                          NULL,
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  0,
									  0,
									  30,
									  25,
									  hWnd,
									  (HMENU)IDB_MAX,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDB_MAX), 
				                               GWL_WNDPROC,
											   (LONG)MyMaxButtonProc);

			hCloseButton = CreateWindow(TEXT("Button"),
			                          NULL,
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  0,
									  0,
									  30,
									  25,
									  hWnd,
									  (HMENU)IDB_CLOSE,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDB_CLOSE), GWL_WNDPROC, (LONG)MyCloseButtonProc);

			hSettingButton = CreateWindow(TEXT("Button"),
			                          NULL,
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  125,
									  30,
									  109,
									  60,
									  hWnd,
									  (HMENU)IDB_SETTING,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDB_SETTING), GWL_WNDPROC, (LONG)MySettingButtonProc);
			
			hChatButton = CreateWindow(TEXT("Button"),
			                          NULL,
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  30,
									  30,
									  75,
									  60,
									  hWnd,
									  (HMENU)IDB_CHAT,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDB_CHAT), GWL_WNDPROC, (LONG)MyChatButtonProc);

			hPortLabel = CreateWindow(TEXT("Button"),
			                          NULL,
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  30,
									  130,
									  120,
									  30,
									  hWnd,
									  (HMENU)IDB_PORT_LABEL,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hPortLabel, GWL_WNDPROC, (LONG)MyPortLabelProc);

			hPortConfirm = CreateWindow(TEXT("Button"),
			                          "Confirm",
									  WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
									  205,
									  180,
									  65,
									  30,
									  hWnd,
									  (HMENU)IDB_PORT_CONFIRM,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hPortConfirm, GWL_WNDPROC, (LONG)MyPortConfirmProc);

			hChatClearButton = CreateWindow(TEXT("Button"),
			                          "Confirm",
									  WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
									  30,
									  100,
									  400,
									  30,
									  hWnd,
									  (HMENU)IDB_CHAT_CLEAR,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hChatClearButton, GWL_WNDPROC, (LONG)MyChatClearButtonProc);

			hStateLabel = CreateWindow(TEXT("Button"),
			                          "state",
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  00,
									  580,
									  200,
									  20,
									  hWnd,
									  (HMENU)IDB_STATE_LABEL,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hStateLabel, GWL_WNDPROC, (LONG)MyStateLabelProc);

			hUDPButton = CreateWindow(TEXT("Button"),
			                          "UDP on",
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  130,
									  180,
									  65,
									  30,
									  hWnd,
									  NULL,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hUDPButton, GWL_WNDPROC, (LONG)MyUDPButtonProc);
			
			hPortEdit = CreateWindow("edit",
			                          NULL,
									  WS_CHILD|WS_VISIBLE|WS_BORDER,
									  150,
									  130,
									  120,
									  30,
									  hWnd,
									  (HMENU)IDE_PORT,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hPortEdit, GWL_WNDPROC, (LONG)MyPortEditProc);

			hChatContent = CreateWindow("listbox",
			                          NULL,
									  WS_CHILD|WS_VISIBLE|WS_BORDER,
									  30,
									  130,
									  400,
									  400,
									  hWnd,
									  (HMENU)IDE_CHAT_CONTENT,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);

			if (isChat)
			{
				ShowWindow(hPortConfirm, SW_HIDE);
				ShowWindow(hPortEdit, SW_HIDE);
				ShowWindow(hPortLabel, SW_HIDE);
			}

			gethostname(szHost, 256);
	        HOSTENT* pHost = gethostbyname(szHost);
	        if (pHost != NULL)
	        {
		        string sIP = "Host IP: ";
		        in_addr* addr = (in_addr*)(pHost->h_addr_list);
				sIP.append(inet_ntoa(addr[0]));
				labelText = stringToWString(sIP.c_str());
	        }		

			HFONT hFont = CreateFont(0,7,0,0,0,0,0,0,0,0,0,0,0,TEXT("Arial"));
            SendMessage(hPortEdit, WM_SETFONT,(WPARAM)hFont,0);

			return 0;
		}

	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO *)lParam)->ptMinTrackSize.x = 460;  
        ((MINMAXINFO *)lParam)->ptMinTrackSize.y = 600;
		return 0;
	}
	

	case WM_PAINT:
		{
			HDC hdc;
		    PAINTSTRUCT ps;
		    hdc = BeginPaint(hWnd, &ps);

			GetWindowRect(hWnd, &rect);

			int windowX      = rect.left;
			int windowY      = rect.top;
			int windowWidth  = rect.right  - rect.left;
			int windowHeight = rect.bottom - rect.top;

			Graphics    graphics(hdc);
			SolidBrush solidBrush1(Color(255, 69, 133, 243));
			graphics.FillRectangle(&solidBrush1, 200, windowHeight - 20, windowWidth - 200, 20);

			Pen pen(Color(255, 143, 143, 143), 1);
			graphics.DrawRectangle(&pen, 0, 0, windowWidth - 1, windowHeight);

			SetWindowPos(hMinButton,   NULL,       windowWidth - 91, 1, 30, 25, SWP_NOSIZE);
			SetWindowPos(hMaxButton,   hMinButton, windowWidth - 61, 1, 30, 25, SWP_NOSIZE);
			SetWindowPos(hCloseButton, hMaxButton, windowWidth - 31, 1, 30, 25, SWP_NOSIZE);
			SetWindowPos(hChatButton, hCloseButton,  30, 30, 75, 60, SWP_NOSIZE);
			SetWindowPos(hSettingButton, hChatButton, 125, 30, 109, 60, SWP_NOSIZE);

			SetWindowPos(hStateLabel, hSettingButton, 0, windowHeight - 20, 200, 20, SWP_SHOWWINDOW);

		    EndPaint(hWnd, &ps);
		    return 0;
		}

	case WM_SOCKET:
		{
			onSocket(wParam, lParam);
			return 0;
		}
			
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		    return 0;
		}

	case WM_NCHITTEST:
		{
			int mouseX = LOWORD(lParam);
			int mouseY = HIWORD(lParam);
		
			GetWindowRect(hWnd, &rect);

			int windowX      = rect.left;
			int windowY      = rect.top;
			int windowWidth  = rect.right  - rect.left;
			int windowHeight = rect.bottom - rect.top;

			if ((windowY + 6 < mouseY && mouseY < windowY + windowHeight - 6)
				&& (windowX < mouseX && mouseX < windowX + 6))
				return HTLEFT;
			if ((windowY + 6 < mouseY && mouseY < windowY + windowHeight - 6)
				&& (windowX + windowWidth - 6 < mouseX && mouseX < windowX + windowWidth))
				return HTRIGHT;
			if ((windowY < mouseY && mouseY < windowY + 6)
				&& (windowX + 6 < mouseX && mouseX < windowX + windowWidth - 6))
				return HTTOP;
			if ((windowY + windowHeight - 6 < mouseY && mouseY < windowY + windowHeight)
				&& (windowX + 6 < mouseX && mouseX < windowX + windowWidth - 6))
				return HTBOTTOM;
			if ((windowY < mouseY && mouseY < windowY + 6)
				&& (windowX < mouseX && mouseX < windowX + 6))
				return HTTOPLEFT;
			if ((windowY < mouseY && mouseY < windowY + 6)
				&& (windowX + windowWidth - 6 < mouseX && mouseX < windowX + windowWidth))
				return HTTOPRIGHT;
			if ((windowY + windowHeight - 6 < mouseY && mouseY < windowY + windowHeight)
				&& (windowX < mouseX && mouseX < windowX + 6))
				return HTBOTTOMLEFT;
			if ((windowY + windowHeight - 6 < mouseY && mouseY < windowY + windowHeight)
				&& (windowX + windowWidth - 6 < mouseX && mouseX < windowX + windowWidth))
				return HTBOTTOMRIGHT;

			return HTCAPTION;
		}
	}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK MyMinButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
		    Image image(L"Assets/min.png");
		    graphics.DrawImage(&image, 0, 0, 30, 25);
			graphics.ReleaseHDC(hdc);

			EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_MOUSEHOVER:
		{
			Graphics graphics(GetDC(hwnd));
			Image image(L"Assets/min_on.png");
		    graphics.DrawImage(&image, 0, 0, 30, 25);
			graphics.ReleaseHDC(GetDC(hwnd));
			return 0;
		}

	case WM_MOUSELEAVE:
		{
			Graphics graphics(GetDC(hwnd));
			Image image(L"Assets/min.png");
		    graphics.DrawImage(&image, 0, 0, 30, 25);
			graphics.ReleaseHDC(GetDC(hwnd));
			return 0;
		}

	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme; 
            tme.cbSize = sizeof(tme); 
			tme.dwFlags = TME_HOVER|TME_LEAVE; 
			tme.dwHoverTime = 1;
			tme.hwndTrack = hwnd;
			TrackMouseEvent(&tme); 
			return 0;
		}

	case WM_LBUTTONUP:
		{
			SendMessage(hMainWindow, WM_SYSCOMMAND, SC_MINIMIZE, 0);
			isMin = 1;
		}


	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK MyMaxButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
			if (isMax)
			{
				Image image(L"Assets/restore.png");
				graphics.DrawImage(&image, 0, 0, 30, 25);
			}
			else
			{
				Image image(L"Assets/max.png");
				graphics.DrawImage(&image, 0, 0, 30, 25);
			}
		    
			graphics.ReleaseHDC(hdc);

			EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_MOUSEHOVER:
		{
			Graphics graphics(GetDC(hwnd));
			if (isMax)
			{
				Image image(L"Assets/restore_on.png");
				graphics.DrawImage(&image, 0, 0, 30, 25);
			}
			else
			{
				Image image(L"Assets/max_on.png");
				graphics.DrawImage(&image, 0, 0, 30, 25);
			}
			graphics.ReleaseHDC(GetDC(hwnd));
			return 0;
		}

	case WM_MOUSELEAVE:
		{
			Graphics graphics(GetDC(hwnd));
			if (isMax)
			{
				Image image(L"Assets/restore.png");
				graphics.DrawImage(&image, 0, 0, 30, 25);
			}
			else
			{
				Image image(L"Assets/max.png");
				graphics.DrawImage(&image, 0, 0, 30, 25);
			}
			graphics.ReleaseHDC(GetDC(hwnd));
			return 0;
		}

	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme; 
            tme.cbSize = sizeof(tme); 
			tme.dwFlags = TME_HOVER|TME_LEAVE; 
			tme.dwHoverTime = 1;
			tme.hwndTrack = hwnd;
			TrackMouseEvent(&tme); 
			return 0;
		}

	case WM_LBUTTONUP:
		{
			if (isMax)
			{
				SendMessage(hMainWindow, WM_SYSCOMMAND, SC_RESTORE, 0);
				isMax = 0;
			}
			else
			{
				SendMessage(hMainWindow, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				isMax = 1;
			}
		}

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK MyCloseButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{	
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
			Image image(L"Assets/close.png");
		    graphics.DrawImage(&image, 0, 0, 30, 25);
			graphics.ReleaseHDC(hdc);

			EndPaint(hwnd, &ps);
			return 0;
		}	
		
	case WM_MOUSEHOVER:
		{
			Graphics graphics(GetDC(hwnd));
			Image image(L"Assets/close_on.png");
		    graphics.DrawImage(&image, 0, 0, 30, 25);
			graphics.ReleaseHDC(GetDC(hwnd));
			return 0;
		}

	case WM_MOUSELEAVE:
		{
			Graphics graphics(GetDC(hwnd));
			Image image(L"Assets/close.png");
		    graphics.DrawImage(&image, 0, 0, 30, 25);
			graphics.ReleaseHDC(GetDC(hwnd));
			return 0;
		}

	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme; 
            tme.cbSize = sizeof(tme); 
			tme.dwFlags = TME_HOVER|TME_LEAVE; 
			tme.dwHoverTime = 1;
			tme.hwndTrack = hwnd;
			TrackMouseEvent(&tme); 
			return 0;
		}

	case WM_LBUTTONUP:
		{
			SendMessage(hMainWindow, WM_CLOSE, wParam, lParam);
		}

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK MySettingButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{	
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
			if (isChat)
			{
				Image image(L"Assets/setting_back.png");
				graphics.DrawImage(&image, 0, 0, 109, 60);
			}
			else
			{
				Image image(L"Assets/setting.png");
				graphics.DrawImage(&image, 0, 0, 109, 60);
			}
			graphics.ReleaseHDC(hdc);

			EndPaint(hwnd, &ps);
			return 0;
		}	



	case WM_LBUTTONUP:
		{
			isChat = 0;

			EnableWindow(hPortEdit, TRUE);
			EnableWindow(hPortConfirm, TRUE);
			EnableWindow(hUDPButton, TRUE);
			EnableWindow(hChatContent, FALSE);

			ShowWindow(hSettingButton, SW_HIDE);
			ShowWindow(hSettingButton, SW_SHOW);

			ShowWindow(hChatButton, SW_HIDE);
			ShowWindow(hChatButton, SW_SHOW);

			ShowWindow(hChatContent, SW_HIDE);
			ShowWindow(hChatClearButton, SW_HIDE);

			ShowWindow(hPortConfirm, SW_SHOW);
			ShowWindow(hPortEdit, SW_SHOW);
			ShowWindow(hPortLabel, SW_SHOW);
		}
		
	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK MyChatButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{	
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
			if (isChat)
			{
				Image image(L"Assets/chat.png");
				graphics.DrawImage(&image, 0, 0, 75, 60);
			}
			else
			{
				Image image(L"Assets/chat_back.png");
				graphics.DrawImage(&image, 0, 0, 75, 60);
			}
			graphics.ReleaseHDC(hdc);

			EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_LBUTTONUP:
		{
			isChat = 1;

			EnableWindow(hPortEdit, FALSE);
			EnableWindow(hPortConfirm, FALSE);
			EnableWindow(hUDPButton, FALSE);
			EnableWindow(hChatContent, TRUE);

			ShowWindow(hChatButton, SW_HIDE);
			ShowWindow(hChatButton, SW_SHOW);

			ShowWindow(hSettingButton, SW_HIDE);
			ShowWindow(hSettingButton, SW_SHOW);
			
			ShowWindow(hPortConfirm, SW_HIDE);
			ShowWindow(hPortEdit, SW_HIDE);
			ShowWindow(hPortLabel, SW_HIDE);
			
			ShowWindow(hChatContent, SW_SHOW);
			ShowWindow(hChatClearButton, SW_SHOW);

			
		}

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK MyPortEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{	
		
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
		    Pen blackPen(Color(255, 69, 133, 243), 1);
			graphics.DrawRectangle(&blackPen, 0, 0, 119, 29);


		
			EndPaint(hwnd, &ps);
			return 0;
		}	



	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK MyPortLabelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{		
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics    graphics(hdc);

			SolidBrush solidBrush1(Color(255, 69, 133, 243));
			graphics.FillRectangle(&solidBrush1, 0, 0, 120, 30);

			FontFamily  fontFamily(L"Arial");
			Font        font(&fontFamily, 16, FontStyleRegular, UnitPixel);
			PointF      pointF(0.0f, 5.0f);
			SolidBrush  solidBrush(Color(255, 255, 255, 255));
			graphics.DrawString(L"Server Port", -1, &font, pointF, &solidBrush);


			EndPaint(hwnd, &ps);
			return 0;
		}	

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
	
}

LRESULT CALLBACK MyPortConfirmProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{		
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);

		    SolidBrush solidBrush1(Color(255, 69, 133, 243));
			graphics.FillRectangle(&solidBrush1, 0, 0, 65, 30);

			FontFamily  fontFamily(L"Arial");
			Font        font(&fontFamily, 16, FontStyleRegular, UnitPixel);
			PointF      pointF(0.0f, 5.0f);
			SolidBrush  solidBrush(Color(255, 255, 255, 255));
			if (isListen)
			{
				graphics.DrawString(L"Stop", -1, &font, pointF, &solidBrush);
			}
			else
			{
				graphics.DrawString(L"Confirm", -1, &font, pointF, &solidBrush);
			}
			

			EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_LBUTTONDOWN:
		{
			if (serverSocket == INVALID_SOCKET)
			{
				char temp[10];
				GetWindowText(hPortEdit, temp, 5);
				port = (int)strtod(temp, NULL);
				if (port < 1 || port > 65535)
				{
					MessageBox(NULL, "Wrong Port Number", "Error", MB_OK);
					return 0;
				}

				if (!CreateAndListen(port))
				{
					MessageBox(NULL, "Error in Start Service", "Error", MB_OK);
					return 0;
				}
				isListen = 1;
				labelText = L"Listening...";
				InvalidateRect(hPortConfirm, NULL, TRUE);
				InvalidateRect(hStateLabel, NULL, TRUE);
				EnableWindow(hPortEdit, FALSE);
				return 0;
			}
			else
			{
				CloseAllSocket();
				isListen = 0;
				labelText = L"Idle";
				InvalidateRect(hPortConfirm, NULL, TRUE);
				InvalidateRect(hStateLabel, NULL, TRUE);
				EnableWindow(hPortEdit, TRUE);
				return 0;
			}
			
		}
	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK MyChatClearButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{		
	case WM_PAINT:
		{	
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
		    SolidBrush solidBrush1(Color(255, 69, 133, 243));
			graphics.FillRectangle(&solidBrush1, 0, 0, 400, 30);

			FontFamily  fontFamily(L"Arial");
			Font        font(&fontFamily, 16, FontStyleRegular, UnitPixel);
			PointF      pointF(350.0f, 5.0f);
			SolidBrush  solidBrush(Color(255, 255, 255, 255));
			graphics.DrawString(L"Clear", -1, &font, pointF, &solidBrush);
			

			EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_LBUTTONDOWN:
		{
			SendMessage(hChatContent, LB_RESETCONTENT, 0, 0);
			return 0;			
		}
	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK MyStateLabelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{		
	case WM_PAINT:
		{
			HDC hdc;
	        PAINTSTRUCT ps;
	        hdc = BeginPaint(hwnd, &ps);
	        
	        Graphics    graphics(hdc);
	        
	        SolidBrush solidBrush1(Color(255, 69, 133, 243));
	        graphics.FillRectangle(&solidBrush1, 0, 0, 200, 20);
	        
	        FontFamily  fontFamily(L"Arial");
	        Font        font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	        PointF      pointF(0, 2);
	        SolidBrush  solidBrush(Color(255, 255, 255, 255));
	        graphics.DrawString(labelText, -1, &font, pointF, &solidBrush);

	        EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_LABELUPDATE:
		{
			//setLabelText(hStateLabel, labelText, 200, 20, 0, 2);
			return 0;
		}

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK MyUDPButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{		
	case WM_PAINT:
		{	
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
		    SolidBrush solidBrush1(Color(255, 69, 133, 243));
			graphics.FillRectangle(&solidBrush1, 0, 0, 400, 30);

			FontFamily  fontFamily(L"Arial");
			Font        font(&fontFamily, 16, FontStyleRegular, UnitPixel);
			PointF      pointF(0.0f, 5.0f);
			SolidBrush  solidBrush(Color(255, 255, 255, 255));
			if (isUDP)
			{
				graphics.DrawString(L"UDPON", -1, &font, pointF, &solidBrush);
			}
			else
			{
				graphics.DrawString(L"TCPON", -1, &font, pointF, &solidBrush);
			}
			
			
			EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_LBUTTONDOWN:
		{
			if (isUDP)
			{
				isUDP = 0;
			}
			else
			{
				isUDP = 1;
			}
			InvalidateRect(hwnd, NULL, TRUE);
		}
		
	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}

BOOL CreateAndListen(int nPort)
{
	if (serverSocket == INVALID_SOCKET)
		closesocket(serverSocket);
	
	if (isUDP)
	{
		serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	else
	{
		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	
	if (serverSocket == INVALID_SOCKET)
		return FALSE;

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(serverSocket, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
		return FALSE;

	if (isUDP)
	{
		WSAAsyncSelect(serverSocket, hMainWindow, WM_SOCKET, FD_READ|FD_WRITE|FD_CLOSE);
	}
	else
	{
		WSAAsyncSelect(serverSocket, hMainWindow, WM_SOCKET, FD_ACCEPT|FD_CLOSE);
		listen(serverSocket, 5);
	}
	return TRUE;
}

void CloseAllSocket()
{
	if (serverSocket != INVALID_SOCKET)
	{
		closesocket(serverSocket);
		serverSocket = INVALID_SOCKET;
	}

	for (int i = 0; i < clientNumber; i++)
	{
		closesocket(clientSocks[i]);
	}
	clientNumber = 0;
}

void RemoveClient(SOCKET s)
{
	BOOL bFind = FALSE;
	int i;
	for (i = 0; i < clientNumber; i++)
	{
		if (clientSocks[i] == s)
		{
			bFind = TRUE;
			break;
		}
	}

	if (bFind)
	{
		clientNumber--;
		for (int j = i; j < clientNumber; j++)
		{
			clientSocks[j] = clientSocks[j + i];
		}
	}
}

BOOL AddClient(SOCKET s)
{
	if (clientNumber < MAX_SOCKET)
	{
		clientSocks[clientNumber++] = s;
		return TRUE;
	}
	return FALSE;
}

long onSocket(WPARAM wParam, LPARAM lParam)
{
	SOCKET s = wParam;
	if (WSAGETASYNCERROR(lParam))
	{
		RemoveClient(s);
		closesocket(s);
		return 0;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		{
			if (clientNumber < MAX_SOCKET)
			{
				SOCKET client = accept(s, NULL, NULL);
				int i = WSAAsyncSelect(client, hMainWindow, WM_SOCKET, FD_READ|FD_WRITE|FD_CLOSE);
				AddClient(client);
			}
			else
			{
				MessageBox(NULL, "Too many clients", "Error", MB_OK);
			}
			break;
		}

	case FD_CLOSE:
		{
			RemoveClient(s);
			closesocket(s);
		}

	case FD_READ:
		{
			
			sockaddr_in sockAddr;
			memset(&sockAddr, 0, sizeof(sockAddr));
			int nSockAddrLen = sizeof(sockAddr);

			char szText[1024] = {0};
			string strItem;

			if (isUDP)
			{
				recvfrom(serverSocket, szText, 1024, 0, (SOCKADDR*)&sockAddr, &nSockAddrLen);
			    char* sPeerIP = inet_ntoa(sockAddr.sin_addr);
				strItem.append("[");
			    strItem.append(sPeerIP);
			    strItem.append("]");
			    strItem.append(szText);
			}
			else
			{
				getpeername(s, (SOCKADDR*)&sockAddr, &nSockAddrLen);

			    int nPeerPort = ntohs(sockAddr.sin_port);
			    char* sPeerIP = inet_ntoa(sockAddr.sin_addr);
			    
			    DWORD dwIP = inet_addr(sPeerIP);
			    hostent* pHost = gethostbyaddr((LPSTR)&dwIP, 4, AF_INET);
			    char szHostName[256];
			    
			    strncpy(szHostName, pHost->h_name, 256);
				recv(s, szText, 1024, 0);
			
			    strItem.append("[");
			    strItem.append(sPeerIP);
			    strItem.append("]");
			    strItem.append(szText);
			}		

			SendMessage(hChatContent, LB_ADDSTRING, NULL, (LPARAM)strItem.c_str());
			break;
		}
	}

	return 0;
}

void setLabelText(HWND hwnd, LPWSTR text, int bgWidth, int bgHeight, float fontX, float fontY)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);

	Graphics    graphics(hdc);

	//SolidBrush solidBrush1(Color(255, 69, 133, 243));
	SolidBrush solidBrush1(Color(255, 0, 0, 0));
	graphics.FillRectangle(&solidBrush1, 0, 0, bgWidth, bgHeight);

	FontFamily  fontFamily(L"Arial");
	Font        font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	PointF      pointF(fontX, fontY);
	SolidBrush  solidBrush(Color(255, 255, 255, 255));
	graphics.DrawString(text, -1, &font, pointF, &solidBrush);


	EndPaint(hwnd, &ps);
}

LPWSTR stringToWString(const char* szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	return lpszPath;
}