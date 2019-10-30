#include "hedcut.h"
#include <time.h>

bool compareSortableFloat(SortableFloat sf1, SortableFloat sf2)
{
    return sf1.value < sf2.value;
}

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
    
    bgColor = false;
    diskColorFlag = false;
    useAvgDiskColor = false;
    useGrayscaleColor = false;
    
    
    //bool scaleDisks;            
    // scale the disks according to how big their
    
    // Set the backgroundColor
    backgroundColor.val[0] = 255;
    backgroundColor.val[1] = 255;
    backgroundColor.val[2] = 255;
    backgroundColor.val[3] = 0;
    
    // Set the diskColor to black
    diskColor.val[0] = 0;
    diskColor.val[1] = 0;
    diskColor.val[2] = 0;
    diskColor.val[3] = 0;
            
    minAreaParamSet = false;
    minAreaParam = 0.0f;
    
    maxAreaParamSet = false;
    maxAreaParam = 10.0f;
    
    defaultAreaParamSet = false;
    defaultAreaParam = 5.0f; 
    
    intensityScalingParamSet = false;
    intensityScalingParam = 1.0f;
    
    areaScalingParamSet = false;
    areaScalingParam = 1.0f;
    
    regularizationParamSet = false;
    regularizationParam = 0.0f;
    
    edgeDetection = false;
    edgeDetectionWeight = 0.0;
}



