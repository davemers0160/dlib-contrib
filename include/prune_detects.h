#ifndef _PRUNE_DUPLICATE_DETECTS_H
#define _PRUNE_DUPLICATE_DETECTS_H


// C++ includes
#include <cstdint>
#include <vector>
#include <cmath>

// DLIB includes
#include <dlib/image_transforms.h>

//----------------------------------------------------------------------------------
void prune_detects(
    std::vector<dlib::mmod_rect> &dets,
    const double overlap_threshold=0.25
)
{
    uint64_t idx, jdx;

    for (idx=0; idx<dets.size(); ++idx)
    {
        for (jdx = dets.size()-1; jdx > idx; --jdx)
        {
            if(dets[idx].label == dets[jdx].label)
            {
                double inner = dets[idx].rect.intersect(dets[jdx].rect).area();
                //if (inner == 0)
                //        break;

                double outer = dets[idx].rect.area() + dets[jdx].rect.area() - inner;

                //if (dets[idx].rect.intersect(dets[idx].rect).area() >= overlap_threshold)
                if (inner/outer >= overlap_threshold)
                {
                        dets[idx].rect.set_left(std::min(dets[idx].rect.left(), dets[jdx].rect.left()));
                        dets[idx].rect.set_top(std::min(dets[idx].rect.top(), dets[jdx].rect.top()));
                        dets[idx].rect.set_right(std::max(dets[idx].rect.right(), dets[jdx].rect.right()));
                        dets[idx].rect.set_bottom(std::max(dets[idx].rect.bottom(), dets[jdx].rect.bottom()));
                        dets.erase(dets.begin()+jdx);
                }
            }
        }
    }

}       // end of prune_detects

#endif  // _PRUNE_DUPLICATE_DETECTS_H
