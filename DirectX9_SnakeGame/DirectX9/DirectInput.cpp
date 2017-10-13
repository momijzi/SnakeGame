#include  "DirectInput.h"

DirectInput* DirectInput::pInstance =nullptr;

DirectInput::DirectInput()
{
	pDInput = nullptr;

	pKeyDevice = nullptr;
	pMouseDevice = nullptr;

	mousePos.Set(0, 0);
	mousePosDelta.Set(0, 0);
	mousePosPreview.Set(0, 0);


	//配列変数の初期化//全てゼロ
	
	ZeroMemory(KeyInput, sizeof(int)*INPUT_BUFFER_SIZE);
	ZeroMemory(KeyInputPreview, sizeof(int)*INPUT_BUFFER_SIZE);

}

//デストラクタ
DirectInput::~DirectInput()
{
	Release();
}

HRESULT DirectInput::Init(HWND hWnd)
{
	this->hWnd = hWnd;

	if (initialized == false)
	{
		//DirectInputオブジェクトの作成
		HRESULT hresult
			= DirectInput8Create
			(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
				IID_IDirectInput8, (VOID**)&pDInput, NULL);

		if (FAILED(hresult))
		{
			return hresult;
		}

		//オブジェクトが出来たので
		//キーボード用のデバイスを初期化
		hresult = InitKey();

		if (FAILED(hresult))
		{
			return hresult;
		}

		//マウス使用の準備
		hresult = InitMouse();
		if (FAILED(hresult))
		{
			return hresult;
		}
		//全ての初期化が終了
		//初期化済みとする
		initialized = true;
	}

	//初期化成功
	return S_OK;
}

HRESULT DirectInput::InitKey()
{
	HRESULT hresult;

	//CreateDeviceの第一引数に作りたいデバイスのタイプが入る
	//第二引数が作ったデバイスの保存先が入るポインタ
	//タイプにかかわらず　第二引数の型は
	//LPDIRECTINPUTDEVICE8*　であることに注意
	hresult = pDInput
		->CreateDevice(GUID_SysKeyboard, &pKeyDevice, NULL);

	if (FAILED(hresult))
	{
		return hresult;
	}

	//データフォーマットの設定 キーボードを選ぶ
	hresult = pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hresult))
	{
		return hresult;
	}

	//協調モードの設定　
	//OSや他のアプリケーションとのデバイス共有の為の設定
	//デフォルト設定　
	hresult = pKeyDevice->SetCooperativeLevel
	(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	if (FAILED(hresult))
	{
		return hresult;
	}
	
	//入力デバイスへのアクセス権を取得
	pKeyDevice->Acquire();

	return S_OK;

}

//マウス関連の初期化
//マウス入力を使うための準備
HRESULT DirectInput::InitMouse()
{
	HRESULT hresult;

	//マウスデバイスの作成
	//キーボードのデバイス作成と同じ要領
	//作るデバイスのタイプが違うだけ
	hresult = pDInput->CreateDevice(GUID_SysMouse, &pMouseDevice, NULL);

	//失敗したら中断
	if (FAILED(hresult))return hresult;

	//データフォーマットの設定
	hresult = pMouseDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hresult))return hresult;

	//協調モードの設定
	hresult = pMouseDevice->SetCooperativeLevel
	(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	if (FAILED(hresult))return hresult;

	//軸モードの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;//相対
											//Axis 軸
											//Rel Relative 相対
											//ABS absolute 絶対
	//軸モード設定
	hresult = pMouseDevice->SetProperty
	(DIPROP_AXISMODE, &diprop.diph);

	if (FAILED(hresult))return hresult;

	//入力デバイスへのアクセス権を取得
	hresult = pMouseDevice->Acquire();

	if (FAILED(hresult))return hresult;

	//マウスの座標をウィンドウの中心に設定
	//クライアント領域の上下左右の端の座標を取得
	RECT rect;
	GetClientRect(hWnd, &rect);

	//幅と高さを求める
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	POINT localPos;
	//localとしたのはクライアント領域の左上角を
	//基準にした座標だから

	localPos.x = width / 2;
	localPos.y = height / 2;

	//クライアント座標からスクリーン座標への変換
	POINT worldPos = localPos;
	ClientToScreen(hWnd, &worldPos);
	//マウスのスタート位置の変更
	//スクリーン座標での指定
	SetCursorPos(worldPos.x, worldPos.y);

	//マウスの座標はクライアント座標で管理したい
	//mousePosとmousePosPreviewが同じ値でスタートさせたい
	//多分これは好みの問題
	mousePos.Set(localPos.x, localPos.y);
	mousePosPreview.Set(localPos.x, localPos.y);
	mousePosDelta.Set(0,0);

	return S_OK;
}
HRESULT DirectInput::Update()
{
	//初期化が済んでいなかったら実行するのに
	//問題がいろいろと出るので　関数を中断
	if (initialized == false)
	{
		return S_FALSE;
	}

	//キーボードの状態を取得して
	//メンバ変数の更新

	//アクセス権取得
	HRESULT hresult = pKeyDevice->Acquire();

	//S_FALSE以外の失敗は通さない
	if (hresult == DI_OK || hresult == S_FALSE)
	{
		//前回のフレームのキー情報をPreviewの方にコピー
		memcpy(KeyInputPreview, 
			KeyInput, 
			sizeof(int)*INPUT_BUFFER_SIZE);

		//状態　まずデバイスから現在のON OFF情報を取得
		//t temporaly 一時の
		//work　作業用の
		BYTE tKeyInputWork[INPUT_BUFFER_SIZE];

		//デバイスからキー状態(on off)の取得
		pKeyDevice->
			GetDeviceState(sizeof(BYTE)*INPUT_BUFFER_SIZE, 
				&tKeyInputWork);

		//キーのOnOffから
		//何フレーム押され続けているかという情報に加工
		for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
		{
			if (tKeyInputWork[i])//キーが押されている
			{
				if (KeyInput[i] < 2147483647)
				{
					//押され続けているフレーム数を加算
					KeyInput[i]++;
				}
			}
			else
			{
				//押されていない場合は　
				//連続フレームを 0 にする
				KeyInput[i]=0;
			}
		}
	}
	//マウス入力
	//アクセス権取得
	hresult = pMouseDevice->Acquire();

	if (hresult == DI_OK || hresult == S_FALSE)
	{
		//前回のマウス状態をまるっとコピー
		memcpy(&mouseInputPreview, &mouseInput, sizeof(DIMOUSESTATE2));

		pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseInput);

		for (int i = 0; i < MOUSE_BUTTON_MAX; i++)
		{
			//マウスのボタンが押されているか判定している
			if (mouseInput.rgbButtons[i])
			{
				if (mouseButtonCounter[i] < 2147483647)
				{
					//押されているフレーム数のカウントアップ
					mouseButtonCounter[i]++;
				}
				else
				{
					//押されていないばあいカウントリセット
					mouseButtonCounter[i] = 0;
				}
			}
		}
		//マウスの座標の取得関数は
		//Directinputのコードをしようしていない
		//座標の取り扱いがこちらの方がわかりやすかった


		//スクリーン中のマウスの座標
		POINT screenPos;
		//カーソル位置をスクリーン座標で取得
		GetCursorPos(&screenPos);

		//クライアント座標に変換
		POINT localPos = screenPos;
		ScreenToClient(hWnd, &localPos);

		//1フレーム前の座標を退避
		mousePosPreview = mousePos;
		//現在の座標を更新
		mousePos.Set(localPos.x, localPos.y);

		//移動した差分を計算
		mousePosDelta = mousePos - mousePosPreview;
	}

	return S_OK;
}

