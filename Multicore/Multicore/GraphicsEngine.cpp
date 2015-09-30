#include "GraphicsEngine.h"



GraphicsEngine::GraphicsEngine()
{
	mWVPBufferID.reg = 0;
	mWVPBufferIDVS.reg = 0;
	mInstanceBufferID.reg = 2;
	mCamerManager = new CameraManager();
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
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
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
		D3D11_CREATE_DEVICE_DEBUG,
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
	res = dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();


	devcon->OMSetRenderTargets(1, &backbuffer, NULL);
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	devcon->RSSetViewports(1, &viewport);
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
	CreateShader(PixelShader, &mPixelShader, L"PixelShader.hlsl", "PShader", nullptr, NULL);
	CreateShader(VertexShader, &mVertexShader->ShaderHandle, L"VertexShader.hlsl", "VShader", &mVertexShader->InputLayout, ied);
}

void GraphicsEngine::InitGraphics()
{
	Vertex OurVertices[] =
	{
		{ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f},
		{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f}, //Framsidan
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f},
		{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f},

		{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f }, //4
		{ -0.5f, -0.5, 0.5f, 1.0f, 1.0f }, //5  Baksidan
		{ 0.5f, -0.5f, 0.5f, 0.0f, 1.0f }, //6
		{ 0.5f, 0.5f, 0.5f, 0.0f, 0.0f },  //7

		
		{ -0.5f, 0.5f, -0.5f, 0.0f,0.0f},  //ovanp� 8
		{ -0.5f, 0.5, 0.5f, 0.0f, 1.0f},   //// 9
		{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f},   ////10
		{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f },  //11

		{ -0.5f, -0.5f, 0.5f, 0.0f,0.0f},  //under 12
		{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f },   //// 13
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f },   ////14
		{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f },  //15

		{ -0.5f, 0.5f, -0.5f, 0.0f,0.0f},  //v�nster 16
		{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f },   //// 17
		{ -0.5f, -0.5f, 0.5f, 1.0f, 1.0f },   ////18
		{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f },  //19

		{ 0.5f, 0.5f, 0.5f, 0.0f,0.0f },  //h�ger 20
		{ 0.5f, -0.5, 0.5f, 0.0f, 1.0f },   //// 21
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f },   ////22
		{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f },  //23
	};

	int OurIndices[] =
	{
		2,1,0,
		2,0,3,

		4,5,6,
		4,6,7,

		8,9,10,
		8,10,11,

		12,13,14,
		12,14,15,

		16,17,18,
		16,18,19,

		20,21,22,
		20,22,23,
	};
	//FOR VERTEX BUFFER FOR VERTEXSHADER
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(OurVertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ID3D11Buffer* tHolder1;
	HRESULT res = dev->CreateBuffer(&bd, NULL, &tHolder1);
	if (res != S_OK)
	{
		return;
	}
	mBuffers.push_back(tHolder1);
	mVertexBufferIDVS = mBuffers.size() - 1;
	PushToDevice(mVertexBufferIDVS, OurVertices, sizeof(OurVertices));
	//FOR VERTEX BUFFER
	ID3D11Buffer* tVB;
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	//vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(OurVertices);
	vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	//vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	vbd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = OurVertices;
	res = dev->CreateBuffer(&vbd, &InitData, &mVertexBufferID);

	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
	ZeroMemory(&rvDesc, sizeof(rvDesc));
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	rvDesc.BufferEx.FirstElement = 0;
	rvDesc.Format = DXGI_FORMAT_UNKNOWN;
	rvDesc.BufferEx.NumElements = vbd.ByteWidth / vbd.StructureByteStride;
	res = dev->CreateShaderResourceView(mVertexBufferID, &rvDesc, &mResourceView);
	//mBuffers[tVbufferHandle]->Release();

	//FOR INDEX BUFFER
	ZeroMemory(&vbd, sizeof(vbd));
	//vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(OurIndices);
	vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	//vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	vbd.StructureByteStride = sizeof(int) ;
	D3D11_SUBRESOURCE_DATA InitDataIndex;
	InitDataIndex.pSysMem = OurIndices;
	res = dev->CreateBuffer(&vbd, &InitDataIndex, &mIndexBufferHandle);

	ZeroMemory(&rvDesc, sizeof(rvDesc));
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	rvDesc.BufferEx.FirstElement = 0;
	rvDesc.Format = DXGI_FORMAT_UNKNOWN;
	rvDesc.BufferEx.NumElements = vbd.ByteWidth / vbd.StructureByteStride;
	res = dev->CreateShaderResourceView(mIndexBufferHandle, &rvDesc, &mIndexResourceView);
	//FOR MATRIX BUFFER
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.ByteWidth = sizeof(MatrixBufferType);
	
	ID3D11Buffer* tHolder;

	res = dev->CreateBuffer(&vbd, NULL, &tHolder);
	if (res != S_OK)
	{
		return;
	}
	mBuffers.push_back(tHolder);
	mWVPBufferID.bufferID = mBuffers.size() - 1;
	
	//MATRIX BUFFER FOR VERTEX SHADER
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(MatrixBufferType);
	vbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	ID3D11Buffer* tHolder2;
	res = dev->CreateBuffer(&vbd, NULL, &tHolder2);
	if (res != S_OK)
	{
		return;
	}
	mBuffers.push_back(tHolder2);
	mWVPBufferIDVS.bufferID = mBuffers.size() - 1;
	//mWVPBufferIDVS.bufferID = CreateBuffer(vbd, nullptr); //Might be bugging if u dont have any initial data
	MatrixBufferType tBufferInfo;
	XMStoreFloat4x4(&tBufferInfo.world, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&tBufferInfo.view, XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f)), XMLoadFloat3(&XMFLOAT3(0, 0, 1)), XMLoadFloat3(&XMFLOAT3(0, 1, 0)))));
	XMStoreFloat4x4(&tBufferInfo.projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(45.0f, SCREEN_HEIGHT / SCREEN_WIDTH, 0.1f, 100)));
	PushToDevice(mWVPBufferIDVS.bufferID, &tBufferInfo, sizeof(tBufferInfo), mWVPBufferIDVS.reg, VertexShader);

	SetActiveShader(VertexShader, mVertexShader);
	SetActiveShader(PixelShader, mPixelShader);
}

