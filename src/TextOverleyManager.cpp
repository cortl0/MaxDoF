// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "MaxDoFGameState.h"

#include "GraphicsSystem.h"
#include "OgreFrameStats.h"
#include "OgreHlmsPbs.h"
#include "OgreOverlay.h"
#include "OgreOverlayContainer.h"
#include "OgreOverlayManager.h"

#include <OgreHlmsManager.h>
#include <OgreHlmsUnlit.h>
#include <OgreHlmsUnlitDatablock.h>
#include <OgreRoot.h>

namespace MaxDoF
{
    TextOverleyManager::TextOverleyManager( const Ogre::String &helpDescription ) :
        mGraphicsSystem( 0 ),
        mHelpDescription( helpDescription ),
        mDebugText( 0 ),
        mDebugTextShadow( 0 ),
        mDisplayHelpMode( 1 ),
        mNumDisplayHelpModes( 2 )
    {
    }
    //-----------------------------------------------------------------------------------
    TextOverleyManager::~TextOverleyManager()
    {
    }
    //-----------------------------------------------------------------------------------
    void TextOverleyManager::_notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem )
    {
        mGraphicsSystem = graphicsSystem;
    }
    //-----------------------------------------------------------------------------------
    void TextOverleyManager::createDebugTextOverlay()
    {
        Ogre::v1::OverlayManager &overlayManager = Ogre::v1::OverlayManager::getSingleton();
        Ogre::v1::Overlay *overlay = overlayManager.create( "DebugText" );

        Ogre::v1::OverlayContainer *panel = static_cast<Ogre::v1::OverlayContainer *>(
            overlayManager.createOverlayElement( "Panel", "DebugPanel" ) );
        mDebugText = static_cast<Ogre::v1::TextAreaOverlayElement *>(
            overlayManager.createOverlayElement( "TextArea", "DebugText" ) );
        mDebugText->setFontName( "DebugFont" );
        mDebugText->setCharHeight( 0.025f );

        mDebugTextShadow = static_cast<Ogre::v1::TextAreaOverlayElement *>(
            overlayManager.createOverlayElement( "TextArea", "0DebugTextShadow" ) );
        mDebugTextShadow->setFontName( "DebugFont" );
        mDebugTextShadow->setCharHeight( 0.025f );
        mDebugTextShadow->setColour( Ogre::ColourValue::Black );
        mDebugTextShadow->setPosition( 0.002f, 0.002f );

        panel->addChild( mDebugTextShadow );
        panel->addChild( mDebugText );
        overlay->add2D( panel );
        overlay->show();
    }
    //-----------------------------------------------------------------------------------
    void TextOverleyManager::generateDebugText( float timeSinceLast )
    {
        Ogre::String finalText;
        if( mDisplayHelpMode == 0 )
        {
            finalText = mHelpDescription;
            finalText += "\n\nPress F1 to toggle help";
            finalText +=
                "\n\nProtip: Ctrl+F1 will reload PBS shaders (for real time template editing).\n"
                "Ctrl+F2 reloads Unlit shaders.\n"
                "Ctrl+F3 reloads Compute shaders.\n"
                "Note: If the modified templates produce invalid shader code, "
                "crashes or exceptions can happen.\n";
        }
        else
        {
            const Ogre::FrameStats *frameStats = mGraphicsSystem->getRoot()->getFrameStats();
            finalText.reserve( 128 );
            finalText = "Frame time:\t";
            finalText += Ogre::StringConverter::toString( timeSinceLast * 1000.0f );
            finalText += " ms\n";
            finalText += "Frame FPS:\t";
            finalText += Ogre::StringConverter::toString( 1.0f / timeSinceLast );
            finalText += "\nAvg time:\t";
            finalText += Ogre::StringConverter::toString( frameStats->getRollingAverage() * 1000.0 );
            finalText += " ms\n";
            finalText += "Avg FPS:\t";
            finalText += Ogre::StringConverter::toString( frameStats->getRollingAverageFps() );
            //finalText += "\n\nPress F1 to toggle help";
        }

        mDebugText->setCaption( finalText );
        mDebugTextShadow->setCaption( finalText );
    }

    void TextOverleyManager::createCrosshair()
    {
        Ogre::Root* root = Ogre::Root::getSingletonPtr();
        Ogre::HlmsManager* hlmsManager = root->getHlmsManager();

        // 1. ПРОГРАММНОЕ СОЗДАНИЕ МАТЕРИАЛА ДЛЯ ЛИНИЙ ПРИЦЕЛА
        Ogre::HlmsUnlit* hlmsUnlit = static_cast<Ogre::HlmsUnlit*>(hlmsManager->getHlms(Ogre::HLMS_UNLIT));
        Ogre::String materialName = "CrosshairMaterial";

        Ogre::HlmsDatablock* datablock = hlmsUnlit->getDatablock(materialName);
        if (!datablock)
        {
            Ogre::HlmsUnlitDatablock* unlitDatablock = static_cast<Ogre::HlmsUnlitDatablock*>(
                hlmsUnlit->createDatablock(materialName, materialName, Ogre::HlmsMacroblock(), Ogre::HlmsBlendblock(), Ogre::HlmsParamVec())
            );

            // Устанавливаем чисто белый цвет прицела
            unlitDatablock->setUseColour(true);
            unlitDatablock->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));

            // Отключаем проверку глубины Z-буфера, чтобы прицел всегда рисовался поверх 3D-мира
            Ogre::HlmsMacroblock macroblock = *unlitDatablock->getMacroblock();
            macroblock.mDepthCheck = false;
            macroblock.mDepthWrite = false;
            unlitDatablock->setMacroblock(macroblock);
        }

        // 2. СОЗДАНИЕ 2D ИНТЕРФЕЙСА ПЕРЕКРЕСТИЯ
        auto& overlayManager = Ogre::v1::OverlayManager::getSingleton();

        // Создаем невидимый корневой контейнер. Он жестко зафиксирует центр экрана.
        auto* crosshairRoot = static_cast<Ogre::v1::OverlayContainer*>(
            overlayManager.createOverlayElement("Panel", "CrosshairRootPanel")
        );
        crosshairRoot->setMetricsMode(Ogre::v1::GMM_PIXELS);
        crosshairRoot->setDimensions(0.0f, 0.0f); // Размер нулевой, это просто точка привязки
        crosshairRoot->setHorizontalAlignment(Ogre::v1::GHA_CENTER);
        crosshairRoot->setVerticalAlignment(Ogre::v1::GVA_CENTER);
        crosshairRoot->setLeft(0.0f);
        crosshairRoot->setTop(0.0f);

        // Размеры линий перекрестия в пикселях (крестик 24х24 пикселя, толщиной 2)
        float lineLength = 24.0f;
        float lineWidth = 2.0f;

        // --- Создаем горизонтальную линию ---
        auto* horizLine = overlayManager.createOverlayElement("Panel", "CrosshairHorizLine");
        horizLine->setMetricsMode(Ogre::v1::GMM_PIXELS);
        horizLine->setDimensions(lineLength, lineWidth);
        horizLine->setMaterialName(materialName);
        horizLine->setLeft(-lineLength / 2.0f);
        horizLine->setTop(-lineWidth / 2.0f);

        // --- Создаем вертикальную линию ---
        auto* vertLine = overlayManager.createOverlayElement("Panel", "CrosshairVertLine");
        vertLine->setMetricsMode(Ogre::v1::GMM_PIXELS);
        vertLine->setDimensions(lineWidth, lineLength);
        vertLine->setMaterialName(materialName);
        vertLine->setLeft(-lineWidth / 2.0f);
        vertLine->setTop(-lineLength / 2.0f);

        // Прикрепляем обе линии к центральному невидимому родителю
        crosshairRoot->addChild(horizLine);
        crosshairRoot->addChild(vertLine);

        // Добавляем всю конструкцию на слой оверлея и включаем отображение
        auto* crosshairOverlay = overlayManager.create("CrosshairOverlay");
        crosshairOverlay->add2D(crosshairRoot);
        crosshairOverlay->show();
    }

}  // namespace MaxDoF
