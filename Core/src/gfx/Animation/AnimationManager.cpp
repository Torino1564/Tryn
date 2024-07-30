#include "TrynPCH.h"
#include "AnimationManager.h"
#include <Core/src/utl/Assert.h>
#include <assimp/anim.h>
#include <Core/src/gfx/Assimp.h>
namespace tryn::gfx::ani
{
    uint32_t AnimationManager::New(const std::string& path, aiAnimation& anim)
    {
        // New Animation
        auto id = ResolveID();
        animationPtrArray[id] = std::make_shared<Animation>(path, id, anim);
        return id;
    }
    uint32_t AnimationManager::New(const std::string& path)
    {
        auto& imp = AssimpManager::Get();
        const auto pScene = imp.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ConvertToLeftHanded |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace
        );

        for (int i = 0; i < pScene->mNumAnimations; i++)
        {
            return New(path, *pScene->mAnimations[i]);
        }
    }
    std::shared_ptr<Animation> AnimationManager::Resolve(const uint32_t key)
    {
        trynass_msg(IDbooker[key] == false, L"Attempted to resolve an invalid animation!");

        return animationPtrArray[key];
    }
    std::shared_ptr<Animation> AnimationManager::Resolve(const std::string& key)
    {
        auto it = nameMapper.find(key);

        trynass_msg(it == nameMapper.end(), L"Attempted to resolve an invalid animation!");

        return animationPtrArray[it->second];
    }
    AnimationManager::AnimationManager()
    {
        Resize(100);
    }
    uint32_t AnimationManager::ResolveID()
    {
        bool foundSlot = false;
        size_t slot = 0;
        while (!foundSlot)
        {
            slot = IDbooker.find_next(idPointer - 1);
            if (slot != IDbooker.npos)
            {
                idPointer = slot;
                foundSlot = true;
                IDbooker[slot].flip();
                continue;
            }
            Resize(upperLimit + 50);
        }

        return slot;
    }
    void AnimationManager::Resize(const size_t newSize)
    {
        IDbooker.resize(newSize, true);
        animationPtrArray.resize(newSize);
        upperLimit = newSize;
    }
}
