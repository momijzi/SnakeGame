#pragma once

#include "Direct3D.h"

class Texture;

class Sprite
{
	//Direct3Dからはメンバにアクセス可能にする
	friend class Direct3D;

private:
	D3DXVECTOR2 pos;//スプライトの中心座標
	float width;//スプライトの幅
	float height;//スプライトの高さ

	float angle;//回転の値　弧度法（ラジアン）

	float alpha;//α値

public:
	Sprite();
	virtual ~Sprite();

	void SetPos(float x, float y);//座標の設定

	void SetSize(float width, float height);//幅と高さの設定

	void SetAngle(float rad);//回転の値を設定

	void SetAlpha(float alpha);//透明度の設定

	void Draw(Texture& tex);//描画

	//スプライトの中心座標の取得
	D3DXVECTOR2 GetCenterPos() 
	{ return pos; }

	//幅の取得
	float GetWidth()
	{ return width; }

	//高さの取得
	float GetHeight()
	{ return height;}

	//回転の取得
	float GetAngle_Rad()
	{ return angle;}

	//透明度
	float GetAlpha()
	{ return alpha; }

	


};