bool Hedcut::build(cv::Mat & input_image, int n)
{
    //std::cout << "Running Hedcut::build" << std::endl;
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
    cvt.detectEdges = this->edgeDetection;
    cvt.edgeWeight = this->edgeDetectionWeight;

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
    //std::cout << "Running Hedcut::sample_initial_points" << std::endl;
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
    std::vector<HedcutDisk> diskVector;
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
    
    //backgroundColor.val[0] = static_cast<uchar>(avgR);
    //backgroundColor.val[1] = static_cast<uchar>(avgG);
    //backgroundColor.val[2] = static_cast<uchar>(avgB);
    
    

              
    float maxRadius = 0.0f;
    float maxArea = 0.0f;
    float minArea = 999999.0f;
    float maxLogArea = 0.0f;
    float minLogArea = 999999.0f;
    float minCellIntensity = 9999.0f;
    float maxCellIntensity = 0.0f;
    
    for(auto& cell : cvt.getCells())
    {
        float currentArea = static_cast<float>(cell.coverage.size());
               
        if(currentArea > maxArea) 
        {
            maxArea = currentArea;
        }
        
        if(currentArea < minArea)
        {
            minArea = currentArea;
        }
        
        float currentLogArea = static_cast<float>(log(log(currentArea + 1.0) + 1.0));
        
        if(currentLogArea > maxLogArea)
        {
            maxLogArea = currentLogArea;
        }
        
        if(currentLogArea < minLogArea)
        {
            minLogArea = currentLogArea;
        }
        
        float currentRadius = sqrt(1.0f * cell.coverage.size()) / 2.0f;
        //std::cout << "Current disk radius (actual) = " << currentRadius << std::endl;
        if(currentRadius > maxRadius)
        {
            maxRadius = currentRadius;
        }
        
        unsigned int totalCellIntensity = 0;
        
        for (auto & resizedPix : cell.coverage)
		{
			cv::Point pix(resizedPix.x, resizedPix.y);
			totalCellIntensity += img.at<cv::Vec3b>(pix.x, pix.y)[2];
			totalCellIntensity += img.at<cv::Vec3b>(pix.x, pix.y)[1];
			totalCellIntensity += img.at<cv::Vec3b>(pix.x, pix.y)[0];
		}
		
		float currentCellIntensity = static_cast<float>((1.0 * totalCellIntensity) / cell.coverage.size());
        
        if(currentCellIntensity > maxCellIntensity)
        {
            maxCellIntensity = currentCellIntensity;
        }
        
        if(currentCellIntensity < minCellIntensity)
        {
            minCellIntensity = currentCellIntensity;
        }
    }
    

        
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
		unsigned int r2 = floor(r / cell.coverage.size());
		unsigned int g2 = floor(g / cell.coverage.size());
		unsigned int b2 = floor(b / cell.coverage.size());
        
        unsigned int avgGray = floor((r + g + b) / (3 * cell.coverage.size()));
        float combinedIntensity = static_cast<float>((1.0 * (r + g + b)) / cell.coverage.size());

		//create a disk
		HedcutDisk disk;
		disk.center.x = cell.site.y; //x = col
		disk.center.y = cell.site.x; //y = row
		/*
		std::cout << "Disk Color Flag = " << diskColorFlag << ", Disk Color = [" <<
            (int) diskColor.val[0] << ", " << (int) diskColor.val[1] << ", " << (int) diskColor.val[2] << "]" << std::endl;
        */
        
        if(useAvgDiskColor) 
        {
            disk.color.val[0] = r2;
            disk.color.val[1] = g2;
            disk.color.val[2] = b2;
            disk.color.val[3] = 0;
        }
        else if(useGrayscaleColor)
        {

            disk.color.val[0] = avgGray;
            disk.color.val[1] = avgGray;
            disk.color.val[2] = avgGray;
            disk.color.val[3] = 0;
        }
        else if(diskColorFlag)
        {
            disk.color = cv::Scalar(diskColor);
        }
        else 
        {
            disk.color = cv::Scalar::all(0); //black
        }
        
        //disk.radius = 1; 
        float currentRadius = sqrt(1.0f * cell.coverage.size()) / 2;
        float currentArea = static_cast<float>(cell.coverage.size());
        float currentLogArea = static_cast<float>(log(log(currentArea + 1.0) + 1.0));
        
        //disk.radius = defaultRadius + (((maxRadius - currentRadius) / maxRadius) * diskScalingFactor);
        //disk.radius = defaultRadius + (((maxArea - currentArea) / maxArea) * diskScalingFactor);
        float totalAreaRange = maxArea - minArea;
        float currentAdjustedArea = currentArea - minArea;
        float areaZeroToOne = currentAdjustedArea / totalAreaRange;
        
        //std::cout << "Computing Area. Range is " << minArea << " - " << maxArea << " (" << totalAreaRange << "), current = " << currentArea << ", zt1 value = " << areaZeroToOne << std::endl;
        
        
        float totalIntensityRange = maxCellIntensity - minCellIntensity;
        float currentAdjustedIntensity = combinedIntensity - minCellIntensity;
        float intensityZeroToOne = currentAdjustedIntensity / totalIntensityRange;
        
        float calculatedDiskArea = 0.0f;
        
        if(areaScalingParamSet || intensityScalingParamSet)
        {
            if(defaultAreaParamSet)
            {
                calculatedDiskArea = defaultAreaParam;
            }
            
            if(areaScalingParamSet)
            {
                if(areaScalingParam < 0.0f)
                {
                    calculatedDiskArea += ((1.0f - areaZeroToOne) * (areaScalingParam * -1.0f));
                }
                else
                {
                    calculatedDiskArea += (areaZeroToOne * areaScalingParam);
                }
            }
            
            //std::cout << "Done Area Scaling. Param = " << areaScalingParam << ", final value = " << calculatedDiskArea << std::endl;
            
            // In this computation, higher intensity is more white, which is counterintuitive. Therefore,
            // we swap the operations here, as opposed to area.
            if(intensityScalingParamSet)
            {
                if(intensityScalingParam < 0.0f)
                {
                    calculatedDiskArea += (intensityZeroToOne * (intensityScalingParam * -1.0f));
                }
                else
                {
                    calculatedDiskArea += ((1.0f - intensityZeroToOne) * intensityScalingParam);
                }
            }
        }
        else if(defaultAreaParamSet) 
        {
            calculatedDiskArea = defaultAreaParam;
        }
        else if(minAreaParamSet && maxAreaParamSet)
        {
            calculatedDiskArea = (minAreaParam + maxAreaParam) / 2.0f;
        }
        else if(minAreaParamSet)
        {
            calculatedDiskArea = minAreaParam;
        }
        else if(maxAreaParamSet)
        {
            calculatedDiskArea = maxAreaParam;
        }
        else
        {
            calculatedDiskArea = PI_VALUE;
        }
        
        
        //disk.radius = defaultRadius + (inverseSize * diskScalingFactor);
        
        
        disk.radius = sqrt(calculatedDiskArea / PI_VALUE);
        
        //std::cout << "Radius = " << disk.radius << ", Area  was: " << calculatedDiskArea << std::endl;
        
        //sqrt(cell.coverage.size() * 1.0f) / 3;
        
        //std::cout << "Output radius = " << disk.radius << ", disk radius (actual) = " << currentRadius <<
        //    ", diskScalingFactor = " << diskScalingFactor << std::endl;

		//remember
        diskVector.push_back(disk);
		//this->disks.push_back(disk);
	} //end for cell
	
	//std::cout << "Reg Set " << regularizationParamSet << ", Area Scaling Set " << areaScalingParamSet << ", intensityScalingParamSet " << intensityScalingParamSet << std::endl;
	
    if(regularizationParamSet && (areaScalingParamSet || intensityScalingParamSet))
    {
        //std::cout << "Max disk radius (actual) = " << maxRadius << std::endl;
        //SortableFloat* sfArray;
        //sfArray = new SortableFloat[cvt.getCells().size()];
        std::vector<SortableFloat> diskRadiusValues;
        float maxDiskRadius;
        float minDiskRadius;
        bool firstRecord = true;
                
        for(int i = 0; i < diskVector.size(); i++)
        {
            float currentDiskRadius = diskVector[i].radius;
            
            if(firstRecord)
            {
                maxDiskRadius = currentDiskRadius;
                minDiskRadius = currentDiskRadius;
                firstRecord = false;
            }
            else
            {
                if(currentDiskRadius > maxDiskRadius)
                {
                    maxDiskRadius = currentDiskRadius;
                }
                
                if(currentDiskRadius < minDiskRadius)
                {
                    minDiskRadius = currentDiskRadius;
                }
            }
            
            // This is a test from Will
            SortableFloat myRecord;
            myRecord.id = i;
            myRecord.value = currentDiskRadius;
            diskRadiusValues.push_back(myRecord);
        }
        
        //std::cout << "Sorted. Min = " << minDiskRadius << ", Max = " << maxDiskRadius << std::endl;
        
        if(maxDiskRadius > minDiskRadius)
        {
            std::sort(diskRadiusValues.begin(), diskRadiusValues.end(), compareSortableFloat);
            float radiusRange = maxDiskRadius - minDiskRadius;
            
            for(int i = 0; i < diskRadiusValues.size(); i++)
            {
                SortableFloat sfi = diskRadiusValues[i];
                float oldRadius = sfi.value;
                float targetRadius = minDiskRadius + ((radiusRange / (diskRadiusValues.size() - 1)) * i);
                float delta = targetRadius - oldRadius;
                
                float newRadius = oldRadius + regularizationParam * delta;
                diskVector[sfi.id].radius = newRadius;
               // std::cout << "Regularizing!! Range = " << minDiskRadius << " - " << maxDiskRadius << ", oldRadius = " << oldRadius << ", id = " << sfi.id << 
               //     ", sorted rank = " << i << " / " << diskRadiusValues.size() - 1 << ", new radius = " << newRadius << std::endl;
            }
        }
    }
    
    if(minAreaParamSet || maxAreaParamSet)
    {
        float smallestPossibleDiskRadius = 0.0f;
        float largestPossibleDiskRadius = 1000.0f;
        
        if(minAreaParamSet) 
        {
            smallestPossibleDiskRadius = sqrt(max(minAreaParam, 0.0f) / PI_VALUE);
        }
        
        if(maxAreaParamSet)
        {
            largestPossibleDiskRadius = sqrt(max(maxAreaParam, 0.0f) / PI_VALUE);
        }
        
        // Clamp into [MIN ... MAX]
        for(auto &diskRef : diskVector)
        {
            if(minAreaParamSet) 
            {
                diskRef.radius = max(diskRef.radius, smallestPossibleDiskRadius);
            }
            
            if(maxAreaParamSet)
            {
                diskRef.radius = min(diskRef.radius, largestPossibleDiskRadius);
            }
        }
    }
    
    for(auto &diskRef : diskVector) 
    {
        this->disks.push_back(diskRef);
    }

	//done
}
