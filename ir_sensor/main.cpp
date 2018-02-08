#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdio>
#include <sstream>

#include "SPI.h"
#include "Lepton_I2C.h"

// network:libs
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#define SERVER "192.168.0.100"
#define BUFLEN 512  //Max length of buffer
#define PORT 1234   //The port on which to send data
// network:libs

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27
#define MIN_CONTOUR_SIZE 18
#define WIDTH 80
#define HEIGHT 60



using namespace cv;
using namespace std;
unsigned char img[60][80];

// network:function
void die(char *s)
{
    perror(s);
    exit(1);
}
// network:function


/// Global variables
//
Point center_pt;
// just for debug
int contours_size = 0;
bool someone = false;

int threshold_value = 180;
int threshold_type = 3;

int const max_value = 255;
int const max_type = 4;

// to show 
int show_rect = 0;
int show_ord = 0;
int show_thr= 0;
char* window_name = "flir";

string calculate_coord()
{
   if(center_pt.x >= 40)
     {
	if(center_pt.y >= 30)
	  return "SE";
	else
	  return "NE";
     }
   else
     {
	if (center_pt.y >= 30)
	  return "SW";
	else 
	  return "NW";
     }
   

}


void ShowTrackBar()
{
   char* trackbar_type = "Type:";
   char* trackbar_value = "Value";
   char * trackbar_rect ="Show track ";
   char * trackbar_ord  ="Show coord ";
   char * trackbar_thr  ="Show threshold ";
   
   // Create Trackbar to choose type of Threshold
//   createTrackbar( trackbar_type,window_name, &threshold_type,max_type, NULL );
//   createTrackbar( trackbar_value,window_name, &threshold_value,max_value, NULL);
   createTrackbar( trackbar_rect,window_name, &show_rect,1, NULL );
   createTrackbar( trackbar_ord,window_name, &show_ord,1, NULL );
createTrackbar( trackbar_thr,window_name, &show_thr,1, NULL );
}

void ShowImgWithCV(Mat * cvMat)
{

   if(show_ord){
      line( *cvMat, Point(40,0),Point(40,60) ,CV_RGB(255,0,255) , 1, 4 );
      line( *cvMat, Point(0,30),Point(80,30) ,CV_RGB(255,0,255) , 1, 8 );
      circle(*cvMat,center_pt, 1,Scalar( 255, 0, 255 ),-1,8 );
      char str[200];
      string coord = calculate_coord();
      sprintf(str,"(%d,%d)",center_pt.x,center_pt.y);//threshold_value);
      putText(*cvMat, coord, Point(10, 10), FONT_HERSHEY_PLAIN, 0.5, CV_RGB(255,0,255), 0.5);

   }


   resize(*cvMat,*cvMat, (*cvMat).size()*3);
   imshow("flir", *cvMat);
   waitKey(1);
}


void GetDataFromSpi(Mat * cvMat)
{
   uint8_t result[PACKET_SIZE*PACKETS_PER_FRAME];
   uint16_t *frameBuffer;

   int resets = 0;
   for(int j=0;j<PACKETS_PER_FRAME;j++)
     {
	//if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
	read(spi_cs0_fd, result+sizeof(uint8_t)*PACKET_SIZE*j, sizeof(uint8_t)*PACKET_SIZE);
	int packetNumber = result[j*PACKET_SIZE+1];
	if(packetNumber != j)
	  {
	     j = -1;
	     resets += 1;
	     usleep(1000);
	     //Note: we've selected 750 resets as an arbitrary limit, since there should $
	     //By polling faster, developers may easily exceed this count, and the down p$
	     if(resets == 750)
	       {
		  SpiClosePort(0);
		  usleep(750000);
		  SpiOpenPort(0);
	       }	     
	  }	
     }
   frameBuffer = (uint16_t *)result;
   int row, column;
   uint16_t value;
   uint16_t minValue = 65535;
   uint16_t maxValue = 0;
   
   for(int i=0;i<FRAME_SIZE_UINT16;i++)
     {

	//skip the first 2 uint16_t's of every packet, they're 4 header bytes
	if(i % PACKET_SIZE_UINT16 < 2)
	  {
	     
	     continue;
	  }
	
	
	//flip the MSB and LSB at the last second
	int temp = result[i*2];
	result[i*2] = result[i*2+1];
	result[i*2+1] = temp;
	
	value = frameBuffer[i];
	if(value > maxValue)
	  {
	     
	     maxValue = value;
	  }
	
	if(value < minValue)
	  {
	     
	     minValue = value;
	  }
	
	column = i % PACKET_SIZE_UINT16 - 2;
	row = i / PACKET_SIZE_UINT16 ;
     }
   float diff = maxValue - minValue;
   float scale = 255/diff;
   
   int n=0;
   int somme= 0;
   for(int i=0;i<FRAME_SIZE_UINT16;i++)
     {
	
	if(i % PACKET_SIZE_UINT16 < 2)
	  {
	     
	     continue;
	  }
	
	value = (frameBuffer[i] - minValue) * scale;	     
	column = (i % PACKET_SIZE_UINT16 ) - 2;
	row = i / PACKET_SIZE_UINT16;	     
	somme +=frameBuffer[i]-minValue;
	n++;
	img[row][column]= (frameBuffer[i]-minValue) * scale;
	
     }
   

   Mat m=Mat(60, 80, CV_8U, img);
   *cvMat=m;
}

