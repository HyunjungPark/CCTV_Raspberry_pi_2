#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#define CAM_WIDTH 640
#define CAM_HEIGHT 480


int main(int argc, char** argv)
{
    int value1, value2, value3;
    int flag = 0;
    int cnt = 0;

    time_t curr_t, passed_t;
    double diff_t;

    CvCapture* capture;		          //variable for camera
    IplImage *curr, *prev;	          //variable for image
    CvVideoWriter *videowriter = NULL;    //for VideoWrite

    //choose a camera for capturing
    capture = cvCaptureFromCAM(0);
    if(capture == 0){
        perror("Opencv : open Webcam\n");
	return -1;
    }


    //setting properties of capture image
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);

    curr = cvQueryFrame(capture);
    prev = cvCreateImage(cvGetSize(curr),8,3);
    cvCopy(curr, prev);


    videowriter = cvCreateVideoWriter("camVideo.avi",CV_FOURCC('M','J','P','G'), 30, cvSize(CAM_WIDTH,CAM_HEIGHT),1);



    while(1){
	curr = cvQueryFrame(capture);

//	cvWriteFrame(videowriter,curr);
        for (int x = 0; x < curr->height; x++){    //calculating difference between two images
            for (int y = 0; y < curr->width; y++){
                value1 = abs((curr->imageData[x*curr->widthStep + 3 * y + 0]) - (prev->imageData[x*prev->widthStep + 3 * y + 0]));
                value2 = abs((curr->imageData[x*curr->widthStep + 3 * y + 1]) - (prev->imageData[x*prev->widthStep + 3 * y + 1]));
                value3 = abs((curr->imageData[x*curr->widthStep + 3 * y + 2]) - (prev->imageData[x*prev->widthStep + 3 * y + 2]));

		//r,g,b values of prev and curr image aren't different, cnt is increased
                //if (value1<10 && value2<10 && value3<10){
		if(value1>50 && value2>50 && value3>50){
		   cnt++;
		}

            }
        }
	//if the number of different pixels are less than 200, we think there is no difference between prev and curr image
        if (cnt<50){
	    if(flag == 0){	//At first, check the time
                time(&curr_t);
                flag = 1;
            }
            else{		//still there is no difference, check the elapsed time
                time(&passed_t);
                diff_t = difftime(passed_t, curr_t);
		printf("diff_t is %f\n", diff_t);

		//checking elapsed time
	        if(diff_t>=10){
        	    printf("there is no difference\n elasped time is %f\n", diff_t);
	            printf(ctime(&curr_t));

	            //save .avi file
	            cvWriteFrame(videowriter,curr);

	        }

            }
	    cnt =0;
	}//end of first if
	else{
	    flag=0;
	    cnt=0;
	    diff_t =0;
	    printf("there is difference! \n");
	}


        cvShowImage("current", curr);
	cvCopy(curr,prev);

        if(cvWaitKey(10)== 'p')
            break;
        //cvWaitKey(100);


    }//end of while

    cvReleaseCapture(&capture);
    cvReleaseImage(&curr);
    cvReleaseImage(&prev);
    cvReleaseVideoWriter(&videowriter);

    return 0;
}
