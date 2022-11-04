#include "pch.h"
#include "dxh.h"

#include "ImageLoader.h"

DXHandler::DXHandler(HWND handle)
{
	if (!SetupDirectX(handle))
	{
		util::ErrorMessageBox("Failed to set up DirectX.");
		exit(-1);
	}
	
	if (!SetupPipeline(handle))
	{
		util::ErrorMessageBox("Failed to set up pipeline.");
		exit(-2);
	}

	//ID3D11Debug* pDebug;
	//device->QueryInterface(IID_ID3D11DEBUG, (VOID**)(&pDebug));
	//if (pDebug)
	//{
	//	pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//	pDebug->Release();
	//}

	flagged = false;
}

DXHandler::~DXHandler()
{
	device->Release();
	context->Release();
	swapchain->Release();
	rendertargetview->Release();
	dsTexture->Release();
	dsv->Release();
	dsState->Release();
	vertexshader->Release();
	pixelshader->Release();
	inputlayout->Release();
	bMatrix->Release();
	bMaterial->Release();
	bLighting->Release();
	bVertex->Release();
	bIndex->Release();
}

bool DXHandler::SetupDirectX(HWND handle)
{
	if (!CreateDeviceAndSwapChain(handle, swapchain, device, context)) {
		return false;
	}
	if (!CreateRenderTargetView(device, swapchain, rendertargetview))
	{
		return false;
	}
	
	RECT rc{};
	GetClientRect(handle, &rc);
	//Sets initial viewport to be the same as the handle client area
	SetViewport(this->context, FLOAT(rc.right - rc.left), FLOAT(rc.bottom - rc.top), 0, 0, 1, 0);

	return true;
}

bool DXHandler::SetupPipeline(HWND handle)
{
	//Get window client area for later
	RECT rc{};
	GetClientRect(handle, &rc);

	if (!CreateShaders(device, context, vertexshader, pixelshader, inputlayout)) {
		util::ErrorMessageBox("Failed to compile shaders.");
		return false;
	}

	if (!CreateMesh(device, context, bVertex, bIndex))
	{
		util::ErrorMessageBox("Failed to create mesh.");
		return false;
	}

	if (!SetupMatrixBuffer(device, context, bMatrix, wvp))
	{
		util::ErrorMessageBox("Could not set up wvp matrix.");
		return false;
	}

	if (!SetupLightBuffer(device, context, bLighting))
	{
		util::ErrorMessageBox("could not set up light.");
		return false;
	}

	if (!SetupMaterialBuffer(device, context, bMaterial))
	{
		util::ErrorMessageBox("Failed to set up material buffer");
		return false;
	}

	if (!CreateVertexBuffer(device, context, bVertex, mesh))
	{
		util::ErrorMessageBox("Failed to set up Vertex Buffer");
		return false;
	}
	if (!CreateIndexBuffer(device, context, bIndex, mesh))
	{
		util::ErrorMessageBox("Failed to set up Index Buffer");
		return false;
	}

	if (!CreateRasterizerState(device, context, rasterizerstate))
	{
		util::ErrorMessageBox("Failed to create rasterizer state.");
		return false;
	}

	if (!CreateDepthStencil(device, rc.right - rc.left, rc.bottom - rc.top, dsTexture, dsv, dsState))
	{
		util::ErrorMessageBox("Failed to create Depth Stencil");
		return false;
	}

	if (!CreateTexture(device, context, gTexture, gTextureView))
	{
		util::ErrorMessageBox("Failed to create texture or resource view!");
		return false;
	}
	if (!CreateSamplerState(device, context, gSamplerState))
	{
		util::ErrorMessageBox("Failed to create sampler state!");
		return false;
	}

	SetTopology(context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

// **********************************************************************************************************
// INTERNAL/HELPER FUNCTIONS
// **********************************************************************************************************

std::string DXHandler::ReadShaderData(std::string filepath){
	
	std::string data;
	std::ifstream fstr(filepath, std::ios::binary | std::ios::ate);
	if (!fstr.is_open())
	{
		util::ErrorMessageBox("Shader file \"" + filepath + "\" could not be opened.");
		return "";
	}

	fstr.seekg(0, std::ios::end);
	data.reserve(static_cast<unsigned int>(fstr.tellg()));
	fstr.seekg(0, std::ios::beg);

	data.assign((std::istreambuf_iterator<char>(fstr)), std::istreambuf_iterator<char>());
	
	return data;
}

bool DXHandler::CreateDeviceAndSwapChain(HWND handle, IDXGISwapChain*& swapchain, ID3D11Device*& device, ID3D11DeviceContext * &context)
{
	DXGI_SWAP_CHAIN_DESC scd{ 0 };
	RECT rc{};
	GetClientRect(handle, &rc);

	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = (unsigned int)(rc.right - rc.left);
	scd.BufferDesc.Height = (unsigned int)(rc.bottom - rc.top);
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferCount = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = handle;
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT hr = 
		D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain, //storage pointer for swapchain
		&device,	//storage pointer for device
		NULL,
		&context //storage pointer for immediate context
	);

	if (FAILED(hr)) {
		util::ErrorMessageBox("Failed to create device and swapchain. Error code: " + LASTERR);
		return false;
	}

	return true;
}

void DXHandler::SetTopology(ID3D11DeviceContext*& context, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP) 
{
	context->IASetPrimitiveTopology(topology);
}

bool DXHandler::CreateRenderTargetView(ID3D11Device*& device, IDXGISwapChain*& swapchain, ID3D11RenderTargetView*& rtv)
{
	ID3D11Texture2D* backbuffer = NULL;
	HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to get back buffer!");
		return false;
	}

	hr = device->CreateRenderTargetView(backbuffer, NULL, &rtv);
	backbuffer->Release();
	
	if (FAILED(hr)) {
		util::ErrorMessageBox("Failed to create Render Target View");
		return false;
	}
	context->OMSetRenderTargets(1, &rtv, NULL);

	return true;
}

