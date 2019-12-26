/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jean Pierre Cimalando
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "CheckBox.h"
#include "Cairo.hpp"
#include "Window.hpp"

///
CheckBox::CheckBox(uint32_t frameColor, uint32_t fillColor, Widget *group)
    : CControl(group), fFrameColor(frameColor), fFillColor(fillColor)
{
}

bool CheckBox::isChecked() const
{
    return _isChecked(getValue());
}

bool CheckBox::_isChecked(double value) const
{
    return value == fValueBound2;
}

void CheckBox::setValue(double value, NotifyMode notify)
{
    CControl::setValue(_isChecked(value) ? fValueBound2 : fValueBound1, notify);
}

void CheckBox::setValueBounds(double v1, double v2)
{
    bool checked = isChecked();
    fValueBound1 = v1;
    fValueBound2 = v2;
    setValue(checked ? v2 : v1);
}

bool CheckBox::onMouse(const MouseEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    if (!fIsDragging && event.press && event.button == 1)
    {
        bool insideX = mpos.getX() >= 0 && (unsigned)mpos.getX() < wsize.getWidth();
        bool insideY = mpos.getY() >= 0 && (unsigned)mpos.getY() < wsize.getHeight();

        if (!insideX || !insideY)
            return false;

        fIsDragging = true;
        fIsDraggingOver = true;
        repaint();
        return true;
    }
    else if (fIsDragging && !event.press && event.button == 1)
    {
        if (fIsDraggingOver)
            setValue(isChecked() ? fValueBound1 : fValueBound2);

        fIsDragging = false;
        repaint();
        return true;
    }

    return false;
}

bool CheckBox::onMotion(const MotionEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    if (fIsDragging) {
        bool insideX = mpos.getX() >= 0 && (unsigned)mpos.getX() < wsize.getWidth();
        bool insideY = mpos.getY() >= 0 && (unsigned)mpos.getY() < wsize.getHeight();

        bool isDraggingOver = insideX && insideY;
        if (fIsDraggingOver != isDraggingOver) {
            fIsDraggingOver = isDraggingOver;
            repaint();
        }

        return true;
    }

    return false;
}

void CheckBox::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;

    cairo_rectangle(cr, 0.5, 0.5, getWidth(), getHeight());

    bool fill = isChecked();
    if (fIsDragging)
        fill ^= fIsDraggingOver;

    if (fill) {
        cairo_set_source_rgba32(cr, fFillColor);
        cairo_fill_preserve(cr);
    }

    cairo_set_source_rgba32(cr, fFrameColor);
    cairo_stroke(cr);
}
