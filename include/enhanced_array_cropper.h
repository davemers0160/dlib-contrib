#ifndef _ENHANCED_ARRAY_CROPPER_H_
#define _ENHANCED_ARRAY_CROPPER_H_

#include <cstdint>
#include <mutex>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>

#include "dlib/threads.h"
#include "dlib/image_transforms/interpolation.h"
#include "dlib/image_processing/full_object_detection.h"
#include "dlib/rand.h"

// ----------------------------------------------------------------------------------------

struct cropper_stats
{
public:

    uint64_t img_index;
    double angle;
    std::vector<dlib::mmod_rect> rect;
    //cropper_stats() = default;
};

// ----------------------------------------------------------------------------------------


class enhanced_array_cropper
{
    dlib::chip_dims dims = dlib::chip_dims(300, 300);

    uint32_t min_object_length_long_dim = 75;   // cropped object will be at least this many pixels along its longest edge.
    uint32_t min_object_length_short_dim = 30;  // cropped object will be at least this many pixels along its shortest edge.
    double max_object_size = 0.7;               // cropped object will be at most this fraction of the size of the image.
    double background_crops_fraction = 0.5;
    double translate_amount = 0.10;

    double max_rotation_degrees = 30.0;
    double step_degrees = 10.0;

    std::string stats_filename = "cropper_stats.txt";
    std::ofstream cropper_stream;
    bool record_cropper_stats = false;

    dlib::rand rnd;
    std::mutex rnd_mutex;

public:

    void set_seed(time_t seed) 
    {
        rnd = dlib::rand(seed);
    }

    double get_translate_amount() const { return translate_amount; }

    void set_translate_amount(double value)
    {
        DLIB_CASSERT(0 <= value);
        translate_amount = value;
    }

    double get_background_crops_fraction() const { return background_crops_fraction; }

    void set_background_crops_fraction(double value)
    {
        DLIB_CASSERT(0 <= value && value <= 1);
        background_crops_fraction = value;
    }

    const dlib::chip_dims& get_chip_dims() const { return dims; }

    void set_chip_dims(const dlib::chip_dims& dims_) 
    {
        dims = dims_;
    }

    void set_chip_dims(unsigned long rows, unsigned long cols) 
    {
        set_chip_dims(dlib::chip_dims(rows, cols));
    }

    double get_max_rotation_degrees() const { return max_rotation_degrees; }
    void set_max_rotation_degrees(double value) 
    {
        max_rotation_degrees = std::abs(value);
    }

    double get_step_degrees() const { return step_degrees; }
    void set_step_degrees(double value)
    {
        step_degrees = std::abs(value);
    }

    long get_min_object_length_long_dim() const { return min_object_length_long_dim; }
    long get_min_object_length_short_dim() const { return min_object_length_short_dim; }

    void set_min_object_size(long long_dim, long short_dim)
    {
        DLIB_CASSERT(0 < short_dim && short_dim <= long_dim);
        min_object_length_long_dim = long_dim;
        min_object_length_short_dim = short_dim;
    }

    double get_max_object_size() const { return max_object_size; }
    void set_max_object_size(double value)
    {
        DLIB_CASSERT(0 < value);
        max_object_size = value;
    }

    // this does nothing for the enhanced cropper only here for backwards compat with random_array_cropper
    void set_randomly_flip(bool value) { /* do nothing */ }

    std::string get_stats_filename(void) const { return stats_filename; }
    void set_stats_filename(std::string fn)
    {
        stats_filename = fn;
        cropper_stream.open(fn, ios::out);
        record_cropper_stats = cropper_stream.is_open();
    }

    void close_cropper_stream(void)
    {
        if(record_cropper_stats)
            cropper_stream.close();
    }

// ----------------------------------------------------------------------------------------

