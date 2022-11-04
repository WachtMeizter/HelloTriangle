//TEXTURES & SAMPLER STATE
Texture2D testTexture :register(t0);
SamplerState testSampler;

//CONSTANT BUFFER OBJECTS
cbuffer SIMPLELIGHT : register(b0)
{
	float4	light_pos : POSITION;
	float4	light_color : COLOR;
	float4	camera_pos : POSITION;
	float	light_ambient; 
};

cbuffer SIMPLEMATERIAL : register(b1)
{
	float3 material_specular_color : COLOR;
	float  material_specular_factor;
}

//VERTEX SHADER OUTPUT
struct PS_IN
{
	float4 vpos : SV_POSITION;
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

float4 main(PS_IN input) : SV_TARGET
{

	// INITIALISE VALUES
	float3 final_pixel = (float3)0; //return value, initialized to 0, 0, 0, 0
	// SAMPLE TEXTURE
	float4 tex = float4(testTexture.Sample(testSampler, input.uv).xyz, 1.0f);
	// AMBIENT
	float3 ambient_lighting = light_color.xyz * light_ambient;
	
	// DIFFUSE CALCULATIONS

	float3 normal = normalize(input.normal);
	float3 lightdir = normalize(input.pos - light_pos.xyz); //directional vector from light to surface
	float  diffuse_factor = max(dot(normal, lightdir), 0);
	float3 diffuse_lighting = light_color.xyz * diffuse_factor;
	
	// SPECULAR CALCULATIONS
	//float  specular_factor	= material_specular_factor;
	//float3 specular_light	= material_specular_color.xyz;
	//float3 reflection = -(2 * normalize(input.normal - viewdir.xyz));
	// BWAH
	/*float3 specular_color = material_specular_color;
	float3 halfway = normalize(normalize(camera_pos.xyz - input.pos) + lightdir);
	float3 specular_lighting = specular_color * (light_color.xyz * pow((normal * halfway), material_specular_factor));*/
	
	float3 viewdir = normalize(camera_pos.xyz - input.pos); //Directional vector from camera to point on surface
	float3 reflection = reflect(lightdir, -normal);
	float spec = pow(max(dot(viewdir, reflection), 0.0), material_specular_factor);
	float3 specular_lighting = spec * light_color;

	// APPLY COLORS
	final_pixel = (ambient_lighting +diffuse_lighting + specular_lighting) * tex.xyz;
	//final_pixel = material_specular_color;
	return float4(final_pixel, 1.0f);
}