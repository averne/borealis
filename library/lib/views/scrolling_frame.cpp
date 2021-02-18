/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020-2021  natinusala

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <borealis/core/animations.hpp>
#include <borealis/core/application.hpp>
#include <borealis/core/util.hpp>
#include <borealis/views/scrolling_frame.hpp>
#include <borealis/core/touch/pan_gesture_recognizer.hpp>

namespace brls
{

// TODO: don't forget absolute positioning
// TODO: RestoreFocus flag in Box to remember focus (sidebar...)
// TODO: image (showcase + put in appletframe/tabframe header)
// TODO: use a menu timer in tabframe to defer the tab loading on input like HOS does
// TODO: fancy pants scrolling
// TODO: clarify addView / removeView -> setContentView, add it for everything or remove it
// TODO: streamline TransitionAnimation, it should be generic to activities
// TODO: does the fade in animation work?
// TODO: use fmt to format the ugly XML logic_errors
// TODO: find a way to reduce the number of invalidations on boot
// TODO: use HasNewLayout and MarkAsSeen around onLayout() (so in the event ? does it work ? or does the event only trigger on new layouts already ? or on all layouts ?)

// TODO: rework the highlight pulsation animation, it's not good enough

// TODO: it's time to do proper documentation using doxygen or whatever

// TODO: fix shitty frame pacer - try cpp high precision clock - see if it works fine on Switch, in which case only enable it there
// TODO: recycling, asynctask

// TODO: translate everything in fr
// TODO: make sure all useless invalidate calls are removed
// TODO: solve all TODOs in the diff
// TODO: clean the other TODOs

// TODO: ASAN time
// TODO: decomment everything in borealis.hpp
// TODO: cleanup the old example files
// TODO: copyright on all changed files
// TODO: clean the project board and rewrite documentation once this is out
// TODO: change the brls description once this is out: it's a cross-platform controller / gaming console oriented UI library with a Switch look and feel

ScrollingFrame::ScrollingFrame()
{
    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "scrollingBehavior", ScrollingBehavior, this->setScrollingBehavior,
        {
            { "natural", ScrollingBehavior::NATURAL },
            { "centered", ScrollingBehavior::CENTERED },
        });
    
    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "axis", ScrollingAxis, this->setScrollingAxis,
        {
            { "horizontal", ScrollingAxis::HORIZONTAL },
            { "vertical", ScrollingAxis::VERTICAL },
        });

    this->setMaximumAllowedXMLElements(1);

    addTouchRecognizer();
}

void ScrollingFrame::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    // Update scrolling - try until it works
    if (this->updateScrollingOnNextFrame && this->updateScrolling(false))
        this->updateScrollingOnNextFrame = false;

    // Enable scissoring
    nvgSave(vg);
    float scrollingTop    = this->getScrollingAreaTopBoundary();
    float scrollingHeight = this->getScrollingAreaHeight();
    float scrollingLeft    = this->getScrollingAreaLeftBoundary();
    float scrollingWidth = this->getScrollingAreaWidth();
    nvgIntersectScissor(vg, scrollingLeft, scrollingTop, scrollingWidth, scrollingHeight);

    // Draw children
    Box::draw(vg, x, y, width, height, style, ctx);

    //Disable scissoring
    nvgRestore(vg);
}

void ScrollingFrame::addView(View* view)
{
    this->setContentView(view);
}

void ScrollingFrame::removeView(View* view)
{
    this->setContentView(nullptr);
}

void ScrollingFrame::setContentView(View* view)
{
    if (this->contentView)
    {
        Box::removeView(this->contentView); // will delete and call willDisappear
        this->contentView = nullptr;
    }

    if (!view)
        return;

    // Setup the view and add it
    this->contentView = view;

    view->detach();
    view->setCulled(false);
    
    if (axis == ScrollingAxis::VERTICAL)
        view->setMaxWidth(this->getWidth());
    else
        view->setMaxHeight(this->getHeight());
    
    view->setDetachedPosition(this->getX(), this->getY());

    Box::addView(view); // will invalidate the scrolling box, hence calling onLayout and invalidating the contentView
}

void ScrollingFrame::onLayout()
{
    if (this->contentView)
    {
        if (axis == ScrollingAxis::VERTICAL)
            this->contentView->setMaxWidth(this->getWidth());
        else
            this->contentView->setMaxHeight(this->getHeight());
        
        this->contentView->setDetachedPosition(this->getX(), this->getY());
        this->contentView->invalidate();
    }
}

float ScrollingFrame::getScrollingAreaTopBoundary()
{
    return this->getY();
}

float ScrollingFrame::getScrollingAreaLeftBoundary()
{
    return this->getX();
}