    template <typename array_type>
    void operator() (
        size_t num_crops,
        const array_type& images,
        const std::vector<std::vector<dlib::mmod_rect>>& rects,
        array_type& crops,
        std::vector<std::vector<dlib::mmod_rect>>& crop_rects
        )
    {
        DLIB_CASSERT(images.size() == rects.size());
        crops.clear();
        crop_rects.clear();
        append(num_crops, images, rects, crops, crop_rects);
    }

// ----------------------------------------------------------------------------------------

    template<typename img_type>
    void file_append(
        size_t num_crops,
        const std::string data_directory,
        const std::vector<std::vector<std::string>> data_file,
        std::vector<img_type>& crops,
        std::vector<std::vector<dlib::mmod_rect>>& crop_rects
    )
    {

        expansion_factor = (uint32_t)(floor(2.0 * (((2.0 * max_rotation_degrees) / step_degrees) + 1)));

        auto original_size = crops.size();
        const uint64_t img_count = data_file.size();

        crops.resize(original_size + (num_crops * expansion_factor));
        crop_rects.resize(original_size + (num_crops * expansion_factor));

        std::vector<cropper_stats> cr_stats(original_size + (num_crops * expansion_factor));

        dlib::parallel_for(original_size, original_size + num_crops, [&](long idx)
        {
            uint64_t index = rnd.get_integer(img_count);
            img_type tmp_img;
            std::vector<dlib::mmod_rect> tmp_label;

            load_single_set(data_directory, data_file[index], tmp_img, tmp_label);

            for (uint64_t t = 0; t < expansion_factor; ++t)
            {
                cr_stats[idx*expansion_factor + t].img_index = index;
            }
            get_crops(tmp_img, tmp_label, &crops[idx*expansion_factor], &crop_rects[idx*expansion_factor], &cr_stats[idx*expansion_factor]);
        });

    }   // end of file_append

// ----------------------------------------------------------------------------------------

    template <typename array_type>
    void append(
        size_t num_crops,
        const array_type& images,
        const std::vector<std::vector<dlib::mmod_rect>>& rects,
        array_type& crops,
        std::vector<std::vector<dlib::mmod_rect>>& crop_rects
    )
    {
        DLIB_CASSERT(images.size() == rects.size());
        DLIB_CASSERT(crops.size() == crop_rects.size());

        // calculate the expansion factor - this is used to size the vectors for holding data
        // this is based on the number of angles to generate
        // (2.0 * max_rotation_degrees) --> this rotates the image from [-max_rotation_degrees, +max_rotation_degrees]
        // +1 --> allows us to account for the inclussive end above
        // 2.0 * --> accounts for the left-right flip of all of the images and rects
        expansion_factor = (uint32_t)(floor(2.0 * (((2.0 * max_rotation_degrees) / step_degrees) + 1)));

        auto original_size = crops.size();
        const uint64_t img_count = images.size();

        crops.resize(original_size + (num_crops * expansion_factor));
        crop_rects.resize(original_size + (num_crops * expansion_factor));

        std::vector<cropper_stats> cr_stats(original_size + (num_crops * expansion_factor));
        //std::vector<uint64_t> img_index(original_size + (num_crops * expansion_factor));
        //std::vector<dlib::chip_details> crop_plan(original_size + (num_crops * expansion_factor));

        dlib::parallel_for(original_size, original_size + num_crops, [&](long idx) 
        {
            uint64_t index = rnd.get_integer(img_count);
            for(uint64_t t=0; t< expansion_factor; ++t)
                cr_stats[idx*expansion_factor+t].img_index = index;
            //std::fill(cr_stats.begin()+(idx*expansion_factor), cr_stats.begin()+(idx*expansion_factor + expansion_factor), index);
            //en_cr[idx*expansion_factor].img_index = index;
            //(*this)(images[index], rects[index], &crops[idx*expansion_factor], &crop_rects[idx*expansion_factor], &cr_stats[idx*expansion_factor]);
            get_crops(images[index], rects[index], &crops[idx*expansion_factor], &crop_rects[idx*expansion_factor], &cr_stats[idx*expansion_factor]);
        });

        save_cropper_stats(cr_stats);
    }

// ----------------------------------------------------------------------------------------

