#include "stdafx.h"
#include "common.h"
#include "math.h"
#include <queue>;
#include <random>;
#include <fstream>;
#include <stdio.h>;
using namespace cv;

namespace ProcessingStore {


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


	void testSnap()
	{
		VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
		if (!cap.isOpened()) // openenig the video device failed
		{
			printf("Cannot open video capture device.\n");
			return;
		}

		Mat frame;
		char numberStr[256];
		char fileName[256];

		// video resolution
		Size capS = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
			(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));

		// Display window
		const char* WIN_SRC = "Src"; //window for the source frame
		namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
		cvMoveWindow(WIN_SRC, 0, 0);

		const char* WIN_DST = "Snapped"; //window for showing the snapped frame
		namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
		cvMoveWindow(WIN_DST, capS.width + 10, 0);

		char c;
		int frameNum = -1;
		int frameCount = 0;

		for (;;)
		{
			cap >> frame; // get a new frame from camera
			if (frame.empty())
			{
				printf("End of the video file\n");
				break;
			}

			++frameNum;

			imshow(WIN_SRC, frame);

			c = cvWaitKey(10);  // waits a key press to advance to the next frame
			if (c == 27) {
				// press ESC to exit
				printf("ESC pressed - capture finished");
				break;  //ESC pressed
			}
			if (c == 115){ //'s' pressed - snapp the image to a file
				frameCount++;
				fileName[0] = NULL;
				sprintf(numberStr, "%d", frameCount);
				strcat(fileName, "Images/A");
				strcat(fileName, numberStr);
				strcat(fileName, ".bmp");
				bool bSuccess = imwrite(fileName, frame);
				if (!bSuccess)
				{
					printf("Error writing the snapped image\n");
				}
				else
					imshow(WIN_DST, frame);
			}
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

	void negativeImage(){
		Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		imshow("original image", img);
		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
			}
		}

