/***********************************************
 * Compute Voronoi diagram using OpenGL.
 ***********************************************/
#pragma once

#include <list>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <limits>
#include <iomanip>
#include <numeric>
#include <iostream>

#undef check

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace std;

// Data
static std::vector<cv::Point2d> vorPoints;
static int vorWidth;
static int vorHeight;
static int x = 3;
static int glutWindowId;
static bool glutInitialized = false;

void renderVoronoiDiagram(void);
void populateRandomColor(float (&colorArr)[3]);
void populateDestination(cv::Mat& dest);

void colorToIndex(int listSize, uchar (&color)[3], int &index);
void indexToColor(int listSize, uchar (&color)[3], int &index);

class VorGPU
{
    public:
        VorGPU(int imageWidth, int imageHeight);
        void updateCenterPoints(const std::vector<cv::Point2d>& centerPoints);
        void refreshVoronoiDiagram(cv::Mat& mat);

    private:
        void initializeOpenGL();
        void initializeGLUT();
        
};
