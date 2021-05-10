//
//  sound_test.hpp
//  Borealis
//
//  Created by Daniil Vinogradov on 16.02.2021.
//

#pragma once

#include <borealis.hpp>


#ifdef __SWITCH__
#include <pulsar.h>
#endif

#define SOUNDS 35

class SoundTestTab : public brls::Box
{
public:
    SoundTestTab();

    static brls::View* create();
private:
    void initSound();
    bool playSound();
    int selectedSound = 0;
    
    
#ifdef __SWITCH__
    PLSR_BFSAR qlaunchBfsar;
    PLSR_PlayerSoundId sounds[SOUNDS];
#endif
    
};
