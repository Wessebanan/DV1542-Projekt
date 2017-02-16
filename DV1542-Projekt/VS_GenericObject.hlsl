
struct VS_IN {
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};


cbuffer TRANSFORM_BUFFER : register(b0) {
	matrix world;
	matrix view;
	matrix proj;
}

VS_OUT main( VS_IN input ) {

	VS_OUT output = (VS_OUT)0;

	matrix wvp = mul(proj, mul(view, world));


	output.Pos = mul(wvp,float4(input.Pos,1.0f));
	output.WPos = mul(world, input.Pos);
	output.Normal = mul(world, input.Normal);
	output.TexCoord = input.TexCoord;

	return output;
}