    //template <typename array_type, typename image_type>
    //void operator() (
    //    const array_type& images,
    //    const std::vector<std::vector<dlib::mmod_rect>>& rects,
    //    image_type& crop,
    //    std::vector<dlib::mmod_rect>& crop_rects
    //    )
    //{
    //    DLIB_CASSERT(images.size() == rects.size());
    //    size_t idx;
    //    { 
    //        std::lock_guard<std::mutex> lock(rnd_mutex);
    //        idx = rnd.get_integer(images.size());
    //    }
    //    (*this)(images[idx], rects[idx], crop, crop_rects);
    //}

// ----------------------------------------------------------------------------------------

    //template <typename image_type1>
    //image_type1 operator() (
    //    const image_type1& img
    //)
    //{
    //    image_type1 crop;
    //    std::vector<dlib::mmod_rect> junk1, junk2;
    //    (*this)(img, junk1, crop, junk2);
    //    return crop;
    //}

// ----------------------------------------------------------------------------------------

    template <typename image_type1, typename image_type2>
    //void operator() (
    void get_crops(
        const image_type1& img,
        const std::vector<dlib::mmod_rect>& rects,
        image_type2 img_crop,
        std::vector<dlib::mmod_rect>* rects_crop,
        cropper_stats *cr_stats
        )
    {
        uint32_t idx, jdx;
        //uint32_t index = 0;
        uint32_t exp_fact = (expansion_factor >> 1);
        uint32_t array_depth = img.size();

        DLIB_CASSERT(array_depth > 0);
        //DLIB_CASSERT(num_rows(img[0])*num_columns(img[0]) != 0);
        dlib::chip_details cp;

        double min_angle = -max_rotation_degrees * (dlib::pi / 180.0);
        double max_angle = max_rotation_degrees * (dlib::pi / 180.0);
        double step = (step_degrees) * (dlib::pi / 180.0);

        make_crop_plan(img[0], rects, cp);

        cp.angle = -max_angle;

        //while (cp.angle <= max_angle)
        for(idx = 0; idx < exp_fact; ++idx)
        {
            // save the angle stats
            cr_stats[idx].angle = cp.angle;
            cr_stats[idx+ exp_fact].angle = cp.angle;

            for (jdx = 0; jdx < array_depth; ++jdx)
            {
                dlib::extract_image_chip(img[jdx], cp, img_crop[idx][jdx]);
                dlib::flip_image_left_right(img_crop[idx][jdx], img_crop[idx + exp_fact][jdx]);
            }

            const dlib::rectangle_transform tform = dlib::get_mapping_to_chip(cp);

            // copy rects into crop_rects and set ones that are outside the crop to ignore or
            // drop entirely as appropriate.

            rects_crop[idx].clear();
            cr_stats[idx].rect.clear();

            rects_crop[idx + exp_fact].clear();
            cr_stats[idx + exp_fact].rect.clear();

            for (auto rect : rects)
            {
                // map to crop
                rect.rect = tform(rect.rect);

                // if the rect is at least partly in the crop
                if (dlib::get_rect(img_crop[idx][0]).intersect(rect.rect).area() != 0)
                {
                    // set to ignore if not totally in the crop or if too small.
                    if (!dlib::get_rect(img_crop[idx][0]).contains(rect.rect) ||
                        ((uint32_t)rect.rect.height() < min_object_length_long_dim && (uint32_t)rect.rect.width() < min_object_length_long_dim) ||
                        ((uint32_t)rect.rect.height() < min_object_length_short_dim || (uint32_t)rect.rect.width() < min_object_length_short_dim))
                    {
                        rect.ignore = true;
                    }
                    rects_crop[idx].push_back(rect);
                    cr_stats[idx].rect.push_back(rect);
                    //(*(rects_crop + idx)).push_back(rect);
                }
            }
           
            //for (auto&& rect : (*(rects_crop + idx)))
            for (auto&& rect : rects_crop[idx])
            {
                dlib::mmod_rect tmp = dlib::mmod_rect(dlib::impl::flip_rect_left_right(rect.rect, get_rect(img_crop[idx + exp_fact][0])), rect.detection_confidence, rect.label);
                tmp.ignore = rect.ignore;
                    //(*(rects_crop + idx + exp_fact)).push_back(dlib::impl::flip_rect_left_right(rect.rect, get_rect(img_crop[idx + exp_fact][0])));
                rects_crop[idx + exp_fact].push_back(tmp);
                cr_stats[idx + exp_fact].rect.push_back(rect);
            }

            cp.angle += step;
            //++idx;
        }

        // Also randomly flip the image
        //if (should_flip_crop)
        //{
        //    image_type2 temp;
        //    for (idx = 0; idx < array_depth; ++idx)
        //    {
        //        dlib::flip_image_left_right(crop[idx], temp[idx]);
        //        swap(crop[idx], temp[idx]);
        //    }
        //    for (auto&& rect : crop_rects)
        //        rect.rect = dlib::impl::flip_rect_left_right(rect.rect, get_rect(crop[0]));
        //}

    }   // end of void operator()

// ----------------------------------------------------------------------------------------
    
private:

