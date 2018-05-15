// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <stdlib.h>
#include <random>
#include "queue"
#include <algorithm>
using namespace cv;

//Declarations

float p[255];
int WH = 5;
float TH = 0.0003;
float lista[255];
float vector_m[255];
int some = 0;
int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
std::queue<Point2i> q;
std::default_random_engine gen;
std::uniform_int_distribution<int>d(0, 255);
int dx2[] = { -1, -1, 0, 1 };
int dy2[] = { 0, -1, -1, -1 };
int dirx[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
int diry[] = { 0, -1, -1, -1, 0, 1, 1, 1 };
int dx7[] = { -1, 1, 0, 0 };
int dy7[] = { 0, 0, -1, 1 };



//Declarations


void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1, dst2;
		//without interpolation
		resizeImg(src, dst1, 320, false);
		//with interpolation
		resizeImg(src, dst2, 320, true);
		imshow("input image", src);
		imshow("resized image (without interpolation)", dst1);
		imshow("resized image (with interpolation)", dst2);
		waitKey();
	}
}

void testVideoSequence()
{
	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}

	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, CV_BGR2GRAY);
		imshow("source", frame);
		imshow("gray", grayFrame);
		c = cvWaitKey(0);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n");
			break;  //ESC pressed
		};
	}
}


// Display window
const char* WIN_SRC = "Src"; //window for the source frame
const char* WIN_SRC2 = "Src2"; //window for the source frame
const char* WIN_SRC3 = "Src3"; //window for the source frame
Mat frame;
vector<Point> vec;
vector<vector<Point>> stack;
struct parking {
	vector < Point > rect;
	int state;
};
vector<parking> allParkings;

void MyCallBackFunc(int event, int x, int y, int flags, void* ptr)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Point p;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		if (vec.size() != 3){
			p.x = x;
			p.y = y;
			vec.push_back(p);
		}
		else {
			allParkings.clear();
			p.x = x;
			p.y = y;
			vec.push_back(p);
			stack.push_back(vec);
			for (std::vector <vector<Point >> ::iterator it = stack.begin(); it != stack.end(); ++it) {
				vector < Point > rect = *it;
				Point poly[1][4];
				poly[0][0] = rect[0];
				poly[0][1] = rect[1];
				poly[0][2] = rect[2];
				poly[0][3] = rect[3];
				const Point* ppt[1] = { poly[0] };
				int npt[] = { 4 };
				fillPoly(frame, ppt, npt, 1, Scalar(255, 255, 255), 8);
				imshow(WIN_SRC, frame);
				parking park;
				park.rect = rect;
				park.state = 0;
				allParkings.push_back(park);
			}

			vec.clear();
		}
	}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / PI)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i<hist_cols; i++)
	if (hist[i] > max_hist)
		max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

void L1_NegativeImage(){

	Mat img = imread("Images/saturn.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("image", img);

	Mat newImg = img;
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			newImg.at<unsigned char>(i, j) = 255 - img.at<unsigned char>(i, j);
		}
	}
	imshow("New Image", newImg);
	waitKey(0);
}

void L1_ChangeGrayLevels(){
	int value = 0;
	scanf("%d", &value);

	Mat img = imread("Images/saturn.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("image", img);

	int data = 0;
	Mat newImg = img;
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			if (value + img.at<unsigned char>(i, j) > 255){
				data = 255;
			}
			else if (value + img.at<unsigned char>(i, j) < 0){
				data = 0;
			}
			else  data = value + img.at<unsigned char>(i, j);

			newImg.at<unsigned char>(i, j) = data;
		}
	}
	imshow("New Image", newImg);
	imwrite("Images/New_Image.bmp", newImg);
	waitKey(0);
}

void L1_FourSquaresColorImage(){
	Mat img(256, 256, CV_8UC3);
	Vec3b pixel;
	for (int i = 0; i < 128; i++){
		for (int j = 0; j < 128; j++){
			pixel[0] = 255;
			pixel[1] = 255;
			pixel[2] = 255;
			img.at<Vec3b>(i, j) = pixel;
		}
	}
	for (int i = 0; i < 128; i++){
		for (int j = 128; j < 256; j++){
			pixel[0] = 0;
			pixel[1] = 0;
			pixel[2] = 255;
			img.at<Vec3b>(i, j) = pixel;
		}
	}
	for (int i = 128; i < 256; i++){
		for (int j = 0; j < 128; j++){
			pixel[0] = 0;
			pixel[1] = 255;
			pixel[2] = 0;
			img.at<Vec3b>(i, j) = pixel;
		}
	}
	for (int i = 128; i < 256; i++){
		for (int j = 128; j < 256; j++){
			pixel[0] = 0;
			pixel[1] = 255;
			pixel[2] = 255;
			img.at<Vec3b>(i, j) = pixel;
		}
	}
	imshow("New Image", img);
	waitKey(0);
}

void L1_InverseMatrix(){
	float vals[9] = { 1, 2, 3, 2, 4, 3, 2, 4, 5 };
	Mat img(3, 3, CV_32FC1, vals);

	std::cout << img.inv();
	getchar(); getchar();
	//waitKey(0);
}

void L2_RGB24(){
	Mat img = imread("Images/flowers_24bits.bmp", CV_LOAD_IMAGE_COLOR);
	imshow("image", img);

	Mat newImg1(img.rows, img.cols, CV_8UC1);
	Mat newImg2(img.rows, img.cols, CV_8UC1);
	Mat newImg3(img.rows, img.cols, CV_8UC1);
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){

			Vec3b Colors = img.at<Vec3b>(i, j);
			newImg1.at<uchar>(i, j) = Colors[2];
			newImg2.at<uchar>(i, j) = Colors[1];
			newImg3.at<uchar>(i, j) = Colors[0];

		}
	}
	imshow("New Image1", newImg1);

	imshow("New Image2", newImg2);

	imshow("New Image3", newImg3);
	waitKey(0);
}

void L2_ColorToGrayscale(){

	Mat img = imread("Images/flowers_24bits.bmp", CV_LOAD_IMAGE_COLOR);
	imshow("image", img);

	Mat newImg1(img.rows, img.cols, CV_8UC1);
	Mat newImg2(img.rows, img.cols, CV_8UC1);
	Mat newImg3(img.rows, img.cols, CV_8UC1);
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			Vec3b Colors = img.at<Vec3b>(i, j);
			newImg3.at<uchar>(i, j) = (Colors[0] + Colors[1] + Colors[2]) / 3;

		}
	}

	imshow("New Image3", newImg3);
	waitKey(0);
}

void L2_GrayscaleToBW(){

	Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("image", img);

	Mat newImg1(img.rows, img.cols, CV_8UC1);
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			if (img.at<uchar>(i, j) > 127){
				newImg1.at<uchar>(i, j) = 255;
			}
			else {
				newImg1.at<uchar>(i, j) = 0;
			}


		}
	}

	imshow("New Image3", newImg1);
	waitKey(0);
}

