#include "Sprite.h"

Sprite::Sprite()
{
	//インスタンスが作られた時の初期値
	pos.x = pos.y = 0.0f;
	width = height = 0.0f;

	angle = 0.0f;

	alpha = 1.0f;
}

Sprite::~Sprite()
{
	//特に必要な処理なし
}

//中心座標の変更
void Sprite::SetPos(float x, float y)
{
	pos.x = x;
	pos.y = y;
}

//幅と高さの設定
void Sprite::SetSize(float width, float height)
{
	this->width  = width;
	this->height = height;
}

//回転の設定
void Sprite::SetAngle(float rad)
{
	angle = rad;
}

//透明度の設定
void Sprite::SetAlpha(float a)
{
	if (a <= 0.0f) 
	{
		alpha = 0.0f;
	}
	else if (a >= 1.0f)
	{
		alpha = 1;
	}
	else
	{
		alpha = a;
	}
}

//描画
void Sprite::Draw(Texture& tex)
{
	//Direct3Dのインスタンスを取得
	Direct3D& d3d =
		Direct3D::GetInstance();

	//this =　自分へのポインタ
	
	//　(*ポインタ)なので変数本体を示す
	//参照として関数に渡す
	d3d.DrawSprite(*this,tex);
}