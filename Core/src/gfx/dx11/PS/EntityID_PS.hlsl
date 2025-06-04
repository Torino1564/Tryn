
cbuffer EntityID : register(b0)
{
    uint entityID;
    uint archetypeID;
    uint padding;
    uint empty;
}

uint1x4 main() : SV_TARGET
{
    return uint1x4(entityID, archetypeID, padding, empty);
}