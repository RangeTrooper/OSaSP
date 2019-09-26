#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
int DrawImage();
int RenewScreen();
int MyTimerCountdown(HWND hWnd);
void HandleMsWheel(WPARAM wParam);
int CloseMyWindow(HWND hWnd);
void GetNewCoordinatesBothAxes();
int GetNewCoordinates(int currPos, int minValue, int maxValue, int size, int* speed, int boost);
void Resize(HWND hWnd);
byte BackgrndCol = COLOR_WINDOW;



int X, Y = 0;
int step = 5;
int width = 800; int height = 600;
int window_width, window_height;
int rect_width = 122; int rect_height = 106;
int X_speed = 0, Y_speed = 0;
int X_boost = 50, Y_boost = 50;
RECT winRect{ 0, 0, window_width, window_height };
HDC hdc;
HINSTANCE HInstance;
int IMG_WIDTH = 122;
int IMG_HEIGHT = 106;
boolean isResized = false;

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE         hPrevInstance,
	LPSTR             lpCmdLine,
	int               nCmdShow)
{

	HInstance = hInstance;
	LPCTSTR lpzClass = TEXT("lab1");

	if (!RegMyWindowClass(hInstance, lpzClass))
		return 1;

	RECT screen_rect, window_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect);
	int x = screen_rect.right / 2 - width / 2;
	int y = screen_rect.bottom / 2 - height / 2;

	HWND hWnd = CreateWindow(lpzClass, TEXT("lab1"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_BORDER, x, y, width, height, NULL, NULL,
		hInstance, NULL);

	GetClientRect(hWnd, &window_rect);
	window_width = window_rect.right - window_rect.left;
	window_height = window_rect.bottom - window_rect.top;
	X = window_rect.right / 2;
	Y = window_rect.bottom / 2;
	if (!hWnd) return 2;

	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	DrawImage();

	SetTimer(hWnd, 1, 80, NULL);
	MSG msg = { 0 };
	int iGetOk = 0;
	while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (iGetOk == -1) return 3;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
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
		HandleMsWheel(wParam);
		break;

	case WM_SIZE:
		Resize(hWnd);
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
	FillRect(hdc, &winRect, HBRUSH(BackgrndCol));
	GetNewCoordinatesBothAxes();
	return DrawImage();
}


void HandleMsWheel(WPARAM wParam)
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
	if ((X_speed == 0) && (Y_speed == 0)) { return 0; }
	X = X + X_speed;
	Y = Y + Y_speed;
	X_speed = div(X_speed, 2).quot;
	Y_speed = div(Y_speed, 2).quot;
	return RenewScreen();
}

void GetNewCoordinatesBothAxes()
{
	X = GetNewCoordinates(X, 0, window_width, rect_width, &X_speed, X_boost);
	if (isResized)
		Y = GetNewCoordinates(Y, 0, window_height, rect_height+45, &Y_speed, Y_boost);
	else
		Y = GetNewCoordinates(Y, 0, window_height, rect_height, &Y_speed, Y_boost);
	return;
}

int GetNewCoordinates(int currPos, int minValue, int maxValue, int size, int* speed, int boost)
{
	if ((currPos >= minValue) && (currPos <= maxValue - size)) {
		return currPos;
	}
	if (currPos < minValue)//проверяем на границы окна 
	{
		*speed += boost;
		return minValue;
	}
	*speed -= boost;
	return maxValue - size;
}

void Resize(HWND hWnd)
{
	RECT myWind = {};
	LPRECT window_rect = &myWind;
	GetWindowRect(hWnd, window_rect);
	window_width = window_rect->right - window_rect->left;
	window_height = window_rect->bottom - window_rect->top;
	if (window_height < IMG_HEIGHT+45)
		window_height = IMG_HEIGHT+45;
	if (window_width < IMG_WIDTH)
		window_width = IMG_WIDTH;
	GetNewCoordinatesBothAxes();
	winRect = { 0, 0, window_width, window_height };
	if (window_height != 600)
		isResized = true;
	SetWindowPos(hWnd, HWND_TOPMOST, window_rect->left, window_rect->top, window_width, window_height, SWP_SHOWWINDOW);
	RenewScreen();
}

int DrawImage()
{
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hBmpCirc = (HBITMAP)LoadImage(NULL, "osasp_img.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	SelectObject(hMemDC, hBmpCirc);
	TransparentBlt(hdc, X, Y, IMG_WIDTH, IMG_HEIGHT, hMemDC, 0, 0, IMG_WIDTH, IMG_HEIGHT, RGB(255, 255, 255));//здесь меняем размер рисунка
	return 0;
}




