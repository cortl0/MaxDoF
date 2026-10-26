// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include <GraphicsSystem.h>

#include "GameClient/TextOverleyManager.h"
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
        const Ogre::String ortSphereMeshName{"MyV2Sphere"};
        const Ogre::String ortCubeMeshName{"MyV2Cube"};

        Graphics( const Ogre::String &helpDescription );
        ~Graphics();

        Demo::GraphicsSystem* getGraphicsSystem(){ return mGraphicsSystem; }
        void _notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem );
        void initialize();
        void changeTexture(Ogre::Item*, const Ogre::String& newTextureFileName);
        void changeUniqueTexture(Ogre::Item*, const Ogre::String& newTextureFileName, const Ogre::String& uniqueMaterialName);
        Ogre::SceneNode* createCube(const GameCommon::Types::GraphPose&, const Ogre::Vector3& halfSize);
        Ogre::SceneNode* createPointLight(Ogre::Vector3 position);
        Ogre::SceneNode* createShape( const Ogre::String& meshName, const GameCommon::Types::GraphPose&);
        Ogre::SceneNode* createSphere(const GameCommon::Types::GraphPose&, float radius);
        void setPositionOfGlobalStaticNode(const Ogre::Vector3&);
        void printAllMaterials();
    };
}
