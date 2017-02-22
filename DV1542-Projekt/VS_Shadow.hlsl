cbuffer transformBuffer : register(b0)
{
	float4x4 object_world;
	float4x4 light_view;
	float4x4 ortho_proj;
};

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

float4 main( VS_IN input ) : SV_POSITION
{
	//Placing the object correctly in the world, setting the light "position" as
	//the eye position and projecting orthographically so parallel lines remain parallel.
	float4x4 wvp = mul(ortho_proj, mul(light_view, object_world));
	return mul(wvp, float4(input.pos, 1));
}