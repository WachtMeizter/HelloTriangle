#pragma once

#include "pch.h"
//Implementing directx

#include "Utilities.h"
#include "CustomDataTypes.h"

class DXHandler 
{
private:
	bool InitializeSwapChainAndDevice(HWND handle, IDXGISwapChain*& swapchain, ID3D11Device*& device, ID3D11DeviceContext*& context);
	bool CreateRenderTargetView(ID3D11Device*& device, IDXGISwapChain*& swapchain, ID3D11RenderTargetView *& rendertargetview);
	bool SetupPipeline();
	bool CompileShaders(ID3D11Device* &device, ID3D11VertexShader* &vertexshader, ID3D11PixelShader * &pixelshader);
	bool CreateMesh(ID3D11Device *& device, ID3D11Buffer *& vertexbuffer, ID3D11Buffer*& indexbuffer);
	bool SetupVertexBuffer(ID3D11Device *& device, ID3D11Buffer *& vbuffer, const dxh::Mesh &mesh);
	bool SetupIndexBuffer(ID3D11Device*& device, ID3D11Buffer*& vbuffer, const dxh::Mesh& mesh);
	bool LoadTexture();
	bool SetupWVP();
	bool SetupLight();
	bool CreateVertexShader(ID3D11Device*& device, ID3D11VertexShader*& vshader, std::string filepath);	
	bool CreatePixelShader(ID3D11Device*& device, ID3D11PixelShader*& pshader, std::string filepath);
	std::string ReadShaderData(std::string filepath);
	ID3D11Device * device;
	ID3D11DeviceContext * context;
	IDXGISwapChain * swapchain;
	ID3D11RenderTargetView * rendertargetview;
	ID3D11VertexShader * vertexshader;
	ID3D11Buffer* matrixbuffer;
	ID3D11Buffer* lightingbuffer;
	ID3D11PixelShader * pixelshader;
	ID3D11Buffer * vertexbuffer;
	ID3D11Buffer * indexbuffer;

public:

	DXHandler(HWND handle);
	~DXHandler() {}
	void Render(float dt = 0.0f);
};