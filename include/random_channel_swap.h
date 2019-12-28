#ifndef _RANDOM_COLOR_CHANNEL_SWAP_H_
#define _RANDOM_COLOR_CHANNEL_SWAP_H_

#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <string>


// dlib includes
#include <dlib/matrix.h>
#include <dlib/image_transforms.h>
#include <dlib/rand.h>

extern const uint32_t array_depth;
extern const uint32_t secondary;

//-----------------------------------------------------------------------------

template<typename img_data_type>
void swap_channels(std::array<img_data_type, array_depth> &img, dlib::rand &rnd)
{
    DLIB_CASSERT(array_depth >= 3);

    uint64_t index = rnd.get_integer(6);
    //uint64_t index = rnd.get_random_8bit_number();
    //index = index % 6;

    switch (index)
    {
        case 1:
            dlib::swap(img[2], img[1]);
            break;
        case 2:
            dlib::swap(img[1], img[0]);
            break;
        case 3:
            dlib::swap(img[1], img[0]);
            dlib::swap(img[1], img[2]);
            break;
        case 4:
            dlib::swap(img[2], img[0]);
            dlib::swap(img[2], img[1]);
            break;
        case 5:
            dlib::swap(img[2], img[0]);
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------

#endif  // _RANDOM_COLOR_CHANNEL_SWAP_H_
