
#include <Windows.h>
#pragma comment(lib,"winmm.lib")

#include <tchar.h>

#include"Direct3D.h"

#include "Sprite.h"
#include "Texture.h"

#include "DirectInput.h"

#include"time.h"

//マップの値の表すもの
//プレイヤー　0以上　//壁の中　-1//壁　-2　//餌　-3

#define screenWidth 960
#define screenHeight 960
#define Pixel 36

//ウィンドウプロシージャ
LRESULT CALLBACK WndPrc
(
	HWND hWnd,	//ウィンドウのハンドル
	UINT msg,	//メッセージ
	WPARAM wParam,
	LPARAM lParam
)
{
	//メッセージ処理のための関数
	//DispatchMessageによって呼び出される

	//ウィンドウクラスの登録時に
	//各ウィンドウに設定される

	//アプリケーションがメッセージを
	//取得したら呼び出される

	//メッセージの例

	//WM_DESTROY ウィンドウが破棄された
	//			 ×ボタンが押された時など
	//WM_MOVE    ウィンドウが移動
	//WM_SIZE    ウィンドウサイズの変更
	//等

	//特別な処理を必要とする場合
	//または自分で独自の処理を定義したい場合
	//ここでメッセージの種類に応じた
	//処理を行う

	//取りあえず WM_DESTROYが来た時の
	//終了処理のみ定義

	switch (msg)
	{
		case WM_DESTROY:

			PostQuitMessage(0);

			//メッセージキューに
			//新しくWM_QUITメッセージを送る

			//いずれメッセージキューから
			//取り出され
			//メインループが終了する
			//(メインループの終了条件を
			//そのように作る)
			break;
	}

	//独自の処理を行ったら
	//デフォルトのウィンドウプロシージャを実行

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//ウィンドウ設定の名前
static const TCHAR* WC_BASIC = _T("Basic");

//作りたいウィンドウの設定を
//登録する関数
HRESULT RegistClassEx(HINSTANCE hInstance)
{
	//ウィンドウの設定を保存する構造体
	WNDCLASSEX wcex;
	
	//wcexの先頭アドレスから
	//WNDCLASSEXの長さ分のメモリを
	//全て0で埋める
	memset(&wcex, 0, sizeof(WNDCLASSEX));
	//つまりはwcexが使用しているメモリ領域を
	//全て0で初期化する処理

	//ウィンドウの設定

	//構造体のサイズ
	wcex.cbSize = sizeof(WNDCLASSEX);

	//ウィンドウの設定情報に追加する捕捉を付けないので
	//0で固定
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;

	//Windowのスタイル
	//それぞれがビット単位のフラグであるため
	//オア演算で合成したものがスタイルの最終的な設定
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

	//CS_HREDRAW 横サイズが変わったときウィンドウ全体の再描画
	//   VREDRAW 縦サイズが変わったときウィンドウ全体の再描画
	//   DBLCLKS ダブルクリックが発生したことをウィンドウに通知

	//WinMainのインスタンスハンドル
	wcex.hInstance = hInstance;

	//ウィンドウプロシージャのアドレス
	//関数ポインタの設定
	//ウィンドウプロシージャをOSが呼び出せるようにする
	wcex.lpfnWndProc = (WNDPROC)WndPrc;

	//背景色
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	//メニュー　なし
	wcex.lpszMenuName = NULL;

	//ウィンドウクラスの名前　このウィンドウ設定の名前
	wcex.lpszClassName = WC_BASIC;

	//アイコン　デフォルトのを使う
	wcex.hIcon = (HICON)LoadImage(NULL,
		MAKEINTRESOURCE(IDI_APPLICATION),
		IMAGE_ICON,
		0, 
		0, 
		LR_DEFAULTSIZE | LR_SHARED);
	wcex.hIconSm = NULL;

	//カーソル デフォルトのものを使う
	wcex.hCursor= (HCURSOR)LoadImage(NULL,
		MAKEINTRESOURCE(IDC_ARROW),
		IMAGE_CURSOR,
		0,
		0,
		LR_DEFAULTSIZE | LR_SHARED);

	//設定したウィンドウクラスを登録
	//登録したウィンドウ設定でないと
	//ウィンドウを作ることができない
	return RegisterClassEx(&wcex); 
	//HRESULT型の実行結果が戻される
}


//登録した設定を基にウィンドウを作成する
HRESULT MakeWindow
(HINSTANCE hInstance,//WinMainの識別子
	HWND &refHWnd,	//ウィンドウの識別子
				   //正しくウィンドウの作成ができたら
				   //この変数に識別子を代入する
	int width = screenWidth,	//クライアント領域の幅
	int height = screenHeight)	//クライアント領域の高さ
	//クライアント領域はウィンドウ全体から
	//外枠やメニューの部分を除いた物と今は思っておけばOK
{
	//クライアント領域のおおきさを決める
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = width;
	rect.bottom = height;

	//クライアント領域を保持するのに必要な
	//ウィンドウのおおきさを計算
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE/*TRUE*/);
	//正しく実行されるとrectの値が更新される

	//WS_OVERLAPPEDWINDOWはウィンドウのスタイルの一つ

	//ウィンドウのハンドル
	//ウィンドウの識別に使用する
	HWND hWnd;

	hWnd = CreateWindowEx
	(	0,						//ウィンドウ拡張スタイル
		WC_BASIC,				//作りたいウィンドウクラス
								//あらかじめ登録されたもの
		_T("タイトル"),			//ウィンドウのタイトル
		WS_OVERLAPPEDWINDOW,	//ウィンドウのスタイル
		CW_USEDEFAULT,			//位置x座標 デフォルトの値
		CW_USEDEFAULT,			//位置y座標 デフォルトの値
		rect.right - rect.left,	//ウィンドウ全体の幅
		rect.bottom - rect.top, //ウィンドウ全体の高さ
		NULL,					//親ウィンドウのハンドル
		NULL,					//メニューのハンドル
		hInstance,				//WinMainのハンドル

		NULL);					//ウィンドウ作成データ
								//ウィンドウに好きな32ビットのデータを
								//付属させることができる
								//追加情報構造体へのポインタなど
								//を付けるのが無難な使い方か
								//今回は使わない

	//ウィンドウが正しく作れたかをチェック
	//正しく作れたらhWndにNULL以外が入るはず
	if (hWnd == NULL)
	{
		return S_FALSE;//HRESULT型　失敗を表す識別子の一つ
	}

	//ウィンドウをアクティブにして表示
	//hWndでウィンドウの識別をしていることに注目
	ShowWindow(hWnd, SW_SHOWNORMAL);

	//クライアント領域の更新
	UpdateWindow(hWnd);

	//参照渡しになっている引数に
	//ウィンドウの識別ハンドルをコピーする
	//これによって今作ったウィンドウの識別ハンドルを
	//呼び出し元のプログラムで得ることができる
	refHWnd = hWnd;

	//HRESULT型　成功を表す識別子の一つ
	return S_OK;
}

