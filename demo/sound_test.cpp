//
//  sound_test.cpp
//  Borealis
//
//  Created by Daniil Vinogradov on 16.02.2021.
//

#include "sound_test.hpp"

char* soundsMap[] = {
    "SeNaviFocus",
    "SeNaviDecide",
    "SeTouchUnfocus",
    "SeBtnFocus",
    "SeBtnDecide",
    "SeBtnDecideRepeat",
    "SeToggleBtnFocus",
    "SeToggleBtnOn",
    "SeToggleBtnOff",
    "SeCheckboxFocus",
    "SeCheckboxOn",
    "SeCheckboxOff",
    "SeRadioBtnFocus",
    "SeRadioBtnOn",
    "SeSliderFocus",
    "SeSliderTickOver",
    "SeSliderRelease",
    "SeKeyError",
    "SeFooterFocus",
    "SeFooterDecideBack",
    "SeFooterDecideFinish",
    "SeWaiting",
    "SeSelectFocus",
    "SeSelectCheck",
    "SeSelectUncheck",
    "SeDialogOpen",
    "SeTouchInner",
    "SeTouch",
    "SeKeyErrorCursor",
    "SeKeyErrorScroll",
    "SeKeyErrorTouch",
    "SeHelpBtnFocus",
    "SeKeyRecieved",
    "SeAccountDecide",
    "SeAccountFocus"
};

#ifdef __SWITCH__
#define QLAUNCH_PID 0x0100000000001000
#define QLAUNCH_MOUNT_POINT "qlaunch"
#define QLAUNCH_BFSAR_PATH "qlaunch:/sound/qlaunch.bfsar"
#endif

SoundTestTab::SoundTestTab()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/sounds_test.xml");

    brls::Label* soundName = (brls::Label*)this->getView("sound_label");
    soundName->setText(soundsMap[this->selectedSound]);
    
    initSound();
    
    // Get a handle to the button and register the action directly
    brls::Button* playButton = (brls::Button*)this->getView("button_play_sound");
    playButton->registerAction(
        "Play sound", brls::BUTTON_A, [this](brls::View* view) {
        return playSound();
    }, false, brls::SOUND_NONE);

    registerAction("Prev sound", brls::BUTTON_LB, [this](brls::View* view) {
        if (this->selectedSound > 0)
            this->selectedSound--;

        brls::Label* soundName = (brls::Label*)this->getView("sound_label");
        soundName->setText(soundsMap[this->selectedSound]);

        return true;
    });

    registerAction("Next sound", brls::BUTTON_RB, [this](brls::View* view) {
        if (this->selectedSound < sizeof(sounds) / sizeof(sounds[0]) - 1)
            this->selectedSound++;

        brls::Label* soundName = (brls::Label*)this->getView("sound_label");
        soundName->setText(soundsMap[this->selectedSound]);

        return true;
    });
}

void SoundTestTab::initSound()
{
#ifdef __SWITCH__
    PLSR_RC rc = plsrPlayerInit();
    if (PLSR_RC_FAILED(rc))
    {
        brls::Logger::error("Unable to init Pulsar player: {:#x}", rc);
        return;
    }

    // Mount qlaunch ROMFS for the BFSAR
    Result result = romfsMountDataStorageFromProgram(QLAUNCH_PID, QLAUNCH_MOUNT_POINT);
    if (!R_SUCCEEDED(result))
    {
        brls::Logger::error("Unable to mount qlaunch ROMFS: {:#x}", result);

        plsrPlayerExit();
        return;
    }

    // Open qlaunch BFSAR
    rc = plsrBFSAROpen(QLAUNCH_BFSAR_PATH, &this->qlaunchBfsar);
    if (PLSR_RC_FAILED(rc))
    {
        brls::Logger::error("Unable to open qlaunch BFSAR: {:#x}", rc);

        plsrPlayerExit();
        return;
    }

    for (size_t sound = 0; sound < SOUNDS; sound++)
        this->sounds[sound] = PLSR_PLAYER_INVALID_SOUND;
#endif
}

bool SoundTestTab::playSound()
{
#ifdef __SWITCH__
    if (this->sounds[this->selectedSound] == PLSR_PLAYER_INVALID_SOUND)
    {
        PLSR_RC rc = plsrPlayerLoadSoundByName(&this->qlaunchBfsar, soundsMap[this->selectedSound], &this->sounds[this->selectedSound]);
        if (PLSR_RC_FAILED(rc))
        { 
            brls::Logger::warning("Unable to load sound {}: {:#x}", soundsMap[this->selectedSound], rc);
            return false;
        }
    }
    
    PLSR_RC rc = plsrPlayerPlay(this->sounds[this->selectedSound]);
    if (PLSR_RC_FAILED(rc))
    {
        brls::Logger::error("Unable to play sound");
        return false;
    }
#endif
    return true;
}

brls::View* SoundTestTab::create()
{
    // Called by the XML engine to create a new ComponentsTab
    return new SoundTestTab();
}
