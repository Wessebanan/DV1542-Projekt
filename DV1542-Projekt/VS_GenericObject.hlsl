struct VS_IN 
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION0;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 lightPos : POSITION1;
};


cbuffer TRANSFORM_BUFFER : register(b0) {
	matrix world;
	matrix view;
	matrix proj;
	matrix lightView;
	matrix lightProj;
}

VS_OUT main( VS_IN input ) {

	VS_OUT output = (VS_OUT)0;

	matrix wvp = mul(proj, mul(view, world));
	matrix lightWvp = mul(lightProj, mul(lightView, world));

	output.Pos = mul(wvp,float4(input.Pos,1.0f));
	output.WPos = mul(world, float4(input.Pos, 1.0f)).xyz;
	output.Normal = normalize(mul(world, input.Normal));
	output.TexCoord = input.TexCoord;
	output.lightPos = mul(lightWvp, float4(input.Pos, 1.0f));

	return output;
}