    uint32_t expansion_factor = 2;

    template <typename image_type1>
    void make_crop_plan(
        const image_type1& img,
        const std::vector<dlib::mmod_rect>& rects,
        dlib::chip_details& crop_plan
    )
    {
        std::lock_guard<std::mutex> lock(rnd_mutex);
        dlib::rectangle crop_rect;

        if (has_non_ignored_box(rects) && rnd.get_random_double() >= background_crops_fraction)
        {
            auto rect = rects[randomly_pick_rect(rects)].rect;

            // perturb the location of the crop by a small fraction of the object's size.
            const dlib::point rand_translate = dlib::dpoint(rnd.get_double_in_range(-translate_amount, translate_amount)*std::max(rect.height(), rect.width()),
                rnd.get_double_in_range(-translate_amount, translate_amount)*std::max(rect.height(), rect.width()));

            // We are going to grow rect into the cropping rect.  First, we grow it a
            // little so that it has the desired minimum border around it.  
            dlib::drectangle drect = centered_drect(center(rect) + rand_translate, rect.width() / max_object_size, rect.height() / max_object_size);

            // Now make rect have the same aspect ratio as dims so that there won't be
            // any funny stretching when we crop it.  We do this by growing it along
            // whichever dimension is too short.
            const double target_aspect = dims.cols / (double)dims.rows;
            if (drect.width() / drect.height() < target_aspect)
                drect = centered_drect(drect, target_aspect*drect.height(), drect.height());
            else
                drect = centered_drect(drect, drect.width(), drect.width() / target_aspect);

            // Now perturb the scale of the crop.  We do this by shrinking it, but not
            // so much that it gets smaller than the min object sizes require. 
            double current_width = dims.cols*rect.width() / drect.width();
            double current_height = dims.rows*rect.height() / drect.height();

            // never make any dimension smaller than the short dim.
            double min_scale1 = std::max(min_object_length_short_dim / current_width, min_object_length_short_dim / current_height);
     
            // at least one dimension needs to be longer than the long dim.
            double min_scale2 = std::min(min_object_length_long_dim / current_width, min_object_length_long_dim / current_height);
            double min_scale = std::max(min_scale1, min_scale2);

            min_scale = std::min(min_scale, 1.0);

            const double rand_scale_perturb = 1.0 / rnd.get_double_in_range(min_scale, 1);
            crop_rect = dlib::centered_drect(drect, drect.width()*rand_scale_perturb, drect.height()*rand_scale_perturb);

        }
        else
        {
            crop_rect = make_random_cropping_rect(img);
        }

        //const double angle = rnd.get_double_in_range(-max_rotation_degrees, max_rotation_degrees)*pi / 180;
        crop_plan = dlib::chip_details(crop_rect, dims, 0.0);
    }

// ----------------------------------------------------------------------------------------

