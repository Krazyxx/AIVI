#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "Classifier.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>

//#include "ReaderEyeTrackerResults.hpp"

void
usage(const char *cmd)
{
	std::cerr << "Usage: " << cmd << " deploy.prototxt network.caffemodel mean.binaryproto labels.txt input_video_rgb input_video_tmp stride output_folder" << std::endl;
	exit(EXIT_FAILURE);
}

cv::Mat getGaussianKernelWithCenter(int length, int center, int sigma);
//int getNbLinesFile(char * File);


int
main(int argc, char **argv)
{

	if(argc != 9)
		usage(argv[0]);

	// Init
	string model_file   = argv[1];
        string trained_file = argv[2];
	string mean_file    = argv[3];
	string label_file   = argv[4];
	Classifier classifier(model_file, trained_file, mean_file, label_file);
	cv::VideoCapture video_rgb(argv[5]);
	if(!video_rgb.isOpened()) {
		std::cerr << "Erreur de lecture du fichier video: " << argv[5] << std::endl;
		return EXIT_FAILURE;
	}

	cv::VideoCapture video_tmp(argv[6]);
	if(!video_tmp.isOpened()) {
		std::cerr << "Erreur de lecture du fichier video: " << argv[6] << std::endl;
		return EXIT_FAILURE;
	}
	const int CONST_STRIDE = atoi(argv[7]);
	const int CONST_PATCH = 100;

	int nbFrames = video_rgb.get(CV_CAP_PROP_FRAME_COUNT);
	int WIDTH=video_rgb.get(CV_CAP_PROP_FRAME_WIDTH);
	int HEIGHT=video_rgb.get(CV_CAP_PROP_FRAME_HEIGHT);
 	int  width_limit = (int)(WIDTH/CONST_PATCH) * (int)(CONST_PATCH / CONST_STRIDE );
	int  height_limit = (int)(HEIGHT/CONST_PATCH) * (int)(CONST_PATCH / CONST_STRIDE );
	float  array_res[width_limit][height_limit] = {};
	int num_j, num_i;
	cv::Mat mix;
	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;

	std::string extension = ".png";
	std::string title;

	int currentProcessedFrames=0;
	std::cout << currentProcessedFrames << "/" <<  nbFrames << std::endl;
	int stepProgressDisplay=100;

   	while (true) {
	cv::Mat tab_saliency = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC1);
        cv::Mat frame_rgb;
        video_rgb >> frame_rgb; // get a new frame from camera
        cv::Mat frame_tmp;
        video_tmp >> frame_tmp; // get a new frame from camera
	cv::Mat kernelXY = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC1);
	cv::Mat kernelX;
	cv::Mat kernelY;
	int sigma=75;
	cv::Size s_rgb = frame_rgb.size();
	int rows_rgb = s_rgb.height;
	int cols_rgb = s_rgb.width;
	cv::Size s_tmp = frame_tmp.size();
	int rows_tmp = s_tmp.height;
	int cols_tmp = s_tmp.width;
	if((rows_rgb == rows_tmp)  && (cols_rgb==cols_tmp)){ //the frames are ready and already captured
         num_j= 0;
            while (num_j*CONST_STRIDE+CONST_PATCH < HEIGHT) {
	      num_i= 0;
	       while (num_i*CONST_STRIDE+CONST_PATCH < WIDTH ){

		//TO DO
		// select a region from rgb frame and its corresponding in tmp frame


		//create a region that concatenate rgb and tmp in order to obtain a 4 channels patch

		//do the prediction of the  4 channels patch in a vector called predictions


		/////////////////////////////////////////////////////
		std::vector<Prediction> predictions;
 		  if (predictions[0].second >predictions[1].second)
		    array_res[num_i][num_j]= predictions[0].second * 0;
		  else
		    array_res[num_i][num_j]= predictions[1].second * 1;


		  if (array_res[num_i][num_j] > 0){

			   for (int prob_i =0; prob_i< int(10 * array_res[num_i][num_j]); prob_i++){
				kernelX = getGaussianKernelWithCenter(int(HEIGHT), num_j*CONST_STRIDE+int(CONST_PATCH/2), sigma);

			        kernelY = getGaussianKernelWithCenter(int(WIDTH), num_i*CONST_STRIDE+int(CONST_PATCH/2), sigma);

				kernelXY += kernelX * kernelY.t();
			   }
                  }

		num_i = num_i +1;
		}
            num_j = num_j +1;
            }

         minMaxLoc( kernelXY, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
	 kernelXY=kernelXY *(1/maxVal) *255;
	 kernelXY.convertTo(kernelXY,CV_8U);
		//cv::applyColorMap(kernelXY, kernelXY, cv::COLORMAP_JET);
	 mix = kernelXY;

      		//save saliency file
		std::ostringstream count_str;
		count_str << "/" << std::setw(8) << std::setfill('0') << currentProcessedFrames;
		title=argv[8]+count_str.str()+extension;

		cv::imwrite(title, mix);


		// Show Progress
		currentProcessedFrames++;
		if (currentProcessedFrames % stepProgressDisplay == 0)
		{
			std::cout << currentProcessedFrames << "/" <<  nbFrames << std::endl;
		}



		//cv::imshow("mix", mix);
		//cv::imshow("trame", trame);
		//cv::imshow("gauss", kernelXY);
		//cv::waitKey(0);

	}
  }

	std::cout << "Finished creating files in " << argv[8] << std::endl;
	return EXIT_SUCCESS;
}


cv::Mat getGaussianKernelWithCenter(int length, int center, int sigma)
{
	cv::Mat auxKernel =  cv::getGaussianKernel(length*3, sigma, CV_32F);
	cv::Mat gaussianKernel = auxKernel.rowRange(length + (length/2-center), 2*length + (length/2-center));
	return gaussianKernel;
}
