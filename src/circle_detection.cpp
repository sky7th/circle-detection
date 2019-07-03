#include <string>
#include <vector>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "circle_taehwa.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	// Write your path of the training images.
	String path("../data/training/*.jpg");
	vector<String> fn;
	glob(path, fn, false); //폴더 안에 있는 파일들을 빠르게 뭉터기로 읽어오는 함수

	int num_images = (int)(fn.size());
	for (int e = 0; e<num_images; ++e)
	{
		// Load image(write number of image)
		//printf("%s\n", fn[k].c_str());
		Mat src = imread(fn[e]);

		// Extract the file name without its extension.
		string str(fn[e].c_str());
		size_t pos1=str.find_last_of('/');
		size_t pos2 = str.find_last_of('\\');
		size_t pos3 = str.find_last_of('.');
		size_t pos = pos1;
		if (pos2 != string::npos)
			pos = pos2;
	
		string substr = str.substr(pos + 1, pos3-pos-1);
		printf("%s\n", substr.c_str());

		if (src.empty())
		{
			printf("Please check the data path.\n");
			return 0;
		}

		vector<Vec3f> circles;

		circles = circle_taehwa(src, 30, 100, 8);	
				// circle_taehwa(영상, 최소 반지름, 최대 반지름, 투표 수 threshold)

		// Please do not change the below, which saves the circle parameters and result images.

		Vec3i c = circles[0];
		Point center = Point(c[0], c[1]);
		// Circle center
		circle(src, center, 1, Scalar(0, 255, 0), 3, LINE_AA);
		// Circle outline
		int radius = c[2];
		circle(src, center, radius, Scalar(255, 0, 255), 3, LINE_AA);


		// Write the circle parameters. 
		char fo[1024];
		sprintf_s(fo, "../result/%s.txt", substr.c_str());
		FILE* fpo = fopen(fo, "w");
		// Wrtie your center x,y & radius 
		// Write only one coordinates and radius per image.
		fprintf(fpo, "%f %f %f\n", circles[0][0], circles[0][1], circles[0][2]);

		fclose(fpo);

		// Write your result image in the result folder
		imwrite(format("../result/%s.jpg", substr.c_str()), src);
	}
}
