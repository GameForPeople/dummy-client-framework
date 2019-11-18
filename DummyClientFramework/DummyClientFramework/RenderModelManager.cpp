#include "stdafx.h"

#include "Custom.hh"
#include "RenderModelManager.h"

#ifdef WONSY_RENDERMODEL_MANAGER
//-----------------------------------------------------
// BitModel
//-----------------------------------------------------

BaseModel::BaseModel(const std::wstring inResourcePath)
{
	imageOfModel = std::make_unique<CImage>();
	imageOfModel->Load(inResourcePath.c_str());
}

BaseModel::~BaseModel()
{
	imageOfModel->Destroy();
}

//-----------------------------------------------------
// BitModel
//-----------------------------------------------------

BitModel::BitModel(const std::wstring& inResourcePath)
	: BaseModel(inResourcePath)
{}

void BitModel::Render(HDC pHDC, _PosType posX, _PosType posY)
{
	const _PosType tempX = static_cast<_PosType>((static_cast<float>(posX - GAME::ZONE_MIN_X) / GAME::ZONE_X_SIZE) * WINDOW::WINDOW_WIDTH);
	const _PosType tempY = static_cast<_PosType>((static_cast<float>(posY - GAME::ZONE_MIN_Y) / GAME::ZONE_Y_SIZE) * WINDOW::WINDOW_HEIGHT);

	imageOfModel->BitBlt(pHDC,
		tempX,
		tempY
	);
}
#endif

