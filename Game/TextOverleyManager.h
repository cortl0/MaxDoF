// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#ifndef MaxDoF_GameClient_TextOverleyManager_H_
#define MaxDoF_GameClient_TextOverleyManager_H_

#include <GraphicsSystem.h>
#include <OgreTextAreaOverlayElement.h>

#include "GameCommon/Types/Telemetry.h"

namespace MaxDoF::GameClient
{
    class TextOverleyManager
    {
    protected:
        Demo::GraphicsSystem *mGraphicsSystem;
        Ogre::String mHelpDescription;
        Ogre::v1::TextAreaOverlayElement *mDebugText;
        Ogre::v1::TextAreaOverlayElement *mDebugTextShadow;
    public:
        Ogre::uint16 mDisplayHelpMode;
        Ogre::uint16 mNumDisplayHelpModes;

        virtual void createDebugTextOverlay();
        virtual void generateDebugText( float timeSinceLast, const GameCommon::Types::Telemetry& );

        TextOverleyManager( const Ogre::String &helpDescription );
        ~TextOverleyManager();
        void _notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem );
        void createCrosshair();
    };
}  // namespace MaxDoF::GameClient

#endif
