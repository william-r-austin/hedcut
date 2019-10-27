#include "vorgpu.hpp"
#include <iostream>

// This is the new includes for OpenGL
#include <GL/freeglut.h>

VorGPU::VorGPU(int imageWidth, int imageHeight)
{
    //vorPoints = centerPoints;
    vorWidth = imageWidth;
    vorHeight = imageHeight;
}

void VorGPU::updateCenterPoints(const std::vector<cv::Point2d>& centerPoints)
{
    vorPoints = centerPoints;
}

void colorToIndex(int listSize, uchar (&color)[3], int &index)
{
    int colorScaleInterval = (256 * 256 * 256) / listSize;
    int r = static_cast<int>(color[0]);
    int g = static_cast<int>(color[1]);
    int b = static_cast<int>(color[2]);
   
    int combinedColor = (r * 256 * 256) + (g * 256) + b;
    /*
    std::cout << "=== Reverse computation, colorScaleInterval " << colorScaleInterval << 
        ", r g b = " << r << " " << g <<  " " << b << ", combined = " << combinedColor << ::endl;
      */  
    index = combinedColor / colorScaleInterval;
}

void indexToColor(int listSize, uchar (&color)[3], int &index)
{
    int colorScaleInterval = (256 * 256 * 256) / listSize;
    int combinedColor = colorScaleInterval * index;
    //std::cout << "Color Scale Interval = " << colorScaleInterval << ", index = " << index << ", combined = " << combinedColor << std::endl;
        
    //populateRandomColor(triColor);
    //glColor3f(triColor[0], triColor[1], triColor[2]);
    int bInt = combinedColor % 256;
    int intermediate1 = (combinedColor - bInt) / 256;
    int gInt = intermediate1 % 256;
    int intermediate2 = (intermediate1 - gInt) / 256;
    int rInt = intermediate2 % 256;
        /*
    std::cout << "bInt = " << bInt << ", intermediate1 = " << intermediate1 << ", gInt = " << gInt << 
        ", intermediate2 = " << intermediate2 << ", rInt = " << rInt << std::endl;
    */
    /*
    uchar b = static_cast<uchar>(combinedColor % 256);
    
    int intermediate = (combinedColor - (combinedColor % 256)) / 256;
    uchar g = static_cast*/
    
    color[0] = static_cast<uchar>(rInt); //(combinedColor / (256 * 256)) % 256;
    color[1] = static_cast<uchar>(gInt); //((combinedColor - (color[0] * 256 * 256)) / 256) % 256;
    color[2] = static_cast<uchar>(bInt); //(combinedColor - (color[0] * 256 * 256) - (color[1] * 256)) % 256;
}

void renderVoronoiDiagram(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float triColor[3];
    glBegin(GL_TRIANGLES);
    float scale = static_cast<float>(std::max(vorWidth, vorHeight));
    size_t totalPoints = vorPoints.size();
    int vorPointsSize = vorPoints.size();
    //int colorScaleInterval = (256 * 256 * 256) / vorPointsSize;
    //std::cout << "vorPoints size = " << vorPointsSize << ", colorScaleInterval = " << colorScaleInterval << endl;
    
    float TWO_PI = 2.0f * 3.14159265f;
    
    for(size_t k = 0; k < totalPoints; k++)
    {
        uchar pixelColor[3];
        int currentIndex = static_cast<int>(k);
        indexToColor(vorPointsSize, pixelColor, currentIndex);
        /*
        int combinedColor = colorScaleInterval * k; 
        
        //populateRandomColor(triColor);
        //glColor3f(triColor[0], triColor[1], triColor[2]);
        uchar redVal = (combinedColor / (256 * 256)) % 256;
        uchar greenVal = ((combinedColor - (redVal * 256 * 256)) / 256) % 256;
        uchar blueVal = (combinedColor - (redVal * 256 * 256) - (greenVal * 256)) % 256;*/
        /*
        std::cout << "k = " << k << //", combined = " << combinedColor << 
            ", RGB val = [" << static_cast<int>(pixelColor[0]) << 
            ", " << static_cast<int>(pixelColor[1]) << 
            ", " << static_cast<int>(pixelColor[2]) << "]" << std::endl;
        */
        glColor3ub(pixelColor[0], pixelColor[1], pixelColor[2]);
        
        cv::Point2d currentPoint = vorPoints[k];
               
        float xCenter = static_cast<float>(currentPoint.y);
        float yCenter = static_cast<float>(vorHeight - currentPoint.x - 1);
        
        //std::cout << "Calculated center for k = " << k << ": (" << xCenter << ", " << yCenter << ")" << std::endl;
                
        int fanCount = 64;
        
        for(int fanNum = 0; fanNum < fanCount; fanNum++)
        {                
            float xVertex = xCenter + scale * cos(TWO_PI * (fanNum / (fanCount * 1.0f)));
            float yVertex = yCenter + scale * sin(TWO_PI * (fanNum / (fanCount * 1.0f)));
            
            //std::cout << "Calculated fan point: (" << xVertex << ", " << yVertex << ")" << std::endl;
            
            float xVertexNext = xCenter + scale * cos(TWO_PI * ((fanNum + 1) / (fanCount * 1.0f)));
            float yVertexNext = yCenter + scale * sin(TWO_PI * ((fanNum + 1) / (fanCount * 1.0f)));
            
            glVertex3f(xCenter, yCenter, 0.99f);
            glVertex3f(xVertex, yVertex, 0.01f);
            glVertex3f(xVertexNext, yVertexNext, 0.01f);
        }
    }

    glEnd();
    glFlush();
    glutSwapBuffers();
    
    //cv::Mat m(vorHeight, vorWidth, CV_8UC3);
    //populateDestination(m);
}

