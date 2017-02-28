Texture2D<float> origSM : register (t0);
RWTexture2D<float> blurSM : register (u0);

cbuffer dimensions : register(b0)
{
	int height;
	int width;
};

//TODO: något klyftigt med dimensions för numthreads
[numthreads(32, 32, 1)]
void main( uint gIndex : SV_GroupIndex,
		   uint3 dThreadID : SV_DispatchThreadID)
{
	blurSM[dThreadID.xy] = origSM.Load(dThreadID);
}