/*
    Copyright 2021 XITRIX

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <borealis/core/application.hpp>
#include <borealis/core/util.hpp>
#include <borealis/core/touch/pan_gesture.hpp>
#include <borealis/views/slider.hpp>

namespace brls
{

Slider::Slider()
{
    input = Application::getPlatform()->getInputManager();
    
    line = new Rectangle();
    lineEmpty = new Rectangle();
    pointer = new Rectangle();
    
    line->detach();
    lineEmpty->detach();
    pointer->detach();
    
    addView(pointer);
    addView(line);
    addView(lineEmpty);
    
    setHeight(40);
    
    line->setHeight(7);
    line->setCornerRadius(3.5f);
    
    lineEmpty->setHeight(7);
    lineEmpty->setCornerRadius(3.5f);
    
    pointer->setHeight(38);
    pointer->setWidth(38);
    pointer->setCornerRadius(19);
    pointer->setHighlightCornerRadius(21);
    pointer->setBorderThickness(4);
    pointer->setShadowType(ShadowType::GENERIC);
    pointer->setShadowVisibility(true);
    pointer->setFocusable(true);
    
    Theme theme = Application::getTheme();
    pointer->setColor(theme["brls/slider/pointer_color"]);
    pointer->setBorderColor(theme["brls/slider/pointer_border_color"]);
    
    line->setColor(theme["brls/slider/line_filled"]);
    lineEmpty->setColor(theme["brls/slider/line_empty"]);
    
    pointer->registerAction("Right Click Blocker", BUTTON_RIGHT, [this](View* view)
    {
        return true;
    }, true, SOUND_NONE);
    
    pointer->registerAction("Right Click Blocker", BUTTON_LEFT, [this](View* view)
    {
        return true;
    }, true, SOUND_NONE);
    
    pointer->addGestureRecognizer(new PanGestureRecognizer([this](PanGestureStatus status)
    {
        Application::giveFocus(pointer);
        
        static float lastProgress = progress;
        if (status.state == GestureState::START)
        {
            lastProgress = progress;
        }
        
        float paddingWidth = getWidth() - pointer->getWidth();
        float delta = status.position.x - status.startPosition.x;
        
        setProgress(lastProgress + delta / paddingWidth);
    }, PanAxis::HORIZONTAL));
    
    progress = 0.33f;
}

void Slider::onLayout()
{
    Box::onLayout();
    updateUI();
}


View* Slider::getDefaultFocus()
{
    return pointer;
}

void Slider::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    if (pointer->isFocused())
    {
        ControllerState state;
        input->updateControllerState(&state);
        static bool repeat = false;
        
        if (state.buttons[BUTTON_RIGHT])
        {
            setProgress(progress += 0.5f / 60.0f);
            if (progress >= 1 && !repeat)
            {
                repeat = true;
                pointer->shakeHighlight(FocusDirection::RIGHT);
                Application::getAudioPlayer()->play(SOUND_FOCUS_ERROR);
            }
        }
        
        if (state.buttons[BUTTON_LEFT])
        {
            setProgress(progress -= 0.5f / 60.0f);
            if (progress <= 0 && !repeat)
            {
                repeat = true;
                pointer->shakeHighlight(FocusDirection::LEFT);
                Application::getAudioPlayer()->play(SOUND_FOCUS_ERROR);
            }
        }
        
        if ((!state.buttons[BUTTON_RIGHT] && !state.buttons[BUTTON_LEFT]) || (progress > 0 && progress < 1))
        {
            repeat = false;
        }
    }
    
    Box::draw(vg, x, y, width, height, style, ctx);
}

void Slider::setProgress(float progress)
{
    this->progress = progress;
    
    if (this->progress < 0)
        this->progress = 0;
    
    if (this->progress > 1)
        this->progress = 1;
    
    progressEvent.fire(this->progress);
    updateUI();
}

void Slider::updateUI()
{
    float paddingWidth = getWidth() - pointer->getWidth();
    float lineStart = pointer->getWidth() / 2;
    float lineStartWidth = paddingWidth * progress;
    float lineEnd = paddingWidth * progress + pointer->getWidth() / 2;
    float lineEndWidth = paddingWidth * (1 - progress);
    float lineYPos = getHeight() / 2 - line->getHeight() / 2;
    
    line->setDetachedPosition(lineStart, lineYPos);
    line->setWidth(lineStartWidth);
    
    lineEmpty->setDetachedPosition(lineEnd, lineYPos);
    lineEmpty->setWidth(lineEndWidth);
    
    pointer->setDetachedPosition(lineEnd - pointer->getWidth() / 2, 0);
}

View* Slider::create()
{
    return new Slider();
}

} // namespace brls
