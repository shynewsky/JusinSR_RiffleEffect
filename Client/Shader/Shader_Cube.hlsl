

float3			g_PlayerPosition;
float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture			g_Texture;

float3 LightDirection;
float4 LightDiffuse;
float4 LightAmbient;
float3 LightPosition;
float LightRange;
float LightAttenuation;


sampler TextureSampler = sampler_state {
	texture = g_Texture;
	MINFILTER = linear;
	MAGFILTER = linear;
	MIPFILTER = linear;
};


/* VertexShader_진입점 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float3 vNormal : TEXCOORD0;
    float3 vTexUV : TEXCOORD1;
    float3 vWorldPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = mul(float4(In.vNormal, 0.0f), g_WorldMatrix).xyz;
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;

    return Out;
}

/* w 나누기연산. */
/* 뷰포트 변환. */
/* 래스터라이즈  */

struct PS_IN
{
    float4 vPosition : POSITION;
    float3 vNormal : TEXCOORD0;
    float3 vTexUV : TEXCOORD1;
    float3 vWorldPos : TEXCOORD2;

};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

  // Normalize the normal
    float3 normal = normalize(In.vNormal);

    // Calculate the vector from the point to the light source
    float3 lightDir = LightPosition - In.vWorldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    // Calculate the diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);

    // Calculate the attenuation
    float attenuation = 1.0 / (1.0 + LightAttenuation * distance);

    // Combine the diffuse and ambient lighting
    float4 color = (LightAmbient + diff * LightDiffuse) * attenuation;

    // Sample the cube texture using normalized normal (vNormal)
    float4 texColor = texCUBE(TextureSampler, In.vTexUV);

    // Combine texture color with lighting
    Out.vColor = color * texColor;
    Out.vColor.a = 1.0;

    return Out;

}

PS_OUT PS_BLIND(PS_IN In)
{
    PS_OUT Out;

  // Normalize the normal
    float3 normal = normalize(In.vNormal);

    // Calculate the vector from the point to the light source
    float3 lightDir = LightPosition - In.vWorldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    // Calculate the diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);

    // Calculate the attenuation
    float attenuation = 1.0 / (1.0 + LightAttenuation * distance);

    // Combine the diffuse and ambient lighting
    float4 color = (LightAmbient + diff * LightDiffuse) * attenuation;

    // Sample the cube texture using normalized normal (vNormal)
    float4 texColor = texCUBE(TextureSampler, In.vTexUV);

    // Combine texture color with lighting
    Out.vColor = color * texColor;
    Out.vColor.a = 0.5;

	return Out;
}


technique DefaultTechnique
{
    pass DefaultPass_with_AlphaTest
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        CullMode = CCW;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_MAIN();
    }

    pass PassBlind
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        CullMode = NONE;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_BLIND();
    }

}