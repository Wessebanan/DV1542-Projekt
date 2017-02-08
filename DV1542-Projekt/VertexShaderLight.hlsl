cbuffer TRANSFORM_BUFFER : register(b0)
{
	matrix world;
	matrix view;
	matrix proj;
}
struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};
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

VS_OUT main(uint vI : SV_VERTEXID)
{
	VS_OUT output;
	output.texcoord = float2(vI % 2,vI % 4 / 2); 
	output.pos = float4((output.texcoord.x - 0.5f) * 2, -(output.texcoord.y - 0.5f) * 2, 0, 1);
	return output;
}