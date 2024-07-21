#include <windows.h>
#include <cmath>
#include <stdint.h>
#include <stdlib.h>

/* Definition Area Start */
typedef union _RGBQUAD{
	COLORREF rgb;
	struct{
		BYTE b;
		BYTE g;
		BYTE r;
		BYTE unused;
	};
} *PRGBQUAD;
/* Definition Area End */

/* Function Area Start*/
DWORD xs;
void InitDPI(){
	HMODULE hModule = LoadLibraryA("user32.dll");
	BOOL(WINAPI* SetProcessDPIAware)(VOID) = (BOOL(WINAPI*)(VOID))GetProcAddress(hModule, "SetProcessDPIAware");
	if (SetProcessDPIAware){
		SetProcessDPIAware();
	}
	FreeLibrary(hModule);
}
void SeedXorshift32(DWORD seed){
	xs = seed;
}
DWORD Xorshift32(){
	xs ^= xs << 13;
	xs ^= xs >> 17;
	xs ^= xs << 5;
	return xs;
}
int Product(int max, int min){
	int value = 1;
	for(int n = min; n <= max; n++){
		value *= n;
	}
	return value;
}	// min ¡Ü max
int nCr(int n, int r){
	return Product(n, n - r + 1) / Product(r, 1);
}	// nCr(n,r) = (n * (n-1) * (n-2) * ... * (n-r+1)) / r! (r ¡Ü n)

BITMAPINFO CreateCompatibleBitmapInfo(int width, int height){
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = width;		/* bitmap width */
	bmi.bmiHeader.biHeight = height;	/* bitmap height */
	return bmi;
}
POINT BezierPathToPoint(int BezierPointCount, PPOINT BezierPoint, float value){
	//range: 2 ¡Ü BezierPointCount ¡Ü 10
	if(BezierPointCount < 2 || BezierPointCount > 10){
		return { 0 };
	}
	
	double outputX, outputY = 0;
	for(int loop_0 = 0; loop_0 < BezierPointCount; loop_0++){
		double powValue = pow(1.f - value, loop_0) * pow(value, BezierPointCount - loop_0);
		int intValueX = nCr(BezierPointCount, loop_0) * BezierPoint[loop_0].x;
		int intValueY = nCr(BezierPointCount, loop_0) * BezierPoint[loop_0].y;
		outputX += static_cast<double>(intValueX) * powValue;
		outputY += static_cast<double>(intValueY) * powValue;
	}
	
	POINT output;
	output.x = static_cast<int>(outputX);
	output.y = static_cast<int>(outputY);
	return output;
}
/* Function Area End */

/* Shader Area Start */
POINT BezierDrawPoint[4];
void BeforeGDIShader(HDC hdcDst, HDC hdcSrc, int times, int width, int height){
	if(times % 10 == 0){
		for(int loop_0 = 0; loop_0 < 4; loop_0++){
			BezierDrawPoint[loop_0].x = Xorshift32() % width;
			BezierDrawPoint[loop_0].y = Xorshift32() % height;
		}
	}
	
	int halfIconSize = GetSystemMetrics(10) / 2;
	float BezierDrawUnit = 1.f / static_cast<float>((times % 10 + 5) * 5);
	for(float loop_1 = 0.f; loop_1 <= 1.f; loop_1 += BezierDrawUnit){
		POINT tmpPoint = BezierPathToPoint(4, BezierDrawPoint, loop_1);
		tmpPoint.x -= halfIconSize, tmpPoint.y -= halfIconSize;
		DrawIcon(hdcSrc, tmpPoint.x, tmpPoint.y, LoadIconA(NULL, IDI_ERROR));
	}
}

void AfterGDIShader(HDC hdcDst, HDC hdcSrc, int times, int width, int height){
	
}

void GDIShader(PRGBQUAD rgbDst, PRGBQUAD rgbSrc, int times, int x, int y, int width, int height){
	
}
/* Shader Area End */

HDC hdcDst, hdcSrc;
HBITMAP bmpSrc;
BITMAPINFO bmpInfo;
PRGBQUAD prgbDst, prgbSrc;
int width, height;
int loop_0, loop_1, loop_2;

void EffectProcess(bool isReset, int delay){
	width = GetSystemMetrics(0), height = GetSystemMetrics(1);
	bmpInfo = CreateCompatibleBitmapInfo(width, height);
	
	hdcDst = GetDC(0);
	hdcSrc = CreateCompatibleDC(hdcDst);
	bmpSrc = CreateDIBSection(hdcDst, &bmpInfo, 0, (void**)&prgbDst, NULL, 0);
	SelectObject(hdcSrc, bmpSrc);
	BitBlt(hdcSrc, 0, 0, width, height, hdcDst, 0, 0, 0x00cc0020);
	
	int64_t nSize = static_cast<int64_t>(width * height) * sizeof(_RGBQUAD);
	prgbSrc = new _RGBQUAD[(unsigned int)nSize];
	ZeroMemory(prgbSrc, (SIZE_T)nSize);
	for(loop_0 = 0; loop_0 < width * height; loop_0++){
		prgbSrc[loop_0].rgb = prgbDst[loop_0].rgb;
	}
	
	for(int loop_0 = 0; loop_0 < 200; loop_0++){
		hdcDst = GetDC(0);
		BitBlt(hdcSrc, 0, 0, width, height, hdcDst, 0, 0, 0x00cc0020);
		
		if(isReset){
			for(loop_1 = 0; loop_1 < width * height; loop_1++){
				prgbDst[loop_1].rgb = prgbSrc[loop_1].rgb;
		}}
		
		BeforeGDIShader(hdcDst, hdcSrc, loop_0, width, height);
		for(loop_2 = 0; loop_2 < height; loop_2++){
			for(loop_1 = 0; loop_1 < width; loop_1++){
				GDIShader(prgbDst, prgbSrc, loop_0, loop_1, loop_2, width, height);
			}
		}
		AfterGDIShader(hdcDst, hdcSrc, loop_0, width, height);
		
		BitBlt(hdcDst, 0, 0, width, height, hdcSrc, 0, 0, 0x00cc0020);
		
		ReleaseDC(0, hdcDst);
		DeleteObject(hdcDst);
		Sleep(delay);
	}
	ReleaseDC(0, hdcSrc);
	DeleteDC(hdcSrc);
	DeleteObject(bmpSrc);
	delete []prgbSrc;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow){
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	
	SeedXorshift32(GetTickCount());
	InitDPI();
	RedrawWindow(NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
	Sleep(100);
	
	int msg = MessageBoxA(NULL, "Are you sure you want to run the program?", "Bezier_Icon", 0x00000021L);
	if(msg == IDCANCEL) return 0;
	Sleep(1000);
	EffectProcess(true, 20);
	
	return 0;
}