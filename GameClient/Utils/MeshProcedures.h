// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#ifndef MaxDoF_GameClient_Utils_MeshProcedures_H_
#define MaxDoF_GameClient_Utils_MeshProcedures_H_

#include <OgrePrerequisites.h>

namespace MaxDoF::GameClient::Utils
{
    void createProceduralCubeV2( Ogre::SceneManager*, const Ogre::String &meshName, float halfX, float halfY, float halfZ );
    void createProceduralSphereV2( Ogre::SceneManager*, const Ogre::String& meshName, float radius, unsigned int rings, unsigned int segments );
}  // namespace MaxDoF::GameClient::Utils

#endif
