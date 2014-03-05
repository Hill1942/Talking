

#define _CRT_SECURE_NO_WARNINGS


#include <WinSock2.h>
#include <windows.h>
#include <GdiPlus.h>
#include <wingdi.h>

#include <string>

#include "resource.h"

#pragma comment (lib, "Gdiplus.lib")
#pragma comment (lib, "WS2_32.lib")

using namespace Gdiplus;
using namespace std;

#define WM_SOCKET    1111
#define MAX_SOCKET     16

WNDPROC OrginProc;
HINSTANCE hApp;
HWND      hMainWindow;

int g_width;
int g_height;

HWND hMinButton;
HWND hMaxButton;
HWND hCloseButton;
HWND hSettingButton;
HWND hChatButton;

HWND hServerAddressLabel;
HWND hServerAddressEdit;
HWND hPortEdit;
HWND hPortLabel;
HWND hPortConfirm;

HWND hChatContent;
HWND hChatMessageEdit;
HWND hChatSendButton;
HWND hChatClearButton;

HWND hStateLabel;




int isMin = 0;
int isMax = 0;
int isCloseHover = 0;
int isChat = 1;
int isConnected = 0;

int port;
int clientNumber;

char* szClassName = "MainWindow";
LPWSTR labelText;
LPWSTR confirmButtonText;

SOCKET clientSocket;

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
LRESULT CALLBACK MyServerAddressLabelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyServerAddressEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MyChatMessageEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyChatSendButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MyChatClearButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MyStateLabelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LPWSTR stringToWString(const char* szString);
long onSocket(WPARAM wParam, LPARAM lParam);
BOOL Connect(LPCTSTR pszRemoteAddr, u_short nPort);



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
	wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
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

	labelText = L"Idel";
	confirmButtonText = L"Confirm";
	
	clientSocket = INVALID_SOCKET;
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusStartupToken);

	WSACleanup();

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
			OrginProc = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDB_MAX), GWL_WNDPROC, (LONG)MyMaxButtonProc);

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
									  180,
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
									  240,
									  65,
									  30,
									  hWnd,
									  (HMENU)IDB_PORT_CONFIRM,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hPortConfirm, GWL_WNDPROC, (LONG)MyPortConfirmProc);

			hServerAddressLabel = CreateWindow(TEXT("Button"),
			                          "Confirm",
									  WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
									  30,
									  130,
									  120,
									  30,
									  hWnd,
									  (HMENU)IDB_SERVER_ADDRESS_LABEL,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hServerAddressLabel, GWL_WNDPROC, (LONG)MyServerAddressLabelProc);

			hChatSendButton = CreateWindow(TEXT("Button"),
			                          "Send",
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  390,
									  450,
									  40,
									  30,
									  hWnd,
									  (HMENU)IDB_CHAT_SEND,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hChatSendButton, GWL_WNDPROC, (LONG)MyChatSendButtonProc);

			hChatClearButton = CreateWindow(TEXT("Button"),
			                          "Clear",
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
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
			                          "label",
									  WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
									  0,
									  580,
									  200,
									  20,
									  hWnd,
									  (HMENU)IDB_STATE_LABEL,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hStateLabel, GWL_WNDPROC, (LONG)MyStateLabelProc);
			
			hPortEdit = CreateWindow("edit",
			                          NULL,
									  WS_CHILD|WS_VISIBLE|WS_BORDER,
									  150,
									  180,
									  120,
									  30,
									  hWnd,
									  (HMENU)IDE_PORT,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hPortEdit, GWL_WNDPROC, (LONG)MyPortEditProc);

			hServerAddressEdit = CreateWindow("edit",
			                          NULL,
									  WS_CHILD|WS_VISIBLE|WS_BORDER,
									  150,
									  130,
									  120,
									  30,
									  hWnd,
									  (HMENU)IDE_SERVER_ADDRESS_EDIT,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hServerAddressEdit, GWL_WNDPROC, (LONG)MyPortEditProc);

			hChatContent = CreateWindow("listbox",
			                          NULL,
									  WS_CHILD|WS_VISIBLE|WS_BORDER,
									  30,
									  130,
									  400,
									  300,
									  hWnd,
									  (HMENU)IDE_CHAT_CONTENT,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			//OrginProc = (WNDPROC)SetWindowLong(hChatContent, GWL_WNDPROC, (LONG)MyPortEditProc);

			hChatMessageEdit = CreateWindow("EDIT",
			                          NULL,
									  WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOVSCROLL,
									  30,
									  450,
									  360,
									  30,
									  hWnd,
									  (HMENU)IDE_CHAT_MESSAGE,
									  ((LPCREATESTRUCT)lParam)->hInstance,
									  NULL);
			OrginProc = (WNDPROC)SetWindowLong(hChatMessageEdit, GWL_WNDPROC, (LONG)MyChatMessageEditProc);



			HFONT hFont = CreateFont(0,12,0,0,0,0,0,0,0,0,0,0,0,TEXT("Arial"));
            SendMessage(hChatMessageEdit, WM_SETFONT,(WPARAM)hFont,0);

			

			if (isChat)
			{
				ShowWindow(hPortConfirm, SW_HIDE);
				ShowWindow(hPortEdit, SW_HIDE);
				ShowWindow(hPortLabel, SW_HIDE);

				//ShowWindow(hChatClearButton, SW_HIDE);
				//ShowWindow(hChatClearButton, SW_SHOW);
			}
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

			g_width = windowWidth;
			g_height = windowHeight;

			Graphics    graphics(hdc);
			SolidBrush solidBrush1(Color(255, 69, 133, 243));
			graphics.FillRectangle(&solidBrush1, 200, windowHeight - 20, windowWidth - 200, 20);
			

			SetWindowPos(hMinButton,   NULL,       windowWidth - 90, 0, 30, 25, SWP_NOSIZE);
			SetWindowPos(hMaxButton,   hMinButton, windowWidth - 60, 0, 30, 25, SWP_NOSIZE);
			SetWindowPos(hCloseButton, hMaxButton, windowWidth - 30, 0, 30, 25, SWP_NOSIZE);
			SetWindowPos(hChatButton, hCloseButton,  30, 30, 75, 60, SWP_NOSIZE);
			SetWindowPos(hSettingButton, hChatButton, 125, 30, 109, 60, SWP_NOSIZE);

			SetWindowPos(hStateLabel, hSettingButton, 0, windowHeight - 20, 200, 20, SWP_SHOWWINDOW);

			//SetWindowPos(hSettingWindow, hSettingButton , 200, 200, 400, 400, SWP_SHOWWINDOW);
			

		    EndPaint(hWnd, &ps);
		    return 0;
		}