bool DXHandler::CreateVertexShader(ID3D11Device*& device, ID3D11DeviceContext *&context, ID3D11VertexShader*& vshader, ID3D11InputLayout *& inputLayout, std::string filepath)
{
	std::string data = ReadShaderData(filepath);

	if (FAILED(device->CreateVertexShader(data.c_str(), data.length(), NULL, &vshader)))
	{
		util::ErrorMessageBox("Could not create vertex shader. Last error: " + LASTERR);
		return false;
	}
	context->VSSetShader(vshader, nullptr, 0);

	CreateInputLayout(device, context, inputLayout, data);

	return true;
}

bool DXHandler::CreatePixelShader(ID3D11Device*& device, ID3D11PixelShader*& pshader, std::string filepath) {
	std::string data = ReadShaderData(filepath);

	if (FAILED(device->CreatePixelShader(data.c_str(), data.length(), NULL, &pshader)))
	{
		util::ErrorMessageBox("Could not create pixel shader. Last error: " + LASTERR);
		return false;
	}
	context->PSSetShader(pshader, nullptr, 0);

	return true;
}

bool DXHandler::CreateShaders(ID3D11Device *& device, ID3D11DeviceContext *& context, ID3D11VertexShader *& vertexshader, ID3D11PixelShader *& pixelshader, ID3D11InputLayout *& inputLayout) 
{
	if (!CreateVertexShader(device, context, vertexshader, inputLayout, "D:/repos/HelloTriangle/Debug/VertexShader.cso"))
		return false;
	if (!CreatePixelShader(device, pixelshader, "D:/repos/HelloTriangle/Debug/PixelShader.cso"))
		return false;
	return true;
}

bool DXHandler::CreateRasterizerState(ID3D11Device *& device, ID3D11DeviceContext *& context, ID3D11RasterizerState*& ppRasterizerState) 
{
	D3D11_RASTERIZER_DESC rzd{};
	rzd.FillMode = D3D11_FILL_SOLID;
	rzd.CullMode = D3D11_CULL_BACK;
	rzd.FrontCounterClockwise = false;
	rzd.MultisampleEnable = true;
	rzd.AntialiasedLineEnable = true;
	rzd.ScissorEnable = false;
	rzd.DepthClipEnable = false;

	HRESULT hr = device->CreateRasterizerState(&rzd, &ppRasterizerState);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Could not create rasterizer state. Last error: " + LASTERR);
		return false;
	}
	context->RSSetState(ppRasterizerState);
	return true;
}


bool DXHandler::CreateConstantBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& cBuffer, UINT byteWidth)
{
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = byteWidth;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, &cBuffer);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create constant buffer! Last error " + LASTERR);
		return false;
	}

	

	return true;
}

