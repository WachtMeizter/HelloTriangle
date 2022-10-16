
struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

float4 main(PS_IN input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}