void L2_RBGtoHSV(){
	Mat img = imread("Images/flowers_24bits.bmp", CV_LOAD_IMAGE_COLOR);
	imshow("image", img);

	Mat colH(img.rows, img.cols, CV_8UC1);
	Mat colS(img.rows, img.cols, CV_8UC1);
	Mat colV(img.rows, img.cols, CV_8UC1);


	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			float H, S, V;
			Vec3b Colors = img.at<Vec3b>(i, j);
			float ColorsHSV[3] = { 0, 0, 0 };
			ColorsHSV[0] = (float)Colors[0] / 255;
			ColorsHSV[1] = (float)Colors[1] / 255;
			ColorsHSV[2] = (float)Colors[2] / 255;

			float Max = max(max(ColorsHSV[0], ColorsHSV[1]), ColorsHSV[2]);
			float Min = min(min(ColorsHSV[0], ColorsHSV[1]), ColorsHSV[2]);
			float C = Max - Min;

			V = Max;

			if (V){
				S = C / V;
			}
			else{
				S = 0;
			}
			if (C){
				if (Max == ColorsHSV[2]){
					H = 60 * (ColorsHSV[1] - ColorsHSV[0]) / C;
				}
				if (Max == ColorsHSV[1]){
					H = 120 + 60 * (ColorsHSV[0] - ColorsHSV[2]) / C;
				}
				if (Max == ColorsHSV[0]){
					H = 240 + 60 * (ColorsHSV[2] - ColorsHSV[1]) / C;
				}
			}
			else {
				H = 0;
			}
			if (H < 0){
				H += 360;
			}
			colH.at<uchar>(i, j) = H * 255 / 360;
			colS.at<uchar>(i, j) = S * 255;
			colV.at<uchar>(i, j) = V * 255;
		}
	}
	imshow("H", colH);
	imshow("S", colS);
	imshow("V", colV);

	waitKey(0);
}

void L3_histogram(Mat img, char* name){
	int* vector_mine = (int*)malloc(sizeof(int)* 256);
	for (int i = 0; i <= 255; i++){
		vector_mine[i] = 0;
	}
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			int level = (int)img.at<uchar>(i, j);
			vector_mine[level]++;
		}
	}
	showHistogram(name, vector_mine, 255, 500);
}

Mat L3_multi_level_thresholding(Mat img){

	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			int level = (int)img.at<uchar>(i, j);
			vector_m[level]++;
		}
	}

	int M = img.rows*img.cols;

	for (int i = 0; i <= 255; i++){
		p[i] = vector_m[i] / M;
	}

	lista[0] = 0;

	for (int k = WH; k <= 255 - WH; k++){
		float sum = 0;
		float count = 0;
		float max = 0;
		for (int i = (k - WH); i <= (k + WH); i++){
			sum += p[i];
			count++;
			if (p[i] > max) {
				max = p[i];
			}
		}

		float average = sum / count;
		//printf("%f ", max);

		if (p[k] > (average + TH) && p[k] >= max){
			some++;
			lista[some] = k;
		}

	}
	some++;
	lista[some] = 255;
	printf("\n");
	for (int i = 0; i <= some; i++){
		printf("%f ", lista[i]);
	}

	Mat newImg1(img.rows, img.cols, CV_8UC1);

	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			//////////////////////////////////////
			float dminimum = 256;
			float minimum = 256;
			float levelG = (float)img.at<uchar>(i, j);
			for (int i = 0; i <= some; i++){
				if (abs(levelG - lista[i]) < dminimum)
				{
					dminimum = abs(levelG - lista[i]);
					minimum = lista[i];
				}
			}

			newImg1.at<uchar>(i, j) = minimum;
		}
	}
	return newImg1;
}

void L3_floyd_steinberg_algorithm(){


}

void MyCallBackFunction(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == CV_EVENT_LBUTTONDBLCLK)
	{
		int count = 0;
		x, y,
			(int)(*src).at<Vec3b>(y, x)[2],
			(int)(*src).at<Vec3b>(y, x)[1],
			(int)(*src).at<Vec3b>(y, x)[0];
		for (int i = 0; i < (*src).rows; i++){
			for (int j = 0; j < (*src).cols; j++){
				if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
					count++;
				}

			}
		}
		printf("\nAREA : %d", count);
		float centerR = 0, centerC = 0;
		for (int i = 0; i < (*src).rows; i++){
			for (int j = 0; j < (*src).cols; j++){
				if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
					centerR += i;
					centerC += j;
				}

			}
		}
		centerR /= count;
		centerC /= count;
		printf("\nCENTER OF MASS : R: %.3f,C: %.3f ", centerR, centerC);

		float axUp = 0, axDown1 = 0, axDown2 = 0;
		for (int i = 0; i < (*src).rows; i++){
			for (int j = 0; j < (*src).cols; j++){
				if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
					axUp += (i - centerR)*(j - centerC);
					axDown1 += (j - centerC)*(j - centerC) - (i - centerR)*(i - centerR);
				}


			}
		}
		float fi = (0.5)*atan2(2 * axUp, axDown1);
		float angle = (fi * 180) / CV_PI;
		if (angle < 0){
			angle += 180;
		}
		printf("\nALONGATION : %.3f", angle);

		float perimeter = 0;
		for (int i = 1; i < (*src).rows - 1; i++){
			for (int j = 1; j < (*src).cols - 1; j++){
				if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
					if (((*src).at<Vec3b>(i - 1, j) != (*src).at<Vec3b>(y, x)) ||
						((*src).at<Vec3b>(i - 1, j - 1) != (*src).at<Vec3b>(y, x)) ||
						((*src).at<Vec3b>(i - 1, j + 1) != (*src).at<Vec3b>(y, x)) ||
						((*src).at<Vec3b>(i, j - 1) != (*src).at<Vec3b>(y, x)) ||
						((*src).at<Vec3b>(i + 1, j - 1) != (*src).at<Vec3b>(y, x)) ||
						((*src).at<Vec3b>(i + 1, j) != (*src).at<Vec3b>(y, x)) ||
						((*src).at<Vec3b>(i + 1, j + 1) != (*src).at<Vec3b>(y, x)) ||
						((*src).at<Vec3b>(i, j + 1) != (*src).at<Vec3b>(y, x))){
						perimeter++;
					}
				}
			}
		}


		perimeter *= CV_PI;
		perimeter /= 4;
		printf("\nPERIMETER : %.3f", perimeter);

		float thiness_ratio = 4 * CV_PI * (count / (perimeter*perimeter));
		printf("\nTHINESS RATIO : %.3f", thiness_ratio);

		float maxC = 0, maxR = 0, minC = 100000, minR = 100000;
		for (int i = 0; i < (*src).rows; i++){
			for (int j = 0; j < (*src).cols; j++){
				if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
					if (i < minR){
						minR = i;
					}
					if (i > maxR){
						maxR = i;
					}
					if (j < minC){
						minC = j;
					}
					if (j> maxC){
						maxC = j;
					}
				}
			}
		}
		float aspect_ratio = (maxC - minC + 1) / (maxR - minR + 1);
		printf("\nASPECT RATIO : %.3f", aspect_ratio);

		Point A(minC, (centerR + tan(fi)*(minC - centerC)));
		Point B(maxC, (centerR + (tan(fi)*(maxC - centerC))));
		line((*src), A, B, CV_RGB(100, 150, 159), 1);

		int counter = (*src).rows - 1;

		for (int j = 0; j < (*src).cols; j++){
			counter = (*src).rows - 1;

			for (int i = 0; i < (*src).rows; i++){
				if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
					(*src).at<Vec3b>(counter, j) = Vec3b(0, 0, 0);
					counter--;
				}
			}

		}


		circle((*src), Point(centerC, centerR), 2, CV_RGB(255, 255, 255), 1);
		imshow("new", (*src));

	}
}

