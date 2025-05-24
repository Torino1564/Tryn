cbuffer EntityID : register(b0)
{
    uint entityID;
}

uint main() : SV_TARGET
{
    return entityID;
}