    bool has_non_ignored_box(
        const std::vector<dlib::mmod_rect>& rects
    ) const
    {
        for (auto&& b : rects)
        {
            if (!b.ignore)
                return true;
        }
        return false;
    }

// ----------------------------------------------------------------------------------------

    size_t randomly_pick_rect(
        const std::vector<dlib::mmod_rect>& rects
    )
    {
        DLIB_CASSERT(has_non_ignored_box(rects));
        size_t idx = rnd.get_integer(rects.size());
        while (rects[idx].ignore)
        {
            idx = rnd.get_integer(rects.size());
        }

        return idx;
    }

// ----------------------------------------------------------------------------------------

    template <typename image_type>
    dlib::rectangle make_random_cropping_rect(
        const image_type& img_
    )
    {
        dlib::const_image_view<image_type> img(img_);

        // Figure out what rectangle we want to crop from the image.  We are going to
        // crop out an image of size this->dims, so we pick a random scale factor that
        // lets this random box be either as big as it can be while still fitting in
        // the image or as small as a 3x zoomed in box randomly somewhere in the image. 
        double mins = 1.0 / 3.0, maxs = std::min(img.nr() / (double)dims.rows, img.nc() / (double)dims.cols);
        mins = std::min(mins, maxs);
        auto scale = rnd.get_double_in_range(mins, maxs);
        dlib::rectangle rect(scale*dims.cols, scale*dims.rows);

        // randomly shift the box around
        dlib::point offset(rnd.get_integer(1 + img.nc() - rect.width()),
            rnd.get_integer(1 + img.nr() - rect.height()));

        return move_rect(rect, offset);
    }

// ----------------------------------------------------------------------------------------

    //void save_cropper_stats(std::vector<uint64_t> &img_index, std::vector<dlib::chip_details> &crop_plan)
    void save_cropper_stats(std::vector<cropper_stats> &cr_stats)
    {
        
        uint64_t idx, jdx;

        if (record_cropper_stats)
        {
            for (idx = 0; idx < cr_stats.size(); ++idx)
            {
                cropper_stream << cr_stats[idx].img_index << "," << cr_stats[idx].angle;
                for (jdx = 0; jdx < cr_stats[idx].rect.size(); ++jdx)
                {
                    cropper_stream << "," << cr_stats[idx].rect[jdx].label << "," << cr_stats[idx].rect[jdx].ignore;
                }
                cropper_stream << std::endl;
            }
        }

    }   // end of save_cropper_stats

// ----------------------------------------------------------------------------------------

};  // end of class

// ----------------------------------------------------------------------------------------

inline std::ostream& operator<< (
    std::ostream& out,
    const enhanced_array_cropper& item
    )
{
    using std::endl;
    out << "enhanced_array_cropper details: " << endl;
    out << "  chip_dims.rows:              " << item.get_chip_dims().rows << std::endl;
    out << "  chip_dims.cols:              " << item.get_chip_dims().cols << std::endl;
    out << "  max_rotation_degrees:        " << item.get_max_rotation_degrees() << std::endl;
    out << "  step_degrees:                " << item.get_step_degrees() << std::endl;
    out << "  min_object_length_long_dim:  " << item.get_min_object_length_long_dim() << std::endl;
    out << "  min_object_length_short_dim: " << item.get_min_object_length_short_dim() << std::endl;
    out << "  max_object_size:             " << item.get_max_object_size() << std::endl;
    out << "  background_crops_fraction:   " << item.get_background_crops_fraction() << std::endl;
    out << "  translate_amount:            " << item.get_translate_amount() << std::endl;
    out << "  stats_filename:              " << item.get_stats_filename() << std::endl;
    return out;
}

// ----------------------------------------------------------------------------------------


#endif  // _ENHANCED_ARRAY_CROPPER_H_