void L4_geometrical_features(){
	Mat img_initial = imread("Images/geometrical_features.bmp", CV_LOAD_IMAGE_COLOR);
	namedWindow("My Window", 1);
	setMouseCallback("My Window", MyCallBackFunction, &img_initial);
	imshow("My Window", img_initial);



	waitKey(0);
}

void L5_Breath_first_traversal(){
	Mat img_initial = imread("Images/shapes.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("My Window", img_initial);
	int label = 0;
	Mat labels = Mat::zeros(img_initial.rows, img_initial.cols, CV_32SC1);
	for (int i = 0; i < img_initial.rows; i++){
		for (int j = 0; j < img_initial.cols; j++){
			if (img_initial.at<unsigned char>(i, j) == 0 && labels.at<int>(i, j) == 0){
				label++;
				labels.at<int>(i, j) = label;
				q.push({ i, j });
				while (!q.empty()){
					Point2i qq = q.front();
					q.pop();
					for (int k = 0; k < 8; k++){
						if (img_initial.at<unsigned char>(qq.x + dx[k], qq.y + dy[k]) == 0 &&
							labels.at<int>(qq.x + dx[k], qq.y + dy[k]) == 0){
							labels.at<int>(qq.x + dx[k], qq.y + dy[k]) = label;
							q.push({ qq.x + dx[k], qq.y + dy[k] });
						}
					}
				}
			}
		}
	}


	printf("THIS IS : %d", label);
	Mat coloredImg(img_initial.rows, img_initial.cols, CV_8UC3);
	int colors[100];
	for (int i = 0; i < label * 3; i++){
		colors[i] = d(gen);
	}


	for (int i = 0; i < img_initial.rows; i++){
		for (int j = 0; j < img_initial.cols; j++){
			if (labels.at<int>(i, j) != 0){
				Vec3b pixel;
				pixel[0] = colors[labels.at<int>(i, j)];
				pixel[1] = colors[labels.at<int>(i, j) * 2];
				pixel[2] = colors[labels.at<int>(i, j) * 3];
				coloredImg.at<Vec3b>(i, j) = pixel;
			}
		}
	}

	imshow("My Window", coloredImg);


	waitKey(0);
}

int minim(std::vector<int> L){
	int min = 10000000000;
	for (int i = 0; i < L.size(); i++){
		if (min>L[i])
			min = L[i];
	}
	return min;
}

void L5_Two_pass_traversal(){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> d(0, 255);
	Mat img = imread("Images/letters2.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat findImg(img.rows, img.cols, CV_8UC3);

	int label = 0;
	Mat labels = Mat::zeros(img.rows, img.cols, CV_32SC1);
	vector<vector<int>> edges;
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			if (img.at<uchar>(i, j) == 0 && labels.at<int>(i, j) == 0){
				std::vector<int> L;
				for (int k = 0; k < 4; ++k){
					if (labels.at<int>(i + dy2[k], j + dx2[k])>0){
						L.push_back(labels.at<int>(i + dy2[k], j + dx2[k]));
					}
				}
				if (L.size() == 0){
					label++;
					edges.resize(label + 1);
					labels.at<int>(i, j) = label;
				}
				else{
					int x = minim(L);
					labels.at<int>(i, j) = x;
					for (int w = 0; w < L.size(); w++){
						if (L[w] != x){

							edges[x].push_back(L[w]);
							edges[L[w]].push_back(x);
						}
					}
				}
			}
		}
	}
	printf("LABELS = %d", label);
	int newLabel = 0;
	int *newLabels = (int*)malloc((label + 1)*sizeof(int));
	for (int i = 0; i <= label; i++)
		newLabels[i] = 0;

	for (int i = 1; i <= label; i++){
		if (newLabels[i] == 0){
			newLabel++;
			newLabels[i] = newLabel;
			std::queue<int> Q;
			Q.push(i);
			while (!Q.empty()){
				int x = Q.front();
				Q.pop();
				for (int y = 0; y < edges[x].size(); y++){
					if (newLabels[edges[x][y]] == 0){
						newLabels[edges[x][y]] = newLabel;
						Q.push(edges[x][y]);
					}
				}
			}
		}
	}
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			labels.at<int>(i, j) = newLabels[labels.at<int>(i, j)];
		}
	}

	int *red = (int*)malloc(label*sizeof(int));
	int *green = (int*)malloc(label*sizeof(int));
	int *blue = (int*)malloc(label*sizeof(int));
	red[0] = 0;
	green[0] = 0;
	blue[0] = 0;

	for (int i = 1; i < label; i++){
		int r, g, b;
		r = d(gen);
		g = d(gen);
		b = d(gen);
		red[i] = r;
		green[i] = g;
		blue[i] = b;
	}
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			int l = labels.at<int>(i, j);
			Vec3b col(blue[l], green[l], red[l]);
			findImg.at<Vec3b>(i, j) = col;
		}
	}


	imshow("color", findImg);
	imshow("My Window", img);
	waitKey(0);

}

vector<int> computeDC(std::vector<int> sir){
	vector<int> aux;
	for (int i = 0; i < sir.size() - 1; i++){
		aux.push_back((sir[i + 1] - sir[i] + 8) % 8);
	}
	aux.push_back((sir[sir.size() - 1] - sir[0] + 8) % 8);
	return aux;
}

void L6_Border_Tracing(){
	Mat img = imread("Images/star.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = Mat::zeros(img.rows, img.cols, CV_8UC1);
	Mat findImg(img.rows, img.cols, CV_8UC3);

	imshow("original image!", img);

	Point p(0, 0), p1(0, 0), pn(0, 0), pn1(0);
	int dir = 7;
	int n = 0;
	std::vector<int> acumulator;


	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			if (img.at<uchar>(i, j) == 0){
				p.x = j;
				p.y = i;
				i = img.rows;
				j = img.cols;
			}
		}
	}

	if (dir % 2 == 0){
		dir = (dir + 7) % 8;
	}
	else dir = (dir + 6) % 8;
	while (img.at<uchar>(p.y + diry[dir], p.x + dirx[dir]) != 0){
		dir = (dir + 1) % 8;
	}
	acumulator.push_back(dir);
	p1.x = p.x + dirx[dir];
	p1.y = p.y + diry[dir];

	n = 2;
	pn1 = p1;
	pn = p1;

	do{

		if (dir % 2 == 0){
			dir = (dir + 7) % 8;
		}
		else dir = (dir + 6) % 8;

		while (img.at<uchar>(pn.y + diry[dir], pn.x + dirx[dir]) != 0){
			dir = (dir + 1) % 8;
		}
		acumulator.push_back(dir);
		pn1 = pn;

		pn.y = pn.y + diry[dir];
		pn.x = pn.x + dirx[dir];
		img2.at<uchar>(pn.y, pn.x) = 255;

	} while (!((pn == p1) && (pn1 == p) && (n >= 2)));

	imshow("alter image", img2);

	vector<int> dc = computeDC(acumulator);

	waitKey(0);
}

