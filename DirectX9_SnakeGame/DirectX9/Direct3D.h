#pragma once

//インクルードファイル　Direct3D関係
#include <d3d9.h>
#include <d3dx9.h>

//ライブラリファイルの読み込み
//Direct3D関係
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3dxof.lib")
#pragma comment(lib,"dxguid.lib")

//インクルードファイル　windows
#include <Windows.h>

//ライブラリファイル　windows
#pragma comment(lib,"winmm.lib")


#include <tchar.h>

//板ポリゴンの頂点情報(１頂点分の情報)
//SpriteFVFの内容と合わせる必要がある 
//FVFは頂点構造体の構成情報が保存されたもの
//MSDNの頂点フォーマットのページなど参照
struct SpriteVertex
{
	//メンバの宣言の順番もFVFによって決まるので
	//宣言の順番に注意すること

	//三次元座標
	D3DXVECTOR3 pos;

	//スクリーン座標に変換済みかどうかを表すフラグ
	//1.0なら頂点シェーダでの変換が行われない
	float rhw;

	//頂点色 //diffuse
	D3DCOLOR color;

	//この頂点とテクスチャ中の座標の対応付け
	float u, v;
};

//頂点構造体の構成情報
static const DWORD SPRITE_FVF
= D3DFVF_XYZRHW |D3DFVF_DIFFUSE| D3DFVF_TEX1;

//レンダーステートの種類　設定用
enum RENDERSTATE
{
	RENDER_DEFAULT,		//デフォルト
	RENDER_ALPHATEST,	//αテスト
	RENDER_ALPHABLEND	//αブレンド
};

//使用するクラス名を宣言
//Sprite.hをインクルードせずに
//クラスSpriteの名前をヘッダー内で使う 
//名前以外の要素を使おうとするとエラー
//Dirct3d.cppではSprite.hをインクルードする必要あり
class Sprite;

//上のSpriteクラスと同様の理由
class Texture;

//シングルトンなクラスにする
//インスタンスがひとつしか作られないことを保証するクラス
//コンストラクタや代入演算子をprivateにして
//オブジェクトの作成の手段を制限
class Direct3D
{
private  :
	//クラス外からはオブジェクトが増える処理をさせない
	Direct3D();
	~Direct3D();

	Direct3D(const Direct3D& obj) {};		//コピーコンストラクタ
	void operator=(const Direct3D& obj) {};	//代入演算

	//唯一のオブジェクトを指すポインタ
	static Direct3D* pInstance;

public :
	
	//唯一のインスタンスへの参照を取得する関数
	//最初の実行の時にインスタンスを作成する
	static Direct3D& GetInstance()
	{
		if (pInstance == nullptr)
		{
			pInstance = new Direct3D();
		}
		return *pInstance;//唯一のインスタンスへの参照を返す
	}

	//作成した唯一のインスタンスを破棄する
	static  void DestroyInstance()
	{
		if (pInstance != nullptr)
		{
			delete pInstance;
			pInstance = nullptr;
		}
	}

	//開放関数
	//コンストラクタ実行時と同じ状態に戻す
	void Release();

	//レンダーステートの設定　描画の設定
	void SetRenderState(RENDERSTATE state);

private:
	//Direct3Dのデバイス
	//描画をしたり画像をロードしたりするために使用するオブジェクト
	IDirect3DDevice9 * pDevice3D;

	//Direct3Dデバイスの作成を行うためのオブジェクト
	IDirect3D9*  pD3D9;
	
	//以下　メンバ関数の宣言
private :
	bool Create(HWND hWnd);

public :

	//Direct3Dデバイスの作成を試みる
	//失敗するとfalseが返るようにする
	bool TryCreate(HWND hWnd);
	
	//beginとendの間で行われた描画処理でないと描画されない

	//描画の開始の合図をDirectXへ送る
	HRESULT BeginScene();
	//描画終了の合図をDirectXへ送る
	HRESULT EndScene();

	//バックバッファのクリア
	HRESULT ClearScreen();

	//バックバッファをフロント画面に反映
	HRESULT Present();

	//テクスチャのロード
	//戻り値は成功したか失敗したか
	//ロードが成功したら tex のメンバの
	//pTextureにアドレスが割り当てられる
	bool LoadTexture(Texture& tex, TCHAR*FilePath);

	//スプライトの描画
	void DrawSprite(Sprite& sprite,Texture& tex);
};