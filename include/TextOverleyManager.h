// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_TextOverleyManager_H_
#define MaxDoF_TextOverleyManager_H_

#include "GameState.h"
#include "OgreTextAreaOverlayElement.h"

namespace Ogre
{
    namespace v1
    {
        class TextAreaOverlayElement;
    }
}  // namespace Ogre

namespace MaxDoF
{
    /// Base game state for the tutorials. All it does is show a little text on screen :)
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
}  // namespace MaxDoF

#endif