void L6_Contour_Reconstruction(){

	Mat img = imread("Images/gray_background.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	vector<int>acumulator;
	int size;
	Point point(0, 0);
	int x, y, n;
	FILE *f = fopen("reconstruct.txt", "r");

	if (f != NULL){
		fscanf(f, "%d %d", &y, &x);
		point.x = x;
		point.y = y;
		fscanf(f, "%d", &n);
		while (fscanf(f, "%d ", &x) == 1){
			acumulator.push_back(x);
		}
		Point aux(0, 0);
		aux.x = point.x;
		aux.y = point.y;
		img.at<uchar>(aux.y, aux.x) = 0;
		for (int i = 0; i < acumulator.size(); i++){
			img.at<uchar>(aux.y + diry[acumulator[i]], aux.x + dirx[acumulator[i]]) = 0;
			aux.y = aux.y + diry[acumulator[i]];
			aux.x = aux.x + dirx[acumulator[i]];
		}
	}
	else printf("ERR!");

	imshow("RESULT", img);

	waitKey();
}

Mat L7_Dilation(Mat img){

	int times = 0;

	for (int tt = 0; tt < times; tt++){

		Mat newImg(img.rows, img.cols, CV_8UC1, Scalar(0));
		for (int i = 1; i < img.rows - 1; i++){
			for (int j = 1; j < img.cols - 1; j++){
				if (img.at<uchar>(i, j) == 255){
					for (int contor = 0; contor < 4; contor++){
						newImg.at<uchar>(i + dx7[contor], j + dy7[contor]) = 255;
						newImg.at<uchar>(i, j) = 255;
					}
				}
			}
		}

		img = newImg;
	}
	return img;

}

void L7_Erosion(){
	Mat img = imread("Images/3_Open/cel4thr3_bw.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	int times = 0;
	scanf("%d", &times);

	imshow("INITIAL", img);


	for (int tt = 0; tt < times; tt++){

		Mat newImg(img.rows, img.cols, CV_8UC1, Scalar(255));
		for (int i = 1; i < img.rows - 1; i++){
			for (int j = 1; j < img.cols - 1; j++){
				if (img.at<uchar>(i, j) == 0){
					int test = 0;
					for (int contor = 0; contor < 4; contor++){
						if (!(img.at<uchar>(i + dx7[contor], j + dy7[contor]) == 0)){
							test = 1;
						}
					}
					if (test == 0){
						newImg.at<uchar>(i, j) = 0;
					}
				}
			}
		}

		img = newImg;
	}
	imshow("RESULT", img);

	waitKey();
}

void L7_Closing(){
	Mat img = imread("Images/3_Open/cel4thr3_bw.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);

	Mat newImg(img.rows, img.cols, CV_8UC1, Scalar(255));
	for (int i = 1; i < img.rows - 1; i++){
		for (int j = 1; j < img.cols - 1; j++){
			if (img.at<uchar>(i, j) == 0){
				for (int contor = 0; contor < 4; contor++){
					newImg.at<uchar>(i + dx7[contor], j + dy7[contor]) = 0;
					newImg.at<uchar>(i, j) = 0;
				}
			}
		}
	}

	img = newImg;

	Mat newImg2(img.rows, img.cols, CV_8UC1, Scalar(255));
	for (int i = 1; i < img.rows - 1; i++){
		for (int j = 1; j < img.cols - 1; j++){
			if (img.at<uchar>(i, j) == 0){
				int test = 0;
				for (int contor = 0; contor < 4; contor++){
					if (!(img.at<uchar>(i + dx7[contor], j + dy7[contor]) == 0)){
						test = 1;
					}
				}
				if (test == 0){
					newImg2.at<uchar>(i, j) = 0;
				}
			}
		}
	}

	img = newImg2;



	imshow("RESULT", img);

	waitKey();

}

void L7_Opening(){
	Mat img = imread("Images/3_Open/cel4thr3_bw.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);

	Mat newImg(img.rows, img.cols, CV_8UC1, Scalar(255));
	for (int i = 1; i < img.rows - 1; i++){
		for (int j = 1; j < img.cols - 1; j++){
			if (img.at<uchar>(i, j) == 0){
				int test = 0;
				for (int contor = 0; contor < 4; contor++){
					if (!(img.at<uchar>(i + dx7[contor], j + dy7[contor]) == 0)){
						test = 1;
					}
				}
				if (test == 0){
					newImg.at<uchar>(i, j) = 0;
				}
			}
		}
	}

	img = newImg;

	Mat newImg2(img.rows, img.cols, CV_8UC1, Scalar(255));
	for (int i = 1; i < img.rows - 1; i++){
		for (int j = 1; j < img.cols - 1; j++){
			if (img.at<uchar>(i, j) == 0){
				for (int contor = 0; contor < 4; contor++){
					newImg2.at<uchar>(i + dx7[contor], j + dy7[contor]) = 0;
					newImg2.at<uchar>(i, j) = 0;
				}
			}
		}
	}

	img = newImg;

	imshow("RESULT", img);

	waitKey();

}

void L7_Boundary_Extraction(){
	Mat img = imread("Images/3_Open/cel4thr3_bw.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);


	Mat newImg(img.rows, img.cols, CV_8UC1, Scalar(255));
	for (int i = 1; i < img.rows - 1; i++){
		for (int j = 1; j < img.cols - 1; j++){
			if (img.at<uchar>(i, j) == 0){
				int test = 0;
				for (int contor = 0; contor < 4; contor++){
					if (!(img.at<uchar>(i + dx7[contor], j + dy7[contor]) == 0)){
						test = 1;
					}
				}
				if (test == 0){
					newImg.at<uchar>(i, j) = 0;
				}
			}
		}
	}

	Mat newImg2(img.rows, img.cols, CV_8UC1, Scalar(255));
	for (int i = 1; i < img.rows - 1; i++){
		for (int j = 1; j < img.cols - 1; j++){

			if (img.at<uchar>(i, j) != newImg.at<uchar>(i, j)){
				newImg2.at<uchar>(i, j) = 0;
			}
		}
	}


	imshow("RESULT", newImg2);

	waitKey();
}

void L7_Region_Filling(){

}

float H[255];
float P[255];
void L8_Mean_Std_Dev(){
	Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("INITIAL", img);

	float L = 255;
	float M = img.cols * img.rows;


	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			int level = (int)img.at<uchar>(i, j);
			H[level]++;
		}
	}

	float mean = 0;
	for (int i = 0; i <= L; i++){
		mean += (i*H[i]);
	}
	mean /= M;

	for (int i = 0; i < 256; i++){
		P[i] = H[i] / M;
	}

	float dev1 = 0;
	for (int i = 0; i <= L; i++){
		dev1 += ((i - mean)*(i - mean) * P[i]);
	}

	float dev = sqrt(dev1);

	printf("Mean value : %f\n", mean);
	printf("Std dev value : %f\n", dev);

	waitKey();

}

void L8_Basic_Global_Thresh_Alg(){
	Mat img = imread("Images/eight.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("INITIAL", img);

	float L = 256;
	float M = img.cols * img.rows;
	float max_I = 0;
	float min_I = 256;

	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			int level = (int)img.at<uchar>(i, j);
			H[level]++;
			if (level < min_I){
				min_I = level;
			}
			if (level > max_I){
				max_I = level;
			}
		}
	}


	float T = (min_I + max_I) / 2;
	printf("T value is : %f\n", T);
	float T1 = 100000000;

	while (abs(T1 - T) > 0.1){

		float mean_1 = 0;
		float mean_2 = 0;
		float N1 = 0;
		float N2 = 0;


		for (int g = min_I; g <= T; g++){
			mean_1 += (g*H[g]);
			N1 += H[g];
		}
		for (int g = T + 1; g <= max_I; g++){
			mean_2 += (g*H[g]);
			N2 += H[g];
		}

		mean_1 /= N1;
		mean_2 /= N2;

		T1 = T;
		T = (mean_1 + mean_2) / 2;
		printf("T new value is : %f\n", T);


	}

	Mat newImg(img.rows, img.cols, CV_8UC1);
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			if (img.at<uchar>(i, j) > T){
				newImg.at<uchar>(i, j) = 255;
			}
			if (img.at<uchar>(i, j) < T){
				newImg.at<uchar>(i, j) = 0;
			}

		}
	}
	imshow("RESULT", newImg);

	waitKey();
}

