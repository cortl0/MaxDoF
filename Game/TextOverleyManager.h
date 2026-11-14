// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#ifndef MaxDoF_Game_TextOverleyManager_H_
#define MaxDoF_Game_TextOverleyManager_H_

#include <OgreRoot.h>
#include <OgreTextAreaOverlayElement.h>

#include "Common/Types/Telemetry.h"

namespace MaxDoF::Game
{
    class TextOverleyManager
    {
    protected:
        Ogre::Root *mRoot;
        Ogre::String mHelpDescription;
        Ogre::v1::TextAreaOverlayElement *mDebugText;
        Ogre::v1::TextAreaOverlayElement *mDebugTextShadow;
    public:
        Ogre::uint16 mDisplayHelpMode;
        Ogre::uint16 mNumDisplayHelpModes;

        virtual void createDebugTextOverlay();
        virtual void generateDebugText( float timeSinceLast, const Common::Types::Telemetry& );

        TextOverleyManager( const Ogre::String &helpDescription );
        virtual ~TextOverleyManager();
        void _notifyRoot( Ogre::Root *root );
        void createCrosshair();
    };
}  // namespace MaxDoF::Game

#endif
