//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

//global variables
Mat frame; //current frame
Mat fgMaskMOG; //fg mask generated by MOG method
Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
Ptr<BackgroundSubtractor> pMOG; //MOG Background subtractor
Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
int keyboard;
Point one(50,300);
Point two(300,450);


//function declarations
void help();
void processVideo(char* videoFilename);
void processImages(char* firstFrameFilename);
void processWebcam();
void processStream();
bool inBed(Point, double, Point, Point);

void help()
{
    cout
        << "--------------------------------------------------------------------------"  << endl
        << "This program shows how to use background subtraction methods provided by "   << endl
        << " OpenCV. You can process both videos (-vid) and images (-img)."              << endl
        << endl
        << "Usage:"                                                                      << endl
        << "./bs {-vid <video filename>|-img <image filename>}"                          << endl
        << "for example: ./bs -vid video.avi"                                            << endl
        << "or: ./bs -img /data/images/1.png"                                            << endl
        << "--------------------------------------------------------------------------"  << endl
        << endl;
}

int main(int argc, char* argv[])
{
    //print help information
    help();

    //check for the input parameter correctness
    if(argc != 3&&argc != 2) {
        cerr <<"Incorret input list" << endl;
        cerr <<"exiting..." << endl;
        return EXIT_FAILURE;
    }

    //create GUI windows
    namedWindow("Frame");
    namedWindow("FG Mask MOG");
    namedWindow("FG Mask MOG 2");

    //create Background Subtractor objects
   //NOTE HERE!!!!
    pMOG= new BackgroundSubtractorMOG(); //MOG approach
    pMOG2 = new BackgroundSubtractorMOG2(); //MOG2 approach

    if(strcmp(argv[1], "-vid") == 0) {
        //input data coming from a video
        processVideo(argv[2]);
    }
    else if(strcmp(argv[1], "-img") == 0) {
        //input data coming from a sequence of images
        processImages(argv[2]);
    }
    else if(strcmp(argv[1], "-webc") == 0){
        //input data coming from webcam
        processWebcam();
    }
    else if(strcmp(argv[1], "-stream") == 0){
        //input data coming from stream
        processStream();
    }
    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}

void processVideo(char* videoFilename) {
    //create the capture object
    VideoCapture capture("C:/Users/Jordan/Downloads/ffmpeg-20160301-git-1c7e2cf-win64-static/bin/stream.mp4");
    //if(!capture.isOpened()){
        //error in opening the video input
    //    cerr << "Unable to open video file: " << videoFilename << endl;
    //    exit(EXIT_FAILURE);
    //}
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        //update the background model
           //AND HERE!!!
        pMOG->operator()(frame, fgMaskMOG);
        pMOG2->operator()(frame, fgMaskMOG2);
        //get the frame number and write it on the current frame
        stringstream ss;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
            cv::Scalar(255,255,255), -1);
        ss << capture.get(CV_CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
            FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask MOG", fgMaskMOG);
        imshow("FG Mask MOG 2", fgMaskMOG2);
        //get the input from the keyboard
        keyboard = waitKey( 30 );
    }
    //delete capture object
    capture.release();
}

void processImages(char* fistFrameFilename) {
    //read the first file of the sequence
    frame = imread(fistFrameFilename);
    if(!frame.data){
        //error in opening the first image
        cerr << "Unable to open first image frame: " << fistFrameFilename << endl;
        exit(EXIT_FAILURE);
    }
    //current image filename
    string fn(fistFrameFilename);
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //update the background model
            //ALSO HERE!!!!
        pMOG->operator()(frame, fgMaskMOG);
        pMOG2->operator()(frame, fgMaskMOG2);
        //get the frame number and write it on the current frame
        size_t index = fn.find_last_of("/");
        if(index == string::npos) {
            index = fn.find_last_of("\\");
        }
        size_t index2 = fn.find_last_of(".");
        string prefix = fn.substr(0,index+1);
        string suffix = fn.substr(index2);
        string frameNumberString = fn.substr(index+1, index2-index-1);
        istringstream iss(frameNumberString);
        int frameNumber = 0;
        iss >> frameNumber;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
            cv::Scalar(255,255,255), -1);
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
            FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask MOG", fgMaskMOG);
        imshow("FG Mask MOG 2", fgMaskMOG2);
        //get the input from the keyboard
        keyboard = waitKey(30);
        //search for the next image in the sequence
        ostringstream oss;
        oss << (frameNumber + 1);
        string nextFrameNumberString = oss.str();
        string nextFrameFilename = prefix + nextFrameNumberString + suffix;
        //read the next frame
        frame = imread(nextFrameFilename);
        if(!frame.data){
            //error in opening the next image in the sequence
            cerr << "Unable to open image frame: " << nextFrameFilename << endl;
            exit(EXIT_FAILURE);
        }
        //update the path of the current frame
        fn.assign(nextFrameFilename);
    }
}