//現在押されているか　単純にOn　Off
bool DirectInput::KeyState(int code)
{
	if (code >= 0 && code < INPUT_BUFFER_SIZE)
	{
		//離されている状態が0なので
		//0より大きいと押されていることがわかる
		return KeyInput[code] > 0;
	}
	
	//範囲外はfalse
	return false;
}

//このフレームがこのキーの押された瞬間であるか
bool DirectInput::KeyJustPressed(int code)
{
	if (code >= 0 && code < INPUT_BUFFER_SIZE)
	{
		//0から1に切り替わった瞬間が
		//押された瞬間を示している
		return KeyInput[code] ==1;
	}

	//範囲外はfalse
	return false;
}

//このフレームがキーの離された瞬間か
bool DirectInput::KeyJustReleased(int code)
{
	if (code >= 0 && code < INPUT_BUFFER_SIZE)
	{
		//現在は0だけど
		//1フレーム前では押されていた
		//これが離された瞬間
		
		return 
			(KeyInput[code] == 0 &&
			KeyInputPreview[code]>0);
	}

	//範囲外はfalse
	return false;
}

//押され続けているフレーム数を返す
int DirectInput::KeyCount(int code)
{
	if (code >= 0 && code < INPUT_BUFFER_SIZE)
	{
		return KeyInput[code] ;
	}

	//範囲外は0
	return 0;
}

bool DirectInput::MouseButton(int num)
{
	if (num >= 0 && num < MOUSE_BUTTON_MAX)
	{
		return mouseInput.rgbButtons[num] >= 1;
	}
	else
	{
		return false;//配列範囲外
	}

}
bool DirectInput::MouseButtonJustPressed(int num)
{
	if (num >= 0 && num < MOUSE_BUTTON_MAX)
	{
		return (mouseInput.rgbButtons[num] >= 1) && (mouseInputPreview.rgbButtons[num] == 0);
	}
	else
	{
		return false;//配列範囲外
	}

}
bool DirectInput::MouseButtonJustReleased(int num)
{
	if (num >= 0 && num < MOUSE_BUTTON_MAX)
	{
		return (mouseInput.rgbButtons[num] ==0) && (mouseInputPreview.rgbButtons[num] >=1);
	}
	else
	{
		return false;//配列範囲外
	}

}

int DirectInput::MouseButtonCount(int num)
{
	if (num >= 0 && num < MOUSE_BUTTON_MAX)
	{
		return mouseButtonCounter[num];
	}
	else
	{
		return 0;//範囲外処理
	}
}

HRESULT DirectInput::Release()
{
	if (pMouseDevice)
	{
		pMouseDevice->Unacquire();
		pMouseDevice->Release();
		pMouseDevice = NULL;
	}

	if (pKeyDevice)
	{
		pKeyDevice->Unacquire();
		pKeyDevice->Release();
		pKeyDevice = NULL;
	}
	if (pDInput)
	{
			pDInput->Release();
			pDInput = NULL;
	}
	return S_OK;
}