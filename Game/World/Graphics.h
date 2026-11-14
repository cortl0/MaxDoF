// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Game/Core/MaxDoFGraphicsSystem.h"
#include "Game/TextOverleyManager.h"
#include "Common/Types/Types3d.h"
#include "Common/Types/Poses.h"

namespace MaxDoF::Game::World
{
    class Graphics
    {
    protected:
        TextOverleyManager mTextOverleyManager;
        MaxDoF::Game::Core::MaxDoFGraphicsSystem* mGraphicsSystem{ nullptr };
        Ogre::SceneNode* globalStaticNode{ nullptr };

    public:
        const Ogre::String cOrtSphereMeshName{"MyV2Sphere"};
        const Ogre::String cOrtCubeMeshName{"MyV2Cube"};
        const Ogre::String cAsteroidMeshName{"MyV2Asteroid"};

        Graphics( const Ogre::String &helpDescription );
        ~Graphics();

        MaxDoF::Game::Core::MaxDoFGraphicsSystem* getGraphicsSystem(){ return mGraphicsSystem; }
        void _notifyGraphicsSystem(MaxDoF::Game::Core::MaxDoFGraphicsSystem*);
        void destroyAllObjects(Ogre::SceneNode*);
        void destroyNodeRecursively(Ogre::SceneNode*);
        void initialize();
        void changeTexture(Ogre::Item*, const Ogre::String& newTextureFileName);
        void changeUniqueTexture(Ogre::Item*, const Ogre::String& newTextureFileName, const Ogre::String& uniqueMaterialName);
        Ogre::SceneNode* createCube(const Common::Types::GraphPose&, const Ogre::Vector3& halfSize, Ogre::SceneNode* parent = nullptr);
        Ogre::SceneNode* createMesh(const Common::Types3d::MeshData& meshData);
        Ogre::SceneNode* createPointLight(Ogre::Vector3 position);
        Ogre::SceneNode* createShape( const Ogre::String& meshName, const Common::Types::GraphPose&, Ogre::SceneNode* parent = nullptr);
        Ogre::SceneNode* createSphere(const Common::Types::GraphPose&, float radius, Ogre::SceneNode* parent = nullptr);
        void setPositionOfGlobalStaticNode(const Ogre::Vector3&);
        void printAllMaterials();
    };
}