bool DXHandler::SetupMatrixBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& cBuffer, dxh::WVP &wvp)
{
	DirectX::XMMATRIX world{}, view{}, projection{};
	

	wvp.world	= DirectX::XMMatrixIdentity();
	wvp.view	= DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH({ camerapos.x, camerapos.y, camerapos.z }, { 0, 0, 0 }, { 0, 1, 0 }));
	wvp.project = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovRH(0.25*RAD, 800/600.0f, 0.1f, 20.0f));

	if (!CreateConstantBuffer(device, context, cBuffer, sizeof(dxh::WVP)))
	{
		return false;
	}
	context->VSSetConstantBuffers(0, 1, &cBuffer);
	return true;
};

bool DXHandler::SetupLightBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& cBuffer) 
{

	if (!CreateConstantBuffer(device, context, cBuffer, sizeof(dxh::SingleLight)))
	{
		util::ErrorMessageBox("Failed to create light buffer! Last error " + LASTERR);
		return false;
	}

	this->light.pos = { 1.0f, 0.0f, -2.0f, 1.0f};
	this->light.color = { 1.0f, 1.0f, 1.0f, 1.0f};
	this->light.camerapos = { camerapos.x, camerapos.y, camerapos.z, 1.0f };
	this->light.ambient = 0.2f;

	context->PSSetConstantBuffers(0, 1, &cBuffer);

	return true;
}

bool DXHandler::SetupMaterialBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& cBuffer) 
{
	if (!CreateConstantBuffer(device, context, cBuffer, sizeof(dxh::SimpleMaterial)))
	{
		util::ErrorMessageBox("Failed to create constant buffer! Last error " + LASTERR);
		return false;
	}

	material.spec_factor = 20.0f;
	material.spec_color = { 1.0f, 1.0f, 1.0f};
	context->PSSetConstantBuffers(1, 1, &cBuffer);
	return true;
}


bool DXHandler::CreateDepthStencil(ID3D11Device*& device, UINT width, UINT height, ID3D11Texture2D *& texture, ID3D11DepthStencilView*& dsview, ID3D11DepthStencilState *& dsstate) 
{
	D3D11_TEXTURE2D_DESC dstexdesc{};
	dstexdesc.Width = width;
	dstexdesc.Height = height;
	dstexdesc.MipLevels = 1;
	dstexdesc.ArraySize = 1;
	dstexdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dstexdesc.SampleDesc.Count = 1;
	dstexdesc.SampleDesc.Quality = 0;
	dstexdesc.Usage = D3D11_USAGE_DEFAULT;
	dstexdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dstexdesc.CPUAccessFlags = 0;
	dstexdesc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&dstexdesc, NULL, &texture);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create 2D texture for depth stencil. Last error: " + LASTERR);
		return false;
	}

	hr = device->CreateDepthStencilView(texture, NULL, &dsview);
	if(FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create Depth Stencil View. Last error: " + LASTERR);
		return false;
	}


	D3D11_DEPTH_STENCIL_DESC dsDesc{};
	// Depth test parameters
	dsDesc.DepthEnable		= true;
	dsDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc		= D3D11_COMPARISON_LESS;
	// Stencil test parameters
	dsDesc.StencilEnable	= true;
	dsDesc.StencilReadMask	= 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
	
	hr = device->CreateDepthStencilState(&dsDesc, &dsstate);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create Depth Stencil State. Lasterror: " + LASTERR);
		return false;
	}
	context->OMSetDepthStencilState(dsstate, 0);
	//context->OMSetRenderTargets(1, &rendertargetview, dsview);

	return true;

}