void L8_Brightness_Change(){
	float offset = 0;
	scanf("%f", &offset);

	Mat img = imread("Images/eight.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("INITIAL", img);
	L3_histogram(img, "first");

	Mat newImg(img.rows, img.cols, CV_8UC1);


	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			newImg.at<uchar>(i, j) = img.at<uchar>(i, j) + offset;
			if (img.at<uchar>(i, j) + offset > 255){
				newImg.at<uchar>(i, j) = 255;
			}
			if (img.at<uchar>(i, j) + offset < 0){
				newImg.at<uchar>(i, j) = 0;
			}
		}
	}
	L3_histogram(newImg, "second");
	imshow("RESULT", newImg);

	waitKey();
}

void L8_Contrast_Change(){
	float offset_min = 0;
	float offset_max = 0;
	scanf("%f", &offset_min);
	scanf("%f", &offset_max);

	Mat img = imread("Images/eight.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	L3_histogram(img, "first");
	imshow("INITIAL", img);

	Mat newImg(img.rows, img.cols, CV_8UC1);

	float max_I = 0;
	float min_I = 256;

	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			int level = (int)img.at<uchar>(i, j);
			if (level < min_I){
				min_I = level;
			}
			if (level > max_I){
				max_I = level;
			}
		}
	}

	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			newImg.at<uchar>(i, j) = offset_min + (img.at<uchar>(i, j) - min_I)*((offset_max - offset_min) / (max_I - min_I));
		}
	}
	L3_histogram(newImg, "second");
	imshow("RESULT", newImg);

	waitKey();

}

void L8_Gamma_Correction(){
	float gama = 0;
	scanf("%f", &gama);

	Mat img = imread("Images/Lena_gray.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	L3_histogram(img, "first");
	imshow("INITIAL", img);

	Mat newImg(img.rows, img.cols, CV_8UC1);

	float L = 255;
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			newImg.at<uchar>(i, j) = L* pow(((float)img.at<uchar>(i, j) / L), gama);
		}
	}
	L3_histogram(newImg, "second");
	imshow("RESULT", newImg);

	waitKey();
}

float PC[255];
void L8_Histogram_Equalization(){

	Mat img = imread("Images/Hawkes_Bay_NZ.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	L3_histogram(img, "first");
	imshow("INITIAL", img);

	Mat newImg(img.rows, img.cols, CV_8UC1);

	float L = 255;
	float M = img.cols * img.rows;

	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			int level = (int)img.at<uchar>(i, j);
			H[level]++;
		}
	}

	for (int i = 0; i < 256; i++){
		P[i] = H[i] / M;
	}

	PC[0] = P[0];
	for (int k = 1; k < 255; k++){
		PC[k] = PC[k - 1] + P[k];
	}
	PC[255] = 1;
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			newImg.at<uchar>(i, j) = 255 * PC[img.at<uchar>(i, j)];
		}
	}
	L3_histogram(newImg, "second");
	imshow("RESULT", newImg);

	waitKey();
}

int Mean3[3][3]{{ 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
int Mean5[5][5]{{ 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 } };
void L9_Mean_filter3x3(){
	Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);
	Mat newImg(img.rows, img.cols, CV_8UC1);

	int size = 3;
	int normalize = 9;
	int draw = size / 2;
	int dcol = size / 2;

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			float value = 0;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					value += img.at<uchar>(r - draw + i, c - dcol + j) * Mean3[r][c];
				}
			}
			value /= normalize;
			newImg.at<uchar>(i, j) = value;
		}
	}

	imshow("RESULT", newImg);
	waitKey();

}

void L9_Mean_filter5x5(){
	Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);
	Mat newImg(img.rows, img.cols, CV_8UC1);

	int size = 5;
	int normalize = 25;
	int draw = size / 2;
	int dcol = size / 2;

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			float value = 0;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					value += img.at<uchar>(r - draw + i, c - dcol + j) * Mean5[r][c];
				}
			}
			value /= normalize;
			newImg.at<uchar>(i, j) = value;
		}
	}

	imshow("RESULT", newImg);
	waitKey();
}

int Gaus[3][3]{{ 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };
void L9_Gaussian_filter(){

	Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);
	Mat newImg(img.rows, img.cols, CV_8UC1);

	int size = 3;
	int normalize = 16;
	int draw = size / 2;
	int dcol = size / 2;

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			float value = 0;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					value += img.at<uchar>(r - draw + i, c - dcol + j) * Gaus[r][c];
				}
			}
			value /= normalize;
			newImg.at<uchar>(i, j) = value;
		}
	}

	imshow("RESULT", newImg);
	waitKey();

}

int Laplace[3][3]{{ -1, -1, -1 }, { -1, 8, -1 }, { -1, -1, -1 } };
void L9_laplace_filter(){
	Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);
	Mat newImg(img.rows, img.cols, CV_8UC1);

	int size = 3;
	int normalize = 16;
	int draw = size / 2;
	int dcol = size / 2;

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			float value = 0;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					value += img.at<uchar>(r - draw + i, c - dcol + j) * Laplace[r][c];
				}
			}

			if (value < 0){
				value = 0;
			}
			if (value > 250){
				value = 250;
			}
			newImg.at<uchar>(i, j) = value;
		}
	}

	imshow("RESULT", newImg);
	waitKey();

}

int high[3][3]{{ -1, -1, -1 }, { -1, 9, -1 }, { -1, -1, -1 } };

