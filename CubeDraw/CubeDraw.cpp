#include <windows.h>
#include <math.h>
#define PI 3.141592653589793f


/* ----- global variables definition start ----- */
float scrWidth = (float)GetSystemMetrics(0), scrHeight = (float)GetSystemMetrics(1);
float scrPosX = 0.0f, scrPosY = 0.0f;
float turnX = 1.0f, turnY = 1.0f;
float cubeScale = 100.0f, cube_OutIn_Scale = 0.5f;
float rotateAngle = PI / 36.f;
float lineHue = 0.f;
/* ----- global variables definition end -----*/


/* ----- coordinate definition start ----- */
typedef struct {
	float x;
	float y;
	float z;
	
	def_coordinate(){
		this->x = 0.0f, this->y = 0.0f, this->z = 0.0f;
	}
	def_coordinate(float x, float y, float z){
		this->x = x, this->y = y, this->z = z;
	}
} def_coordinate;

float rotateSineAngle = sin(rotateAngle);
float rotateCosineAngle = sin(rotateAngle + PI / 2.0f);

#define ROTATE_X 0
#define ROTATE_Y 1
#define ROTATE_Z 2

#define ROTATE_FUNC1(v1, v2, rad) (v1 * rotateCosineAngle - v2 * rotateSineAngle)
#define ROTATE_FUNC2(v1, v2, rad) (v2 * rotateCosineAngle + v1 * rotateSineAngle)

	void RotateCoordinate(def_coordinate (*coord)[8], int status, float radSet){
		int rotateNum = 0;
		float x_tmp, y_tmp, z_tmp = 0.f;
		switch(status){
			case ROTATE_X:
				for( ; rotateNum < 8; rotateNum++){
					y_tmp = (*coord)[rotateNum].y, z_tmp = (*coord)[rotateNum].z;
					(*coord)[rotateNum].y = ROTATE_FUNC1(y_tmp, z_tmp, radSet);
					(*coord)[rotateNum].z = ROTATE_FUNC2(y_tmp, z_tmp, radSet);
				}
				break;

			case ROTATE_Y:
				for( ; rotateNum < 8; rotateNum++){
					x_tmp = (*coord)[rotateNum].x, z_tmp = (*coord)[rotateNum].z;
					(*coord)[rotateNum].x = ROTATE_FUNC1(x_tmp, z_tmp, radSet);
					(*coord)[rotateNum].z = ROTATE_FUNC2(x_tmp, z_tmp, radSet);
				}
				break;

			case ROTATE_Z:
				for( ; rotateNum < 8; rotateNum++){
					x_tmp = (*coord)[rotateNum].x, y_tmp = (*coord)[rotateNum].y;
					(*coord)[rotateNum].x = ROTATE_FUNC1(x_tmp, y_tmp, radSet);
					(*coord)[rotateNum].y = ROTATE_FUNC2(x_tmp, y_tmp, radSet);
				}
				break;

			default:;
		}
	}

#undef ROTATE_FUNC2
#undef ROTATE_FUNC1

void CubePositionCalculate(POINT (*pos_Out)[8], POINT (*pos_In)[8], def_coordinate (*coord)[8]){
	for(int fillNum = 0; fillNum < 8; fillNum++){
		(*pos_Out)[fillNum].x = (*coord)[fillNum].x * cubeScale + scrPosX;
		(*pos_Out)[fillNum].y = (*coord)[fillNum].y * cubeScale + scrPosY;
		(*pos_In)[fillNum].x = (*coord)[fillNum].x * cubeScale * cube_OutIn_Scale + scrPosX;
		(*pos_In)[fillNum].y = (*coord)[fillNum].y * cubeScale * cube_OutIn_Scale + scrPosY;
	}
}
/* ----- coordinate definition end ----- */


