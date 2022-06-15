#include "CInput.h"
#include <windows.h>

//ウィンドウポインタ
GLFWwindow* CInput::mpWindow = nullptr;
int CInput::sm_wheel = 0;

/*
初期化
Init(ウィンドウポインタ)
*/
void CInput::Init(GLFWwindow* w) {
	mpWindow = w;
	glfwSetScrollCallback(mpWindow, CInput::MouseScrollCB);
}

/*
マウスカーソルの座標を取得する
GetMousePos(X座標ポインタ,Y座標ポインタ)
ディスプレイの左上が原点(0,0)
画面右方向がX軸プラス
画面下方向がY軸プラス
*/
void CInput::GetMousePos(int* px, int* py) {
	POINT Point;
	//WindowsAPI
	GetCursorPos(&Point);
	*px = Point.x;
	*py = Point.y;
	return;
}

/*
マウスボタンの状態
GetMouseButton(ボタン)
ボタン:
GLFW_MOUSE_BUTTON_LEFT:左ボタン
GLFW_MOUSE_BUTTON_RIGHT:右ボタン
return;
true:押されている
false:押されていない
*/
bool CInput::GetMouseButton(int button) {
	int state = glfwGetMouseButton(mpWindow, button);
	return state == GLFW_PRESS;
}

/*
マウスカーソルの座標を設定する
SetCursorPos(X座標,Y座標)
ディスプレイの左上が原点(0,0)
*/
void CInput::SetMousePos(int x, int y) {
	//WindowsAPI
	SetCursorPos(x, y);
}

/*
マウスカーソルの座標を取得する
GetMousePosW(X座標ポインタ,Y座標ポインタ)
ウィンドウの左上が原点(0,0)
*/
void CInput::GetMousePosW(int* px, int* py)
{
	double xpos, ypos;
	glfwGetCursorPos(mpWindow, &xpos, &ypos);
	*px = (int)xpos;
	*py = (int)ypos;
	return;
}

//スクロールコールバック
void CInput::MouseScrollCB(GLFWwindow* window, double x, double y) {
	sm_wheel = (int)y;
}
int CInput::GetWheelValue()
{
	return sm_wheel;
}
void CInput::InputReset()
{
	sm_wheel = 0;
}

void CInput::SetMousePosW(int x, int y)
{
	glfwSetCursorPos(mpWindow, x, y);
}

