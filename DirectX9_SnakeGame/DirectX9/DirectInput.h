#pragma once

//DirectInput デバッグフラグ
#define D3D_DEBUG_INFO

//DirectInput バージョン
#define DIRECT_INPUT_VERSION 0x0800

//マウスのボタンに名前を付ける
#define MOUSE_BUTTON_LEFT	0
#define MOUSE_BUTTON_RIGHT	1
#define MOUSE_BUTTON_MIDDLE 2

//マウスボタンの最大は８ボタン
#define MOUSE_BUTTON_MAX 8

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3dxof.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")

#pragma comment(lib,"dinput8.lib") //directInput

#include <comdef.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include <dinput.h>//directInput

#include"Vector2D.h"

#define INPUT_BUFFER_SIZE 256 //キーの数　

class DirectInput
{
	//シングルトンなクラスにする
private:
	static DirectInput* pInstance;
	//唯一のインスタンスを指すポインタ
public :

	//唯一のインスタンスを指すポインタの取得
	static DirectInput*GetInstance()
	{
		if (pInstance == nullptr)
		{
			pInstance =new DirectInput();
		}

		return pInstance;
	}

private :
	//privateなメンバ変数

	LPDIRECTINPUT8 pDInput;

	//キーボード用のデバイス
	LPDIRECTINPUTDEVICE8 pKeyDevice;
	//マウス用のデバイス
	LPDIRECTINPUTDEVICE8 pMouseDevice;

	//キーボードの情報
	//何フレーム押され続けているか
	int KeyInput[INPUT_BUFFER_SIZE];

	//1フレーム前のキーインプットの状態
	//入力の変化を見たい 特にキーを離した瞬間が判断できる
	int KeyInputPreview[INPUT_BUFFER_SIZE];

	//現在のマウス座標
	Vector2<int> mousePos;
	//１フレーム前のマウス座標
	Vector2<int> mousePosPreview;
	//1フレーム前と現在のマウス座標の差分	//このフレームで移動した距離
	Vector2<int> mousePosDelta;

	//マウスのボタンの状態
	DIMOUSESTATE2 mouseInput;
	//１フレーム前のマウスボタンの状態
	DIMOUSESTATE2 mouseInputPreview;
	//マウスのボタンが何フレーム押され続けているか
	int mouseButtonCounter[MOUSE_BUTTON_MAX];//最大８かい判定

	//ウィンドウのハンドル
	HWND hWnd;
	
	bool initialized;


public :
	//publicなメンバ関数

private:
	//privateなメンバ関数

	//シングルトンなクラスにするので
	//二つ以上のインスタンスが発生するような処理は
	//privateにして公開しない
	DirectInput();
	~DirectInput();
	DirectInput(DirectInput& refDi) {};
	DirectInput& operator=(DirectInput& refDi) {};

	
	HRESULT InitKey();//キーボードの準備
	HRESULT InitMouse();//マウスの準備

public :
	//publicなメンバ関数
	
	//初期化の処理　InitKeyやInitMouse等の実行
	HRESULT Init(HWND hWnd);

	//解放処理
	HRESULT Release();

	//入力状態の更新
	HRESULT Update();

	//keyの状態の取得
	bool KeyState(int code);//on offの状態
	bool KeyJustPressed(int code);//いま押された瞬間か
	bool KeyJustReleased(int code);//いま離された瞬間か
	int KeyCount(int code);//押され続けているフレーム数

	//マウスの状態を取得
	bool MouseButton(int num);//押されているか　on off　の状態
	bool MouseButtonJustPressed(int num);//押された瞬間か
	bool MouseButtonJustReleased(int num);//離された瞬間か
	int MouseButtonCount(int num);//押され続けているフレーム数

	//現在の座標
	Vector2<int> MousePosition() { return mousePos; }
	//前フレームからの移動距離（差分）
	Vector2<int> MousePositionDelta() { return mousePosDelta; }


};