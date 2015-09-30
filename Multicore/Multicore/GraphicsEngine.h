#pragma once
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <windows.h>
#include <windowsx.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <vector>
#include "DDSTextureLoader.h"
#include "Globals.h"
#include "CameraManager.h"

#ifdef DEBUG
	#include <iostream>
#endif // DEBUG



using namespace DirectX;
using namespace std;

struct Vertex
{
	float x, y, z;
	float texcoord[2];
};

struct MatrixBufferType
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

struct MovementBufferType
{
	float time;
	XMFLOAT3 filler;
};

struct InstanceBufferType
{
	 XMMATRIX translationMatrices;
	 float color[4];
};


class GraphicsEngine
{
private:
	struct VertexShaderComponents
	{
		ID3D11VertexShader* ShaderHandle;
		ID3D11InputLayout * InputLayout;
	};
public:
	GraphicsEngine();
	~GraphicsEngine();
	// global declarations

	IDXGISwapChain *swapchain;           
	ID3D11Device *dev;                   
	ID3D11DeviceContext *devcon;         
	ID3D11RenderTargetView *backbuffer;  
	
	ID3D11Texture2D *mDepthBuffer;
	ID3D11DepthStencilView *mDepthView;


	XMMATRIX mTranslationMatrices[5];
	vector <InstanceBufferType> mInstanceBuffer;
	float time;

	void InitD3D(HWND hWnd);     // sets 
	void InitPipeline();
	void CleanD3D(void);         // close
	void RenderFrame(void);
	void InitGraphics();
	void Update(float pDT, UserCMD pUserCMD);


private:
	enum ShaderType { VertexShader, PixelShader, ComputeShader };
	bool CreateShader(ShaderType pType, void* oShaderHandle, LPCWSTR pShaderFileName, LPCSTR pEntryPoint, ID3D11InputLayout** oInputLayout, D3D11_INPUT_ELEMENT_DESC pInputDescription[]);
	bool SetActiveShader(ShaderType pType, void* oShaderHandle);
	int CreateBuffer(D3D11_BUFFER_DESC pBufferDescription, void* pInitialData);
	bool PushToDevice(int pBufferID, void* pDataStart, unsigned int pSize);
	bool PushToDevice(int pBufferID, void* pDataStart, unsigned int pSize, unsigned int pRegister, ShaderType pType);

	//Variables
	ID3D11ComputeShader* mComputeShader;
	ID3D11PixelShader* mPixelShader;
	VertexShaderComponents* mVertexShader = new VertexShaderComponents;
	vector< ID3D11Buffer*> mBuffers; //int id
	MatrixBufferType mMatrixBufferInfo;

	CameraManager* mCamerManager;
	int mIndexBufferID;
	int mVertexBufferIDVS;
	struct ConstantBufferType
	{
		int bufferID;
		int reg;
	};	

	ConstantBufferType mWVPBufferID;
	ConstantBufferType mWVPBufferIDVS;
	ConstantBufferType mInstanceBufferID;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mVertexBufferID;
	ID3D11Buffer* mIndexBufferHandle;
	ID3D11UnorderedAccessView* mBackBufferUAV;
	ID3D11UnorderedAccessView* mVertexBufferUAV;
	ID3D11UnorderedAccessView* mMatrixBufferUAV;
	ID3D11ShaderResourceView* mResourceView;
	ID3D11ShaderResourceView* mIndexResourceView;
	ID3D11ShaderResourceView* mMatrixResourceView;
	
	ID3D11ShaderResourceView* mCubesTexture;
	ID3D11SamplerState* mCubesTexSamplerState;
};

