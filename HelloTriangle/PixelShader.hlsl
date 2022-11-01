Texture2D testTexture :register(t0);
SamplerState testSampler;

struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 surfacepoint : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};


float4 main(PS_IN input) : SV_TARGET
{
	float4 amb = { 0.2f, 0.2f, 0.2f, 1.0f };

	float4 lightpos = {0.0f, 0.0f, -1.0f, 0.0f};
	float3 lightdir = normalize(input.surfacepoint - lightpos.xyz);
	float4 pixel_out = (float4)0;
	float intensity = 0.0f;

	//calculate light intensity, 0 if
	intensity = max(dot(lightdir, input.normal), 0.0f);
	
	//if light intensity is less than ambient, just apply ambient
	float4 tex = float4(testTexture.Sample(testSampler, input.uv).xyz, 1.0f);
	pixel_out = tex * intensity;


	
	return pixel_out;
}