/*
	case WM_SETFOCUS:
		SetFocus(hSettingWindow);
		break;*/
		
	case WM_SIZE:
		{
		    g_width  = LOWORD(lParam);
			g_height = HIWORD(lParam);

			
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
		/*
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDB_PORT_CONFIRM:
				MessageBox(NULL, "asfa", "sf", MB_OK);
				break;
			

			default:
				break;
			}
			return 0;
		}	*/

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
/*
	case WM_MOUSEHOVER:
		{
			Graphics graphics(GetDC(hwnd));
			Image image(L"Assets/setting_on.png");
			graphics.DrawImage(&image, 0, 0, 109, 60);
			graphics.ReleaseHDC(GetDC(hwnd));
			return 0;
		}

	case WM_MOUSELEAVE:
		{
			Graphics graphics(GetDC(hwnd));
			Image image(L"Assets/setting.png");
			graphics.DrawImage(&image, 0, 0, 109, 60);
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
		}*/

	case WM_LBUTTONUP:
		{
			isChat = 0;
			ShowWindow(hSettingButton, SW_HIDE);
			ShowWindow(hSettingButton, SW_SHOW);

			ShowWindow(hChatButton, SW_HIDE);
			ShowWindow(hChatButton, SW_SHOW);

			ShowWindow(hChatContent, SW_HIDE);
			ShowWindow(hChatMessageEdit, SW_HIDE);
			ShowWindow(hChatSendButton, SW_HIDE);
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
/*
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
		}*/

	case WM_LBUTTONUP:
		{
			isChat = 1;
			
			ShowWindow(hChatButton, SW_HIDE);
			ShowWindow(hChatButton, SW_SHOW);

			ShowWindow(hSettingButton, SW_HIDE);
			ShowWindow(hSettingButton, SW_SHOW);

			ShowWindow(hChatContent, SW_SHOW);
			ShowWindow(hChatMessageEdit, SW_SHOW);
			ShowWindow(hChatSendButton, SW_SHOW);
			ShowWindow(hChatClearButton, SW_SHOW);

			ShowWindow(hPortConfirm, SW_HIDE);
			ShowWindow(hPortEdit, SW_HIDE);
			ShowWindow(hPortLabel, SW_HIDE);
		}

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}



LRESULT CALLBACK MyPortEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{	
		/*
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

		

			EndPaint(hwnd, &ps);
			return 0;
		}	*/



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
			SolidBrush  solidBrush2(Color(255, 255, 255, 255));
			graphics.DrawString(L"Port Number", -1, &font, pointF, &solidBrush2);

			EndPaint(hwnd, &ps);
			return 0;
		}	

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
	
}

