
float3 g_PlayerPosition;
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture g_Texture;

sampler TextureSampler = sampler_state
{
    texture = g_Texture;
    MINFILTER = linear;
    MAGFILTER = linear;
    MIPFILTER = linear;
};

/* 버텍스 쉐이더 진입점 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
    float3 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
		
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;

    return Out;
}

VS_OUT VS_MAIN2(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
		
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;
    
    
    float3 color1 = float3(1.0f, 0.5f, 0.2);
    float3 color2 = float3(1.0f, 0.3f, 0);
    
    float3 tempColor = lerp(color2, color1, pow(saturate(0.3 / 1.0f), 2.0));
    
    Out.vWorldPos = tempColor * saturate(1);


    return Out;
}

//---------------------
/* 가우시안필터 데이터*/
//---------------------
float4 gaussFilter[7] =
{
    { 0.0, -3.0, 0.0, 1.0 / 64.0, },
    { 0.0, -2.0, 0.0, 6.0 / 64.0, },
    { 0.0, -1.0, 0.0, 15.0 / 64.0, },
    { 0.0, 0.0, 0.0, 20.0 / 64.0, },
    { 0.0, 1.0, 0.0, 15.0 / 64.0, },
    { 0.0, 2.0, 0.0, 6.0 / 64.0, },
    { 0.0, 3.0, 0.0, 1.0 / 64.0, },
};
float texScaler = 1.0 / 128.0;
float texOffset = 0.0;

//---------------------
/* 레인보우*/
//---------------------
float3 rainbow[7] =
{
    { 1.0f, 0.0f, 0.0f }, // Red
    { 1.0f, 0.65f, 0.0f }, // Orange
    { 1.0f, 1.0f, 0.0f }, // Yellow
    { 0.0f, 1.0f, 0.0f }, // Green
    { 0.0f, 0.0f, 1.0f }, // Blue
    { 0.3f, 0.0f, 0.5f }, // Indigo
    { 0.5f, 0.0f, 1.0f } // Violet/Purple
};



struct PS_IN
{
    float4 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
    float3 vWorldPos : TEXCOORD1;

};

struct PS_OUT
{
    vector vColor : COLOR0;
};

/*여기서부터 픽셀쉐이더*/
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = tex2D(TextureSampler, In.vTexUV);
    //Out.vColor.rgb *= 2;
    //Out.vColor.a = 1.0;
    
    return Out;
}

PS_OUT PS_HIT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    //Out.vColor = tex2D(TextureSampler, In.vTexUV);

    float2 uv = In.vTexUV;
    
    //if (input.primID % 4 == 0 || input.primID % 4 == 2)
    //{
    //    uv.x -= 0.5;
    //    uv.x = -uv.x;
    //    uv.x += 0.5;
    //}
    
    //if (input.primID % 4 == 1 || input.primID % 4 == 2)
    //{
    //    uv.y -= 0.5;
    //    uv.y = -uv.y;
    //    uv.y += 0.5;
    //}
    
    float4 sprite = tex2D(TextureSampler, uv);
    Out.vColor = float4(In.vWorldPos.rgb * sprite.rgb * sprite.a * 0.5, 1);
    return Out;
	
    //return Out;
}

PS_OUT PS_ROLL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = tex2D(TextureSampler, In.vTexUV);
    //Out.vColor.rgb *= 1.5;
    float black = 0;
    
    if (Out.vColor.r == black && Out.vColor.g == black)
        Out.vColor.a = 0;
    else
    {
        Out.vColor.rgb = 0;
        Out.vColor.a = 0.3;
    }
    
    return Out;
}

PS_OUT PS_MOVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = tex2D(TextureSampler, In.vTexUV);
    //Out.vColor.rgb *= 1.5;
    float black = 0;
    
    if (Out.vColor.r == black && Out.vColor.g == black)
        Out.vColor.a = 0;
    else
    {
        Out.vColor.rgb *= 1.5;
        Out.vColor.a = 0.3;
    }
    
    return Out;
}

PS_OUT PS_NERO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = tex2D(TextureSampler, In.vTexUV);
    //Out.vColor.rgb *= 1.5;
    float black = 0;
    
    if (Out.vColor.r == black && Out.vColor.g == black)
        Out.vColor.a = 0;
    else
    {
        Out.vColor.rg *= 1.8;
        Out.vColor.a = 0.3;
    }
    
    return Out;
}

PS_OUT PS_Flame(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = tex2D(TextureSampler, In.vTexUV);
    //Out.vColor.rgb *= 1.5;
    float black = 0;
    black = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.0;
    if (black <= 0.1)
    {
        Out.vColor.a = 0;
    }
        
    else
    {
        Out.vColor.rgb *= 2.0;
        Out.vColor.a = 0.3;
    }

    return Out;
}



technique DefaultTechnique
{
    pass Default_Pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 100;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_MAIN();
    }

    pass Hit_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 100;
        VertexShader = compile vs_3_0 VS_MAIN2();
        PixelShader = compile ps_3_0 PS_HIT();
    }

    pass Roll_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_ROLL();
    }

    pass Move_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_MOVE();
    }

    pass Nero_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_NERO();
    }

    pass Flame_pass
    {
        AlphaBlendEnable = TRUE;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;
        BlendOp = ADD;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_Flame();
    }
}

