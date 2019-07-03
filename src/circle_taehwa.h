#include <opencv.hpp>
#include <math.h>
#include <iostream>

#ifndef CIRCLE_TAEHWA_H
#define CIRCLE_TAEHWA_H

using namespace std;
using namespace cv;

void initializes(int &a, int &b, int &c, int &d, float &e, float &f, float &g, float &h)
{
	a = 0, b = 0, c = 0, d = 0,
		e = 0, f = 0, g = 0, h = 0;
}

vector<Vec3f> circle_taehwa(Mat src, int r_min, int r_max, int vote_threshold) {

	int h = src.rows;
	int w = src.cols;

	int sum_x1, sum_y1, sum_x2, sum_y2, vx, vy, k = 0, p = 0;
	float Avr_center_x1, Avr_center_y1, Avr_center_x2, Avr_center_y2, center_x, center_y, a, b, r;

	//Mat test(src.size(), CV_8U, Scalar(0));

	Mat red(src.size(), CV_8U, Scalar(0));
	uchar* red_fast = red.data;
	Mat YCrCb;
	cvtColor(src, YCrCb, CV_BGR2YCrCb);

	vector<Mat> channel_cuts;
	split(YCrCb, channel_cuts);


	for (int i = 0; i < h; i++) {
		uchar* Y = channel_cuts[0].ptr<uchar>(i);
		uchar* Cr = channel_cuts[1].ptr<uchar>(i);
		uchar* Cb = channel_cuts[2].ptr<uchar>(i);
		for (int j = 0; j < w; j++) {
			if ((153 < Cr[j] && Cr[j] < 210) && ((110 < Cb[j] && Cb[j] < 140) || (Cr[j] > 175 && Cr[j] - Cb[j] > 60)))
				red_fast[w*i + j] = 255;
		}
	}

	//Mat element5(3, 3, CV_8U, Scalar(1));
	//morphologyEx(red, red, MORPH_OPEN, element5);
	//morphologyEx(red, red, MORPH_CLOSE, element5);
	//erode(red, red, Mat(), Point(-1, -1), 10);

	GaussianBlur(red, red, Size(3, 3), 2, 2);

	Canny(red, red, 255, 255, 3, false);
//	imshow("MASK", red);

	vector<vector<vector<int>>> vote(h, vector<vector<int>>(w, vector<int>(2, 0)));

	for (int i = 5; i < h - 5; i++) {
		for (int j = 5; j < w - 5; j++) {

			if (red_fast[w*i + j] == 255) {

				initializes(sum_x1, sum_y1, sum_x2, sum_y2,
					Avr_center_x1, Avr_center_y1, Avr_center_x2, Avr_center_y2);

				/////////////////////////////////  11 마스크  //////////////////////////////////////////

				//--------------------------------------  위, 왼쪽  --------------------------------------//
				for (int m = 0; m < 10; m++) {
					if (red_fast[w*(i - 5) + (j - 5 + m)] == 255) {
						sum_x1 += j - 5 + m;
						sum_y1 += i - 5;
						k++;
					}
					if (red_fast[w*(i - 5 + m + 1) + (j - 5)] == 255) {
						sum_x1 += j - 5;
						sum_y1 += i - 5 + m + 1;
						k++;

					}
					//--------------------------------------  아래, 오른쪽  --------------------------------------//
					if (red_fast[w*(i + 5) + (j + 5 - m)] == 255) {
						sum_x2 += j + 5 - m;
						sum_y2 += i + 5;
						p++;
					}
					if (red_fast[w*(i + 5 - m - 1) + (j + 5)] == 255) {
						sum_x2 += j + 5;
						sum_y2 += i + 5 - m - 1;
						p++;
					}
				}
				if (k != 0 && p != 0) {
					Avr_center_x1 = (float)sum_x1 / k;
					Avr_center_y1 = (float)sum_y1 / k;
					Avr_center_x2 = (float)sum_x2 / p;
					Avr_center_y2 = (float)sum_y2 / p;
					k = 0;
					p = 0;

					////////////////////////////////////   Voting   ////////////////////////////////////////

					if ((Avr_center_y1 - Avr_center_y2) <= -1 || (Avr_center_y1 - Avr_center_y2) >= 1) {
															// 일반 좌표계를 사용하므로 기울기가 무한대로 가는 것을 방지
						center_x = (Avr_center_x1 + Avr_center_x2) / 2;
						center_y = (Avr_center_y1 + Avr_center_y2) / 2;		// 11 마스크로 구한 두 점의 중심 좌표

						a = -(Avr_center_x2 - Avr_center_x1) / ((Avr_center_y2 - Avr_center_y1)); 
														// 원 중심으로 향하는 직선은 위의 두 점을 지나는 직선에 수직하는 직선
						b = -a * j + i;	// 수직하는 직선의 y 절편

						if (-1 < a && a < 1) {			// 기울기의 절대값이 1보다 작을 때는 x값의 변화에 따른 y값으로 계산

							if ((center_x - j) > 0) {
								for (vx = j; vx < w; vx++) {
									if (j + r_max < w) {			// 영상 좌표를 넘어가지 않게
										vy = (int)(a*vx + b + 0.5);
										r = sqrt((i - vy)*(i - vy) + (j - vx)*(j - vx));	// 반지름이 될 수도 있는 r
										if (0 <= vy && vy < h && r_min < r && r < r_max) {	// 반지름 최소, 최대값으로 직선 그리기 제한
											vote[vy][vx][0]++;			// voting
											vote[vy][vx][1] = r;		// r 값 저장
									//		test.at<uchar>(vy, vx) = 255;
										}
									}
								}
							}
							else if ((center_x - j) < 0) {
								for (vx = j; vx > 0; vx--) {
									if (j - r_max > 0) {
										vy = (int)(a*vx + b + 0.5);
										r = sqrt((i - vy)*(i - vy) + (j - vx)*(j - vx));
										if (0 <= vy && vy < h && r_min < r && r < r_max) {
											vote[vy][vx][0]++;
											vote[vy][vx][1] = r;
										//	test.at<uchar>(vy, vx) = 255;
										}
									}
								}
							}
						}
						else {						// 기울기의 절대값이 1보다 클 때는 x값의 변화에 따른 y값으로 계산

							if ((center_y - i) > 0) {
								for (vy = i; vy < h; vy++) {
									if (i + r_max < h) {
										vx = (int)(((vy - b) / a) + 0.5);
										r = sqrt((i - vy)*(i - vy) + (j - vx)*(j - vx));
										if (0 <= vx && vx < w && r_min < r && r < r_max) {
											vote[vy][vx][0]++;
											vote[vy][vx][1] = r;
										//	test.at<uchar>(vy, vx) = 255;
										}
									}
								}
							}
							else if ((center_y - i) < 0) {
								for (vy = i; vy > 0; vy--) {
									if (i - r_max > 0) {
										vx = (int)(((vy - b) / a) + 0.5);
										r = sqrt((i - vy)*(i - vy) + (j - vx)*(j - vx));
										if (0 <= vx && vx < w && r_min < r && r < r_max) {
											vote[vy][vx][0]++;
											vote[vy][vx][1] = r;
										//	test.at<uchar>(vy, vx) = 255;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	k = 0;
	////////////  vote threshold(9) 보다 높은 픽셀들의 평균 좌표(원의 중점) 구하기 //////////////
	int vote_avr_x = 0, vote_avr_y = 0;
	float circle_x = 0, circle_y = 0, r_final = 0;

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (vote[i][j][0] >= vote_threshold) {
				vote_avr_x += j;
				vote_avr_y += i;
				k++;
			}
		}
	}
	if (k != 0) {
		circle_x = (float)vote_avr_x / k;
		circle_y = (float)vote_avr_y / k;
	}

	int circle_X = (int)(circle_x + 0.5);	// 픽셀 위치는 정수 값이므로 반올림
	int circle_Y = (int)(circle_y + 0.5);

	r_final = vote[(int)circle_Y][(int)circle_X][1];	// 원의 중점에 저장되어 있는 반지름 길이

	vector<Vec3f> circles(1, 0);
	circles[0][0] = circle_X;
	circles[0][1] = circle_Y;
	circles[0][2] = r_final;

//	imshow("test", test);

	return circles;
}

#endif