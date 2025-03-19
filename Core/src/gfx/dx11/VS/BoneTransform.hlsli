#define MAX_BONE_PER_VERTEX 4

cbuffer bones : register(b5)
{
    matrix boneMatrices[1023];
};

struct TangentAndBitangent
{
    float3 tangent;
    float3 bitangent;
};

float4 CalculateBoneTransformedPosition(uint4 boneIds, float4 boneWeights, float4 position)
{
    float4 transformedPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < MAX_BONE_PER_VERTEX; i++)  
    {
        transformedPosition += mul(boneMatrices[boneIds[i]], position) * boneWeights[i];
    }
    
    return transformedPosition;
}

float3 CalculateBoneTransformedNormal(uint4 boneIds, float4 boneWeights, float3 normal)
{
    float3 transformedNormal = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < MAX_BONE_PER_VERTEX; i++)
    {
        transformedNormal += mul((float3x3) boneMatrices[boneIds[i]], normal) * boneWeights[i];
    }

    return normalize(transformedNormal);
}

TangentAndBitangent CalculateBoneTransformedTandBT(uint4 boneIds, float4 boneWeights, float3 tangent, float3 bitangent)
{
    TangentAndBitangent outVar;
    
    for (int i = 0; i < MAX_BONE_PER_VERTEX; i++)
    {
        outVar.tangent += mul((float3x3) boneMatrices[boneIds[i]], tangent) * boneWeights[i];
        outVar.bitangent += mul((float3x3) boneMatrices[boneIds[i]], bitangent) * boneWeights[i];
    }

    return outVar;
}