void L9_High_pass_filter(){

	Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);
	Mat newImg(img.rows, img.cols, CV_8UC1);

	int size = 3;
	int normalize = 16;
	int draw = size / 2;
	int dcol = size / 2;

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			float value = 0;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					value += img.at<uchar>(r - draw + i, c - dcol + j) * high[r][c];
				}
			}

			if (value < 0){
				value = 0;
			}
			if (value > 250){
				value = 250;
			}
			newImg.at<uchar>(i, j) = value;
		}
	}

	imshow("RESULT", newImg);
	waitKey();

}

void L9_Log_magn_fourier_sp(){

}

void L9_Ideal_low_pass_filter(){

}

void L9_Ideal_high_pass_filter(){

}

void L9_gaussian_cut_lpf(){

}

void L9_gaussian_cut_hpf(){

}


void L10_median_filter(){
	Mat img = imread("Images/balloons_Salt&Pepper.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);
	Mat newImg = Mat::zeros(img.rows, img.cols, CV_8UC1);

	int size = 0;
	scanf("%d", &size);

	int draw = size / 2;
	int dcol = size / 2;

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			vector <uchar> myVector;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					myVector.push_back(img.at<uchar>(r - draw + i, c - dcol + j));
				}
			}
			sort(myVector.begin(), myVector.end());
			newImg.at<uchar>(i, j) = myVector.at((size*size) / 2);
		}
	}

	imshow("RESULT", newImg);
	waitKey();

}

Mat L10_gaussian_filter_1x2d(Mat img){


	float w = 7;
	float sigma = w / 6;

	Mat newImg = Mat::zeros(img.rows, img.cols, CV_8UC1);
	Mat conv = Mat(w, w, CV_32FC1);

	float firstValue = 1 / (PI * 2 * (sigma *sigma));
	float sumaConv = 0;

	float x0 = w / 2; 
	float y0 = w / 2;
	for (int i = 0; i < w; i++){
		for (int j = 0; j < w; j++){
			conv.at<float>(i, j) = firstValue * exp(-1 * ((i - x0)*(i - x0) + (j - y0)*(j - y0)) / (sigma *sigma * 2));
			sumaConv += conv.at<float>(i, j);
		}
	}

	double t = (double)getTickCount();

	int size = w;
	float normalize = sumaConv;
	int draw = size / 2;
	int dcol = size / 2;

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			float value = 0;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					value += img.at<uchar>(r - draw + i, c - dcol + j) * conv.at<float>(r, c);
				}
			}
			value /= normalize;
			newImg.at<uchar>(i, j) = value;
		}
	}

	t = ((double)getTickCount() - t) / getTickFrequency();

	return newImg;
}

void L10_gaussian_filter_2x1d(){

	Mat img = imread("Images/portrait_Gauss1.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);


	float w = 0;
	scanf("%f", &w);
	float sigma = w / 6;

	Mat newImg = Mat::zeros(img.rows, img.cols, CV_8UC1);
	Mat tmp = Mat::zeros(img.rows, img.cols, CV_8UC1);

	Mat conv = Mat(w, w, CV_32FC1);

	float firstValue = 1 / (PI * 2 * (sigma *sigma));
	float sumaConv = 0;

	float x0 = w / 2; 
	float y0 = w / 2;
	for (int i = 0; i < w; i++){
		for (int j = 0; j < w; j++){
			conv.at<float>(i, j) = firstValue * exp(-1 * ((i - x0)*(i - x0) + (j - y0)*(j - y0)) / (sigma *sigma * 2));
			sumaConv += conv.at<float>(i, j);
		}
	}

	vector<float> convx;
	vector<float> convy;
	float sumaConvX = 0;
	float sumaConvY = 0;
	for (int i = 0; i < w; i++){
		convx.push_back(conv.at<float>(i, w / 2));
		sumaConvX += convx.at(i);
		convy.push_back(conv.at<float>(w / 2, i));
		sumaConvY += convy.at(i);
	}

	double t = (double)getTickCount();

	int size = w;
	float normalize = sumaConv;
	int draw = size / 2;
	int dcol = size / 2;

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			float value = 0;
			for (int r = 0; r < size; r++){
				value += img.at<uchar>(i - draw + r, j) * convx.at(r);
			}
			value /= sumaConvX;
			tmp.at<uchar>(i, j) = (int)value;
		}
	}

	for (int i = draw; i < img.rows - draw; i++){
		for (int j = dcol; j < img.cols - dcol; j++){
			float value = 0;
			for (int c = 0; c < size; c++){
				value += tmp.at<uchar>(i, j - dcol + c) * convy.at(c);
			}
			value /= sumaConvY;
			newImg.at<uchar>(i, j) = (int)value;
		}
	}

	t = ((double)getTickCount() - t) / getTickFrequency();
	imshow("RESULT", newImg);

	printf("PROCESSING TIME : %.3f", t * 1000);
	waitKey();

}

void L11_gaussian_filtering(){

}

int sobelX[3][3]{{ -1, 0, 1 },
{ -2, 0, 2 },
{ -1, 0, 1 } };
int sobelY[3][3]{{ 1, 2, 1 },
{ 0, 0, 0 },
{ -1, -2, -1 } };


void L11_gradient_magnitudeorientation(){


	Mat img = imread("Images/saturn.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	imshow("INITIAL", img);
	Mat newImg = Mat::zeros(img.rows, img.cols, CV_8UC1);
	Mat phi = Mat::zeros(img.rows, img.cols, CV_8UC1);


	int size = 3;
	int dr = size / 2;

	for (int i = dr; i < img.rows - dr; i++){
		for (int j = dr; j < img.cols - dr; j++){
			double valueX = 0;
			double valueY = 0;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					valueX += img.at<uchar>(i - dr + r, j - dr + c) * sobelX[r][c];
					valueY += img.at<uchar>(i - dr + r, j - dr + c) * sobelY[r][c];
				}
			}

			phi.at<uchar>(i, j) = atan2(valueY, valueX);
			newImg.at<uchar>(i, j) = (sqrt(valueX*valueX + valueY*valueY) / (4 * sqrt(2)));
		}
	}



	imshow("RESULT", newImg);
	waitKey();
}

