
cbuffer MATRIX : register(b0)
{
	matrix world;
	matrix view;
	matrix project;
};

struct VS_IN 
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct VS_OUT
{
	float4 vpos : SV_POSITION;
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	matrix transform = mul(world, mul(view, project));

	output.vpos   = mul(float4(input.pos, 1.0f), transform);
	output.pos	  = mul(input.pos, world);
	output.normal = mul(input.normal, world);
	output.uv = input.uv;
	return output;
}