LRESULT CALLBACK MyServerAddressLabelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			graphics.DrawString(L"Server Address", -1, &font, pointF, &solidBrush);

			EndPaint(hwnd, &ps);
			return 0;
		}	

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
	
}

LRESULT CALLBACK MyServerAddressEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{		
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics    graphics(hdc);
			FontFamily  fontFamily(L"Arial");
			Font        font(&fontFamily, 16, FontStyleRegular, UnitPixel);
			PointF      pointF(0.0f, 0.0f);
			SolidBrush  solidBrush(Color(255, 115, 115, 115));
			graphics.DrawString(L"Setting", -1, &font, pointF, &solidBrush);

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
			
			graphics.DrawString(confirmButtonText, -1, &font, pointF, &solidBrush);
			

			EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_LBUTTONDOWN:
		{
			if (clientSocket == INVALID_SOCKET)
			{
				
				char tempAddress[20];			
				GetWindowText(hServerAddressEdit, tempAddress, 20);
				if (strcmp(tempAddress, "") == 0)
				{
					MessageBox(NULL, "Please Enter Address", "Notice", MB_OK);
					return 0;
				}

				char tempPort[10];
				GetWindowText(hPortEdit, tempPort, 5);
				port = (int)strtod(tempPort, NULL);
				if (port < 1 || port > 65535)
				{
					MessageBox(NULL, "Wrong Port Number", "Notice", MB_OK);
					return 0;
				}

				if (!Connect(tempAddress, port))
				{
					MessageBox(NULL, "Can't connect to Server", "Error", MB_OK);
					return 0;
				}
				confirmButtonText = L"Cancel";
				InvalidateRect(hPortConfirm, NULL, TRUE);

				labelText = L"Connecting...";
				InvalidateRect(hStateLabel, NULL, TRUE);
			}
			else
			{
				closesocket(clientSocket);
				clientSocket = INVALID_SOCKET;

				confirmButtonText = L"Confirm";
			    InvalidateRect(hPortConfirm, NULL, TRUE);
			    
			    EnableWindow(hChatMessageEdit, FALSE);
			    EnableWindow(hChatContent, FALSE);
			    EnableWindow(hServerAddressEdit, TRUE);
			    EnableWindow(hPortEdit, TRUE);
			    
			    labelText = L"Idle";
			    InvalidateRect(hStateLabel, NULL, TRUE);
			}
		}
		


	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
	
}

LRESULT CALLBACK MyChatMessageEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{	
		/*
	case WM_CREATE:
		{
			

			return 0;
		}*/
		/*
		}
		
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

		

			EndPaint(hwnd, &ps);
			return 0;
		}	*/



	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
	return 0;

}