		imshow("second image", img);
		waitKey(0);
	}

	void changeGrayLevels(){
		printf("Give the value to be added\n");
		int x = 0;
		scanf("%d", &x);
		printf("Do you want to multiply/add?(0< multiply)");
		int mult = 1;
		scanf("%d", &mult);
		Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		imshow("original image", img);
		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				int aux = 0;
				if (mult>0)
					aux = img.at<uchar>(i, j) + x;
				else aux = img.at<uchar>(i, j)* x;

				if (aux > 255){
					img.at<uchar>(i, j) = 255;
				}
				else if (aux < 0)
				{
					img.at<uchar>(i, j) = 0;
				}
				else img.at<uchar>(i, j) = aux;
			}
		}

		imshow("second image", img);
		imwrite("nume.bmp", img);
		waitKey(0);
	}
	void fourSquares(){

		Mat myImg(256, 256, CV_8UC3);
		int i, j;
		Vec3b aux;
		for (i = 0; i < myImg.rows / 2; i++){
			for (j = 0; j < myImg.rows / 2; j++){
				aux[0] = 255;
				aux[1] = 255;
				aux[2] = 255;
				myImg.at<Vec3b>(i, j) = aux;
			}
		}

		for (i = 0; i < myImg.rows / 2; i++){
			for (j = myImg.rows / 2; j < myImg.cols; j++){
				aux[0] = 0;
				aux[1] = 0;
				aux[2] = 255;
				myImg.at<Vec3b>(i, j) = aux;
			}
		}

		for (i = myImg.rows / 2; i < myImg.rows; i++){
			for (j = 0; j < myImg.rows / 2; j++){
				aux[0] = 0;
				aux[1] = 255;
				aux[2] = 0;
				myImg.at<Vec3b>(i, j) = aux;
			}
		}

		for (i = myImg.rows / 2; i < myImg.rows; i++){
			for (j = myImg.rows / 2; j < myImg.cols; j++){
				aux[0] = 0;
				aux[1] = 255;
				aux[2] = 255;
				myImg.at<Vec3b>(i, j) = aux;
			}
		}
		imshow("MY IMG", myImg);
		waitKey(0);
	}

	void inverseMatrix(){

		float x[9] = { 1, 2, 2, 4, 5, 6, 7, 8, 9 };
		Mat imgNew(3, 3, CV_32FC1, x);

		std::cout << imgNew.inv();
		getchar(); getchar();



	}


	//------------------
	//-----LAB2---------
	void RGB24_SplitChannels(){
		Mat img = imread("Images/flowers_24bits.bmp", CV_LOAD_IMAGE_COLOR);
		imshow("original image", img);
		Mat imgRed(img.rows, img.cols, CV_8UC1);
		Mat imgBlue(img.rows, img.cols, CV_8UC1);
		Mat imgGreen(img.rows, img.cols, CV_8UC1);

		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				Vec3b color = img.at<Vec3b>(i, j);
				imgRed.at<uchar>(i, j) = color[2];
				imgGreen.at<uchar>(i, j) = color[1];
				imgBlue.at<uchar>(i, j) = color[0];
			}
		}
		imshow("Red", imgRed);
		imshow("Green", imgGreen);
		imshow("Blue", imgBlue);
		waitKey(0);

	}

	void ColorToGrayscale(){
		Mat img = imread("Images/flowers_24bits.bmp", CV_LOAD_IMAGE_COLOR);
		imshow("original image", img);
		Mat imgTransf(img.rows, img.cols, CV_8UC1);

		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				Vec3b col = img.at<Vec3b>(i, j);
				imgTransf.at<uchar>(i, j) = (col[0] + col[1] + col[2]) / 3;
			}
		}

		imshow("gray", imgTransf);

		waitKey(0);
	}

	void GrayscaleToBlack(){

		printf("Give the threshold:");
		int thresh = 127;
		scanf("%d", &thresh);

		Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		imshow("original image", img);
		Mat imgTransf(img.rows, img.cols, CV_8UC1);

		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				if (img.at<uchar>(i, j) < thresh){
					imgTransf.at<uchar>(i, j) = 0;
				}
				else imgTransf.at<uchar>(i, j) = 255;
			}
		}

		imshow("Black", imgTransf);

		waitKey(0);



	}

	void RGBtoHSV(){
		Mat img = imread("Images/flowers_24bits.bmp", CV_LOAD_IMAGE_COLOR);
		imshow("original image", img);

		Mat newImgH(img.rows, img.cols, CV_8UC1);
		Mat newImgS(img.rows, img.cols, CV_8UC1);
		Mat newImgV(img.rows, img.cols, CV_8UC1);

		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				Vec3b col = img.at<Vec3b>(i, j);
				float red = (float)col[2] / 255;
				float green = (float)col[1] / 255;
				float blue = (float)col[0] / 255;

				float maxim = max(max(red, green), blue);
				float min = min(min(red, green), blue);
				float C = maxim - min;
				float S, H, V;
				V = maxim;

				if (V){
					S = C / V;
				}
				else S = 0;

				if (C){
					if (maxim == red) H = 60 * (green - blue) / C;
					if (maxim == green) H = 120 + 60 * (blue - red) / C;
					if (maxim == blue) H = 240 + 60 * (red - green) / C;
				}
				else H = 0;

				if (H < 0)
					H += 360;

				newImgH.at<uchar>(i, j) = H * 255 / 360;
				newImgS.at<uchar>(i, j) = S * 255;
				newImgV.at<uchar>(i, j) = V * 255;

			}
		}

		imshow("HUE", newImgH);
		imshow("SATURATION", newImgS);
		imshow("VALUE", newImgV);

		waitKey(0);
	}

	//----------------------------
	//--------------LAB3-----------

	void Histogram(){
		Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		imshow("original image", img);
		int hist[256] = { 0 };
		float p[256] = { 0 };
		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				hist[img.at<uchar>(i, j)]++;
				p[img.at<uchar>(i, j)] = ((float)img.at<uchar>(i, j)) / (img.rows*img.cols);
			}
		}
		showHistogram("Histograma", hist, 256, 500);
		for (int i = 0; i < 255; i++){
			printf("%f\n", p[i]);
		}
		waitKey(0);
	}

	int findClosestHistoMaxim(int oldPixel, int* maximumArr, int dim){
		int max = 0;
		for (int k = 0; k <dim; k++){
			if (oldPixel> maximumArr[k] && oldPixel < maximumArr[k + 1]){
				if (oldPixel - maximumArr[k] < maximumArr[k + 1] - oldPixel)
					max = maximumArr[k];
				else max = maximumArr[k + 1];
			}
		}
		return max;
	}

	void MultiLevelThresh(){
		Mat img = imread("Images/cameraman.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		imshow("original image", img);
		int hist[256] = { 0 };
		float p[256] = { 0 };
		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				hist[img.at<uchar>(i, j)]++;
				p[img.at<uchar>(i, j)] += 1.f / (img.rows*img.cols);
			}
		}
		int WH = 5;
		float TH = 0.0003;
		int  maximumArr[256] = { 0 };
		int dim = 0;
		maximumArr[0] = 0;
		dim = 1;
		for (int k = WH; k <= 255 - WH; k++){
			float sum = 0.0;
			bool ok = true;
			for (int j = k - WH; j <= k + WH; j++){
				sum += p[j];
				if (p[k]<p[j])
					ok = false;
			}
			float v = (float)sum / (2 * WH + 1);

			if (p[k]>(v + TH) && ok){
				maximumArr[dim++] = k;
				printf("MAXIM %d\n", k);
			}
		}
		maximumArr[dim] = 255;


		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				for (int k = 0; k <dim; k++){
					if (img.at<uchar>(i, j)> maximumArr[k] && img.at<uchar>(i, j) < maximumArr[k + 1]){
						if (img.at<uchar>(i, j) - maximumArr[k] < maximumArr[k + 1] - img.at<uchar>(i, j))
							img.at<uchar>(i, j) = maximumArr[k];
						else img.at<uchar>(i, j) = maximumArr[k + 1];
					}
				}
			}
		}
		imshow("Second image", img);

		for (int i = 0; i < 256; i++){
			hist[i] = 0;
		}
		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				hist[img.at<uchar>(i, j)]++;
			}
		}
		showHistogram("Histograma", hist, 256, 500);

		waitKey(0);
	}

	uchar limitRange(int value){
		if (value < 0) return 0;
		if (value > 255) return 255;

		return (uchar)value;
	}


	void FloydSteinbergAlg(){
		Mat img = imread("Images/saturn.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		imshow("original image", img);
		int hist[256] = { 0 };
		float p[256] = { 0 };
		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){
				hist[img.at<uchar>(i, j)]++;
			}
		}

		for (int i = 0; i < 256; i++){
			p[i] = (float)hist[i] / (img.rows*img.cols);
		}

		int WH = 5;
		float TH = 0.0003;
		int  maximumArr[256] = { 0 };
		int dim = 1;
		maximumArr[0] = 0;


		for (int k = WH; k <= 255 - WH; k++){
			float sum = 0.0;
			bool ok = true;
			for (int j = k - WH; j <= k + WH; j++){
				sum += p[j];
				if (p[k]<p[j])
					ok = false;
			}
			float v = sum / (2 * WH + 1);

			if (p[k]>(v + TH) && ok){
				maximumArr[dim++] = k;
			}
		}
		maximumArr[dim] = 255;
		for (int i = 0; i <= dim; i++){
			printf("%d\n", maximumArr[i]);
		}


		for (int y = 0; y < img.rows; y++){
			for (int x = 0; x < img.cols; x++){

				uchar oldP = img.at<uchar>(y, x);
				uchar  newP;

				//detect maximum
				for (int k = 0; k < dim; k++){
					if (img.at<uchar>(y, x) >= maximumArr[k] && img.at<uchar>(y, x) <= maximumArr[k + 1]){
						if (img.at<uchar>(y, x) - maximumArr[k] < maximumArr[k + 1] - img.at<uchar>(y, x))
							newP = maximumArr[k];
						else newP = maximumArr[k + 1];
					}
				}

				img.at<uchar>(y, x) = newP;
				int error = oldP - newP;
				printf("%d\n", error);
				if (x < img.cols - 1)
					img.at<uchar>(y, x + 1) = max(min(img.at<uchar>(y, x + 1) + (int)((float)7 * error / 16), 255), 0);

				if (y<img.rows - 1 && x >0)
					img.at<uchar>(y + 1, x - 1) = max(min(img.at<uchar>(y + 1, x - 1) + (int)((float)7 * error / 16), 255), 0);

				if (y < img.rows - 1)
					img.at<uchar>(y + 1, x) = max(min(img.at<uchar>(y + 1, x) + (int)((float)5 * error / 16), 255), 0);

				if (y < img.rows - 1 && x < img.cols - 1)
					img.at<uchar>(y + 1, x + 1) = max(min(img.at<uchar>(y + 1, x + 1) + (int)((float)error / 16), 255), 0);
			}
		}


		imshow("Second image", img);
		waitKey(0);
	}
	//--------------------------------
	//-------------LAB4---------------


	void MyCallBackFunc1(int event, int x, int y, int flags, void* param){
		Mat* src = (Mat*)param;

		if (event == CV_EVENT_LBUTTONDBLCLK)
		{
			printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
				x, y,
				(int)(*src).at<Vec3b>(y, x)[2],
				(int)(*src).at<Vec3b>(y, x)[1],
				(int)(*src).at<Vec3b>(y, x)[0]);
			int s = 0;
			//Compute area
			for (int i = (*src).rows - 1; i >= 0; i--){
				for (int j = (*src).cols - 1; j >= 0; j--){
					if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
						s++;
					}
				}
			}
			printf("Area : %d\n", s);
			int centerR = 0, centerC = 0;
			for (int i = (*src).rows - 1; i >= 0; i--){
				for (int j = (*src).cols - 1; j >= 0; j--){
					if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
						centerR += i;
						centerC += j;
					}
				}
			}
			printf("Center of mass(x,y): %.3f %.3f\n", (float)centerR / s, (float)centerC / s);
			//alongatie
			int alongAxUp = 0, alongAxDown = 0;
			float cR, cC;
			cR = (float)centerR / s;
			cC = (float)centerC / s;

			for (int i = (*src).rows - 1; i >= 0; i--){
				for (int j = (*src).cols - 1; j >= 0; j--){
					if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
						alongAxUp += (i - cR)*(j - cC);
						alongAxDown += (j - cC)*(j - cC) - (i - cR)*(i - cR);
					}
				}
			}

			float fi = (0.5)*atan2(2 * alongAxUp, alongAxDown);

			float angle = (fi * 180) / CV_PI;
			if (angle < 0)
				angle += 180;
			printf("ANGLE: %f\n", angle);
			// perimeter
			int perim = 0;
			for (int i = (*src).rows - 2; i >= 1; i--){
				for (int j = (*src).cols - 2; j >= 1; j--){
					if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
						if (((*src).at<Vec3b>(i - 1, j - 1) != (*src).at<Vec3b>(y, x)) ||
							((*src).at<Vec3b>(i - 1, j) != (*src).at<Vec3b>(y, x)) ||
							((*src).at<Vec3b>(i - 1, j + 1) != (*src).at<Vec3b>(y, x)) ||
							((*src).at<Vec3b>(i + 1, j - 1) != (*src).at<Vec3b>(y, x)) ||
							((*src).at<Vec3b>(i + 1, j) != (*src).at<Vec3b>(y, x)) ||
							((*src).at<Vec3b>(i + 1, j + 1) != (*src).at<Vec3b>(y, x)) ||
							((*src).at<Vec3b>(i, j - 1) != (*src).at<Vec3b>(y, x)) ||
							((*src).at<Vec3b>(i, j + 1) != (*src).at<Vec3b>(y, x)))
							perim++;

					}
				}
			}

			float permiF = perim*CV_PI / 4;
			printf("Perimeter: %f\n", permiF);


			float thR = 4 * CV_PI * (s / (permiF*permiF));
			printf("Thinness ratio: %f\n", thR);


			int cMax = -1, cMin = 1000000, rMax = -1, rMin = 10000;
			for (int i = (*src).rows - 1; i >= 0; i--){
				for (int j = (*src).cols - 1; j >= 0; j--){
					if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x)){
						if (i <= rMin) rMin = i;
						if (i >= rMax) rMax = i;
						if (j <= cMin) cMin = j;
						if (j >= cMax) cMax = j;

					}
				}
			}
			float aspectRatio = ((float)(cMax - cMin + 1)) / (rMax - rMin + 1);
			printf("Aspect ratio: %f\n", aspectRatio);

			//projections
			int hi[3000] = { 0 }, vi[3000] = { 0 };
			for (int i = 0; i < 3000; i++){
				hi[i] = vi[i] = 0;
			}
			int counter = (*src).rows - 1;

			for (int j = 0; j <= (*src).cols - 1; j++){
				counter = (*src).rows - 1;
				for (int i = (*src).rows - 1; i >= 0; i--){
					if ((*src).at<Vec3b>(i, j) == (*src).at<Vec3b>(y, x))
					{
						(*src).at<Vec3b>(counter, j) = Vec3b(0, 0, 0);
						counter--;

					}

				}
			}



			Point A(cMin, (cR + (tan(fi)*(cMin - cC))));
			Point B(cMax, (cR + (tan(fi)*(cMax - cC))));
			line((*src), A, B, CV_RGB(255, 0, 0), 1);
			circle((*src), Point(cC, cR), 5, CV_RGB(255, 0, 0), 1);
			imshow("NEW IMG", *src);

		}
	}

	void GeometricalFeatures(){
		Mat img = imread("Images/geometrical_features.bmp", CV_LOAD_IMAGE_COLOR);
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc1, &img);
		//show the image
		imshow("My Window", img);

		// Wait until user press some key
		waitKey(0);
	}

	//---------------------------------
	//-------------LAB5---------------


	std::queue<Point2i> Q;

	int di[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int dj[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	void BFS(){
		//CV_32SC1
		std::default_random_engine gen;
		std::uniform_int_distribution<int> d(0, 255);
		Mat img = imread("Images/letters2.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		Mat labels = Mat::zeros(img.rows, img.cols, CV_32SC1);
		Mat findImg(img.rows, img.cols, CV_8UC3);
		int label = 0;

		for (int i = 1; i < img.rows - 1; i++){
			for (int j = 1; j < img.cols - 1; j++){
				if (img.at<uchar>(i, j) == 0 && labels.at<int>(i, j) == 0){
					label++;
					labels.at<int>(i, j) = label;
					Q.push({ i, j });
					while (!Q.empty()){
						Point2i aux = Q.front();
						Q.pop();
						for (int k = 0; k < 8; k++){
							if (img.at<uchar>(aux.x + di[k], aux.y + dj[k]) == 0 && labels.at<int>(aux.x + di[k], aux.y + dj[k]) == 0){
								labels.at<int>(aux.x + di[k], aux.y + dj[k]) = label;
								Q.push({ aux.x + di[k], aux.y + dj[k] });
							}
						}
					}
				}
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
		printf("LABELS = %d", label);
		imshow("My Window", img);
		imshow("color", findImg);
		waitKey(0);
	}


	int dx2[] = { -1, -1, 0, 1 };
	int dy2[] = { 0, -1, -1, -1 };
	int minim(std::vector<int> L){
		int min = 10000000000;
		for (int i = 0; i < L.size(); i++){
			if (min>L[i])
				min = L[i];
		}
		return min;
	}

	void TwoPassTraverse(){

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

	int dirx[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int diry[] = { 0, -1, -1, -1, 0, 1, 1, 1 };

	vector<int> computeDC(std::vector<int> sir){
		vector<int> aux;
		for (int i = 0; i < sir.size() - 1; i++){
			aux.push_back((sir[i + 1] - sir[i] + 8) % 8);
		}
		aux.push_back((sir[sir.size() - 1] - sir[0] + 8) % 8);
		return aux;
	}

	void BorderTracing(){
		Mat img = imread("Images/star.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		Mat img2 = Mat::zeros(img.rows, img.cols, CV_8UC1);
		imshow("original image!", img);
		Mat findImg(img.rows, img.cols, CV_8UC3);
		Point p(0, 0), p1(0, 0), pn(0, 0), pn1(0);
		int dir = 7;
		int n = 0;
		std::vector<int> ac;


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
		ac.push_back(dir);
		p1.x = p.x + dirx[dir];
		p1.y = p.y + diry[dir];

		n = 2;
		pn1 = p1;
		pn = p1;
		printf("POINTS: P1= (%d,%d), P2=(%d,%d) %d\n", p.y, p.x, p1.y, p1.x, dir);

		do{

			if (dir % 2 == 0){
				dir = (dir + 7) % 8;
			}
			else dir = (dir + 6) % 8;

			while (img.at<uchar>(pn.y + diry[dir], pn.x + dirx[dir]) != 0){
				dir = (dir + 1) % 8;
			}
			ac.push_back(dir);
			pn1 = pn;

			pn.y = pn.y + diry[dir];
			pn.x = pn.x + dirx[dir];
			img2.at<uchar>(pn.y, pn.x) = 255;

		} while (!((pn == p1) && (pn1 == p) && (n >= 2)));

		imshow("alter image", img2);
		for (int i = 0; i < ac.size(); i++){
			printf("%d ", ac[i]);
		}

		vector<int> dc = computeDC(ac);
		printf("\n\n\nDC");
		for (int i = 0; i < dc.size(); i++){
			printf("%d ", dc[i]);
		}
		waitKey(0);
	}

	void ContourReconstr(){
		Mat img = imread("Images/gray_background.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		vector<int>ac;
		int size;
		Point p(0, 0);
		int x, y, n;
		FILE *f = fopen("reconstruct.txt", "r");

		if (f != NULL){
			fscanf(f, "%d %d", &y, &x);
			p.x = x;
			p.y = y;
			fscanf(f, "%d", &n);
			printf("%d %d %d\n", p.y, p.x, n);
			while (fscanf(f, "%d ", &x) == 1){
				ac.push_back(x);
				printf("%d ", x);
			}
			printf("AC SIZE = %d", ac.size());
			Point aux(0, 0);
			aux.x = p.x;
			aux.y = p.y;
			img.at<uchar>(aux.y, aux.x) = 0;
			for (int i = 0; i < ac.size(); i++){
				img.at<uchar>(aux.y + diry[ac[i]], aux.x + dirx[ac[i]]) = 0;
				aux.y = aux.y + diry[ac[i]];
				aux.x = aux.x + dirx[ac[i]];
			}
		}
		else printf("ERR!");

		imshow("TEXT", img);

		waitKey();
	}

}