/* ----- main part start ----- */
COLORREF Hue2RGB(float hue){
	COLORREF rgb;
	float r = 0, g = 0, b = 0;
	float h = hue * 6.f;
	float f = h - (int)h;
		switch((int)h){
			case 0:
				r = 1.f, g = f, b = 0.f;
				break;
			case 1:
				r = 1.f - f, g = 1.f, b = 0.f;
				break;
			case 2:
				r = 0.f, g = 1.f, b = f;
				break;
			case 3:
				r = 0.f, g = 1.f - f, b = 1.f;
				break;
			case 4:
				r = f, g = 0.f; b = 1.f;
				break;
			case 5:
				r = 1.f, g = 0.f, b = 1.f - f;
				break;
		}

	rgb = RGB((unsigned char)(r * 255.f + 0.5f), (unsigned char)(g * 255.f + 0.5f), (unsigned char)(b * 255.f + 0.5f));
	return rgb;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {
	UNREFERENCED_PARAMETER( hInstance );
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	UNREFERENCED_PARAMETER( nCmdShow );

	def_coordinate coord[8];
	int initNum = 0;
	for(float initX = -1.0f; initX <= 1.0f; initX += 2.0f){
		for(float initY = -1.0f; initY <= 1.0f; initY += 2.0f){
			for(float initZ = -1.0f; initZ <= 1.0f; initZ += 2.0f){
				coord[initNum].x = initX;
				coord[initNum].y = initY;
				coord[initNum].z = initZ;
				initNum++;
	}}}
	int linePointIndex[12][2] = {
		{0, 2}, {0, 4}, {0, 1},
		{6, 2}, {6, 4}, {6, 7},
		{3, 1}, {3, 7}, {3, 2},
		{5, 1}, {5, 7}, {5, 4}
	};
/*
	coord[8] data:
	(-1,-1,-1)	(-1,-1,1)
	(-1,1,-1)	(-1,1,1)
	(1,-1,-1)	(1,-1,1)
	(1,1,-1)	(1,1,1)
*/

	RedrawWindow(NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
	Sleep(1000);

	HDC hdcDst, hdcSrc;
	HBITMAP hbmpSrc;
	
	hdcDst = GetDC(0);
	hdcSrc = CreateCompatibleDC(hdcDst);
	hbmpSrc = CreateCompatibleBitmap(hdcDst, scrWidth, scrHeight);
	SelectObject(hdcSrc, hbmpSrc);

	BitBlt(hdcSrc, 0, 0, scrWidth, scrHeight, hdcDst, 0, 0, 0x00000042);

	POINT linePoint_In[8];
	POINT linePoint_Out[8];
	for(;;){
		hdcDst = GetDC(0);
		BitBlt(hdcSrc, 0, 0, scrWidth, scrHeight, hdcSrc, 0, 0, 0x00000042);

		RotateCoordinate(&coord, ROTATE_X, (-1) * rotateAngle);
		RotateCoordinate(&coord, ROTATE_Y, (-1) * turnY * rotateAngle);
		RotateCoordinate(&coord, ROTATE_Z, turnX * rotateAngle);
		CubePositionCalculate(&linePoint_Out, &linePoint_In, &coord);

		int drawNum = 0;
		SelectObject(hdcSrc, CreatePen(0, 10, Hue2RGB(lineHue)));
		for(drawNum = 0; drawNum < 12; drawNum++){
			MoveToEx(hdcSrc, linePoint_Out[linePointIndex[drawNum][0]].x, linePoint_Out[linePointIndex[drawNum][0]].y, NULL);
			LineTo(hdcSrc, linePoint_Out[linePointIndex[drawNum][1]].x, linePoint_Out[linePointIndex[drawNum][1]].y);
			MoveToEx(hdcSrc, linePoint_In[linePointIndex[drawNum][0]].x, linePoint_In[linePointIndex[drawNum][0]].y, NULL);
			LineTo(hdcSrc, linePoint_In[linePointIndex[drawNum][1]].x, linePoint_In[linePointIndex[drawNum][1]].y);
		}
		for(drawNum = 0; drawNum < 8; drawNum++){
			MoveToEx(hdcSrc, linePoint_Out[drawNum].x, linePoint_Out[drawNum].y, NULL);
			LineTo(hdcSrc, linePoint_In[drawNum].x, linePoint_In[drawNum].y);
		}
		
		BitBlt(hdcDst, 0, 0, scrWidth, scrHeight, hdcSrc, 0, 0, 0x00CC0020);
		
		int changeTurnNum = 0;
		int saveTurnX = turnX, saveTurnY = turnY;
		for( ;changeTurnNum < 12; changeTurnNum++){
			if(linePoint_Out[linePointIndex[changeTurnNum][0]].x >= scrWidth - 10 || linePoint_Out[linePointIndex[changeTurnNum][1]].x >= scrWidth - 10){turnX = -1.0f;}
			if(linePoint_Out[linePointIndex[changeTurnNum][0]].x <= 10 || linePoint_Out[linePointIndex[changeTurnNum][1]].x <= 10){turnX = 1.0f;}
			if(linePoint_Out[linePointIndex[changeTurnNum][0]].y >= scrHeight - 10 || linePoint_Out[linePointIndex[changeTurnNum][1]].y >= scrHeight - 10){turnY = -1.0f;}
			if(linePoint_Out[linePointIndex[changeTurnNum][0]].y <= 10 || linePoint_Out[linePointIndex[changeTurnNum][1]].y <= 10){turnY = 1.0f;}
		}
		if(saveTurnX != turnX || saveTurnY != turnY){lineHue = fmod(lineHue * 360.f + 30.f, 360.f) / 360.f;}
		scrPosX += (turnX * 10.f), scrPosY += (turnY * 10.f);
		Sleep(50);
	}

	ReleaseDC(0, hdcDst);
	ReleaseDC(0, hdcSrc);
	DeleteObject(hdcDst);
	DeleteObject(hdcSrc);
	DeleteObject(hbmpSrc);
	return 0;
}
/* ----- main part end ----- */