bool DXHandler::CreateTexture(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& shaderresourceview)
{
	//GenerateTexture();
	LoadImageToTexture(device, context, texture, shaderresourceview, cTex, "annie.png");


	D3D11_TEXTURE2D_DESC texDesc{ 0 };
	texDesc.Width = cTex.width; //How wide is the image (in texels [texture pixels])
	texDesc.Height = cTex.height; //How tall is the image (texels)
	texDesc.MipLevels = texDesc.ArraySize = 1; //
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //32 bit color 
	texDesc.SampleDesc.Count = 1; //
	texDesc.SampleDesc.Quality = 0; //
	texDesc.Usage = D3D11_USAGE_IMMUTABLE; //Will read from and write to GPU, no CPU required
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; //
	texDesc.MiscFlags = 0; //
	texDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA texData{};
	texData.pSysMem = &cTex.data[0];
	texData.SysMemPitch = cTex.width*cTex.channels;
	texData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&texDesc, &texData, &texture);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("failed to create 2d texture. last err: " + LASTERR);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = texDesc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	resViewDesc.Texture2D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(texture, &resViewDesc, &shaderresourceview);
	if (FAILED(hr))
	{
		return false;
	}

	context->PSSetShaderResources(0, 1, &shaderresourceview);
	return true;
}

bool DXHandler::CreateSamplerState(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11SamplerState*& samplerstate)
{
	//Sampler description
	D3D11_SAMPLER_DESC samplerDesc{};
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//Create Sampler
	HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerstate);

	if (FAILED(hr))
	{
		return false;
	}
 
	context->PSSetSamplers(0, 1, &samplerstate);
	return true;
}


//Loads an image from a file using the stb_image library and converts it to a usable texture
bool DXHandler::LoadImageToTexture(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& shaderresourceview, dxh::ImageData &target, const std::string filepath)
{
	ImageLoadRaw il;

	return	il.ImageFromFile(target, filepath.c_str());
}

//*********************************************************
//MESH
//*********************************************************

bool DXHandler::CreateMesh(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& bVertex, ID3D11Buffer *&bIndex) {

	dxh::float3 positions[]
	{
		{-0.5f, 0.5f, 0.0f}, // bottom left				  |		 |
		{0.5f, 0.5f, 0.0f},   // bottom right			  +------+
		{0.5f, -0.5f, 0.0f}, // top right				  |		 |
		{-0.5, -0.5f, 0.0f}, // top left                  +------+
	};
	//testing cause ít got fuck't
	//dxh::float3 positions[]
	//{
	//	{0.0f, 0.0f, 0.0f}, // top left
	//	{1.0f, 0.0f, 0.0f}, // top right
	//	{1.0f, 1.0f, 0.0f},  // bottom right
	//	{0.0f, 1.0f, 0.0f}  // bottom left
	//};


	dxh::float3 pos[4];
	
	dxh::float3 normal = dxh::cross(positions[0] - positions[1], positions[0] - positions[2]);
	
	dxh::float2 uv[]
	{ 
		{0, 0}, // top left uv
		{1, 0}, // top right uv
		{1, 1},  // bottom right uv
		{0, 1}, // bottom left uv
	};

	size_t indices[]
	{	
		0, 1, 2, //first triangle
		0, 2, 3  //second triangle
	};
	

	for (int i = 0; i < 6; i++)
	{
		this->mesh.indices.push_back(indices[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		this->mesh.vertices.push_back(dxh::Vertex(positions[i], normal, uv[i]));
	}

	return true;
 }

bool DXHandler::CreateVertexBuffer(ID3D11Device*& device, ID3D11DeviceContext *&context, ID3D11Buffer*& vbuffer, const dxh::Mesh& mesh)
{
	D3D11_BUFFER_DESC buffdesc{ 0 };
	buffdesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	buffdesc.Usage			= D3D11_USAGE_DYNAMIC;
	buffdesc.ByteWidth		= mesh.ByteWidth();
	buffdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	D3D11_SUBRESOURCE_DATA blob{};
	blob.pSysMem = mesh.vertices.data();

	HRESULT hr = device->CreateBuffer(&buffdesc, &blob, &vbuffer);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Could not create vertex buffer. Last error: " + LASTERR);
		return false;
	}

	UINT32 pStride = sizeof(dxh::Vertex);
	UINT32 offset = 0;
	context->IASetVertexBuffers(0, 1, &vbuffer, &pStride, &offset);
	return true;
}


bool DXHandler::CreateInputLayout(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11InputLayout*& layout, const std::string &bytecode)
{

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	HRESULT hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), bytecode.c_str(), bytecode.length(), &layout); //create input-layout for the assembler stage
	
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create input layout. Last error: " + LASTERR);
		return false;
	}

	context->IASetInputLayout(layout);
	return true;
}

