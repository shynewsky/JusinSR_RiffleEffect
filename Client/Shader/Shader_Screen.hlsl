float3 g_PlayerPosition;
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture g_Texture;
float g_Time;

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
PS_OUT PS_MAIN(PS_IN In) //스캔라인
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = tex2D(TextureSampler, In.vTexUV);

    float scanline = 0.3 + 0.7 * sin(In.vTexUV.y * 800.0); // 800.0 controls the frequency of the lines
    
    Out.vColor *= scanline;
    
    return Out;
}

PS_OUT PS_HIT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexUV;

    float4 color = tex2D(TextureSampler, uv);
    float fmax = 0.15 - g_Time *0.2;
    fmax = max(0, fmax);
    float edgeDistance = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));
    float edgeWeight = smoothstep(0.0, fmax, edgeDistance);


    float4 redColor = float4(1.0, 0.0, 0.0, 1.0);
    float4 blendedColor = lerp(color, redColor, 0.3); 
    color = lerp(blendedColor, color, edgeWeight);

    Out.vColor = color;
    

    return Out;
}

PS_OUT PS_WAVE(PS_IN In) //물결
{
    PS_OUT Out = (PS_OUT) 0;
    
    float waveFrequency = 20.0;
    float waveAmplitude = 0.02;
    
    In.vTexUV.y += sin(In.vTexUV.x * waveFrequency + g_Time) * waveAmplitude;
    In.vTexUV.y += cos(In.vTexUV.y * waveFrequency + g_Time) * waveAmplitude;

    Out.vColor = tex2D(TextureSampler, In.vTexUV);
    
    return Out;
}

PS_OUT PS_TONADO(PS_IN In) //소용돌이
{
    PS_OUT Out = (PS_OUT) 0;

    float2 center = float2(0.5, 0.5);
    float2 toCenter = In.vTexUV.rg - center;

    float swirlStrength = min(g_Time * 0.1, 1.0); 

    float angle = length(toCenter) * 30.0 * swirlStrength + g_Time;

    float s = sin(angle);
    float c = cos(angle);
    toCenter = float2(toCenter.x * c - toCenter.y * s, toCenter.x * s + toCenter.y * c);
    In.vTexUV.rg = center + toCenter;

    Out.vColor = tex2D(TextureSampler, In.vTexUV);
    
    
    return Out;
}

PS_OUT PS_GAUS(PS_IN In) //가우시안
{
    PS_OUT Out = (PS_OUT) 0;
    float4 color = 0.0;
    
    float dynamicScaler = g_Time * (1.0 / 128.0);
    
    for (int i = 0; i < 7; ++i)
    {
        color += tex2D(TextureSampler, float2(In.vTexUV.x + gaussFilter[i].y * dynamicScaler, In.vTexUV.y)) * gaussFilter[i].w;
        color += tex2D(TextureSampler, float2(In.vTexUV.x, In.vTexUV.y + gaussFilter[i].y * dynamicScaler)) * gaussFilter[i].w;
    }
    
    Out.vColor = color;
    return Out;
    
    
    return Out;
}

PS_OUT PS_CHROMATIC(PS_IN In) // 크로마틱
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexUV;

    float2 offset = float2(sin(g_Time * 2.0) * 0.005, cos(g_Time * 2.0) * 0.005);
    float4 color = tex2D(TextureSampler, uv);
    float4 colorR = tex2D(TextureSampler, uv + offset);
    float4 colorB = tex2D(TextureSampler, uv - offset);

    Out.vColor = float4(colorR.r, color.g, colorB.b, color.a);
    return Out;
}


PS_OUT PS_NOISE(PS_IN In) // 노이즈
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexUV;

    float4 originalColor = tex2D(TextureSampler, uv);

    float noise = frac(sin(dot(uv * 100.0 + g_Time, float2(12.9898, 78.233))) * 43758.5453);
    float4 noiseColor = float4(noise, noise, noise, 1.0);

    float noiseStrength = 0.5 + 0.5 * sin(g_Time); // 0에서 1 사이로 변화

    Out.vColor = lerp(originalColor, noiseColor, noiseStrength);
    
    return Out;
}

PS_OUT PS_CENEMA(PS_IN In) //시네마
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = tex2D(TextureSampler, In.vTexUV);

    
    if (In.vTexUV.y < 0.2)
        Out.vColor.rgb = 0;
    
    else if (In.vTexUV.y > 0.8)
        Out.vColor.rgb = 0;
    
    return Out;
}

PS_OUT PS_HEAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexUV;

    float4 color = tex2D(TextureSampler, uv);
    float fmax = 0.15 - g_Time * 0.2;
    fmax = max(0, fmax);
    float edgeDistance = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));
    float edgeWeight = smoothstep(0.0, fmax, edgeDistance);


    float4 redColor = float4(0.0, 1.0, 0.0, 1.0);
    float4 blendedColor = lerp(color, redColor, 0.3);
    color = lerp(blendedColor, color, edgeWeight);

    Out.vColor = color;
    

    return Out;
}

PS_OUT PS_BUFF(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexUV;

    float4 color = tex2D(TextureSampler, uv);
    float fmax = 0.15 - g_Time * 0.2;
    fmax = max(0, fmax);
    float edgeDistance = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));
    float edgeWeight = smoothstep(0.0, fmax, edgeDistance);


    float4 redColor = float4(0.3, 0.7, 0.87, 1.0);
    float4 blendedColor = lerp(color, redColor, 0.3);
    color = lerp(blendedColor, color, edgeWeight);

    Out.vColor = color;
    

    return Out;
}

PS_OUT PS_TIME(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexUV;

    float4 color = tex2D(TextureSampler, uv);
    float fmax = 0.15 - g_Time * 0.2;
    fmax = max(0, fmax);
    float edgeDistance = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));
    float edgeWeight = smoothstep(0.0, fmax, edgeDistance);


    float4 redColor = float4(1.0, 1.0, 1.0, 1.0);
    float4 blendedColor = lerp(color, redColor, 0.3);
    color = lerp(blendedColor, color, edgeWeight);

    Out.vColor = color;
    

    return Out;
}


technique DefaultTechnique
{
    pass Default_Pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_MAIN();
    }

    pass Hit_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_HIT();
    }

    pass Wave_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_WAVE();
    }

    pass Tonado_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_TONADO();
    }

    pass Gaussian_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_GAUS();
    }

    pass Chromatic_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_CHROMATIC();
    }

    pass Reflection_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_NOISE();
    }

    pass Cenema_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_CENEMA();
    }

    pass HEAL_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_HEAL();
    }

    pass Buff_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_BUFF();
    }

    pass Time_pass
    {
        AlphaTestEnable = true;
        AlphaFunc = greater;
        AlphaRef = 50;
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_TIME();
    }

}

