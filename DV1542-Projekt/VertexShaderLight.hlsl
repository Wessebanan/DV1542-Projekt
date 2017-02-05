cbuffer TRANSFORM_BUFFER : register(b0)
{
	matrix world;
	matrix view;
	matrix proj;
}
struct VS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

float4 main(VS_IN input) : SV_POSITION
{
	matrix vp = mul(proj, view); //Skippar world för fullscreenquaden.
	float4 output = mul(vp, float4(input.Pos, 1));
	return output;
}