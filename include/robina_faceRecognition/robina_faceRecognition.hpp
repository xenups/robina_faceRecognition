#include <ros/ros.h>

#include <image_transport/image_transport.h>
//#include <cv_bridge/CvBridge.h>
#include <cv_bridge/cv_bridge.h>
#include <queue>
#include <deque>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
/////////////////services
#include "robina_faceRecognition/StoreTemplateface.h"
#include "robina_faceRecognition/loadTemplateface.h"
#include "robina_faceRecognition/recognizeFaceOnce.h"
#include "robina_faceRecognition/stopContiniouslyfaceDetection.h"
#include "robina_faceRecognition/facesAngles.h"
#include "robina_faceRecognition/imageResponse.h"
#include <robina_faceRecognition/ac_faceRecognitionAction.h>
#include <robina_faceRecognition/ac_faceRecognitionContiniouslyAction.h>
#include <robina_faceRecognition/deleteFaces.h>
#include <robina_faceRecognition/waitToDetectFace.h>
////pantilt
#include <robina_faceRecognition/pantilt.hpp>
////////////////actions
#include <actionlib/server/simple_action_server.h>

#include "robina_faceRecognition/Face_Angle.h"
#include "robina_faceRecognition/Face_Angles.h"
#include "robina_faceRecognition/faceRecognition_msg.h"
#include <robina_faceRecognition/LuxandFaceSDK.h>

#include "LuxandFaceSDK.h"
#include <boost/signals2/mutex.hpp>
#define  SERIAL_KEY "NTAmlOelBiP3Nm+yeJV/LwwH6yEYNuhndErN69DMzewzUV5RElzKIqSA6NE2YHCkyLzzK9OvVAW0qpu31iDeXvgckg25pmkOO7o67pv9rDLeNSzCzubasJ7vwoJQxqHtZESWYzAS3AxsNNrXM8KdUUiUxyi0R790gKEyqCAanuo="

using namespace std;
using namespace cv;

typedef struct _FaceTemplate
{
    vector <FSDK_FaceTemplate> templateData;
    string Name;
    vector <string>  nameVector;

}TfaceTemplate;

typedef actionlib::SimpleActionServer<robina_faceRecognition::ac_faceRecognitionAction> faceRecognitionAction;
typedef actionlib::SimpleActionServer<robina_faceRecognition::ac_faceRecognitionContiniouslyAction> faceRecognitionContiniouslyAction;


class faceRecognition
{

public:
    faceRecognition();
    ~faceRecognition();

    int init();
private:
    ///services
    bool storeTempCB (robina_faceRecognition::StoreTemplateface::Request &req,  robina_faceRecognition::StoreTemplateface::Response &res );
    bool loadTempCB (robina_faceRecognition::loadTemplateface::Request &req,   robina_faceRecognition::loadTemplateface::Response &res );
    bool recognizeOnceCB(robina_faceRecognition::recognizeFaceOnce::Request & req , robina_faceRecognition::recognizeFaceOnce::Response &res );
    bool facesAnglesCB(robina_faceRecognition::facesAngles::Request &req, robina_faceRecognition::facesAngles::Response &res);
    bool stopContiniouslyCB(robina_faceRecognition::stopContiniouslyfaceDetection::Request &req, robina_faceRecognition::stopContiniouslyfaceDetection::Response &res);
    bool imageResponseCB(robina_faceRecognition::imageResponse::Request &req, robina_faceRecognition::imageResponse::Response &res);
    bool deleteFacesCB(robina_faceRecognition::deleteFaces::Request &req,robina_faceRecognition::deleteFaces::Response &res);
    bool waitForDetectFaceCB(robina_faceRecognition::waitToDetectFace::Request &req,robina_faceRecognition::waitToDetectFace::Response &res);

    ///actions
    void faceDetectionActionExecuteCB(const robina_faceRecognition::ac_faceRecognitionGoalConstPtr goal);
    void faceDetectionContiniouslyActionExecuteCB(const robina_faceRecognition::ac_faceRecognitionContiniouslyGoalConstPtr goal);

    void faceAnglesTimerCallback(const ros::TimerEvent& e);

    actionlib::SimpleActionServer<robina_faceRecognition::ac_faceRecognitionAction> *faceRecognition_as;
    actionlib::SimpleActionServer<robina_faceRecognition::ac_faceRecognitionContiniouslyAction> *faceRecognitionContiniously_as;

    void saveXml( TfaceTemplate vectorFaces );
    bool loadXml();
    void imageCB( const sensor_msgs::ImageConstPtr& msg );
    bool setPantilt(int tilt);

    int countOfPeople;

    void checkXmlFile();
    void trainFileDataPath();

    ros::NodeHandle nh;
    ros::Rate loop_rate;

    ros::ServiceServer srvStoreTemp_srv;
    ros::ServiceServer srvLoadTemp;
    ros::ServiceServer srvRecognizeOnce;
    ros::ServiceServer srvRecognizeContiniously;
    ros::ServiceServer srvFacesAnlges;
    ros::ServiceServer srvImageResponse;
    ros::ServiceServer srvStopContinouslyDetectFace;
    ros::ServiceServer srvDeleteFaces;
    ros::ServiceServer srvWaitToDetectFace;


    ros::Publisher face_detection_pub;

    ros::Timer faceAnglesTimer;

    bool stopContiniouslyAction;
    bool bufferDone;
    bool waitForDetectFace(int tryNumber);
    //ros::Subscriber image_sub_;
    cv::Mat image;
    IplImage *saveImage;
    sensor_msgs::Image responseImage;
    queue <HImage> buffer;
    HImage   imageHandle;

    TfaceTemplate recognizeVectorFaces;
    TfaceTemplate vectorFaces;
    TfaceTemplate loadVectorName;

    int personNumber;
    int nPersons ;
    int i;
    int NamevectorSize;
    int finalVectorSize;


    char personName[255];
    char npersonAddress[255];
    CvFileStorage *personnumber;

    string train_file_path;
    char xmlAdress[255];
    string saveAddress;

    ///angles
    double newWidth;
    double newHeight;


    sensor_msgs::ImageConstPtr  msg;
    boost::signals2::mutex mutex;
};
