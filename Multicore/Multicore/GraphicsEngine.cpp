#include "GraphicsEngine.h"



GraphicsEngine::GraphicsEngine()
{
	mWVPBufferID.reg = 0;
	mInstanceBufferID.reg = 2;
}


GraphicsEngine::~GraphicsEngine()
{
}

void GraphicsEngine::InitD3D(HWND hWnd) 
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;
	scd.BufferDesc.Width = 800;
	scd.BufferDesc.Height = 600;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;                                 // windowed/full-screen mode

															// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);

	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	HRESULT res = dev->CreateUnorderedAccessView(pBackBuffer, NULL, &mBackBufferUAV);
	pBackBuffer->Release();


	//devcon->OMSetRenderTargets(1, &backbuffer, NULL);
}

void GraphicsEngine::CleanD3D()
{
	// close and release all existing COM objects
	swapchain->Release();
	dev->Release();
	devcon->Release();
}

void GraphicsEngine::InitPipeline()
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	CreateShader(ComputeShader, &mComputeShader, L"ComputeShader.hlsl", "main", nullptr, NULL);
	
}

void GraphicsEngine::InitGraphics()
{
	Vertex OurVertices[] =
	{
		{ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f},
		{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f}, //Framsidan
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f},
		{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f},

		//{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f }, //4
		//{ -0.5f, -0.5, 0.5f, 1.0f, 1.0f }, //5  Baksidan
		//{ 0.5f, -0.5f, 0.5f, 0.0f, 1.0f }, //6
		//{ 0.5f, 0.5f, 0.5f, 0.0f, 0.0f },  //7


		//{ -0.5f, 0.5f, -0.5f, 0.0f,0.0f},  //ovanpå 8
		//{ -0.5f, 0.5, 0.5f, 0.0f, 1.0f},   //// 9
		//{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f},   ////10
		//{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f },  //11

		//{ -0.5f, -0.5f, 0.5f, 0.0f,0.0f},  //under 12
		//{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f },   //// 13
		//{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f },   ////14
		//{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f },  //15

		//{ -0.5f, 0.5f, -0.5f, 0.0f,0.0f},  //vänster 16
		//{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f },   //// 17
		//{ -0.5f, -0.5f, 0.5f, 1.0f, 1.0f },   ////18
		//{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f },  //19

		//{ 0.5f, 0.5f, 0.5f, 0.0f,0.0f },  //höger 20
		//{ 0.5f, -0.5, 0.5f, 0.0f, 1.0f },   //// 21
		//{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f },   ////22
		//{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f },  //23
	};
	ID3D11Buffer* tVB;
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	//vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(OurVertices);
	vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	//vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	vbd.StructureByteStride = sizeof(Vertex);

	int tVbufferHandle = CreateBuffer(vbd, &OurVertices);
	//PushToDevice(tVbufferHandle, &OurVertices, sizeof(OurVertices));
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	
	HRESULT res = dev->CreateUnorderedAccessView(mBuffers[tVbufferHandle], NULL, &mVertexBufferUAV);
	mBuffers[tVbufferHandle]->Release();
}

void GraphicsEngine::RenderFrame(void)
{
	float color[] = {0.0f,0.2f,0.4f,1.0f};
	
	ID3D11UnorderedAccessView* uav[] = { mBackBufferUAV, mVertexBufferUAV };
	devcon->CSSetUnorderedAccessViews(0, 2, uav, NULL);

	SetActiveShader(ComputeShader, mComputeShader);
	devcon->Dispatch(25, 20, 1);
	SetActiveShader(ComputeShader, nullptr);
	swapchain->Present(0, 0);
}