void processWebcam(){
	VideoCapture webcam_0(0);                    //open stream
	if(!webcam_0.isOpened())
	{
		cout << "error: cannot open stream between webcam_0 and application" << endl;
		waitKey(0);
		return;
	}

	Mat frame;
	Mat frameSubbed;                                   //Mat header for frame storing from webcam_0
	int i = 0;


    waitKey(5000);                                //index, we use it for testing

	while ((i++ < 100) && !webcam_0.read(frame)) //skip unread frames
	{
		cout << "frame " << i << " skipped" << endl;
	}

	if (i >= 100)                                //check webcam_0 failure
	{
		cout << "cannot read frames from webcam_0, check drivers" << endl;
		waitKey(0);
		return ;
	} else
	{
		cout << "cam is ready" << endl;
	}

    while ((i++ < 100) && !webcam_0.read(frame)) //skip unread frames
	{
		cout << "frame " << i << " skipped" << endl;
	}

    waitKey(10);
	//char * window_name = "webcam_0 test; press \"ESC\" to exit";
	//namedWindow(window_name, CV_WINDOW_AUTOSIZE);//make new window
	//imshow(window_name, frame);                  //output 1st successfully read frame
    namedWindow("Frame Subbed");
    double avgx, avgy;
    int counter;
    int state = -1;
    bool isInBed = false;
    bool goodbye = false;
    bool goodbyeDone = false;
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!webcam_0.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }

        //update the background model
           //AND HERE!!!
        pMOG->operator()(frame, fgMaskMOG, 0.01);
        pMOG2->operator()(frame, fgMaskMOG2, 0.01);
        //get the frame number and write it on the current frame
        stringstream ss;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
            cv::Scalar(255,255,255), -1);
        ss << webcam_0.get(CV_CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
            FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask MOG", fgMaskMOG);
        imshow("FG Mask MOG 2", fgMaskMOG2);
        frameSubbed = frame.clone();
        avgx = 0;
        avgy = 0;
        counter = 0;


        for (int i = 0; i < fgMaskMOG2.rows; i++){
            for (int j = 0; j < fgMaskMOG2.cols; j++){
                if(fgMaskMOG2.at<unsigned char>(i,j)==0){
                    frameSubbed.at<Vec3b> (i,j)[0] = 0;
                    frameSubbed.at<Vec3b> (i,j)[1] = 0;
                    frameSubbed.at<Vec3b> (i,j)[2] = 0;
                }
                else{
                    counter++;
                    avgx+=j;
                    avgy+=i;
                }
            }
        }
        //MAYBE CHANGE LATER
        if(counter > fgMaskMOG2.rows*fgMaskMOG2.cols*0.008){
            avgx/=counter;
            avgy/=counter;
            double radius = double(counter)*2/(fgMaskMOG2.rows*fgMaskMOG2.cols)*min(fgMaskMOG2.cols,fgMaskMOG2.rows);
            circle(frameSubbed,Point(int(avgx),int(avgy)),radius,Scalar(110,220,0));
            rectangle(frameSubbed, one, two, Scalar(110,220,0));
            if (inBed(Point(int(avgx),int(avgy)),radius, one, two)){
                if (state == 3){
                    if(!isInBed){
                        cout<<"In bed."<<endl;
                        isInBed = true;
                    }
                    state++;
                }
                else if(state == 4){;}
                else if(state > 0 && state < 3){++state;}
                else state = 1;
            }
            else {
                if (avgx > fgMaskMOG2.cols - 60){
                    goodbye = true;
                    goodbyeDone = false;
                }
                else goodbye = false;
                if (state == -3){
                    if(isInBed){
                        isInBed = false;
                        cout <<"Out of bed"<<endl;
                    }
                    state--;
                }
                else if(state == -4){;}
                else if(state < 0&&state>-3){--state;}
                else state = -1;
            }
        }
        else{
            if (goodbye){
                if (!goodbyeDone){
                    cout << "goodbye" <<endl;
                    goodbyeDone= true;
                }
            }
        }
        imshow("Frame Subbed",frameSubbed);

        keyboard = waitKey( 30 );
    }
    //delete capture object
    webcam_0.release();

}

