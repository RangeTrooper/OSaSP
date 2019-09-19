#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
int DrawImage();
int RenewScreen();
int MyTimerCountdown(HWND hWnd);
void mouseWheel(WPARAM wParam);
int CloseMyWindow(HWND hWnd);
void TranslateCoordsXY();
int TranslateCoords(int value, int minValue, int maxValue, int len, int* speed, int boost);
void resize(HWND hWnd);
byte BackgrndCol = COLOR_WINDOW;
int mouseDownFlag = 0;




int X, Y = 0;
int step = 5;
int width = 900; int height = 600;
int window_width, window_height;
int rect_width = 48; int rect_height = 48;
int X_speed = 0, Y_speed = 0;
int X_boost = 50, Y_boost = 50;
RECT win_rect{ 0, 0, window_width, window_height };
HDC hdc;
HINSTANCE HInstance;

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE         hPrevInstance,
	LPSTR             lpCmdLine,
	int               nCmdShow)
{

	HInstance = hInstance;
	LPCTSTR lpzClass = TEXT("lab-1");

	if (!RegMyWindowClass(hInstance, lpzClass))
		return 1;

	RECT screen_rect, window_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect); 
	int x = screen_rect.right / 2 - width/2;
	int y = screen_rect.bottom / 2 - height/2;

	HWND hWnd = CreateWindow(lpzClass, TEXT("OSaSP_lab_1"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_BORDER, x, y, width, height, NULL, NULL,
		hInstance, NULL);

	GetClientRect(hWnd, &window_rect); 
	window_width = window_rect.right - window_rect.left;
	window_height = window_rect.bottom - window_rect.top;

	if (!hWnd) return 2;

	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	DrawImage();
	
	SetTimer(hWnd, 1, 80, NULL);
	MSG msg = { 0 };    //  
	int iGetOk = 0;   //  
	while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0) //  
	{
		if (iGetOk == -1) return 3;  //  GetMessage   - 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}



	return msg.wParam;  //    
}

ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
	WNDCLASS myWindowClass = { 0 };
	myWindowClass.lpfnWndProc = (WNDPROC)WndProc;
	myWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	myWindowClass.hInstance = hInst;
	myWindowClass.lpszClassName = lpzClassName;
	myWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	return RegisterClass(&myWindowClass); 
}

LRESULT CALLBACK WndProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:

		MyTimerCountdown(hWnd);
		break;

	case WM_DESTROY:
		CloseMyWindow(hWnd);
		PostQuitMessage(0);    
		break;
	case WM_MOUSEWHEEL:
		mouseWheel(wParam);
		break;

	case WM_SIZE:
		resize(hWnd);
		break;


	case WM_KEYDOWN: 
		switch (wParam)
		{
			case VK_RIGHT: 
				X_speed += 5;
				RenewScreen();
				break;
			case VK_LEFT: 
				X_speed -= 5;
				RenewScreen();
				break;
			case VK_UP: 
				Y_speed -= 5;
				RenewScreen();
				break;
			case VK_DOWN: 
				Y_speed += 5;
				RenewScreen();
				break;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}

int CloseMyWindow(HWND hWnd)
{
	KillTimer(hWnd, 1);
	return 0;
}

int RenewScreen()
{
	FillRect(hdc, &win_rect, HBRUSH(BackgrndCol));
	TranslateCoordsXY();

	return DrawImage();
}

void mouseWheel(WPARAM wParam)
{
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	if (fwKeys) {
		if (zDelta > 0) {
			X = X + step;
		}
		if (zDelta < 0) {
			X = X - step;
		}
	}
	else {
		if (zDelta > 0) {
			Y = Y + step;
		}
		if (zDelta < 0) {
			Y = Y - step;
		}
	}
	RenewScreen();
}

int MyTimerCountdown(HWND hWnd)
{
	if (mouseDownFlag) {
		POINT pos; RECT rc;
		GetCursorPos(&pos);
		GetClientRect(hWnd, &rc);
		MapWindowPoints(hWnd, GetParent(hWnd), (LPPOINT)&rc, 2);
		X = pos.x - rc.left;
		Y = pos.y - rc.top;
		RenewScreen();
	}
	if ((X_speed == 0) && (Y_speed == 0)) { return 0; }
	X = X + X_speed;
	Y = Y + Y_speed;
	X_speed = div(X_speed, 2).quot; 
	Y_speed = div(Y_speed, 2).quot;
	return RenewScreen();
}

void TranslateCoordsXY()
{
	X = TranslateCoords(X, 0, window_width, rect_width, &X_speed, X_boost);
	Y = TranslateCoords(Y, 0, window_height, rect_height, &Y_speed, Y_boost);
	return;
}

int TranslateCoords(int value, int minValue, int maxValue, int len, int* speed, int boost)
{
	if ((value >= minValue) && (value <= maxValue - len)) {
		return value;
	}
	if (value < minValue)
	{
		*speed += boost;
		return minValue;
	}
	*speed -= boost;
	return maxValue - len;
}

void resize(HWND hWnd)
{
	RECT window_rect;
	GetClientRect(hWnd, &window_rect); //  
	window_width = window_rect.right - window_rect.left;
	window_height = window_rect.bottom - window_rect.top;
	TranslateCoordsXY();
	win_rect = { 0, 0, window_width, window_height };
	RenewScreen();
}

int DrawImage()
{
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hBmpCirc = (HBITMAP)LoadImage(NULL, "mybmp.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	SelectObject(hMemDC, hBmpCirc);
	
	bool ans = TransparentBlt(hdc, X, Y, 48, 48, hMemDC, 0, 0, 262, 172, RGB(255, 255, 0));//здесь меняем размер рисунка


	return 0;
	//RECT rect{ X, Y, X + rect_width, Y + rect_height };
	//return FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 3));
}




