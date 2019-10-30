/*
Wikipedia:

Hedcut is a term referring to a style of drawing, associated with The Wall Street Journal half-column portrait illustrations.
They use the stipple method of many small dots and the hatching method of small lines to create an image, and are designed to
emulate the look of woodcuts from old-style newspapers, and engravings on certificates and currency.
The phonetic spelling of "hed" may be based on newspapers' use of the term hed for "headline."
*/

#pragma once

#include "wcvt.h" //weighted centroidal voronoi tessellation

const float PI_VALUE = 3.1415927f;

struct HedcutDisk
{
	cv::Point2d center;
	float radius;
	cv::Scalar color;
};

struct SortableFloat
{
    int id;
    float value;
};

bool compareSortableFloat(SortableFloat sf1, SortableFloat sf2);


class Hedcut
{
public:

	Hedcut();

	bool build(cv::Mat & input_image, int n);

	const std::list<HedcutDisk> & getDisks() const 
	{
        return disks;
    }
	
	cv::Scalar& getBackgroundColor()
	{
        return backgroundColor;
    }
    
    cv::Scalar& getDiskColor()
    {
        return diskColor;
    }

	// CVT control flags
	int cvt_iteration_limit;     // max number of iterations when building cvf
	float max_site_displacement; // max tolerable site displacement in each iteration.
	bool average_termination;	 // true when the algorithm terminates with average displacement, not max displacement
	bool useOpenGL;              // use OpenGL to create the Voronoi diagram at each timestep.
    float defaultRadius;         // default size that we want disks to be
    float diskScalingFactor;     // factor to scale the disk size by
    bool bgColor;                // specify a specific background color for the background
    bool diskColorFlag;          // specify a disk color
    bool useAvgDiskColor;        // specify to use an average disk color
    bool useGrayscaleColor;      // specify to color the disks based on the grayscale average for their cell
        
    bool debug;                  //if true, debug information will be execute
    
    
    bool edgeDetection;
    float edgeDetectionWeight;   
    
    bool minAreaParamSet;
    float minAreaParam;
    
    bool maxAreaParamSet;
    float maxAreaParam;
    
    bool defaultAreaParamSet;
    float defaultAreaParam; 
    
    bool intensityScalingParamSet;
    float intensityScalingParam;
    
    bool areaScalingParamSet;
    float areaScalingParam;
    
    bool regularizationParamSet;
    float regularizationParam;
    
    float samplingWeight;

private:
	void sample_initial_points(cv::Mat & img, int n, std::vector<cv::Point2d> & pts);
	void create_disks(cv::Mat & img, CVT & cvt);

	std::list<HedcutDisk> disks;
    cv::Scalar backgroundColor;
    cv::Scalar diskColor;
};