/************************************************* /
 * 
 *       OPENCV PROCESS 
 * 
 * ***********************************************/
// CANNY

void CannyThreshold(Mat * cvMat)
{
   int thres = threshold_value;
   Mat canny,toDisplay;
   Canny(*cvMat, canny, thres, thres*3);
   
   // find the contours
   // 
   vector< vector<Point> > contours;
   findContours(canny, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
   // Finds the contour with the largest area
   cvtColor(*cvMat, toDisplay, CV_GRAY2BGR);
   int area = 0;
   int idx=-1;
   for(int i=0; i<contours.size();i++) {
//      cout << "Contour " << i << ": " << contours[i].size() << endl;
      if(area < contours[i].size() && (contours[i].size() > MIN_CONTOUR_SIZE))
	idx = i;
      area = contours[i].size();
   }
   
   // for debug
   if(contours_size != contours.size())
     {
	contours_size = contours.size();
//	cout << "Nombre de rect " << contours.size()<<"\n";
     }
   
   if(idx>=0)
     {
	if(idx >=2 && threshold_value < 180)
	  {
	     threshold_value+=5;
	  }
	
	// Calculates the bounding rect of the largest area contour
	Rect rect = boundingRect(contours[idx]);
	Point pt1,pt2;
	pt1.x = rect.x;
	pt1.y = rect.y;
	pt2.x = rect.x + rect.width;
	pt2.y = rect.y + rect.height;
	center_pt.x = rect.x + (rect.width/2);
	center_pt.y = rect.y + (rect.height/2);
	// Draws the rect in the original image and show it
	if(show_rect)rectangle(toDisplay, pt1, pt2, CV_RGB(0,255,0), 1);
	rectangle(toDisplay, Point(0,0), Point(79,59), CV_RGB(0,255,0), 1);
//	updatePresence(toDisplay,true);
	
     }else
     {
//	updatePresence(toDisplay,false);
	rectangle(toDisplay, Point(0,0), Point(79,59), CV_RGB(0,0,255), 1);
	if(threshold_value > 50) {
	   threshold_value-=5;
	   //   cout << "Threshold" << threshold_value <<endl;
	}
	
     } 
   
   *cvMat=toDisplay;
}





/************************************************* /
 * 
 *       MAIN FUNCTION 
 * 
 * ***********************************************/


int main(int argc, char* argv[])
{

/******************/
/*   VARIABLE     */
/******************/
   Mat cvMat;
   namedWindow( window_name, CV_WINDOW_AUTOSIZE );
/******************/
/*   FFC     */
/******************/
   // if time arg is defined
   // process FFC after argv[1] second
   if(argc==2)
     {
	int timeToSleep=atoi(argv[1]);
        usleep(timeToSleep*1000000);
//	lepton_disable_auto_ffc();	
	// enable telemetry (footer, i.e. packets 61-63)
//	lepton_enable_telemetry(1);
	lepton_perform_ffc();
     }   

/******************/
/*   TRACKBAR     */
/******************/
   ShowTrackBar();

/******************/
/*   MAIN LOOP    */
/******************/
   //open spi port
   SpiOpenPort(0);
  int sequence = 0;
  // network:
  uchar buffer[HEIGHT * WIDTH];
  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);

  if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
      die("socket");
  }

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
    
  if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
  {
      fprintf(stderr, "inet_aton() failed\n");
      exit(1);
  }

  // network:

  while(true) {
      GetDataFromSpi(&cvMat);
      // uchar *buffer = cvMat.data;
      

      // network: serialize
      uchar* p;
      int i;
      int j;
      for (i = 0; i < HEIGHT; ++i) {
        p = cvMat.ptr<uchar>(i);
        for (j = 0; j < WIDTH; ++j) {
            buffer[i * WIDTH + j] = p[j];
        }
      }
      // network: send the buffer
      if (sendto(s, buffer, 4800 , 0 , (struct sockaddr *) &si_other, slen)==-1)
      {
          die("sendto()");
      }

     // network: serialize

      sequence++;
      ShowImgWithCV(&cvMat);
  }
   SpiClosePort(0);
   return 0;
}