float ScrollingFrame::getScrollingAreaHeight()
{
    return this->getHeight();
}

float ScrollingFrame::getScrollingAreaWidth()
{
    return this->getWidth();
}

void ScrollingFrame::willAppear(bool resetState)
{
    this->prebakeScrolling();

    // First scroll all the way to the top
    // then wait for the first frame to scroll
    // to the selected view if needed (only known then)
    if (resetState)
    {
        this->startScrolling(false, 0.0f);
        this->updateScrollingOnNextFrame = true; // focus may have changed since
    }

    Box::willAppear(resetState);
}

void ScrollingFrame::prebakeScrolling()
{
    // Prebaked values for scrolling
    float x      = this->getScrollingAreaLeftBoundary();
    float width = this->getScrollingAreaWidth();
    
    float y      = this->getScrollingAreaTopBoundary();
    float height = this->getScrollingAreaHeight();
    
    this->middleX = x + width / 2;
    this->bottomX = x + width;

    this->middleY = y + height / 2;
    this->bottomY = y + height;
}

void ScrollingFrame::startScrolling(bool animated, float newScroll)
{
    float* scroll;
    if (axis == ScrollingAxis::VERTICAL)
        scroll = &this->scrollY;
    else
        scroll = &this->scrollX;
    
    if (newScroll == *scroll)
        return;

    if (animated)
    {
        Style style = Application::getStyle();
        animateScrolling(newScroll, style["brls/animations/highlight"]);
    }
    else
    {
        menu_animation_ctx_tag tag = (menu_animation_ctx_tag) & *scroll;
        menu_animation_kill_by_tag(&tag);
        
        *scroll = newScroll;
        this->invalidate();
    }
}

void ScrollingFrame::animateScrolling(float newScroll, float time)
{
    menu_animation_ctx_tag tag = (menu_animation_ctx_tag) & this->scrollY;
    menu_animation_kill_by_tag(&tag);
    
    menu_animation_ctx_entry_t entry;
    entry.cb           = [](void* userdata) {};
    entry.duration     = time;
    entry.easing_enum  = EASING_OUT_QUAD;
    entry.subject      = axis == ScrollingAxis::VERTICAL ? &this->scrollY : &this->scrollX;
    entry.tag          = tag;
    entry.target_value = newScroll;
    entry.tick         = [this](void* userdata) { this->scrollAnimationTick(); };
    entry.userdata     = nullptr;

    menu_animation_push(&entry);
    
    this->invalidate();
}

void ScrollingFrame::setScrollingBehavior(ScrollingBehavior behavior)
{
    this->behavior = behavior;
}

void ScrollingFrame::setScrollingAxis(ScrollingAxis axis)
{
    this->axis = axis;
    addTouchRecognizer();
}

float ScrollingFrame::getContentHeight()
{
    if (!this->contentView)
        return 0;

    return this->contentView->getHeight();
}

float ScrollingFrame::getContentWidth()
{
    if (!this->contentView)
        return 0;

    return this->contentView->getWidth();
}

void ScrollingFrame::scrollAnimationTick()
{
    if (this->contentView)
    {
        if (axis == ScrollingAxis::VERTICAL)
            this->contentView->setTranslationY(-(this->scrollY * this->getContentHeight()));
        else
            this->contentView->setTranslationX(-(this->scrollX * this->getContentWidth()));
    }
}