bool GraphicsEngine::CreateShader(ShaderType pType, void* oShaderHandle, LPCWSTR pShaderFileName, LPCSTR pEntryPoint, ID3D11InputLayout** oInputLayout, D3D11_INPUT_ELEMENT_DESC pInputDescription[])
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob *tShader;

	switch (pType)
	{
	case GraphicsEngine::VertexShader:
	{
		D3DCompileFromFile(pShaderFileName, 0, 0, pEntryPoint, "vs_5_0", shaderFlags, 0, &tShader, 0);
		HRESULT res = dev->CreateVertexShader(tShader->GetBufferPointer(), tShader->GetBufferSize(), NULL, (ID3D11VertexShader**)oShaderHandle);

		res = dev->CreateInputLayout(pInputDescription, 2, tShader->GetBufferPointer(), tShader->GetBufferSize(), oInputLayout);
		int a = 1;
	}break;
	case GraphicsEngine::PixelShader:
	{
		D3DCompileFromFile(pShaderFileName, 0, 0, pEntryPoint, "ps_5_0", shaderFlags, 0, &tShader, 0);
		dev->CreatePixelShader(tShader->GetBufferPointer(), tShader->GetBufferSize(), NULL, (ID3D11PixelShader**)oShaderHandle);
	}break;
	case GraphicsEngine::ComputeShader:
	{
		D3DCompileFromFile(pShaderFileName, 0, 0, pEntryPoint, "cs_5_0", shaderFlags, 0, &tShader, 0);
		dev->CreateComputeShader(tShader->GetBufferPointer(), tShader->GetBufferSize(), NULL, (ID3D11ComputeShader**)oShaderHandle);
	}break;
	default:
		break;
	}
	return true;
	}

bool GraphicsEngine::SetActiveShader(ShaderType pType, void* oShaderHandle)
{
	switch (pType)
	{
	case GraphicsEngine::VertexShader:
	{
		VertexShaderComponents* tVS = (VertexShaderComponents*)oShaderHandle;
		devcon->VSSetShader(tVS->ShaderHandle, 0, 0);
		devcon->IASetInputLayout(mVertexShader->InputLayout);
	}break;
	case GraphicsEngine::PixelShader:
	{
		devcon->PSSetShader((ID3D11PixelShader*)oShaderHandle, 0, 0);
	}break;
	case GraphicsEngine::ComputeShader:
	{
		devcon->CSSetShader((ID3D11ComputeShader*)oShaderHandle, 0, 0);
	}break;
	default:
		break;
	}
	return true;
}

int GraphicsEngine::CreateBuffer(D3D11_BUFFER_DESC pBufferDescription, void* pInitialData)
{
	ID3D11Buffer* tHolder;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pInitialData;
	HRESULT res = dev->CreateBuffer(&pBufferDescription, &InitData, &tHolder);
	if (res != S_OK)
	{
		return -1;
	}
	mBuffers.push_back(tHolder);
	int retvalue = mBuffers.size() - 1;
	return retvalue;
}

bool GraphicsEngine::PushToDevice(int pBufferID, void* pDataStart, unsigned int pSize)
{
	D3D11_MAPPED_SUBRESOURCE tMS;
	if (pBufferID > mBuffers.size() - 1 || pBufferID < 0)
	{
#ifdef DEBUG
		cout << "Buffer ID is outside the buffer array";
#endif // DEBUG
		return false;
	}
	ID3D11Buffer* tBufferHandle = mBuffers.at(pBufferID);

	devcon->Map(tBufferHandle, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
	memcpy(tMS.pData, pDataStart, pSize);
	devcon->Unmap(tBufferHandle, NULL);

	return true;
}


bool GraphicsEngine::PushToDevice(int pBufferID, void* pDataStart, unsigned int pSize, unsigned int pRegister, ShaderType pType)
{
	bool res = PushToDevice(pBufferID, pDataStart, pSize);
	if (res != true)
{
		return false;
	}
	switch (pType)
	{
	case GraphicsEngine::VertexShader:
	{
		devcon->VSSetConstantBuffers(pRegister, 1, &mBuffers.at(pBufferID));
	}
		break;
	case GraphicsEngine::PixelShader:
	{
		devcon->PSSetConstantBuffers(pRegister, 1, &mBuffers.at(pBufferID));
	}
		break;
	default:
		break;
	}
}