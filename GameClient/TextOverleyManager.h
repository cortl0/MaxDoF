// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameClient_TextOverleyManager_H_
#define MaxDoF_GameClient_TextOverleyManager_H_

#include <GraphicsSystem.h>
#include <OgreTextAreaOverlayElement.h>

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
        virtual void generateDebugText( float timeSinceLast );

        TextOverleyManager( const Ogre::String &helpDescription );
        ~TextOverleyManager();
        void _notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem );
        void createCrosshair();
    };
}  // namespace MaxDoF::GameClient

#endif