LRESULT CALLBACK MyChatSendButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{		
	case WM_PAINT:
		{	
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);

			Graphics graphics(hdc);
		    Image image(L"Assets/send.png");
		    graphics.DrawImage(&image, 0, 0, 40, 30);
			graphics.ReleaseHDC(hdc);
			

			EndPaint(hwnd, &ps);
			return 0;
		}	

	case WM_LBUTTONDOWN:
		{
			if (clientSocket == INVALID_SOCKET)
				return 0;

			char szText[1024];
			GetWindowText(hChatMessageEdit, szText, 1024);
			//strcat(szText, "\r\n");

			if (send(clientSocket, szText, strlen(szText), 0) != -1)
			{
				SendMessage(hChatContent, LB_ADDSTRING, NULL, (LPARAM)szText);
				SetWindowText(hChatMessageEdit, "");
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
/*
	case WM_LBUTTONDOWN:
		{
			if (serverSocket == INVALID_SOCKET)
			{
				char* temp;
				GetWindowText(hPortEdit, temp, 5);
				port = (int)strtod(temp, NULL);
				if (port < 1 || port > 65535)
				{
					MessageBox(NULL, "Wrong Port Number", "Error", MB_OK);
					break;
				}

				if (!CreateAndListen(port))
				{
					MessageBox(NULL, "Error in Start Service", "Error", MB_OK);
					break;
				}
			}
			else
			{
				CloseAllSocket();
			}
		}
		*/


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
			PointF      pointF(0.0f, 2.0f);
			SolidBrush  solidBrush(Color(255, 255, 255, 255));
			graphics.DrawString(labelText, -1, &font, pointF, &solidBrush);

			EndPaint(hwnd, &ps);
			return 0;
		}	

	default:
		return CallWindowProc(OrginProc, hwnd, message, wParam, lParam);
	}
}




long onSocket(WPARAM wParam, LPARAM lParam)
{
	SOCKET s = wParam;
	if (WSAGETASYNCERROR(lParam))
	{
		if (clientSocket != SOCKET_ERROR)
			SendMessage(hPortConfirm, WM_LBUTTONDOWN, wParam, lParam);
		labelText =L"Error in Connection";
		InvalidateRect(hStateLabel, NULL, TRUE);
		return 0;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
		{
			confirmButtonText = L"Disconnect";
			InvalidateRect(hPortConfirm, NULL, TRUE);

			EnableWindow(hChatMessageEdit, TRUE);
			EnableWindow(hChatContent, TRUE);
			EnableWindow(hServerAddressEdit, FALSE);
			EnableWindow(hPortEdit, FALSE);

			labelText = L"Connected to Server";
			InvalidateRect(hStateLabel, NULL, TRUE);

			break;
		}

	case FD_CLOSE:
		{
			SendMessage(hPortConfirm, WM_LBUTTONDOWN, wParam, lParam);
			break;
		}

	case FD_READ:
		{
			char szText[1024] = {0};
			recv(s, szText, 1024, 0);
			SendMessage(hChatContent, LB_ADDSTRING, NULL, (LPARAM)szText);
			break;
		}
	}

	return 0;
}

BOOL Connect(LPCTSTR pszRemoteAddr, u_short nPort)
{
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
		return FALSE;

	WSAAsyncSelect(clientSocket, hMainWindow, WM_SOCKET, FD_CONNECT|FD_CLOSE|FD_WRITE|FD_READ);

	ULONG uAddr = inet_addr(pszRemoteAddr);
	if (uAddr == INADDR_NONE)
	{
		hostent* pHost = gethostbyname(pszRemoteAddr);
		if (pHost == NULL)
		{
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			return FALSE;
		}
		uAddr = ((struct in_addr*)*(pHost->h_addr_list))->S_un.S_addr;
	}

	sockaddr_in remote;
	remote.sin_addr.S_un.S_addr = uAddr;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(nPort);

	connect(clientSocket, (sockaddr*)&remote, sizeof(sockaddr));

	return TRUE;
}

LPWSTR stringToWString(const char* szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	return lpszPath;
}