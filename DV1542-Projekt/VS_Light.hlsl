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

VS_OUT main(uint vI : SV_VERTEXID)
{
	VS_OUT output;
	output.texcoord = float2(vI % 2,vI % 4 / 2); 
	output.pos = float4((output.texcoord.x - 0.5f) * 2, -(output.texcoord.y - 0.5f) * 2, 0, 1);
	return output;
}