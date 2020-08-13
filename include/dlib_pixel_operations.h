#ifndef DLIB_PIXEL_OPERATIONS_H_
#define DLIB_PIXEL_OPERATIONS_H_

#include <dlib/pixel.h>


// ----------------------------------------------------------------------------
bool operator<(const dlib::hsi_pixel& p1, const dlib::hsi_pixel& p2)
{
    if ((p1.h < p2.h) && (p1.s < p2.s) && (p1.i < p2.i))
        return true;

    return false;
}

bool operator>(const dlib::hsi_pixel& p1, const dlib::hsi_pixel& p2)
{
    if ((p1.h > p2.h) && (p1.s > p2.s) && (p1.i > p2.i))
        return true;

    return false;
}

// ----------------------------------------------------------------------------
bool operator<(const dlib::rgb_pixel& p1, const dlib::rgb_pixel& p2)
{
    if ((p1.red < p2.red) && (p1.green < p2.green) && (p1.blue < p2.blue))
        return true;

    return false;
}

bool operator>(const dlib::rgb_pixel& p1, const dlib::rgb_pixel& p2)
{
    if ((p1.red > p2.red) && (p1.green > p2.green) && (p1.blue > p2.blue))
        return true;

    return false;
}

// ----------------------------------------------------------------------------
bool operator<=(const dlib::hsi_pixel& p1, const dlib::hsi_pixel& p2)
{
    if ((p1.h <= p2.h) && (p1.s <= p2.s) && (p1.i <= p2.i))
        return true;

    return false;
}

bool operator>=(const dlib::hsi_pixel& p1, const dlib::hsi_pixel& p2)
{

    if ((p1.h >= p2.h) && (p1.s >= p2.s) && (p1.i >= p2.i))
        return true;

    return false;
}

// ----------------------------------------------------------------------------
bool operator<=(const dlib::rgb_pixel& p1, const dlib::rgb_pixel& p2)
{

    if ((p1.red <= p2.red) && (p1.green <= p2.green) && (p1.blue <= p2.blue))
        return true;

    return false;
}

bool operator>=(const dlib::rgb_pixel& p1, const dlib::rgb_pixel& p2)
{
    if ((p1.red >= p2.red) && (p1.green >= p2.green) && (p1.blue >= p2.blue))
        return true;

    return false;
}

// ----------------------------------------------------------------------------

#endif  //DLIB_PIXEL_OPERATIONS_H_
