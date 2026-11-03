// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <OgrePrerequisites.h>

#include "GameCommon/LevelMeshes/MeshData.h"

namespace MaxDoF::GameClient::Utils
{
    void createProceduralCubeV2( Ogre::SceneManager*, const Ogre::String &meshName, float halfX, float halfY, float halfZ );
    void createProceduralSphereV2( Ogre::SceneManager*, const Ogre::String& meshName, float radius, unsigned int rings, unsigned int segments );
    void createMeshFromMeshData(Ogre::SceneManager *sceneManager, const Ogre::String &meshName, const MaxDoF::GameCommon::LevelMeshes::MeshData &meshData);
    void removeMesh(const Ogre::String& meshName);
}
