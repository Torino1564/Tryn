
cbuffer EntityID : register(b0)
{
    uint entityID;
    uint archetypeID;
    uint padding;
    uint empty;
}

uint4 main() : SV_TARGET0
{
    return uint4(entityID, archetypeID, padding, empty);
}