Mat L11_non_maxima_supression(Mat call){

	//Mat call = imread("Images/saturn.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img;
	L10_gaussian_filter_1x2d(call).copyTo(img);

	imshow("INITIAL", img);
	Mat newImg = Mat::zeros(img.rows, img.cols, CV_32FC1);
	Mat phi = Mat::zeros(img.rows, img.cols, CV_32FC1);
	Mat GS = Mat::zeros(img.rows, img.cols, CV_8UC1);


	int size = 3;
	int dr = size / 2;

	for (int i = dr; i < img.rows - dr; i++){
		for (int j = dr; j < img.cols - dr; j++){
			double valueX = 0;
			double valueY = 0;
			for (int r = 0; r < size; r++){
				for (int c = 0; c < size; c++){
					valueX += img.at<uchar>(i - dr + r, j - dr + c) * sobelX[r][c];
					valueY += img.at<uchar>(i - dr + r, j - dr + c) * sobelY[r][c];
				}
			}

			phi.at<float>(i, j) = atan2(valueY, valueX);
			newImg.at<float>(i, j) = (sqrt(valueX*valueX + valueY*valueY) / (4 * sqrt(2)));

		}
	}

	for (int i = 1; i < newImg.rows - 1; i++){
		for (int j = 1; j < newImg.cols - 1; j++){
			float v = phi.at<float>(i, j);
			float g = newImg.at<float>(i, j);
			if ((v >= -PI / 8 && v <= PI / 8) || (v >= 7 * PI / 8 || v <= 8 * PI / 8)){
				if ((g >= newImg.at<float>(i, j - 1)) && (g >= newImg.at<float>(i, j + 1))){
					GS.at<uchar>(i, j) = g;
				}
				else {
					GS.at<uchar>(i, j) = 0;
				}
			}
			if ((v >= PI / 8 && v <= 3 * PI / 8) || (v >= -7 * PI / 8 && v <= -5 * PI / 8)){
				//diagonal right
				if ((g >= newImg.at<float>(i + 1, j - 1)) && (g >= newImg.at<float>(i - 1, j + 1))){
					GS.at<uchar>(i, j) = g;
				}
				else {
					GS.at<uchar>(i, j) = 0;
				}
			}
			if ((v >= 3 * PI / 8 && v <= 5 * PI / 8) || (v >= -5 * PI / 8 && v <= -3 * PI / 8)){
				//up down
				if ((g >= newImg.at<float>(i - 1, j)) && (g >= newImg.at<float>(i + 1, j))){
					GS.at<uchar>(i, j) = g;
				}
				else {
					GS.at<uchar>(i, j) = 0;
				}
			}
			if ((v >= 5 * PI / 8 && v <= 7 * PI / 8) || (v >= -3 * PI / 8 && v <= -PI / 8)){
				//left right
				if ((g >= newImg.at<float>(i - 1, j - 1)) && (g >= newImg.at<float>(i + 1, j + 1))){
					GS.at<uchar>(i, j) = g;
				}
				else {
					GS.at<uchar>(i, j) = 0;
				}
			}
		}
	}

	int* vector_mine = (int*)malloc(sizeof(int)* 256);
	for (int i = 0; i <= 255; i++){
		vector_mine[i] = 0;
	}
	for (int i = 1; i < GS.rows - 1; i++){
		for (int j = 1; j < GS.cols - 1; j++){
			int level = (int)GS.at<uchar>(i, j);
			vector_mine[level]++;
		}
	}

	float p = 0.1;
	float noPixels = (GS.rows - 2)*(GS.cols - 2);
	float noEdgePixels = p* (noPixels - vector_mine[0]);

	int sum = 0, i = 255;
	while (sum < noEdgePixels){
		sum += vector_mine[i];
		i--;
	}

	float Th = i;
	float Tl = 0.4*Th;

	Mat AD = Mat::zeros(img.rows, img.cols, CV_8UC1);

	for (int i = 1; i < GS.rows - 1; i++){
		for (int j = 1; j < GS.cols - 1; j++){
			int x = GS.at<uchar>(i, j);
			if (x < Tl){
				AD.at<uchar>(i, j) = 0;
			}
			else if (x > Th){
				AD.at<uchar>(i, j) = 255;
			}
			else {
				AD.at<uchar>(i, j) = 127;
			}
		}
	}

	imshow("3", AD);;


	for (int i = 1; i < AD.rows - 1; i++){
		for (int j = 1; j < AD.cols - 1; j++){
			if (AD.at<uchar>(i, j) == 255){
				///label++;
				//labels.at<int>(i, j) = label;
				q.push({ i, j });
				while (!q.empty()){
					Point2i qq = q.front();
					q.pop();
					for (int k = 0; k < 8; k++){
						if (AD.at<unsigned char>(qq.x + dx[k], qq.y + dy[k]) == 127){
							AD.at<unsigned char>(qq.x + dx[k], qq.y + dy[k]) = 255;
							q.push({ qq.x + dx[k], qq.y + dy[k] });
						}
					}
				}
			}
		}
	}


	for (int i = 1; i < AD.rows - 1; i++){
		for (int j = 1; j < AD.cols - 1; j++){
			if (AD.at<uchar>(i, j) == 127){
				AD.at<uchar>(i, j) = 0;
			}
		}
	}


	return AD;

}

void L12_adaptive_thresholding(){

}

void L12_edge_linking(){

}


int parking_lots_number = 0;
int parking_lots_free = 0;

void parkLotDetect()
{
	VideoCapture cap("Videos/park.mp4");
	//VideoCapture cap(1); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat gray, output, output2, grayLeveled, dilated;
	char numberStr[256];
	char fileName[256];

	// video resolution
	Size capS = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	namedWindow(WIN_SRC, CV_WINDOW_FREERATIO);
	cvMoveWindow(WIN_SRC, 0, 0);

	char c;
	int count = 0;
	int frameNum = -1;
	int frameCount = 0;
	Scalar red = Scalar(0, 0, 255);
	Scalar green = Scalar(0, 255, 0);


	Point p;
	cap >> frame; // get a new frame from camera

	imshow(WIN_SRC, frame);

	setMouseCallback("Src", MyCallBackFunc, &p);

	c = cvWaitKey(0);

	if (c == 32) {
		parking_lots_number = allParkings.size();

		namedWindow(WIN_SRC2, CV_WINDOW_AUTOSIZE);
		cvMoveWindow(WIN_SRC2, 0, 0);

		namedWindow(WIN_SRC3, CV_WINDOW_AUTOSIZE);
		cvMoveWindow(WIN_SRC3, 0, 0);

		for (;;)
		{

			cap >> frame; // get a new frame from camera
			if (frame.empty())
			{
				printf("End of the video file\n");
				break;
			}

			++frameNum;

			cvtColor(frame, gray, CV_BGR2GRAY);
			grayLeveled = L10_gaussian_filter_1x2d(gray);
			Canny(grayLeveled, dilated, 50, 150, 3);
			output = L7_Dilation(dilated);
			frame.copyTo(output2);
			parking_lots_free = 0;
			for (std::vector <parking> ::iterator p = allParkings.begin(); p != allParkings.end(); ++p) {
				Scalar color;

				if (p->state == 0){
					color = green;
					parking_lots_free++;
				}
				else {
					color = red;
				}

				float minY = min(min(p->rect[0].y, p->rect[1].y), min(p->rect[2].y, p->rect[3].y));
				float minX = min(min(p->rect[0].x, p->rect[1].x), min(p->rect[2].x, p->rect[3].x));
				float maxY = max(max(p->rect[0].y, p->rect[1].y), max(p->rect[2].y, p->rect[3].y));
				float maxX = max(max(p->rect[0].x, p->rect[1].x), max(p->rect[2].x, p->rect[3].x));
				rectangle(frame, Point(minX, minY), Point(maxX, maxY), red, 1, 8);



				Point poly[1][4];
				poly[0][0] = p->rect[0];
				poly[0][1] = p->rect[1];
				poly[0][2] = p->rect[2];
				poly[0][3] = p->rect[3];
				const Point* ppt[1] = { poly[0] };
				int npt[] = { 4 };


				fillPoly(output2, ppt, npt, 1, color, 8);
				double alpha = 0.3;
				cv::addWeighted(output2, alpha, frame, 1 - alpha, 0, frame);


				int totalPixels = 0;
				int whitePixels = 0;
				for (int y = minY + 1; y < maxY; y++){
					for (int x = minX + 1; x < maxX; x++){
						if (output2.at<Vec3b>(y, x) == Vec3b(0, 255, 0) || output2.at<Vec3b>(y, x) == Vec3b(0, 0, 255)){
							totalPixels++;
							if (output.at<uchar>(y, x) == 255){
								whitePixels++;
							}
						}

					}
				}

				float density = (whitePixels * 100) / totalPixels;
				std::cout << density << " dense \n";
				if (density > 10){
					p->state = 1;
				}
				else {
					p->state = 0;
				}

			}



			imshow(WIN_SRC, frame);
			imshow(WIN_SRC2, output2);
			imshow(WIN_SRC3, output);

			std::cout << parking_lots_free << "\n";

			c = cvWaitKey(10);  // waits a key press to advance to the next frame
			if (c == 27) {
				// press ESC to exit
				printf("ESC pressed - capture finished");
				break;  //ESC pressed
			}
		}
	}

}



