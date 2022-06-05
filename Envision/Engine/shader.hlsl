struct VS_IN
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

cbuffer CameraBuffer : register(b0)
{
	float4x4 ViewProjection;
}

cbuffer ObjectBuffer : register(b1)
{
	float4x4 World;
}

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;
	output.Position = float4(input.Position, 1.0f);
	output.Position = mul(output.Position, World);
	output.Position = mul(output.Position, ViewProjection);

	// Why is normalize necessay? It should still be length of 1?
	output.Normal = normalize(mul(float4(input.Normal, 0.f), World));

	output.Texcoord = input.Texcoord;

	return output;
}

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float3 dirToSun = -1.f * normalize(float3(-1.0f, -2.f, 0.8f));

	float factor = dot(input.Normal, dirToSun);
	factor = clamp(factor, 0.3f, 1.0f);

	//input.Normal = float3(1.0f, 1.0f, 1.0f);
	//input.Normal = input.Normal / 2.0f;

	return float4(input.Normal, 1.0f);
}