void ScrollingFrame::addTouchRecognizer()
{
    this->gestureRecognizers.clear();
    
    if (axis == ScrollingAxis::VERTICAL)
        addGestureRecognizer(new PanGestureRecognizer([this](PanGestureRecognizer* pan)
        {
            float contentHeight = this->getContentHeight();

            static float startY;
            if (pan->getState() == GestureState::START)
                startY = this->scrollY * contentHeight;

            float newScroll = (startY - (pan->getY() - pan->getStartY())) / contentHeight;
            float bottomLimit = (contentHeight - this->getScrollingAreaHeight()) / contentHeight;
            
            // Bottom boundary
            if (newScroll > bottomLimit)
                newScroll = bottomLimit;

            // Top boundary
            if (newScroll < 0.0f)
                newScroll = 0.0f;

            // Start animation
            if (pan->getState() != GestureState::END)
                startScrolling(true, newScroll);
            else
            {
                float time = pan->getAcceleration().timeY * 1000.0f;
                float newPos = this->scrollY * contentHeight + pan->getAcceleration().distanceY;
                
                // Bottom boundary
                float bottomLimit = contentHeight - this->getScrollingAreaHeight();
                if (newPos > bottomLimit)
                {
                    time = time * (1 - fabs(newPos - bottomLimit) / fabs(pan->getAcceleration().distanceY));
                    newPos = bottomLimit;
                }
                
                // Top boundary
                if (newPos < 0)
                {
                    time = time * (1 - fabs(newPos) / fabs(pan->getAcceleration().distanceY));
                    newPos = 0;
                }
                
                newScroll = newPos / contentHeight;
                
                if (newScroll == this->scrollY || time < 100)
                    return;

                animateScrolling(newScroll, time);
            }
        }, PanAxis::VERTICAL));
    else
        addGestureRecognizer(new PanGestureRecognizer([this](PanGestureRecognizer* pan)
        {
            float contentWidth = this->getContentWidth();

            static float startX;
            if (pan->getState() == GestureState::START)
                startX = this->scrollX * contentWidth;

            float newScroll = (startX - (pan->getX() - pan->getStartX())) / contentWidth;
            float rightLimit = (contentWidth - this->getScrollingAreaWidth()) / contentWidth;
            
            // Bottom boundary
            if (newScroll > rightLimit)
                newScroll = rightLimit;

            // Top boundary
            if (newScroll < 0.0f)
                newScroll = 0.0f;

            // Start animation
            if (pan->getState() != GestureState::END)
                startScrolling(true, newScroll);
            else
            {
                float time = pan->getAcceleration().timeX * 1000.0f;
                float newPos = this->scrollX * contentWidth + pan->getAcceleration().distanceX;
                
                // Bottom boundary
                float rightLimit = contentWidth - this->getScrollingAreaWidth();
                if (newPos > rightLimit)
                {
                    time = time * (1 - fabs(newPos - rightLimit) / fabs(pan->getAcceleration().distanceX));
                    newPos = rightLimit;
                }
                
                // Top boundary
                if (newPos < 0)
                {
                    time = time * (1 - fabs(newPos) / fabs(pan->getAcceleration().distanceX));
                    newPos = 0;
                }
                
                newScroll = newPos / contentWidth;
                
                if (newScroll == this->scrollX || time < 100)
                    return;

                animateScrolling(newScroll, time);
            }
        }, PanAxis::HORIZONTAL));
}

void ScrollingFrame::onChildFocusGained(View* directChild, View* focusedView)
{
    // Start scrolling
    if (!Application::getFocusTouchMode())
        this->updateScrolling(true);

    Box::onChildFocusGained(directChild, focusedView);
}

bool ScrollingFrame::updateScrolling(bool animated)
{
    if (!this->contentView)
        return false;

    if (axis == ScrollingAxis::VERTICAL)
    {
        float contentHeight = this->getContentHeight();

        View* focusedView                  = Application::getCurrentFocus();
        int currentSelectionMiddleOnScreen = focusedView->getY() + focusedView->getHeight() / 2;
        float newScroll                    = -(this->scrollY * contentHeight) - (currentSelectionMiddleOnScreen - this->middleY);

        // Bottom boundary
        if (this->getScrollingAreaTopBoundary() + newScroll + contentHeight < this->bottomY)
            newScroll = this->getScrollingAreaHeight() - contentHeight;

        // Top boundary
        if (newScroll > 0.0f)
            newScroll = 0.0f;

        // Apply 0.0f -> 1.0f scale
        newScroll = abs(newScroll) / contentHeight;

        //Start animation
        this->startScrolling(animated, newScroll);
    }
    else
    {
        float contentWidth = this->getContentWidth();

        View* focusedView                  = Application::getCurrentFocus();
        int currentSelectionMiddleOnScreen = focusedView->getX() + focusedView->getWidth() / 2;
        float newScroll                    = -(this->scrollX * contentWidth) - (currentSelectionMiddleOnScreen - this->middleX);

        // Bottom boundary
        if (this->getScrollingAreaLeftBoundary() + newScroll + contentWidth < this->bottomX)
            newScroll = this->getScrollingAreaWidth() - contentWidth;

        // Top boundary
        if (newScroll > 0.0f)
            newScroll = 0.0f;

        // Apply 0.0f -> 1.0f scale
        newScroll = abs(newScroll) / contentWidth;

        //Start animation
        this->startScrolling(animated, newScroll);
    }

    return true;
}

#define NO_PADDING fatal("Padding is not supported by brls:ScrollingFrame, please set padding on the content view instead");

void ScrollingFrame::setPadding(float top, float right, float bottom, float left)
{
    NO_PADDING
}

void ScrollingFrame::setPaddingTop(float top)
{
    NO_PADDING
}

void ScrollingFrame::setPaddingRight(float right)
{
    NO_PADDING
}

void ScrollingFrame::setPaddingBottom(float bottom)
{
    NO_PADDING
}

void ScrollingFrame::setPaddingLeft(float left) {
    NO_PADDING
}

View* ScrollingFrame::create()
{
    return new ScrollingFrame();
}

} // namespace brls