int main()
{
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");

		printf(" 1 - Open image\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Resize image\n");
		printf(" 4 - Process video\n");
		printf(" 5 - Snap frame from live video\n");
		printf(" 6 - Mouse callback demo\n");
		printf(" 7 - Negative Imege\n");
		printf(" 8 - Change Gray Levels\n");
		printf(" 9 - Four Squares Color Image\n");
		printf(" 10 - Inverse matrix\n");
		printf(" 11 - RGB24 - Split channels\n");
		printf(" 12 - Color to grayscale\n");
		printf(" 13 - Grayscale to black&white\n");
		printf(" 14 - RGB to HSV\n");
		printf(" 15 - Histagram\n");
		printf(" 16 - Multi-level thresholding\n");
		printf(" 17 - Floyd-Steinberg algorithm\n");
		printf(" 18 - Geometrical features\n");
		printf(" 19 - BreathFirstTraversal\n");
		printf(" 20 - TwoPassTraversal\n");
		printf(" 21 - Border Tracing\n");
		printf(" 22 - Contour Reconstruction\n");
		printf(" 23 - Dilation\n");
		printf(" 24 - Erosion\n");
		printf(" 25 - Opening\n");
		printf(" 26 - Closing\n");
		printf(" 27 - Boundary extraction\n");
		printf(" 28 - Region filling\n");
		printf(" 29 - Mean & Std. Dev.\n");
		printf(" 30 - Basic global thresh. alg.\n");
		printf(" 31 - Brightness change\n");
		printf(" 32 - Contrast change\n");
		printf(" 33 - Gamma correction\n");
		printf(" 34 - Histogram equalization\n");
		printf(" 35 - Mean filter (3x3)\n");
		printf(" 36 - Mean filter (5x5)\n");
		printf(" 37 - Gaussian filter\n");
		printf(" 38 - High pass filter\n");
		printf(" 39 - Log magn Fourier sp.\n");
		printf(" 40 - Ideal low pass filter\n");
		printf(" 41 - Ideal high pass filter\n");
		printf(" 42 - Gaussian cut LPF\n");
		printf(" 43 - Gaussian cut HPF\n");
		printf(" 44 - Laplace filter (3x3) \n");
		printf(" 45 - Median filter\n");
		printf(" 46 - Gaussian filter 1x2D \n");
		printf(" 47 - Gaussian filter 2x1D \n");
		printf(" 49 - Gradient magnitude and orientation\n");
		printf(" 50 - Non-maxima suppression\n");
		printf(" 50 - Adaptive thresholding \n");
		printf(" 51 - Edge linking \n");

		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
		case 1:
			testOpenImage();
			break;
		case 2:
			testOpenImagesFld();
			break;
		case 3:
			testResize();
			break;
		case 4:
			testVideoSequence();
			break;
		case 5:
			parkLotDetect();
			break;
		case 6:
			testMouseClick();
			break;
		case 7:
			L1_NegativeImage();
			break;
		case 8:
			L1_ChangeGrayLevels();
			break;
		case 9:
			L1_FourSquaresColorImage();
			break;
		case 10:
			L1_InverseMatrix();
			break;
		case 11:
			L2_RGB24();
			break;
		case 12:
			L2_ColorToGrayscale();
			break;
		case 13:
			L2_GrayscaleToBW();
			break;
		case 14:
			L2_RBGtoHSV();
			break;
		case 15:
			//L3_histogram(Mat img);
			break;
		case 16:
			//L3_multi_level_thresholding();
			break;
		case 17:
			L3_floyd_steinberg_algorithm();
			break;
		case 18:
			L4_geometrical_features();
			break;
		case 19:
			L5_Breath_first_traversal();
			break;
		case 20:
			L5_Two_pass_traversal();
			break;
		case 21:
			L6_Border_Tracing();
			break;
		case 22:
			L6_Contour_Reconstruction();
			break;
		case 23:
			//L7_Dilation();
			break;
		case 24:
			L7_Erosion();
			break;
		case 25:
			L7_Opening();
			break;
		case 26:
			L7_Closing();
			break;
		case 27:
			L7_Boundary_Extraction();
			break;
		case 28:
			L7_Region_Filling();
			break;
		case 29:
			L8_Mean_Std_Dev();
			break;
		case 30:
			L8_Basic_Global_Thresh_Alg();
			break;
		case 31:
			L8_Brightness_Change();
			break;
		case 32:
			L8_Contrast_Change();
			break;
		case 33:
			L8_Gamma_Correction();
			break;
		case 34:
			L8_Histogram_Equalization();
			break;
		case 35:
			L9_Mean_filter3x3();
			break;
		case 36:
			L9_Mean_filter5x5();
			break;
		case 37:
			L9_Gaussian_filter();
			break;
		case 38:
			L9_High_pass_filter();
			break;
		case 39:
			L9_Log_magn_fourier_sp();
			break;
		case 40:
			L9_Ideal_low_pass_filter();
			break;
		case 41:
			L9_Ideal_high_pass_filter();
			break;
		case 42:
			L9_gaussian_cut_lpf();
			break;
		case 43:
			L9_gaussian_cut_hpf();
			break;
		case 44:
			L9_laplace_filter();
			break;
		case 45:
			L10_median_filter();
			break;
		case 46:
			//L10_gaussian_filter_1x2d();
			break;
		case 47:
			L10_gaussian_filter_2x1d();
			break;
		case 48:
			L11_gaussian_filtering();
			break;
		case 49:
			L11_gradient_magnitudeorientation();
			break;
		case 50:
			//	L11_non_maxima_supression();
			break;
		case 51:
			L12_adaptive_thresholding();
			break;
		case 52:
			L12_edge_linking();
			break;
		}
	} while (op != 0);
	return 0;
}