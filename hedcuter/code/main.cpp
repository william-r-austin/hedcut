#include "hedcut.h"
#include "simple_svg_1.0.0.hpp"

// These are the new includes for OpenGL
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

//This variable is defined in wcvt.h
inline string getImageName(const string & img_name)
{
	//
	string output;
	int dot_pos = img_name.rfind(".");
	int slash_pos = img_name.rfind("/");
	if (slash_pos == string::npos)
		output = img_name.substr(0, dot_pos);
	else
		output = img_name.substr(slash_pos + 1, dot_pos - slash_pos - 1);
	//

	return output;
}

void randomColor(float (&colorArr)[3])
{
    for(size_t i = 0; i < 3; i++)
    {
        colorArr[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }
}


void myGlutDisplayFunc(void)
{
        glClearColor(0.f, 1.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float triColor[3];
        glBegin(GL_TRIANGLES);
        float scale = 400.0f;
        int totalPoints = 200;
        
        // glEnableClientState(GL_VERTEX_ARRAY);
        
    
        for(int k = 0; k < totalPoints; k++)
        {
            randomColor(triColor);
            glColor3f(triColor[0], triColor[1], triColor[2]);
            
            float xCenter = 640.0f * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
            float yCenter = 480.0f * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
            std::cout << "Calculated center: (" << xCenter << ", " << yCenter << ")" << std::endl;
            
            int fanCount = 20;
            
            for(int fanNum = 0; fanNum < fanCount; fanNum++)
            {                
                float xVertex = xCenter + scale * cos(2.0f * 3.14159265f * (fanNum / (fanCount * 1.0f)));
                float yVertex = yCenter + scale * sin(2.0f * 3.14159265f * (fanNum / (fanCount * 1.0f)));
                
                std::cout << "Calculated fan point: (" << xVertex << ", " << yVertex << ")" << std::endl;
                
                float xVertexNext = xCenter + scale * cos(2.0f * 3.14159265f * ((fanNum + 1) / (fanCount * 1.0f)));
                float yVertexNext = yCenter + scale * sin(2.0f * 3.14159265f * ((fanNum + 1) / (fanCount * 1.0f)));
                
                glVertex3f(xCenter, yCenter, 0.99f);
                glVertex3f(xVertex, yVertex, 0.01f);
                glVertex3f(xVertexNext, yVertexNext, 0.01f);
            }
/*
            
            glVertex2f(x * 20.0f + 1.0f, y * 20.0f + 1.0f);
            glVertex2f(x * 20.0f + 16.0f, y * 20.0f + 2.0f);
            glVertex2f(x * 20.0f + 10.0f, y * 20.0f + 17.0f);
*/              
                        

                
            
        }
        /*
        glVertexPointer(3, GL_FLOAT,0,  
        GLuint triangleVBO;
        glG
        glGenBuffers(1, &triangleVBO);
        */
                        glEnd();
                
                glFlush();
}

int main(int argc, char ** argv)
{
    /* Start code from William */
    //std::cout << "Starting code from William" << std::endl;
    
    srand(static_cast <unsigned> (time(0)));
    
    if(argc < 0)
    {
        // GLUT setup
        glutInit(&argc, argv);
        glutInitWindowSize(640, 480);
        glutInitWindowPosition(100, 100);
        glutCreateWindow("Test Window");
        
        // Initialization
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
 
        
        gluOrtho2D(0.0, 640.0, 0.0, 480.0);
        //glOrtho(0.0, 640.0, 0.0, 480.0, 0.0, 1.0);
       
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glDepthRange(0.0f, 1.0f);
        
        
        glutDisplayFunc(myGlutDisplayFunc);

        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
        {
            std::cout << "Error initializing OpenGL. Error is: " << gluErrorString(error) << std::endl;
            return -1;
        }  
        
        glutMainLoop();

        /*
        // Show, using OpenCV
        cv::Mat myImg(480, 640, CV_8UC3);
        cv::Mat flipped(640, 480, CV_8UC3);
        glPixelStorei(GL_PACK_ALIGNMENT, (myImg.step & 3) ? 1 : 4);
        glPixelStorei(GL_PACK_ROW_LENGTH, myImg.step / myImg.elemSize());
        
        glReadPixels(0, 0, myImg.cols, myImg.rows, GL_BGR, GL_UNSIGNED_BYTE, myImg.data);
        cv::flip(myImg, flipped, 0);
        
        cv::namedWindow("OpenGL Output", cv::WINDOW_AUTOSIZE);// Create a window for display.
		imshow("OpenGL Output", flipped);                       // Show our image inside it.
        
        cv::waitKey();
        */
        
        //std::cout << "Done with code from William" << std::endl;        
        return 0;
    }
    
	//get imput image
	if (argc < 2)
	{
		cout << " Usage: " << argv[0] << " [OPTIONS] image_file_name" << endl;
		cout << " Command Options: " << endl;
        cout << "    -debug                                        Print additional helpful information from the computations." << endl;
        cout << "    -n                   INT                      The number of disks (Voronoi cells) to create." << endl;
        cout << "    -defaultRadius       FLOAT                    Default disk radius in the output image." << endl;
        cout << "    -diskScalingFactor   FLOAT                    Scales disk based on inverse disk size. May be negative." << endl;
        cout << "    -iteration           INT                      Number of CVT (Centroidal Voronoi Diagram) iterations to perform." << endl;
        cout << "    -maxD                FLOAT                    Maximum CVT site displacement value." << endl;
        cout << "    -avg                                          Calculate the average site displacement instead of the max." << endl;
        cout << "    -useOpenGL                                    Flag to use OpenGL to compute the Voronoi diagrams." << endl;
        cout << "    -bgColor             INT  INT  INT            Set the image to the specified RGB value as the background color." << endl;
        cout << "    -diskColor           INT  INT  INT            Use the specified RGB value as the disk color." << endl;
        cout << "    -useAvgDiskColor                              Color the disks based on the average cell color." << endl;
        cout << "    -useGrayscaleColor                            Color the disks based on the average cell color, converted to grayscale." << endl;
        cout << "    -defaultDiskArea     FLOAT                    Default size of the disk. May be scaled using options below. Must be >= 0." << endl;
        cout << "    -minDiskArea         FLOAT                    If set, all disk sizes are clamped to be at least this area.  Must be >= 0." << endl;
        cout << "    -maxDiskArea         FLOAT                    If set, all disk sizes are clamped to be at most this area.  Must be >= 0." << endl;
        cout << "    -areaScaling         FLOAT                    Scales the disk size based on the area of the Voronoi cell. May be negative." << endl;
        cout << "    -intensityScaling    FLOAT                    Scales the disk size based on the average color intensity of the cell. May be negative." << endl;
        cout << "    -regularize          FLOAT                    Transforms disk size differences towards a uniform distribution. 0 = unchanged, 1 = uniform." << endl;
        /*
        
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
    regularizationParam = 0.0f;*/
        
		return -1;
	}

	Hedcut hedcut;
	bool debug = false;                //output debugging information
	int sample_size = 1000;

	string img_filename;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (string(argv[i]) == "-debug") hedcut.debug=debug = true;
            else if (string(argv[i]) == "-useOpenGL") hedcut.useOpenGL = true;
            //else if (string(argv[i]) == "-defaultRadius" && i + 1 < argc) hedcut.defaultRadius = static_cast<float>(atof(argv[++i]));
            //else if (string(argv[i]) == "-diskScalingFactor" && i + 1 < argc) hedcut.diskScalingFactor = static_cast<float>(atof(argv[++i]));
            //else if (string(argv[i]) == "-scaleDisks") hedcut.scaleDisks = true;
			else if (string(argv[i]) == "-n" && i + 1 < argc) sample_size = atoi(argv[++i]);
			else if (string(argv[i]) == "-iteration" && i + 1 < argc) hedcut.cvt_iteration_limit = atoi(argv[++i]);
			else if (string(argv[i]) == "-maxD" && i + 1 < argc) hedcut.max_site_displacement = atof(argv[++i]);
			else if (string(argv[i]) == "-avg") hedcut.average_termination = true;
            else if (string(argv[i]) == "-bgColor" && i + 3 < argc)
            {
                hedcut.bgColor = true;
                cv::Scalar &backgroundColor = hedcut.getBackgroundColor();
                backgroundColor.val[0] = static_cast<uchar>(atoi(argv[++i]));
                backgroundColor.val[1] = static_cast<uchar>(atoi(argv[++i]));
                backgroundColor.val[2] = static_cast<uchar>(atoi(argv[++i]));
            }
            else if (string(argv[i]) == "-diskColor" && i + 3 < argc)
            {
                //std::cout << "Got here - setting disk color." << std::endl;
                hedcut.diskColorFlag = true;
                cv::Scalar &diskColor = hedcut.getDiskColor();
                diskColor.val[0] = static_cast<uchar>(atoi(argv[++i]));
                diskColor.val[1] = static_cast<uchar>(atoi(argv[++i]));
                diskColor.val[2] = static_cast<uchar>(atoi(argv[++i]));
            }
            else if (string(argv[i]) == "-useAvgDiskColor") hedcut.useAvgDiskColor = true;
            else if (string(argv[i]) == "-useGrayscaleColor") hedcut.useGrayscaleColor = true;
            
            else if (string(argv[i]) == "-defaultDiskArea" && i + 1 < argc)
            {
                hedcut.defaultAreaParamSet = true;
                hedcut.defaultAreaParam = atof(argv[++i]);
            }
            else if (string(argv[i]) == "-minDiskArea" && i + 1 < argc)
            {
                hedcut.minAreaParamSet = true;
                hedcut.minAreaParam = atof(argv[++i]);
            }
            else if (string(argv[i]) == "-maxDiskArea" && i + 1 < argc)
            {
                hedcut.maxAreaParamSet = true;
                hedcut.maxAreaParam = atof(argv[++i]);
            }
            else if (string(argv[i]) == "-areaScaling" && i + 1 < argc)
            {
                hedcut.areaScalingParamSet = true;
                hedcut.areaScalingParam = atof(argv[++i]);
            }
            else if (string(argv[i]) == "-intensityScaling" && i + 1 < argc)
            {
                hedcut.intensityScalingParamSet = true;
                hedcut.intensityScalingParam = atof(argv[++i]);
            }
            else if (string(argv[i]) == "-regularize" && i + 1 < argc)
            {
                hedcut.regularizationParamSet = true;
                hedcut.regularizationParam = atof(argv[++i]);
            }
			else
            {
				cerr << "! Error: Unknown flag " << argv[i] << ".  Ignored." << endl;
            }
		}
		else img_filename = argv[i];
	}

	cv::Mat image = cv::imread(img_filename.c_str(), cv::IMREAD_COLOR);   // Read the file

	if (!image.data)                              // Check for invalid input
	{
		cout << "! Error: Could not open or find the image" << std::endl;
		return -1;
	}

	if (debug)
	{
		cv::namedWindow("Input image", cv::WINDOW_AUTOSIZE);// Create a window for display.
		imshow("Input image", image);                       // Show our image inside it.
	}

	//
	//compute hedcut
	//

	if (hedcut.build(image, sample_size) == false)
		cerr << "! Error: Failed to build hedcut. Sorry." << endl;

	if (debug)
	{
		cout << "- Generated " << hedcut.getDisks().size() << " disks" << endl;
	}

	//
	//save output to svg
	//
	stringstream ss;
	string img_name = getImageName(img_filename);
	ss << img_name << "-" << sample_size << ".svg";
	svg::Dimensions dimensions(image.size().width, image.size().height);
	svg::Document doc(ss.str(), svg::Layout(dimensions, svg::Layout::TopLeft));
    
    if(hedcut.bgColor)
    {
        const cv::Scalar& bgColorScalar = hedcut.getBackgroundColor();
        svg::Color backgroundColor(bgColorScalar.val[0], bgColorScalar.val[1], bgColorScalar.val[2]);
        svg::Rectangle background(svg::Point(0.0, 0.0), image.size().width, image.size().height, svg::Fill(backgroundColor));
        doc << background;
    }

	for (auto& disk : hedcut.getDisks())
	{
		uchar r = disk.color.val[0];
		uchar g = disk.color.val[1];
		uchar b = disk.color.val[2];
		svg::Color color(r, g, b);
		svg::Circle circle(svg::Point(disk.center.x, disk.center.y), disk.radius * 2, svg::Fill(color));
		doc << circle;
	}//end for i

	doc.save();

	if (debug)
	{
		cout << "- Saved " << ss.str() << endl;
	}

	return 0;
}
