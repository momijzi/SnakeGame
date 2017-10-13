#pragma once

#include "Direct3D.h"

//テクスチャとしてロードした画像を
//保持するクラス

class Texture
{
	//Direct3Dクラスの関数からは
	//アクセス指定子を無視して
	//privateなメンバにアクセスできるようにする
	friend class Direct3D;
	//	pTextureをDirect3D内で参照したい

private :

	//ロードした画像データを保持する変数
	//I インターフェースというタイプのクラス
	//  今は特に気にしないでOK
	IDirect3DTexture9* pTexture;

	unsigned int divU;//x軸方向の分割数
	unsigned int divV;//y軸方向の分割数

	unsigned int numU;	//現在指定されている
						//横分割座標
						//divU以上にはならない

	unsigned int numV;	//現在指定されている
						//縦分割座標
						//divV以上にはならない

public :
	Texture();
	~Texture();

	//解放処理　必須　ロードした画像をアンロード
	void Release();

	//ロードする関数
	bool Load(TCHAR* FilePath);

	//分割の設定(左右上下いくつに等分するか)
	void SetDivide(unsigned int DivU,
					unsigned int DivV);

	//分割中の何個目を選択しているか
	void SetNum(unsigned int NumU,
		unsigned int NumV);

};