void VorGPU::refreshVoronoiDiagram(cv::Mat& mat)
{
    bool showWindow = true;
        
    if(showWindow)
    {
        if(!glutInitialized) {
            initializeGLUT();
            glutInitialized = true;
        }
        initializeOpenGL();
        glutDisplayFunc(renderVoronoiDiagram);
        renderVoronoiDiagram();
        glutMainLoopEvent();
        
       // cv::Mat m(vorHeight, vorWidth, CV_8UC3);
        populateDestination(mat);
        
       // cv::imshow("Voronoi Output", m);
       // cv::waitKey(5);

        //glutDestroyWindow(glutWindowId);
    }
    else 
    {
        initializeOpenGL();
        cv::Mat m(vorHeight, vorWidth, CV_8UC3);
        renderVoronoiDiagram();
        populateDestination(m);
    }
}

void VorGPU::initializeOpenGL()
{
    // Initialization
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluOrtho2D(0.0, 1.0 * vorWidth, 0.0, 1.0 * vorHeight);
    //glOrtho(0.0, 640.0, 0.0, 480.0, 0.0, 1.0);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_BLEND);
    glDepthRange(0.0f, 1.0f);
    
    glViewport(0, 0, vorWidth, vorHeight);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glDisable(GL_TEXTURE_2D);
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void VorGPU::initializeGLUT()
{
    // GLUT setup
    int argc = 1;
    char *argv[1] = {(char*) "./hedcuter"};
    glutInit(&argc, argv);
    glutInitWindowSize(vorWidth, vorHeight);
    glutInitWindowPosition(100, 100);
    glutWindowId = glutCreateWindow("Test Window");
}

void populateRandomColor(float (&colorArr)[3])
{
    for(size_t i = 0; i < 3; i++)
    {
        colorArr[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }
}

void populateDestination(cv::Mat& dest)
{
    //cv::Size destSize = dest.size();
    //std::cout << "height = " << destSize.height << ", width = " << destSize.height << std::endl;
    
    cv::Mat temp(dest.rows, dest.cols, CV_8UC3);

    glReadBuffer(GL_BACK);
        
    glPixelStorei(GL_PACK_ALIGNMENT, (temp.step & 3) ? 1 : 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, temp.step / temp.elemSize());
    glReadPixels(0, 0, vorWidth, vorHeight, GL_BGR, GL_UNSIGNED_BYTE, temp.data);
    
    //temp.copyTo(dest);
    
    cv::flip(temp, dest, 0);
    
    /*
    for(int row = 0; row < destSize.height; row++) {
        for(int col = 0; col < destSize.width; col++) {
    
            // X
            GLint x = col;
            GLint y = destSize.height - row - 1;
            
            GLubyte pixels[3];
            glReadPixels(x, y, 1, 1, GL_BGR, GL_UNSIGNED_BYTE, pixels);
            
            if(x % 5 == 0 && y % 5 == 0) {
                std::cout << "Data for (" << x << ", " << y << ") = [" << 
                static_cast<unsigned int>(pixels[0]) << ", " << 
                static_cast<unsigned int>(pixels[1]) << ", " << 
                static_cast<unsigned int>(pixels[2]) << "]" << std::endl;
            }
            
            cv::Vec3b &bgrPix = dest.at<cv::Vec3b>(row, col);
            uchar blueChannel = static_cast<uchar>(row % 256);
            uchar greenChannel = static_cast<uchar>(col % 256);
            uchar redChannel = 100;
            bgrPix.val[0] = pixels[0]; //blueChannel;
            bgrPix.val[1] = pixels[1]; //greenChannel;
            bgrPix.val[2] = pixels[2]; //redChannel;
        }
    }
    */
    
    
//    cv::Mat temp(destSize.height, destSize.width, CV_8UC3);

    /*
    for(int r = 0; r < vorHeight; r++)
    {
        GLubyte pixels[3 * vorWidth];
        glReadPixels(0, 0, vorWidth, 1, GL_BGR, GL_UNSIGNED_BYTE, pixels);
        
        for(int c = 0; c < vorWidth; c++)
        {
            if(r % 5 == 0 && c % 5 == 0) {
                std::cout << "Data for (row, col): (" << r << ", " << c << ") = [" << 
                static_cast<unsigned int>(pixels[c * 3 + 0]) << ", " << 
                static_cast<unsigned int>(pixels[c * 3 + 1]) << ", " << 
                static_cast<unsigned int>(pixels[c * 3 + 2]) << "]" << std::endl;
            }
            
            cv::Vec3b &bgrPix = dest.at<cv::Vec3b>(vorHeight - r - 1, c);
            bgrPix.val[0] = pixels[c * 3 + 0]; //blueChannel;
            bgrPix.val[1] = pixels[c * 3 + 1]; //greenChannel;
            bgrPix.val[2] = pixels[c * 3 + 2]; //redChannel;
        }        
    }
    //cv::flip(temp, dest, 0);*/
    
    
    // Method 2
    /*
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(0, 0, 
    */
    /*
    unsigned char* pixels = new unsigned char[3 * vorWidth * vorHeight];
    glReadPixels(0, 0, vorWidth - 1, vorHeight - 1, GL_BGR, GL_UNSIGNED_BYTE, pixels);
    
    cv::Mat m(vorHeight, vorWidth, CV_8UC3, pixels);
        
    cv::imshow("Voronoi Output", m);
    cv::waitKey(5);
    
    delete[] pixels;
    */
    /*
    unsigned char *pixels = new unsigned char[3 * vorWidth * vorHeight];
    glReadPixels(0, 0, vorWidth, vorHeight, GL_BGR, GL_UNSIGNED_BYTE, pixels);
    
    for(int r = 0; r < vorHeight; r++)
    {
        for(int c = 0; c < vorWidth; c++)
        {
            if(r % 5 == 0 && c % 5 == 0) {
                std::cout << "Data for (row, col): (" << r << ", " << c << ") = [" << 
                static_cast<unsigned int>(pixels[(vorWidth * 3 * r) + (c * 3 + 0)]) << ", " << 
                static_cast<unsigned int>(pixels[(vorWidth * 3 * r) + (c * 3 + 1)]) << ", " << 
                static_cast<unsigned int>(pixels[(vorWidth * 3 * r) + (c * 3 + 2)]) << "]" << std::endl;
            }
            
            cv::Vec3b &bgrPix = dest.at<cv::Vec3b>(vorHeight - r - 1, c);
            bgrPix.val[0] = pixels[(vorWidth * 3 * r) + (c * 3 + 0)]; //blueChannel;
            bgrPix.val[1] = pixels[(vorWidth * 3 * r) + (c * 3 + 1)]; //greenChannel;
            bgrPix.val[2] = pixels[(vorWidth * 3 * r) + (c * 3 + 2)]; //redChannel;
        }        
    }
    
    delete[] pixels;*/
}






