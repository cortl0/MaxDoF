// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_KeyboardController_H_
#define MaxDoF_KeyboardController_H_

#include <SDL_events.h>
#include <OgreVector3.h>

namespace MaxDoF::Input
{
    class KeyboardController
    {
        bool mWASD[4];
        bool mSlideUpDown[2];
        bool mSpeedModifier;

    public:
        struct KeyboardRawState
        {
            bool w{ false };
            bool a{ false };
            bool s{ false };
            bool d{ false };
            bool pageUp{ false };
            bool pageDown{ false };
            bool shift{ false };
        };

        KeyboardController();
        KeyboardRawState getRawState() const;

        /// Обработка нажатий и отпусканий клавиш. Возвращают true, если событие обработано.
        bool keyPressed( const SDL_KeyboardEvent &arg );
        bool keyReleased( const SDL_KeyboardEvent &arg );

        /// Возвращает чистое направление движения на основе зажатых клавиш
        Ogre::Vector3 getMovementDirection() const;

        /// Возвращает true, если зажат Shift (активен буст скорости)
        bool isSpeedBoostActive() const { return mSpeedModifier; }

        /// Сброс состояний (опционально, например при потере фокуса окна)
        void reset();
    };
}  // namespace MaxDoF::Input

#endif
