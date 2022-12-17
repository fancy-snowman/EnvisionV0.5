

// ######################################################################### //
// ##################### COMMON SHADER DATA STRUCTURES ##################### //
// ######################################################################### //

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

// ######################################################################### //
// ###################### VERTEX SHADER STAGE PROGRAM ###################### //
// ######################################################################### //

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;

	output.Position = float4(input.Position, 0.f, 0.f);
	output.Color = input.Color;

	return output;
}

// ######################################################################### //
// ####################### PIXEL SHADER STAGE PROGRAM ###################### //
// ######################################################################### //

float4 PS_main(VS_OUT input) : SV_TARGET
{
	return float4(input.Color, 1.f);
}
