cbuffer TRANSFORM_BUFFER : register(b0)
{
	matrix world;
	matrix view;
	matrix proj;
}
//struct VS_IN
//{
//	float3 Pos : POSITION;
//	float3 Color : COLOR;
//};
//
//float4 main(VS_IN input) : SV_POSITION
//{
//	matrix wvp = mul(proj, mul(view, world));
//	float4 output = mul(wvp, float4(input.Pos, 1));
//	return output;
//}

float4 main(uint vI : SV_VERTEXID) : SV_POSITION
{
	float2 texcoord = float2(vI & 1, vI >> 1); //you can use these for texture coordinates later
	return float4((texcoord.x - 0.5f) * 2,-(texcoord.y - 0.5f) * 2,0,1);
}