//スプライトのインスタンスを作成
//パラメータは適当で
Sprite sprite;
Sprite sprite2;
Texture textureColor;
Texture textureStart;
Texture textureOver;
Texture textureClear;



//エントリーポイント
//プログラムの開始関数

int _stdcall WinMain
(HINSTANCE hInstance,	//プログラムを識別するハンドル
	HINSTANCE hPrevInstance,//アプリケーションの前のインスタンスハンドル
							//win32だと常にNULL
	LPSTR lpCmdLine,		//コマンドライン引数
	int nCmdShow)		//ウィンドウの表示状態
{
	//変数の宣言-------------------------------------
	const int WallLectSize = 16;//ここが壁の大きさを決めるところ

	const int WallHeight = WallLectSize - 1;	//壁の高さ
	const int WallWidth = WallLectSize - 1;		//壁の横幅
	int MapWall[WallLectSize][WallLectSize];	//壁の配列　二次元配列
	int Direction = 0;							//現在の進んでいる方向を示す [1]上　[0]下　[2]左　[3]右
	int PlayerX , PlayerY , PlayerXY;			//蛇の頭の部分のXとY
	int Count;									//時間判定用の変数
	int PlayerFlag[WallLectSize][WallLectSize]; //どの位置でプレイヤーが方向を変えたかを保存する配列
	int Speed ;									//プレイヤーのスピード
	bool gameFlag = true;						//ゲームのオーバーとクリアのフラグ
	int PlayerLength;							//プレイヤーの長さ
	int BaitX ,BaitY,BaitCount;					//餌関係全般　表示する場所判定　と　餌の描画が完了したことを返す変数

	srand((unsigned int)time(NULL));//乱数の初期値設定

	enum GameMode { ZERO, START, PLAY, OVER };
	GameMode game = ZERO;

	enum GameDegree{Easy , Herd , Challenger};
	GameDegree degree = Easy;

	////メッセージボックス
	//MessageBox(NULL,		//ウィンドウのハンドル 
	//	TEXT("最初の設定に成功しましたぱちぱち"),		//本文
	//	TEXT( "テスト-タイトル"),//タイトル
	//	MB_OK);				//メッセージボックスのタイプ
	//						//MB_OK  okのボタンが表示

	if (FAILED(RegistClassEx(hInstance)))
	{
		MessageBox(NULL,
			TEXT("ウィンドウクラス設定失敗"),
			TEXT("テスト-タイトル"),
			MB_OK);
		return 0;
	}

	HWND hWnd = NULL;

	/*正しくウィンドウが作成されれば
	hWndにウィンドウの識別ハンドルが入る*/
	if (FAILED(MakeWindow(hInstance, hWnd)))
	{
		MessageBox(NULL,
			TEXT("ウィンドウ作成失敗"),
			TEXT("テスト-タイトル"),
			MB_OK);
		return 0;
	}

	//Direct3Dを管理するクラス(シングルトン)への参照を取得
	Direct3D& d3d = Direct3D::GetInstance();

	//Direct3DDeviceの作成を試みる
	if (d3d.TryCreate(hWnd))
	{
		/*MessageBox(NULL,
			TEXT("Direct3D Device作成成功"),
			TEXT("テスト-タイトル"),
			MB_OK);*/
	}

	//レンダーステートの設定  αブレンド
	d3d.SetRenderState(RENDERSTATE::RENDER_ALPHABLEND);


	sprite.SetAlpha(0.1);						//透明度の設定
	sprite.SetSize(Pixel, Pixel);				//画像の大きさ
	sprite.SetAngle(0);							//画像の回転

	sprite2.SetAlpha(0.1);						//透明度の設定
	sprite2.SetSize(256, 128);					//画像の大きさ
	sprite2.SetAngle(0);						//画像の回転
	sprite2.SetPos(screenWidth / 4 * 3, screenHeight / 4 * 2.5);//描画する位置

	//テクスチャのインスタンスを作成
	textureColor.Load(_T("Texture/tex.png"));	//0[餌] 1[プレイヤー] 2[壁] 3[ミス]
	textureStart.Load(_T("Texture/start.png"));	//スタート画像
	textureOver.Load(_T("Texture/over.bmp"));	//失敗画像
	textureClear.Load(_T("Texture/clear.png"));	//クリア画像

	//ここで読み込んだ画像の分割処理
	textureColor.SetDivide(4, 0);				//今回は４分割する

	DirectInput * pDi = DirectInput::GetInstance();
	pDi->Init(hWnd);

	//メインループ
	//メッセージループ

	MSG msg = {};

	//quitメッセージが出てくるまでループを繰り返す
	//quitメッセージは上記のウィンドウプロシージャから送信
	//送信の条件などはウィンドウプロシージャを確認
	while (msg.message != WM_QUIT )
	{

		//PeekMessage
		//メッセージキューの中にメッセージがあるかを調べて
		//msgの中に書き込みtrueを返す
		//メッセージがなければfalseを返す
		//PM_REMOVEを設定しているとメッセージの書き込みのあとに
		//メッセージキューから元のメッセージを消す

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//仮想キーメッセージを文字メッセージに変換し
			//それをメッセージキューにポストする
			TranslateMessage(&msg);

			//メッセージの割り当て
			//ウィンドウプロシージャでメッセージを処理
			DispatchMessage(&msg);
		}
		else
		{
			//メッセージキューにメッセージが無かったとき
			//こちらの処理
			//ここにゲーム処理を書き込んでいく

			pDi->Update();//キー状態の更新

			if (pDi->KeyJustPressed(DIK_A))
			{
				MessageBox(NULL,
					TEXT("キー入力確認"),
					TEXT("テスト-タイトル"),
					MB_OK);
			}

			if (pDi->MouseButtonJustPressed(MOUSE_BUTTON_LEFT))
				Vector2<int> vec = pDi->MousePosition();
			{
				int breakpoint = 0;
			}

			//sprite.SetAngle(sprite.GetAngle_Rad()+0.0f);//回転ここのfを大きい値にするほど回転速度が上がる

			static int dir = 1;//α
			sprite.SetAlpha(sprite.GetAlpha() + (dir*0.01));
			switch (dir)
			{
				case 1:
					if (sprite.GetAlpha() >= 1)
					{
						dir = -1;
					}
					break;
				case -1:
					if (sprite.GetAlpha() <= 0)
					{
						dir = 1;
					}
					break;
			}

			switch (game)
			{
				case ZERO:
					d3d.ClearScreen();
					Direction = 0, BaitX = 0, BaitY = 0, Count = 0, BaitCount = 0;
					PlayerX = 1, PlayerY = 1, PlayerXY = 1;
					PlayerLength = 1, Speed = 10;

					//壁の部分を初期化
					for (int y = 0; y < WallLectSize; y++)
					{
						for (int x = 0; x < WallLectSize; x++)
						{
							if (y == 0 || y == WallHeight || x == 0 || x == WallWidth)
							{
								MapWall[y][x] = -2;//壁のあるところ
							}
							else if (PlayerX == x && PlayerY == y)
							{
								//プレイヤーの座標
								MapWall[y][x] = 0;
							}
							else
							{
								MapWall[y][x] = -1;//何もないところ
							}
						}
					}

					game = START;

					break;
				case START:
					//今回難易度設定ではなくコンフィグいじりにしてみる
					//エンター押したときPLAYへ
					if (pDi->KeyJustPressed(DIK_RETURN))
					{
						//難易度追加したかった。。。
						/*switch (degree)
						{
							case Easy:
								Speed = 20;

								break;
							case Herd:
								Speed = 10;

								break;
							case Challenger:
								Speed = 5;

								break;
						}*/
						
						game = PLAY;
					}
					break;
				case PLAY:
					//とりあえず常時蛇が動くプログラム
					Count++;
					while (BaitCount != 1 || PlayerLength == WallHeight * WallWidth)
					{
						BaitX = rand() % WallWidth;
						BaitY = rand() % WallHeight;
						if (MapWall[BaitY][BaitX] == -1)
						{
							BaitCount = 1;
						}
					}

					//やりたいこと
					//マップ単位で行動保存

					//プレイヤーの移動について
					if (pDi->KeyJustPressed(DIK_UP) && Direction != 0)
					{
						PlayerXY = 0;
					}
					else if (pDi->KeyJustPressed(DIK_DOWN) && Direction != 1)
					{
						PlayerXY = 1;
					}
					else if (pDi->KeyJustPressed(DIK_LEFT) && Direction != 3)
					{
						PlayerXY = 2;
					}
					else if (pDi->KeyJustPressed(DIK_RIGHT) && Direction != 2)
					{
						PlayerXY = 3;
					}

					//一回の動く速さ
					if (Count == Speed)
					{
						Count = 0;
						//上に移動
						if (PlayerXY == 0)
						{
							PlayerY--;
							Direction = 1;
						}
						//下に移動
						else if (PlayerXY == 1)
						{
							PlayerY++;
							Direction = 0;
						}
						//左に移動
						else if (PlayerXY == 2)
						{
							PlayerX--;
							Direction = 2;
						}
						//右に移動
						else if (PlayerXY == 3)
						{
							PlayerX++;
							Direction = 3;
						}
						else
						{
							return 0;
						}

						//移動につれ子分とプレイヤーの描画移動
						for (int y = 0; y < WallHeight; y++)
						{
							for (int x = 0; x < WallWidth; x++)
							{
								if (MapWall[y][x] > -1)
								{
									MapWall[y][x]++;
									if (MapWall[y][x] > PlayerLength - 1)
									{
										MapWall[y][x] = -1;
									}
								}
							}
						}
					}
					//プレイヤーがすべてのマスを埋め終わった　もしくは壁または自爆したとき
					if (MapWall[PlayerY][PlayerX] == -2 || MapWall[PlayerY][PlayerX] > 0)
					{
						game = OVER;
						gameFlag = false;
					}
					else if (PlayerLength == WallHeight * WallWidth)
					{
						game = OVER;
						gameFlag = true;
					}
					else
					{
						MapWall[PlayerY][PlayerX] = 0;
					}

					if (PlayerY == BaitY && PlayerX == BaitX)
					{
						PlayerLength++;
						BaitCount = 0;
					}

					break;
				case OVER:
					if (pDi->KeyJustPressed(DIK_RETURN))
					{
						game = ZERO;
					}
					break;
			}

			//まず描画 
			d3d.BeginScene();//描画開始
			//描画
			d3d.ClearScreen();

			//壁の描画----------------------------------------------------------------------------------
			
				for (int x = 0; x < WallLectSize; x++)//横の壁の描写
				{
					//上の壁の描画
					//画像の中心座標  //xは描画する位置に関係していて１上がるごとに右に描画する
					sprite.SetPos(Pixel * x + Pixel / 2, Pixel / 2);
					textureColor.SetNum(2, 0);//ここで表示
					sprite.Draw(textureColor);//スプライトの描画

					//下の壁の描画
					sprite.SetPos(Pixel * x + Pixel / 2, Pixel * WallHeight + Pixel / 2);
					textureColor.SetNum(2, 0);
					sprite.Draw(textureColor);
				}
				for (int y = 1; y < WallLectSize; y++)
				{
					//左の壁描画
					//画像の中心座標 //yは描画する位置に関係していて１上がるごとに下に描画する
					sprite.SetPos(Pixel / 2, Pixel * y + Pixel / 2);
					textureColor.SetNum(2, 0);//これでミスった時の色変更にも対応する
					sprite.Draw(textureColor);//スプライトの描画

					//右の壁描画
					sprite.SetPos(Pixel * WallWidth + Pixel / 2, Pixel * y + Pixel / 2);
					textureColor.SetNum(2, 0);
					sprite.Draw(textureColor);
				}
				//プレイヤーと子分の描画//-------------------------------------------------------------------------------------------


				for (int y = 0; y < WallHeight; y++)
				{
					for (int x = 0; x < WallWidth; x++)
					{
						if (MapWall[y][x] > -1)
						{
							sprite.SetPos(Pixel * x + Pixel / 2, Pixel * y + Pixel / 2);
							textureColor.SetNum(1, 0);
							sprite.Draw(textureColor);
						}
					}
				}
				sprite.SetPos(Pixel * PlayerX + Pixel / 2, Pixel * PlayerY + Pixel / 2);
				textureColor.SetNum(3, 0);
				sprite.Draw(textureColor);

				//餌の描画　//------------------------------------------------------------------------------------------
				if (BaitCount == 1)
				{
					sprite.SetPos(Pixel * BaitX + Pixel / 2, Pixel * BaitY + Pixel / 2);
					textureColor.SetNum(0, 0);
					sprite.Draw(textureColor);
				}
			

		
			
			//ゲームのスタートとオーバー時に表示する画像描画//--------------------------------------------------------------------------------------------
			
			if (game == START || game == OVER)
			{
				//スプライトの描画
				if (game == START)
				{
					sprite2.Draw(textureStart);
				}
				else if (game == OVER)
				{
					if (gameFlag == false)
					{
						sprite2.Draw(textureOver);
					}
					else
					{
						sprite2.Draw(textureClear);
					}
				}
			}

			//描画終了の合図//--------------------------------------------------------------------------------------------

			d3d.EndScene();

			//バックバッファをフロントへ反映
			d3d.Present();
		}
	}

	return 0;
}
