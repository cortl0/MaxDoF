// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_SpacecraftInputTranslator_H_
#define MaxDoF_SpacecraftInputTranslator_H_

#include "SpacecraftController.h"

namespace MaxDoF::Input
{
    class KeyboardController;
    class MouseController;

    class SpacecraftInputTranslator
    {
    public:
        SpacecraftController::ControllerData translate(
            const KeyboardController* keyboard,
            const MouseController* mouse
        );
    };
}  // namespace MaxDoF::Input

#endif