bool DXHandler::CreateIndexBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& bIndex, const dxh::Mesh& mesh)
{
	D3D11_BUFFER_DESC ibd{};
	ibd.Usage				= D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth			= sizeof(UINT) * mesh.indices.size();
	ibd.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags		= 0;
	ibd.MiscFlags			= 0;
	ibd.StructureByteStride	= 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = mesh.indices.data();
	HRESULT hr = device->CreateBuffer(&ibd, &initData, &bIndex);

	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create index buffer");
		return false;
	}

	context->IASetIndexBuffer(bIndex, DXGI_FORMAT_R32_UINT, 0);
	return true;
}

void DXHandler::SetViewport(ID3D11DeviceContext *& context,FLOAT width, FLOAT height, FLOAT topleftx, FLOAT toplefty, FLOAT maxdepth, FLOAT mindepth) {

	D3D11_VIEWPORT vp{ 0 };
	vp.Width	= width;
	vp.Height	= height;
	vp.TopLeftX = topleftx;
	vp.TopLeftY = toplefty;
	vp.MaxDepth = maxdepth;
	vp.MinDepth = mindepth;
	context->RSSetViewports(1, &vp);
}


//generates a default texture
void DXHandler::GenerateTexture()
{
	cTex.height = 512;
	cTex.width = 512;
	cTex.channels = 4;
	cTex.data.resize(cTex.width * cTex.height * 4);

	for (int h = 0; h < cTex.height; ++h)
	{
		for (int w = 0; w < cTex.width; ++w)
		{
			unsigned char r = w < cTex.width / 3 ? 255 : 0;
			unsigned char g = w >= cTex.width / 3 && w <= cTex.width / 1.5f ? 255 : 0;
			unsigned char b = w > cTex.width / 1.5f ? 255 : 0;
			unsigned int pos0 = w * cTex.channels + cTex.width * cTex.channels * h;
			cTex.data[pos0 + 0] = r;
			cTex.data[pos0 + 1] = g;
			cTex.data[pos0 + 2] = b;
			cTex.data[pos0 + 3] = 255;
		}
	}
}


//Rotates world matrix at a rate of 2pi(rad)/rot_time(sec)
void DXHandler::Rotate(float dt, dxh::WVP& wvp) 
{
	// one whole lap in radians, time in seconds for a full rotation
	const float angle = 2 * 3.1415f;
	//approximately one rotation every 4 seconds
	const float rot_time = 8.0f;
	// rotating world matrix around Y axis at a rate
	wvp.world *= DirectX::XMMatrixRotationY(angle * (dt / rot_time));
}

void DXHandler::Render(float dt)
{
	const FLOAT clearColor[] = { 0.0f, 0.0f, 0.2f, 1.0f };
	context->ClearRenderTargetView(rendertargetview, clearColor); //Clear backbuffer

	static D3D11_MAPPED_SUBRESOURCE msrc;

	if (!flagged) {
		context->Map(bVertex, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msrc);
		memcpy(msrc.pData, mesh.vertices.data(), sizeof(dxh::Vertex) * mesh.vertices.size());
		context->Unmap(bVertex, NULL);
		flagged = true;
	}

	memcpy(msrc.pData, nullptr, 0); //emptying the subresource before reusing it

	Rotate(dt, wvp);
	
	//	NOTE: would have liked to use UpdateSubresource since it looks cleaner, but is apparently slower. 
	//	source: https://developer.nvidia.com/sites/default/files/akamai/gamedev/files/gdc12/Efficient_Buffer_Management_McDonald.pdf 
	context->Map(bMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &msrc); //Write data onto CBuffer
	memcpy(msrc.pData, &wvp, sizeof(wvp)); // Store data into subresource
	context->Unmap(bMatrix, 0); 	// Unmap CBuffer

	memcpy(msrc.pData, nullptr, 0); //emptying the subresource before reusing it
	context->Map(bLighting, 0, D3D11_MAP_WRITE_DISCARD, 0, &msrc);
	memcpy(msrc.pData, &light, sizeof(light));
	context->Unmap(bLighting, 0);
	
	memcpy(msrc.pData, nullptr, 0); //emptying the subresource before reusing it
	context->Map(bMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &msrc);
	memcpy(msrc.pData, &material, sizeof(material));
	context->Unmap(bMaterial, 0);

	context->DrawIndexed(mesh.indices.size(), 0, 0);

	swapchain->Present(0, 0);
	return;
}
