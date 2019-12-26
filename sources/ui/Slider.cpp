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

#include "Slider.h"
#include "Cairo.hpp"
#include "Window.hpp"

///
Slider::Slider(cairo_surface_t *imgBody, cairo_surface_t *imgHandle, Widget *group)
    : CControl(group), fImgBody(imgBody), fImgHandle(imgHandle)
{
    unsigned wBody = cairo_image_surface_get_width(imgBody);
    unsigned hBody = cairo_image_surface_get_height(imgBody);
    setSize(wBody, hBody);
}

void Slider::setValue(double value, NotifyMode notify)
{
    CControl::setValue(clampToBounds(value), notify);
}

void Slider::setValueBounds(double v1, double v2)
{
    fValueBound1 = v1;
    fValueBound2 = v2;
    setValue(getValue());
}

void Slider::setLogarithmic(bool log)
{
    if (fIsLogarithmic == log)
        return;

    fIsLogarithmic = log;
    repaint();
}

void Slider::setNumSteps(unsigned numSteps)
{
    fNumSteps = numSteps;
}

void Slider::setOrientation(Orientation ori)
{
    if (fOrientation == ori)
        return;

    fOrientation = ori;
    repaint();
}

bool Slider::onMouse(const MouseEvent &event)
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
        beginChangeGesture();

        double fill = 0;
        if (fOrientation == Horizontal)
            fill = mpos.getX() / (double)wsize.getWidth();
        else
            fill = 1.0 - (mpos.getY() / (double)wsize.getHeight());

        setValue(valueForRatio(fill));

        return true;
    }
    else if (fIsDragging && !event.press && event.button == 1)
    {
        fIsDragging = false;
        endChangeGesture();
        return true;
    }

    return false;
}

bool Slider::onMotion(const MotionEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    if (fIsDragging)
    {
        double fill = 0;
        bool doNotSet = false;
        if (fOrientation == Horizontal)
            fill = mpos.getX() / (double)wsize.getWidth();
        else if (fOrientation == Vertical)
            fill = 1.0 - (mpos.getY() / (double)wsize.getHeight());
        fill = (fill < 0) ? 0 : fill;
        fill = (fill > 1) ? 1 : fill;
        if (!doNotSet)
            setValue(valueForRatio(fill));
        return true;
    }

    return false;
}

bool Slider::onScroll(const ScrollEvent &event)
{
    DGL::Size<uint> wsize = getSize();
    DGL::Point<int> mpos = event.pos;

    bool inside = mpos.getX() >= 0 && mpos.getY() >= 0 && (unsigned)mpos.getX() < wsize.getWidth() &&
                  (unsigned)mpos.getY() < wsize.getHeight();

    if (inside)
    {
        double amount = event.delta.getX() - event.delta.getY();
        if (fOrientation == Vertical)
            amount = -amount;

        double fill = ratioForValue(getValue()) + amount / fNumSteps;
        if (fill < 0) fill = 0;
        if (fill > 1) fill = 1;

        setValue(valueForRatio(fill));

        return true;
    }

    return false;
}

void Slider::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;

    cairo_surface_t *imgBody = fImgBody;
    int wBody = cairo_image_surface_get_width(imgBody);
    int hBody = cairo_image_surface_get_height(imgBody);

    cairo_surface_t *imgHandle = fImgHandle;
    int wHandle = cairo_image_surface_get_width(imgHandle);
    int hHandle = cairo_image_surface_get_height(imgHandle);

    //
    double fill = ratioForValue(getValue());

    //
    cairo_rectangle(cr, 0, 0, wBody, hBody);
    cairo_set_source_surface(cr, imgBody, 0, 0);
    cairo_fill(cr);

    int xHandle = 0;
    int yHandle = 0;

    if (fOrientation == Horizontal)
        xHandle = fill * (wBody - wHandle);
    else
        yHandle = fill * (hBody - hHandle);

    cairo_rectangle(cr, xHandle, yHandle, wHandle, hHandle);
    cairo_set_source_surface(cr, imgHandle, xHandle, yHandle);
    cairo_fill(cr);
}

double Slider::clampToBounds(double value)
{
    double vmin = fValueBound1;
    double vmax = fValueBound2;
    if (vmin > vmax)
        std::swap(vmin, vmax);

    value = (value < vmin) ? vmin : value;
    value = (value > vmax) ? vmax : value;
    return value;
}

double Slider::valueForRatio(double ratio) const
{
    double v1 = fValueBound1;
    double v2 = fValueBound2;
    if (!fIsLogarithmic)
        return v1 + ratio * (v2 - v1);
    else
        return v1 * pow(v2 / v1, ratio);
}

double Slider::ratioForValue(double value) const
{
    double v1 = fValueBound1;
    double v2 = fValueBound2;
    if (!fIsLogarithmic)
        return (value - v1) / (v2 - v1);
    else
        return log(value / v1) / log(v2 / v1);
}
