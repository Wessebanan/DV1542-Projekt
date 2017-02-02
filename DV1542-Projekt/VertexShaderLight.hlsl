

struct VS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

float4 main(VS_IN input) : SV_POSITION
{
	float4 output = (input.Pos, 1);
	return output;
}