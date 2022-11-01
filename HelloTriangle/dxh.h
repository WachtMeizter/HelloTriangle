#pragma once

#include "pch.h"
//Implementing directx

//STB image loader

#include "Utilities.h"
#include "CustomDataTypes.h"


#define TEXTUREPATH "D:\repos\HelloTriangle\Resources"

struct CustomTexture
{
	int width;
	int height;
	int channels; //max 4, rgba
	std::vector<unsigned char> data;
	CustomTexture(int _width = 0, int _height = 0, int _channels = 3) : width(_width), height(_height), channels(_channels) {};
};

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
	bool LoadTextureFromFile(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& shaderresourceview, const std::string filepath);
	//Constant buffer
	bool CreateConstantBuffer(ID3D11Device*& device, ID3D11DeviceContext*& context, ID3D11Buffer*& cBuffer, const D3D11_SUBRESOURCE_DATA* srd = NULL);
	bool SetupMatrix(ID3D11Device *& device, ID3D11DeviceContext *& context, ID3D11Buffer*& cBuffer, dxh::WVP& wvp);
	bool SetupLight();
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
	dxh::Mesh mesh;
	CustomTexture cTex;
public:

	DXHandler(HWND handle);
	~DXHandler();
	bool SetLayout(ID3D11DeviceContext*& context, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) { context->IASetPrimitiveTopology(topology); } //defaults to list
	void Render(float dt = 0.0f);
	void Rotate(float dt, dxh::WVP& wvp);
	void GenerateTexture();
};