void GraphicsEngine::Update(float pDT, UserCMD pUserCMD)
{
	mCamerManager->HandleUserCMD(pDT,pUserCMD);
	mCamerManager->UpdateCamera();
}

void GraphicsEngine::RenderFrame(void)
{


	MatrixBufferType tBufferInfo;
	tBufferInfo.world = mCamerManager->GetWorld();
	tBufferInfo.view = mCamerManager->GetView();
	tBufferInfo.projection = mCamerManager->GetProjection();
	PushToDevice(mWVPBufferIDVS.bufferID, &tBufferInfo, sizeof(MatrixBufferType), mWVPBufferIDVS.reg, VertexShader);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &mBuffers[mVertexBufferIDVS],&stride, &offset);//Might be wrong
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	float color[] = { 0.0f,0.2f,0.4f,1.0f };
	////For compute shader draw
	
	SetActiveShader(ComputeShader, mComputeShader);
	ID3D11ShaderResourceView* tRViews[] = { mResourceView ,mIndexResourceView };
	int s = ARRAYSIZE(tRViews);
	devcon->CSSetShaderResources(0, ARRAYSIZE(tRViews), tRViews);

	ID3D11UnorderedAccessView* uav[] = { mBackBufferUAV };
	devcon->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, NULL);
	mMatrixBufferInfo.world = mCamerManager->GetWorld();
	mMatrixBufferInfo.view = mCamerManager->GetView();
	mMatrixBufferInfo.projection = mCamerManager->GetProjection();
	PushToDevice(mWVPBufferID.bufferID, &mMatrixBufferInfo, sizeof(mMatrixBufferInfo),mWVPBufferID.reg, ComputeShader);

	devcon->Dispatch(25, 25, 1);
	SetActiveShader(ComputeShader, nullptr);
	ID3D11ShaderResourceView* tRemoveRViews[] = { nullptr };
	devcon->CSSetShaderResources(0, ARRAYSIZE(tRemoveRViews), tRemoveRViews);
	ID3D11UnorderedAccessView* tRemoveUAV[] = { nullptr };
	devcon->CSSetUnorderedAccessViews(0, ARRAYSIZE(tRemoveUAV), tRemoveUAV, NULL);
	//devcon->Draw(24, 0);

	swapchain->Present(0, 0);
	devcon->ClearRenderTargetView(backbuffer, color);
}

bool GraphicsEngine::CreateShader(ShaderType pType, void* oShaderHandle, LPCWSTR pShaderFileName, LPCSTR pEntryPoint, ID3D11InputLayout** oInputLayout, D3D11_INPUT_ELEMENT_DESC pInputDescription[])
{
	DWORD shaderFlags = 
		D3DCOMPILE_ENABLE_STRICTNESS |
		D3DCOMPILE_IEEE_STRICTNESS |
		//D3DCOMPILE_WARNINGS_ARE_ERRORS |
		D3DCOMPILE_PREFER_FLOW_CONTROL;
#if defined( DEBUG ) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG ;
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
	case GraphicsEngine::ComputeShader:
	{
		devcon->CSSetConstantBuffers(pRegister, 1, &mBuffers.at(pBufferID));
	}
	break;
	default:
		break;
	}
}