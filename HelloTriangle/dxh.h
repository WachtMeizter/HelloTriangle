#pragma once

#include "pch.h"
//Implementing directx

//STB image loader

#include "Utilities.h"
#include "CustomDataTypes.h"


#define TEXTUREPATH "D:\repos\HelloTriangle\Resources"

class DXHandler
{
private:
	//Used for constructor
	bool SetupDirectX(HWND handle);
	bool SetupPipeline(HWND handle);
	//DirectX
	bool CreateDeviceAndSwapChain(HWND handle, IDXGISwapChain*& swapchain, ID3D11Device*& device, ID3D11DeviceContext*& context);
	bool CreateRenderTargetView(ID3D11Device*& device, IDXGISwapChain*& swapchain, ID3D11RenderTargetView*& rendertargetview);
	bool CreateShaders(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11VertexShader*& vertexshader, ID3D11PixelShader*& pixelshader, ID3D11InputLayout*& inputLayout);
	bool CreateIndexBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& vbuffer, const dxh::Mesh& mesh);
	bool SetRasterizerState(const D3D11_RASTERIZER_DESC * pRasterizerDesc, ID3D11RasterizerState*& ppRasterizerState);
	void SetViewport(ID3D11DeviceContext *& context, FLOAT width, FLOAT height, FLOAT topleftx, FLOAT toplefty, FLOAT maxdepth, FLOAT mindepth); 
	bool CreateDepthStencil(ID3D11Device*& device, UINT width, UINT height, ID3D11Texture2D*& texture, ID3D11DepthStencilView*& dsview, ID3D11DepthStencilState *& dsstate);
	//Pipeline
	bool CreateVertexShader(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11VertexShader*& vshader, ID3D11InputLayout*& inputLayout, std::string filepath);
	bool CreateInputLayout(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11InputLayout*& layout, const std::string& data);
	bool CreateVertexBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& vbuffer, const dxh::Mesh& mesh);
	bool CreatePixelShader(ID3D11Device*& device, ID3D11PixelShader*& pshader, std::string filepath);
	bool CreateRasterizerState(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11RasterizerState*& ppRasterizerState);
	void SetTopology(ID3D11DeviceContext *& context, D3D11_PRIMITIVE_TOPOLOGY topology);
	//Texture
	bool CreateTexture(ID3D11Device *& device, ID3D11DeviceContext *& context,ID3D11Texture2D *& texture, ID3D11ShaderResourceView *& shaderresourceview);
	bool CreateSamplerState(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11SamplerState*& samplerstate);
	bool LoadImageToTexture(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& shaderresourceview, dxh::ImageData &target, const std::string filepath);
	//Constant buffer
	bool CreateConstantBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& cBuffer, UINT byteWidth);
	bool SetupMatrixBuffer(ID3D11Device *& device, ID3D11DeviceContext *& context, ID3D11Buffer*& cBuffer, dxh::WVP& wvp);
	bool SetupLightBuffer(ID3D11Device *&device, ID3D11DeviceContext *& context, ID3D11Buffer *& cBuffer);
	bool SetupMaterialBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& cBuffer);
	//Misc
	bool CreateMesh(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& bVertex, ID3D11Buffer*& indexbuffer);
	std::string ReadShaderData(std::string filepath);
	//Variables
	bool flagged;
	ID3D11Device * device;
	ID3D11DeviceContext * context;
	IDXGISwapChain * swapchain;
	ID3D11RenderTargetView * rendertargetview;
	ID3D11RasterizerState* rasterizerstate;
	//DepthStencil
	ID3D11Texture2D * dsTexture; 
	ID3D11DepthStencilView * dsv;
	ID3D11DepthStencilState* dsState;
	//Pipeline
	ID3D11VertexShader * vertexshader;
	ID3D11InputLayout * inputlayout;
	ID3D11PixelShader * pixelshader;
	//Buffers
	ID3D11Buffer * bVertex;
	ID3D11Buffer * bIndex;
	ID3D11Buffer * bMaterial;
	ID3D11Buffer * bMatrix;
	ID3D11Buffer * bLighting;
	//Texture
	ID3D11Texture2D* gTexture;
	ID3D11ShaderResourceView* gTextureView;
	ID3D11SamplerState* gSamplerState;
	//Misc
	dxh::WVP wvp;
	dxh::SingleLight light;
	dxh::SingleLight diffuse;
	dxh::SingleLight specular;
	dxh::SimpleMaterial material;
	dxh::Mesh mesh;
	dxh::ImageData cTex;
	const dxh::float3 camerapos = { 0.0f, 0.0, -1.0f };
public:

	DXHandler(HWND handle);
	~DXHandler();
	bool SetLayout(ID3D11DeviceContext*& context, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) { context->IASetPrimitiveTopology(topology); } //defaults to list
	void Render(float dt = 0.0f);
	void Rotate(float dt, dxh::WVP& wvp);
	void GenerateTexture();
};