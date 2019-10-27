#include "hedcut.h"
#include <time.h>


Hedcut::Hedcut()
{
	//cvt control flags
	cvt_iteration_limit = 100; //max number of iterations when building cvf
	max_site_displacement = 1.01f; //max tolerable site displacement in each iteration.
	average_termination = false;
    useOpenGL = false;
	debug = false;
    //scaleDisks = false;
    diskScalingFactor = 0.0f;
    defaultRadius = 1.0f;
    //bool scaleDisks;            
    // scale the disks according to how big their
    
    // Set the backgroundColor
    backgroundColor.val[0] = 255;
    backgroundColor.val[1] = 255;
    backgroundColor.val[2] = 255;
    backgroundColor.val[3] = 0;
}



bool Hedcut::build(cv::Mat & input_image, int n)
{
    std::cout << "Running Hedcut::build" << std::endl;
	cv::Mat grayscale;
	cv::cvtColor(input_image, grayscale, cv::COLOR_BGR2GRAY);

	//sample n points
	std::vector<cv::Point2d> pts;
	sample_initial_points(grayscale, n, pts);

	//initialize cvt
	CVT cvt;

	cvt.iteration_limit = this->cvt_iteration_limit;
	cvt.max_site_displacement = this->max_site_displacement;
	cvt.average_termination = this->average_termination;
	cvt.debug = this->debug;
    cvt.useOpenGL = this->useOpenGL;

	clock_t startTime, endTime;
	startTime = clock();

	//compute weighted centroidal voronoi tessellation
	cvt.compute_weighted_cvt(grayscale, pts);	//*****

	endTime = clock();
	std::cout << "Total time: "<< ((double)(endTime - startTime)) / CLOCKS_PER_SEC << std::endl;

	if (debug) cv::waitKey();

	//create disks
	create_disks(input_image, cvt);

	return true;
}


void Hedcut::sample_initial_points(cv::Mat & img, int n, std::vector<cv::Point2d> & pts)
{
    std::cout << "Running Hedcut::sample_initial_points" << std::endl;
	//create n points that spread evenly that are in areas of black points...
	int count = 0;

	cv::RNG rng_uniform(time(NULL));
	cv::RNG rng_gaussian(time(NULL));
	cv::Mat visited(img.size(), CV_8U, cv::Scalar::all(0)); //all unvisited

	while (count < n)
	{
		//generate a random point
		int c = (int)floor(img.size().width*rng_uniform.uniform(0.f, 1.f));
		int r = (int)floor(img.size().height*rng_uniform.uniform(0.f, 1.f));

		//decide to keep basic on a probability (black has higher probability)
		float value = img.at<uchar>(r, c)*1.0/255; //black:0, white:1
		float gr = fabs(rng_gaussian.gaussian(0.8));
		if ( value < gr && visited.at<uchar>(r, c) ==0) //keep
		{
			count++;
			pts.push_back(cv::Point(r, c));
			visited.at<uchar>(r,c)=1;
		}
	}

	if (debug)
	{
		cv::Mat tmp = img.clone();
		for (auto& c : pts)
		{
			cv::circle(tmp, cv::Point(c.y, c.x), 2, CV_RGB(0, 0, 255), -1);
		}
		cv::imshow("samples", tmp);
		cv::waitKey();
	}
}

void Hedcut::create_disks(cv::Mat & img, CVT & cvt)
{
	cv::Mat grayscale;
	cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);

	disks.clear();
    
    double totalIntensity = 0.0;
    double totalPixels = 1.0 * grayscale.rows * grayscale.cols;
    
    double totalR = 0.0;
    double totalG = 0.0;
    double totalB = 0.0;
    
    for(int grayRow = 0; grayRow < grayscale.rows; grayRow++)
    {
        for(int grayCol = 0; grayCol < grayscale.cols; grayCol++)
        {
            cv::Vec3b &colorPixel = img.at<cv::Vec3b>(grayRow, grayCol);
            totalR += static_cast<double>(colorPixel[2]);
            totalG += static_cast<double>(colorPixel[1]);
            totalB += static_cast<double>(colorPixel[0]);
 
            totalIntensity += (1.0 * grayscale.at<uchar>(grayRow, grayCol));
        }
    }
    
    double avgPixelIntensity = totalIntensity / totalPixels;
    double avgR = totalR / totalPixels;
    double avgG = totalG / totalPixels;
    double avgB = totalB / totalPixels;
    
    backgroundColor.val[0] = static_cast<uchar>(avgR);
    backgroundColor.val[1] = static_cast<uchar>(avgG);
    backgroundColor.val[2] = static_cast<uchar>(avgB);
    
    
    float maxRadius = 0.0f;
    
    for(auto& cell : cvt.getCells())
    {
        float currentRadius = sqrt(1.0f * cell.coverage.size()) / 2.0f;
        std::cout << "Current disk radius (actual) = " << currentRadius << std::endl;
        if(currentRadius > maxRadius)
        {
            maxRadius = currentRadius;
        }
    }
    
    std::cout << "Max disk radius (actual) = " << maxRadius << std::endl;
    
	//create disks from cvt
	for (auto& cell : cvt.getCells())
	{
		//compute avg intensity
		unsigned int total = 0;
		unsigned int r = 0, g = 0, b = 0;
		for (auto & resizedPix : cell.coverage)
		{
			cv::Point pix(resizedPix.x, resizedPix.y);
			total += grayscale.at<uchar>(pix.x, pix.y);
			r += img.at<cv::Vec3b>(pix.x, pix.y)[2];
			g += img.at<cv::Vec3b>(pix.x, pix.y)[1];
			b += img.at<cv::Vec3b>(pix.x, pix.y)[0];
		}
		float avg_v = floor(total * 1.0f/ cell.coverage.size());
		r = floor(r / cell.coverage.size());
		g = floor(g / cell.coverage.size());
		b = floor(b / cell.coverage.size());

		//create a disk
		HedcutDisk disk;
		disk.center.x = cell.site.y; //x = col
		disk.center.y = cell.site.x; //y = row
		disk.color = cv::Scalar::all(0); //black
     

        //disk.radius = 1; 
        float currentRadius = sqrt(1.0f * cell.coverage.size()) / 2;
        
        disk.radius = defaultRadius + (((maxRadius - currentRadius) / maxRadius) * diskScalingFactor);
        //sqrt(cell.coverage.size() * 1.0f) / 3;
        
        std::cout << "Output radius = " << disk.radius << ", disk radius (actual) = " << currentRadius <<
            ", diskScalingFactor = " << diskScalingFactor << std::endl;

		//remember
		this->disks.push_back(disk);

	}//end for cell

	//done
}
