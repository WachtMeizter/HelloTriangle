#include "dxh.h"

DXHandler::DXHandler(HWND handle)
{
	if (InitializeSwapChainAndDevice(handle, swapchain, device, context) == false){
		exit(-1);
	}
	if (CreateRenderTargetView(device, swapchain, rendertargetview) == false)
	{
		exit(-2);
	}
	if (CompileShaders(device, vertexshader, pixelshader) == false){
		exit(-3);
	}
	if (CreateMesh(device, vertexbuffer, indexbuffer) == false)
	{
		exit(-4);
	}
	
}

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

bool DXHandler::InitializeSwapChainAndDevice(HWND handle, IDXGISwapChain*& swapchain, ID3D11Device*& device, ID3D11DeviceContext * &context)
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
		NULL,
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

	return true;
}
bool DXHandler::CreateVertexShader(ID3D11Device*& device, ID3D11VertexShader*& vshader, std::string filepath)
{
	std::string data = ReadShaderData(filepath);

	if (FAILED(device->CreateVertexShader(data.c_str(), data.length(), NULL, &vshader)))
	{
		util::ErrorMessageBox("Could not create vertex shader. Last error: " + LASTERR);
		return false;
	}

	return true;
}

bool DXHandler::CreatePixelShader(ID3D11Device*& device, ID3D11PixelShader*& pshader, std::string filepath) {
	std::string data = ReadShaderData(filepath);

	if (FAILED(device->CreatePixelShader(data.c_str(), data.length(), NULL, &pshader)))
	{
		util::ErrorMessageBox("Could not create pixel shader. Last error: " + LASTERR);
		return false;
	}
	return true;
}

bool DXHandler::CompileShaders(ID3D11Device *& device, ID3D11VertexShader *& vertexshader, ID3D11PixelShader *& pixelshader) 
{
	//vertex shader
	if (!CreateVertexShader(device, vertexshader, "D:/repos/HelloTriangle/Debug/VertexShader.cso"))
		return false;
	//pixel shader
	if (!CreatePixelShader(device, pixelshader, "D:/repos/HelloTriangle/Debug/PixelShader.cso"))
		return false;

	return true;
}

bool CreateDepthStencil(ID3D11Device*& device, UINT width, UINT height, ID3D11Texture2D *& dsTexture, ID3D11DepthStencilView*& dsv) 
{
	D3D11_TEXTURE2D_DESC texdesc{};
	texdesc.Width = width;
	texdesc.Height = height;
	texdesc.MipLevels = 1;
	texdesc.ArraySize = 1;
	texdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texdesc.SampleDesc.Count = 1;
	texdesc.SampleDesc.Quality = 0;
	texdesc.Usage = D3D11_USAGE_DEFAULT;
	texdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texdesc.CPUAccessFlags = 0;
	texdesc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&texdesc, NULL, &dsTexture);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create 2D texture for depth stencil. Last error: " + LASTERR);
		return false;
	}	
	hr = device->CreateDepthStencilView(dsTexture, NULL, &dsv);
	if(FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create Depth Stencil View. Last error: " + LASTERR);
		return false;
	}

	return true;

}

bool DXHandler::CreateMesh(ID3D11Device*& device, ID3D11Buffer*& vertexbuffer, ID3D11Buffer *&indexbuffer) {

	dxh::float3 positions[]
	{
		{-0.5, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f}
	};

	dxh::float3 normal = dxh::cross(positions[0] - positions[1], positions[0] - positions[2]);
	dxh::float2 uv[]
	{ {0,0}, {1, 0}, {0, 1}, {1, 1}

	};

	size_t indices[]
	{	0, 1, 2,
		0, 2, 3
	};
	
	dxh::Mesh quad;

	for(int i = 0; i < 6; i++)
		quad.indices.push_back(indices[i]);
	for (int i = 0; i < 4; i++)
	{
		quad.vertices.push_back(dxh::Vertex(positions[i], normal, uv[i]));
	}

	if (SetupVertexBuffer(device, vertexbuffer, quad) == false)
	{
		return false;
	}

	if (SetupIndexBuffer(device, indexbuffer, quad) == false)
	{
		util::ErrorMessageBox("Could not create index buffer.");
	}

	return true;
 }

bool DXHandler::SetupVertexBuffer(ID3D11Device*& device, ID3D11Buffer*& vbuffer, const dxh::Mesh& mesh)
{
	D3D11_BUFFER_DESC buffdesc{ 0 };
	buffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffdesc.Usage = D3D11_USAGE_DYNAMIC;
	buffdesc.ByteWidth = mesh.ByteWidth();
	buffdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	D3D11_SUBRESOURCE_DATA blob{};
	blob.pSysMem = mesh.vertices.data();
	
	HRESULT hr = device->CreateBuffer(&buffdesc, &blob, &vbuffer);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Could not creater error box. Last error: " + LASTERR);
		return false;
	}

	

	return true;
}

bool DXHandler::SetupIndexBuffer(ID3D11Device*& device, ID3D11Buffer*& indexbuffer, const dxh::Mesh& mesh)
{
	D3D11_BUFFER_DESC ibd{};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mesh.indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = mesh.indices.data();
	HRESULT hr = device->CreateBuffer(&ibd, &initData, &indexbuffer);

	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create index buffer");
		return false;
	}

	return true;
}

void DXHandler::Render(float dt) 
{


	return;
}