void processStream(){
	frame = imread("C:/Users/Jordan/Downloads/ffmpeg-20160301-git-1c7e2cf-win64-static/bin/image.png");
    if(!frame.data){
        //error in opening the first image
        cout << "Unable to open first image frame: " << endl;
        exit(EXIT_FAILURE);
    }                    //open stream
	Mat frameSubbed;                                   //Mat header for frame storing from webcam_0

    waitKey(5000);                                //index, we use it for testing


	//char * window_name = "webcam_0 test; press \"ESC\" to exit";
	//namedWindow(window_name, CV_WINDOW_AUTOSIZE);//make new window
	//imshow(window_name, frame);                  //output 1st successfully read frame
    namedWindow("Frame Subbed");
    double avgx, avgy;
    int counter;
    int state = -1;
    bool isInBed = false;
    bool goodbye = false;
    bool goodbyeDone = false;
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        frame = imread("C:/Users/Jordan/Downloads/ffmpeg-20160301-git-1c7e2cf-win64-static/bin/image.png");
        if(!frame.data){
            //error in opening the next image in the sequence
            cerr << "Unable to open image frame: " <<endl;
            exit(EXIT_FAILURE);
        }

        //update the background model
           //AND HERE!!!
        pMOG->operator()(frame, fgMaskMOG, 0.01);
        pMOG2->operator()(frame, fgMaskMOG2, 0.01);
        //get the frame number and write it on the current frame
        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask MOG", fgMaskMOG);
        imshow("FG Mask MOG 2", fgMaskMOG2);
        frameSubbed = frame.clone();
        avgx = 0;
        avgy = 0;
        counter = 0;


        for (int i = 0; i < fgMaskMOG2.rows; i++){
            for (int j = 0; j < fgMaskMOG2.cols; j++){
                if(fgMaskMOG2.at<unsigned char>(i,j)==0){
                    frameSubbed.at<Vec3b> (i,j)[0] = 0;
                    frameSubbed.at<Vec3b> (i,j)[1] = 0;
                    frameSubbed.at<Vec3b> (i,j)[2] = 0;
                }
                else{
                    counter++;
                    avgx+=j;
                    avgy+=i;
                }
            }
        }
        //MAYBE CHANGE LATER
        if(counter > fgMaskMOG2.rows*fgMaskMOG2.cols*0.008){
            avgx/=counter;
            avgy/=counter;
            double radius = double(counter)*2/(fgMaskMOG2.rows*fgMaskMOG2.cols)*min(fgMaskMOG2.cols,fgMaskMOG2.rows);
            circle(frameSubbed,Point(int(avgx),int(avgy)),radius,Scalar(110,220,0));
            rectangle(frameSubbed, one, two, Scalar(110,220,0));
            if (inBed(Point(int(avgx),int(avgy)),radius, one, two)){
                if (state == 3){
                    if(!isInBed){
                        cout<<"In bed."<<endl;
                        isInBed = true;
                    }
                    state++;
                }
                else if(state == 4){;}
                else if(state > 0 && state < 3){++state;}
                else state = 1;
            }
            else {
                if (avgx > fgMaskMOG2.cols - 60){
                    goodbye = true;
                    goodbyeDone = false;
                }
                else goodbye = false;
                if (state == -3){
                    if(isInBed){
                        isInBed = false;
                        cout <<"Out of bed"<<endl;
                    }
                    state--;
                }
                else if(state == -4){;}
                else if(state < 0&&state>-3){--state;}
                else state = -1;
            }
        }
        else{
            if (goodbye){
                if (!goodbyeDone){
                    cout << "goodbye" <<endl;
                    goodbyeDone= true;
                }
            }
        }
        imshow("Frame Subbed",frameSubbed);

        keyboard = waitKey( 30 );
    }
    //delete capture object

}

bool inBed(Point p, double radius, Point r1, Point r2){
    if(radius*2<(r2.x-r1.x))
        if(p.x>r1.x&&p.x<r2.x)
            if(p.y>r1.y&&p.y<r2.y)
                return true;
    return false;
}

