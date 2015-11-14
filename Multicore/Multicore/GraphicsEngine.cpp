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
    mWindow = hWnd;
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


	//devcon->OMSetRenderTargets(1, &backbuffer, NULL); //For Vertex Shader
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	devcon->RSSetViewports(1, &viewport);
    mTimer = new D3D11Timer(dev, devcon);
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
		{ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f,0.0f,-1.0f}, //Lilla boxen
		{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f, 0.0f,0.0f,-1.0f }, //Framsidan
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,0.0f,-1.0f },
        { -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f,0.0f,-1.0f }, //Lilla boxen
        { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,0.0f,-1.0f },
		{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,0.0f,-1.0f },

		{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f , 0.0f,0.0f,1.0f }, //4
		{ -0.5f, -0.5, 0.5f, 1.0f, 1.0f , 0.0f,0.0f,1.0f }, //5  Baksidan
		{ 0.5f, -0.5f, 0.5f, 0.0f, 1.0f , 0.0f,0.0f,1.0f }, //6
		{ 0.5f, 0.5f, 0.5f, 0.0f, 0.0f  , 0.0f,0.0f,1.0f },  //7

		
		{ -0.5f, 0.5f, -0.5f, 0.0f,0.0f, 0.0f,1.0f,0.0f },  //ovanpå 8
		{ -0.5f, 0.5, 0.5f, 0.0f, 1.0f, 0.0f,1.0f,0.0f },   //// 9
		{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,1.0f,0.0f },   ////10
		{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f , 0.0f,1.0f,0.0f },  //11

		{ -0.5f, -0.5f, 0.5f, 0.0f,0.0f, 0.0f,-1.0f,0.0f },  //under 12
		{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f , 0.0f,-1.0f,0.0f },   //// 13
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f , 0.0f,-1.0f,0.0f },   ////14
		{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f , 0.0f,-1.0f,0.0f },  //15

		{ -0.5f, 0.5f, -0.5f, 0.0f,0.0f, -1.0f,0.0f,0.0f },  //vänster 16
		{ -0.5f, -0.5, -0.5f, 0.0f, 1.0f, -1.0f,0.0f,0.0f },   //// 17
		{ -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, -1.0f,0.0f,0.0f },   ////18
		{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f,0.0f,0.0f },  //19

		{ 0.5f, 0.5f, 0.5f, 0.0f,0.0f , 1.0f,0.0f,0.0f },  //höger 20
		{ 0.5f, -0.5, 0.5f, 0.0f, 1.0f , 1.0f,0.0f,0.0f },   //// 21
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f , 1.0f,0.0f,0.0f },   ////22
		{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f , 1.0f,0.0f,0.0f },  //23

		{ -1.5f, 1.5f, -1.5f, 0.0f, 0.0f, 0.0f,0.0f,1.0f }, //Stora boxen, Inverterade normaler då boxen ska "peka" innåt
		{ -1.5f, -1.5, -1.5f, 0.0f, 1.0f, 0.0f,0.0f,1.0f }, //Framsidan
		{ 1.5f, -1.5f, -1.5f, 1.0f, 1.0f, 0.0f,0.0f,1.0f },
		{ 1.5f, 1.5f, -1.5f, 1.0f, 0.0f, 0.0f,0.0f,1.0f },

		{ -1.5f, 1.5f, 1.5f, 1.0f, 0.0f , 0.0f,0.0f,-1.0f }, //4
		{ -1.5f, -1.5, 1.5f, 1.0f, 1.0f , 0.0f,0.0f,-1.0f }, //5  Baksidan
		{ 1.5f, -1.5f, 1.5f, 0.0f, 1.0f , 0.0f,0.0f,-1.0f }, //6
		{ 1.5f, 1.5f, 1.5f, 0.0f, 0.0f  , 0.0f,0.0f,-1.0f },  //7


		{ -1.5f, 1.5f, -1.5f, 0.0f,0.0f, 0.0f,-1.0f,0.0f },  //ovanpå 8
		{ -1.5f, 1.5, 1.5f, 0.0f, 1.0f, 0.0f,-1.0f,0.0f },   //// 9
		{ 1.5f, 1.5f, 1.5f, 1.0f, 1.0f, 0.0f,-1.0f,0.0f },   ////10
		{ 1.5f, 1.5f, -1.5f, 1.0f, 0.0f , 0.0f,-1.0f,0.0f },  //11

		{ -1.5f, -1.5f, 1.5f, 0.0f,0.0f, 0.0f,1.0f,0.0f },  //under 12
		{ -1.5f, -1.5, -1.5f, 0.0f, 1.0f , 0.0f,1.0f,0.0f },   //// 13
		{ 1.5f, -1.5f, -1.5f, 1.0f, 1.0f , 0.0f,1.0f,0.0f },   ////14
		{ 1.5f, -1.5f, 1.5f, 1.0f, 0.0f , 0.0f,1.0f,0.0f },  //15

		{ -1.5f, 1.5f, -1.5f, 0.0f,0.0f, 1.0f,0.0f,0.0f },  //vänster 16
		{ -1.5f, -1.5, -1.5f, 0.0f, 1.0f, 1.0f,0.0f,0.0f },   //// 17
		{ -1.5f, -1.5f, 1.5f, 1.0f, 1.0f, 1.0f,0.0f,0.0f },   ////18
		{ -1.5f, 1.5f, 1.5f, 1.0f, 0.0f, 1.0f,0.0f,0.0f },  //19

		{ 1.5f, 1.5f, 1.5f, 0.0f,0.0f , -1.0f,0.0f,0.0f },  //höger 20
		{ 1.5f, -1.5, 1.5f, 0.0f, 1.0f , -1.0f,0.0f,0.0f },   //// 21
		{ 1.5f, -1.5f, -1.5f, 1.0f, 1.0f , -1.0f,0.0f,0.0f },   ////22
		{ 1.5f, 1.5f, -1.5f, 1.0f, 0.0f , -1.0f,0.0f,0.0f },  //23 asd
	};

	int OurIndices[] =
	{
		2,1,0, //Smal Box
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

		26,25,24, //Big Box
		26,24,27,

		28,29,30,
		28,30,31,

		32,33,34,
		32,34,35,

		36,37,38,
		36,38,39,

		40,41,42,
		40,42,43,

		44,45,46,
		44,46,47,
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

	//FOR SPHERE PRIMITIV
	Sphere tSphere;
	tSphere.origin = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	tSphere.radius = 0.2f;

	ZeroMemory(&vbd, sizeof(vbd));
	//vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(tSphere);
	vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	//vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	vbd.StructureByteStride = sizeof(tSphere);
	D3D11_SUBRESOURCE_DATA InitDataSphere;
	InitDataSphere.pSysMem = &tSphere;
	res = dev->CreateBuffer(&vbd, &InitDataSphere, &mSphereBufferHandle);

	ZeroMemory(&rvDesc, sizeof(rvDesc));
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	rvDesc.BufferEx.FirstElement = 0;
	rvDesc.Format = DXGI_FORMAT_UNKNOWN;
	rvDesc.BufferEx.NumElements = vbd.ByteWidth / vbd.StructureByteStride;
	res = dev->CreateShaderResourceView(mSphereBufferHandle, &rvDesc, &mSphereResourceView);

	//FOR POINTLIGHTS
	for (int i = 0; i < 5; i++)
	{
		pointLightPositions.push_back(XMFLOAT3(sin(i*2 * PI/ 5) * 1, cos(i * 2 * PI / 5) * 1,0));
	}
	for (int i = 0; i < 5; i++)
	{
		pointLightPositions.push_back(XMFLOAT3(0, cos(i * 2 * PI / 5) * 1, sin(i * 2 * PI / 5) * 1));
	}
	//pointLightPositions.push_back(XMFLOAT3(-1.5f, 0.0f, 0.0f));
	//pointLightPositions.push_back(XMFLOAT3(1.5f, 0.9f, 0.0f));
	//pointLightPositions.push_back(XMFLOAT3(0.9f, 1.5f, 0.0f));
	//pointLightPositions.push_back(XMFLOAT3(0.9f, -1.5f, 0.0f));
	//pointLightPositions.push_back(XMFLOAT3(0.0f, 0.0f, 1.5f));
	//pointLightPositions.push_back(XMFLOAT3(0.0f, 0.0f, -1.5f));

	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(XMFLOAT3)*pointLightPositions.size();
	vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE; //| D3D11_BIND_UNORDERED_ACCESS;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	vbd.StructureByteStride = sizeof(XMFLOAT3);
	D3D11_SUBRESOURCE_DATA InitDataPLight;
	InitDataPLight.pSysMem = pointLightPositions.data();
	res = dev->CreateBuffer(&vbd, &InitDataPLight, &mPointlightBufferHandle);

	D3D11_MAPPED_SUBRESOURCE tMS;
	devcon->Map(mPointlightBufferHandle, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
	memcpy(tMS.pData, pointLightPositions.data(), sizeof(XMFLOAT3)*pointLightPositions.size());
	devcon->Unmap(mPointlightBufferHandle, NULL);
   //
	ZeroMemory(&rvDesc, sizeof(rvDesc));
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	rvDesc.BufferEx.FirstElement = 0;
	rvDesc.Format = DXGI_FORMAT_UNKNOWN;
	rvDesc.BufferEx.NumElements = vbd.ByteWidth / vbd.StructureByteStride;
	res = dev->CreateShaderResourceView(mPointlightBufferHandle, &rvDesc, &mPointlightResourceView);




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

	//For texture and 
	HRESULT hr;
	mCubesTexture = 0;
	hr = CreateDDSTextureFromFile(dev, L"1test.dds", nullptr, &mCubesTexture);
	if (FAILED(hr))
	{
		return;
	}
	D3D11_SAMPLER_DESC texSamDesc;
	texSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	texSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	texSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	texSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	texSamDesc.MipLODBias = 0;
	texSamDesc.MaxAnisotropy = 1;
	texSamDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	texSamDesc.BorderColor[0] = 1.0f;
	texSamDesc.BorderColor[1] = 1.0f;
	texSamDesc.BorderColor[2] = 1.0f;
	texSamDesc.BorderColor[3] = 1.0f;
	texSamDesc.MinLOD = -3.402823466e+38F; // -FLT_MAX
	texSamDesc.MaxLOD = 3.402823466e+38F; // FLT_MAX


	hr = dev->CreateSamplerState(&texSamDesc, &mCubesTexSamplerState);
	if (FAILED(hr))
	{
		return;
	}

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
	static float pMove = 0.0f;
	pMove += 0.01;
	vector<XMFLOAT3> newLightPos;
	newLightPos.resize(pointLightPositions.size());
	float s = sin(pMove);
	float c = cos(pMove);
	for (size_t i = 0; i < 5; i++)
	{

		newLightPos[i] = XMFLOAT3(pointLightPositions[i].x * c - pointLightPositions[i].y * s, pointLightPositions[i].x * s + pointLightPositions[i].y * c, pointLightPositions[i].z);

	}
	for (size_t i = 5; i < 10; i++)
	{

		newLightPos[i] = XMFLOAT3(pointLightPositions[i].x , pointLightPositions[i].z * s + pointLightPositions[i].y * c, pointLightPositions[i].z * c - pointLightPositions[i].y * s);

	}
	D3D11_MAPPED_SUBRESOURCE tMS;
	devcon->Map(mPointlightBufferHandle, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
	memcpy(tMS.pData, newLightPos.data(), sizeof(XMFLOAT3) * newLightPos.size());
	devcon->Unmap(mPointlightBufferHandle, NULL);
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

	//devcon->CSSetShaderResources(4, 1, &mCubesTexture);
	devcon->CSSetSamplers(0, 1, &mCubesTexSamplerState);

	ID3D11ShaderResourceView* tRViews[] = { mResourceView ,mIndexResourceView, mSphereResourceView, mPointlightResourceView, mCubesTexture };

	devcon->CSSetShaderResources(0, ARRAYSIZE(tRViews), tRViews);

	ID3D11UnorderedAccessView* uav[] = { mBackBufferUAV };
	devcon->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, NULL);
	mMatrixBufferInfo.world = mCamerManager->GetWorld();
	mMatrixBufferInfo.view = mCamerManager->GetView();
	mMatrixBufferInfo.projection = mCamerManager->GetProjection();
	PushToDevice(mWVPBufferID.bufferID, &mMatrixBufferInfo, sizeof(mMatrixBufferInfo),mWVPBufferID.reg, ComputeShader);

    mTimer->Start();
	devcon->Dispatch(25, 25, 1); //Call to the computeshader for raytracing
    mTimer->Stop();

	SetActiveShader(ComputeShader, nullptr);
	ID3D11ShaderResourceView* tRemoveRViews[] = { nullptr };
	devcon->CSSetShaderResources(0, ARRAYSIZE(tRemoveRViews), tRemoveRViews);
	ID3D11UnorderedAccessView* tRemoveUAV[] = { nullptr };
	devcon->CSSetUnorderedAccessViews(0, ARRAYSIZE(tRemoveUAV), tRemoveUAV, NULL);
	//devcon->Draw(24, 0);

	swapchain->Present(0, 0);
	devcon->ClearRenderTargetView(backbuffer, color);

    char title[256];
    sprintf_s(
        title,
        sizeof(title),
        "BTH - Raytracer - Dispatch time: %f ms",
        mTimer->GetTime()
        );
    SetWindowTextA(mWindow, title);
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
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
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