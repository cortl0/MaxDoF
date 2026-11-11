// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <GraphicsSystem.h>

#include "GameClient/TextOverleyManager.h"
#include "GameCommon/Types/Types3d.h"
#include "GameCommon/Types/Poses.h"
namespace MaxDoF::GameClient::World
{
    class Graphics
    {
    protected:
        TextOverleyManager mTextOverleyManager;
        Demo::GraphicsSystem *mGraphicsSystem;
        Ogre::SceneNode* globalStaticNode;

    public:
        const Ogre::String cOrtSphereMeshName{"MyV2Sphere"};
        const Ogre::String cOrtCubeMeshName{"MyV2Cube"};
        const Ogre::String cAsteroidMeshName{"MyV2Asteroid"};

        Graphics( const Ogre::String &helpDescription );
        ~Graphics();

        Demo::GraphicsSystem* getGraphicsSystem(){ return mGraphicsSystem; }
        void _notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem );
        void destroyAllObjects(Ogre::SceneNode *node);
        void destroyNodeRecursively(Ogre::SceneNode *node);
        void initialize();
        void changeTexture(Ogre::Item*, const Ogre::String& newTextureFileName);
        void changeUniqueTexture(Ogre::Item*, const Ogre::String& newTextureFileName, const Ogre::String& uniqueMaterialName);
        Ogre::SceneNode* createCube(const GameCommon::Types::GraphPose&, const Ogre::Vector3& halfSize, Ogre::SceneNode* parent = nullptr);
        Ogre::SceneNode* createMesh(const GameCommon::Types3d::MeshData& meshData);
        Ogre::SceneNode* createPointLight(Ogre::Vector3 position);
        Ogre::SceneNode* createShape( const Ogre::String& meshName, const GameCommon::Types::GraphPose&, Ogre::SceneNode* parent = nullptr);
        Ogre::SceneNode* createSphere(const GameCommon::Types::GraphPose&, float radius, Ogre::SceneNode* parent = nullptr);
        void setPositionOfGlobalStaticNode(const Ogre::Vector3&);
        void printAllMaterials();
    };
}
