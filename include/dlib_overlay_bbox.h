#ifndef OVERLAY_BBOX_H_
#define OVERLAY_BBOX_H_


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

#include <dlib/pixel.h>
#include <dlib/image_transforms.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>

#include "overlay_bounding_box.h"

// ----------------------------------------------------------------------------------------
inline cv::Rect dlib2cv_rect(dlib::rectangle r)
{
    return cv::Rect(r.left(), r.top(), r.width(), r.height());
}

// ----------------------------------------------------------------------------------------
inline dlib::rectangle cv2dlib_rect(cv::Rect r)
{
    return dlib::rectangle(r.x, r.y, r.x+r.width, r.y+r.height);
}

// ----------------------------------------------------------------------------------------
template<typename image_type>
void dlib_overlay_bbox(image_type &img, dlib::rectangle box, std::string label, dlib::rgb_pixel color, bool show_label=true)
{

    cv::Scalar c(color.blue, color.green, color.red);
    cv::Rect r(box.left(), box.top(), box.width(), box.height());
    
    cv::Mat tmp_img = dlib::toMat(img);

    overlay_bounding_box(tmp_img, r, label, c, show_label);

}

// ----------------------------------------------------------------------------------------
template<typename image_type>
void dlib_overlay_bbox(image_type& img, dlib::rectangle box, dlib::rgb_pixel color)
{

    cv::Scalar c(color.blue, color.green, color.red);
    cv::Rect r(box.left(), box.top(), box.width(), box.height());

    cv::Mat tmp_img = dlib::toMat(img);

    overlay_bounding_box(tmp_img, r, "", c, false);

}

// ----------------------------------------------------------------------------------------
template<typename image_type>
void dlib_overlay_bbox(image_type& img, dlib::mmod_rect box_label, dlib::rgb_pixel color, bool show_label = true)
{

    cv::Scalar c(color.blue, color.green, color.red);
    cv::Rect r(box_label.rect.left(), box_label.rect.top(), box_label.rect.width(), box_label.rect.height());

    cv::Mat tmp_img = dlib::toMat(img);

    overlay_bounding_box(tmp_img, r, box_label.label, c, show_label);

}

#endif  // OVERLAY_BBOX_H_
