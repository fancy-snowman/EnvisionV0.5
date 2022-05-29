struct VS_IN
{
	float2 Position : POSITION;
	float3 Color : COLOR;
};

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
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
	output.Position = float4(input.Position, 0.0f, 1.0f);
	output.Position = mul(output.Position, World);
	output.Position = mul(output.Position, ViewProjection);
	output.Color = input.Color;
	return output;
}

float4 PS_main(VS_OUT input) : SV_TARGET
{
	return float4(input.Color, 1.0f);
}