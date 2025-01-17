#pragma once

//---
#if USE_RENDERMODEL_MANAGER == 1
#define WONSY_RENDERMODEL_MANAGER 
#endif
//---

#ifdef WONSY_RENDERMODEL_MANAGER
namespace WonSY
{
	__interface InterfaceBaseModel
	{
	public:
		virtual void Render(HDC pHDC, _PosType posX, _PosType posY) = 0;
	};
	
	class BaseModel : public InterfaceBaseModel
	{
	public:
		BaseModel(const std::wstring inResourcePath);
		BaseModel() = delete;
		virtual ~BaseModel() = 0;
	
	protected:
		std::unique_ptr<CImage> imageOfModel;
	};
	
	class BitModel final : public BaseModel
	{
	public:
		BitModel(const std::wstring& inResourcePath);
		virtual ~BitModel() override final = default;
	
	public:
		virtual void Render(HDC pHDC, _PosType posX, _PosType posY) override final;
	};
	
	enum class RENDER_MODEL_TYPE
	{
		BACKGROUND
	};
	
	class RenderModelManager
	{
		std::vector<BaseModel*> renderModelCont;
	
	public:
		RenderModelManager();
		~RenderModelManager();
	
	public:
		BaseModel* GetRenderModel(RENDER_MODEL_TYPE);
	};
}
#endif