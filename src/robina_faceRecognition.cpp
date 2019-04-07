////8 may 2014 Amir hosein.Lesani
///version 3.0.0 alpha
#include <sensor_msgs/image_encodings.h>
#include"robina_faceRecognition/robina_faceRecognition.hpp"
#include "sensor_msgs/image_encodings.h"

int faceRecognition::init()
{
    bufferDone = false;
    NamevectorSize = 0;

    if (FSDKE_OK != FSDK_ActivateLibrary(SERIAL_KEY))
    {
        cout<<"Please run the License Key Wizard (Start - Luxand - FaceSDK - License Key Wizard)\n EdetectFaceor activating FaceSDK"<<endl;
        return -1;
    }

    ///fsdk initializing
    FSDK_Initialize("");
    FSDK_InitializeCapturing();
    FSDK_SetFaceDetectionParameters(false, false, 200);
    FSDK_SetFaceDetectionThreshold(3);
    cout<<"activating FaceSDK"<<endl;

    ///services
    srvStoreTemp_srv  = nh.advertiseService("StoreTemplateface"   ,&faceRecognition::storeTempCB   ,this);
    srvLoadTemp       = nh.advertiseService("loadTemplateface"    ,&faceRecognition::loadTempCB    ,this);
    srvRecognizeOnce  = nh.advertiseService("/recognizeFaceOnce",&faceRecognition::recognizeOnceCB  ,this);
    srvStopContinouslyDetectFace = nh.advertiseService("stopContiniouslyfaceDetection" ,&faceRecognition::stopContiniouslyCB ,this);
    srvImageResponse = nh.advertiseService ("imageResponse", &faceRecognition::imageResponseCB ,this);
    srvDeleteFaces = nh.advertiseService ("deleteFaces",&faceRecognition::deleteFacesCB,this);
    srvWaitToDetectFace = nh.advertiseService("waitToDetectFace",&faceRecognition::waitForDetectFaceCB,this);

    ///actions
    faceRecognition_as = new faceRecognitionAction(nh,
                                                   "/faceRecognition",
                                                   boost::bind(&faceRecognition::faceDetectionActionExecuteCB,this, _1),
                                                   false );
    faceRecognition_as->start();

    faceRecognitionContiniously_as = new faceRecognitionContiniouslyAction(nh,"/faceRecognitionContinously",
                                                                           boost::bind(&faceRecognition::faceDetectionContiniouslyActionExecuteCB,this,_1 )
                                                                           ,false);
    faceRecognitionContiniously_as->start();

    ///publishers
    face_detection_pub = nh.advertise<robina_faceRecognition::faceRecognition_msg>("faceDetection",1);

    ////initilize Values
    personNumber = 0;
    nPersons= 0;

    ///image address
    trainFileDataPath();

    ///check XML file
    checkXmlFile();
    buffer.empty();

    //setPantilt(0);
    return 1;
}

faceRecognition::faceRecognition():loop_rate(2),stopContiniouslyAction(false)
{
    init();
}

faceRecognition::~faceRecognition()
{
    delete faceRecognitionContiniously_as;
    delete faceRecognition_as;

}

void faceRecognition::imageCB(const sensor_msgs::ImageConstPtr& msg)
{
    cout<<"call back is running"<<endl;
    mutex.lock();

    try
    {
        responseImage = *msg;
        FSDK_FreeImage(imageHandle);
        cv_bridge::CvImagePtr ptr= cv_bridge::toCvCopy( msg,sensor_msgs::image_encodings::RGB8);

        image=ptr->image;
        FSDK_LoadImageFromBuffer( &imageHandle,(unsigned char*) image.data,image.cols ,
                                  image.rows ,  image.cols * 3 , FSDK_IMAGE_COLOR_24BIT );

        buffer.push(imageHandle);

        cout<<"buffer size is"<<buffer.size()<<endl;
        if( buffer.size() > 5 )
        {
            buffer.pop();
            bufferDone = true;
        }
        if( buffer.size() == 4 )
        {   bufferDone = true;
            cout<<"Buffering done!"<<endl;
        }
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR( "cv_bridge exception: %s", e.what() );
    }
    mutex.unlock();
}


bool faceRecognition::storeTempCB(robina_faceRecognition::StoreTemplateface::Request &req, robina_faceRecognition::StoreTemplateface::Response &res)
{
    int detectedFace;
    TFacePosition facePosition;
    //make subscriber
    bufferDone = false;
    buffer.empty();
    ros::Subscriber image_sub__ = nh.subscribe<sensor_msgs::Image>("/camera/rgb/image_color", 1, &faceRecognition::imageCB,this);
    //wait for buffering done
    bool ready= false;
    do
    {
        mutex.lock();
        ready=bufferDone;
        cout<<"ready is "<<ready<<endl;
        mutex.unlock();
        loop_rate.sleep();

    }while(!ready);

    loop_rate.sleep();
    mutex.lock();
    detectedFace = FSDK_DetectFace( buffer.front(),&facePosition);
    mutex.unlock();

    if( ( waitForDetectFace(20) ) && ( bufferDone == true ) )
    {
        string name;
        name = req.name.data();
        cout<<name<<endl;
        personNumber++;
        vectorFaces.nameVector.push_back(name);
        ///debugging
        cout<<"size name"<<vectorFaces.nameVector.size()<<endl;
        cout<<"size template data"<<vectorFaces.templateData.size()<<endl;
        ///
        for(int i = 0 ; i<10 ; i++)
        {
            mutex.lock();
            if(FSDK_DetectFace( buffer.front(),&facePosition) == FSDKE_OK)
            {
                FSDK_FaceTemplate faceTemp;

                FSDK_GetFaceTemplateInRegion( buffer.front(), &facePosition,&faceTemp );
                cout<<"im training"<<endl;
                vectorFaces.templateData.push_back( faceTemp );
                cout<<"stack is pushing"<<vectorFaces.templateData.size()<<endl;
                mutex.unlock();
            }
            else
            {   //exception (if garbage image gotted)
                int k = vectorFaces.templateData.size();
                for(int j = vectorFaces.templateData.size() ; j > k-i ; j-- )
                {
                    vectorFaces.templateData.pop_back();
                    cout<<j<<"garbaged learn  poped"<<endl;

                }
                personNumber--;
                vectorFaces.nameVector.pop_back();
                cout<<"Lets try again "<<endl;
                mutex.unlock();
                res.response = false;
                return true;
            }


        }

        res.response = true;
        mutex.unlock();
        cout<<"stack size is :"<<vectorFaces.templateData.size()<<endl;
        saveXml( vectorFaces );
        return true;
    }
    else
    {
        loop_rate.sleep();
        res.response = false;

        cout<<"cant detect"<<endl;

        mutex.unlock();

    }
    mutex.unlock();
    buffer.empty();
    bufferDone = false;
    image_sub__.shutdown();
    return true;
}
void faceRecognition::saveXml(TfaceTemplate vectorFaces)
{
    int k = 10;
    int z = 0;
    cout<<"vector face in save xml function is"<<vectorFaces.templateData.size()<<endl;
    /////save persons number in numberXML
    personnumber = cvOpenFileStorage(npersonAddress, 0, CV_STORAGE_WRITE );
    cout<<"file has been opened"<<endl;
    cvWriteInt( personnumber, "nPersons", countOfPeople+1 );
    cout<<"nperson read"<<endl;
    cvReleaseFileStorage( &personnumber );
    cout<<"file released"<<endl;
    //////////////////////////////////////
    /////append Template Data to FaceTemplateXML
    CvFileStorage *storedFile;
    storedFile = cvOpenFileStorage(xmlAdress, 0, CV_STORAGE_APPEND );
    cout<<"xml file is appended"<<endl;
    char varname[255];
    snprintf( varname, sizeof(varname)-1, "personTemplate_%d", (countOfPeople+1) );
    cvStartWriteStruct(storedFile,varname,CV_NODE_MAP, NULL, cvAttrList(0,0));
    cout<<"varname"<<varname<<endl;
    cout<<"cv var name writed"<<endl;
    cout<<"name vector size"<<vectorFaces.nameVector.size()<<endl;
    cout<<"name vector size is"<<NamevectorSize<<endl;
    strcpy(personName,string(vectorFaces.nameVector.at(NamevectorSize)).c_str());
    for(i= 0;i<vectorFaces.nameVector.size();i++)
        vectorFaces.nameVector.at(i);
    char person[255];
    snprintf (person , sizeof(person)-1, "personName_%d",(countOfPeople+1) );
    cvWriteString(storedFile,person,personName);
    cout<<"person name"<<person<<endl;
    cvWriteInt(storedFile,"faceTamplateCount",10);
    char _template[255];
    countOfPeople*10;
    int f = NamevectorSize+1;
    finalVectorSize= 10*f;
    int startVectorSize = finalVectorSize-10;
    cout<<"vector face before saving is:"<<vectorFaces.templateData.size()<<endl;
    for(startVectorSize ; startVectorSize < finalVectorSize; startVectorSize++)
    {
        cout<<"start vector is :"<<startVectorSize<<endl;
        cout<<"final vector size is"<<finalVectorSize<<endl;
        z++;
        snprintf( _template, sizeof(_template)-1, "template_%d", (z) );
        cvStartWriteStruct(storedFile,_template,CV_NODE_SEQ, NULL, cvAttrList(0,0));

        cvWriteRawData(storedFile, (void *)&vectorFaces.templateData.at(startVectorSize), sizeof(FSDK_FaceTemplate),"u");

        cout<<"vector face is"<<vectorFaces.templateData.size()<<endl;
        cvEndWriteStruct(storedFile);

    }

    k = k + 10;
    z = 0;
    countOfPeople++;
    NamevectorSize++;

    cvEndWriteStruct(storedFile);
    cvReleaseFileStorage( &storedFile );

    char imageAdress[255];

    strcpy(imageAdress,(string(train_file_path)+"/data/"+string(personName)+".jpg").c_str());
    cout<<imageAdress<<endl;
    saveAddress = imageAdress;


    FSDK_SaveImageToFile( imageHandle, imageAdress );
    recognizeVectorFaces.templateData.clear();

    mutex.unlock();

    //saveAddresspath.append(imageAdress);


}
bool faceRecognition::loadTempCB(robina_faceRecognition::loadTemplateface::Request &req, robina_faceRecognition::loadTemplateface::Response &res)
{
    CvFileStorage *checkFile;
    checkFile = cvOpenFileStorage( npersonAddress,0,CV_STORAGE_READ );
    if(!checkFile)
    {
        cout<<"Data XML doesnt Stored!"<<endl;
        return false;
    }
    //nPerson is global value ----> person number that stored
    nPersons = cvReadIntByName( checkFile , 0, "nPersons");
    cvReleaseFileStorage(&checkFile);
    //clearing load vector
    loadVectorName.nameVector.clear();
    loadVectorName.templateData.clear();
    res.response = loadXml();
    return true;
}
bool faceRecognition::loadXml()
{
    CvFileStorage *storedFile;
    storedFile = cvOpenFileStorage( xmlAdress,0,CV_STORAGE_READ );

    if(nPersons == 0)
    {
        cout<<"Doesnt Anyone Stored"<<endl;
    }
    if(!storedFile)
    {
        cout<<"Stored file doesnt exist!";
        return false;
    }
    FSDK_FaceTemplate faceTemplate;
    for( i = 0; i< nPersons ; i++)
    {
        char person[255];
        char personTemplate[255];
        string sPersonName;

        snprintf (person , sizeof(person)-1, "personName_%d",(i+1) );
        snprintf (personTemplate , sizeof(personTemplate)-1,"personTemplate_%d",(i+1));
        CvFileNode* node = cvGetFileNodeByName( storedFile, 0, personTemplate );
        sPersonName = cvReadStringByName( storedFile, node, person );
        loadVectorName.nameVector.push_back( sPersonName );
        for(int j = 0 ; j< 10 ; j++)
        {
            char templateCount[255];
            snprintf ( templateCount , sizeof(templateCount)-1,"template_%d",(j+1) );

            CvFileNode* node1 = cvGetFileNodeByName( storedFile, node, templateCount);
            cvReadRawData(storedFile,node1,(void *)&faceTemplate,"u");
            loadVectorName.templateData.push_back(faceTemplate);
        }
    }
    cvReleaseFileStorage( &storedFile );
    cout<<"loading has been finished"<<endl;
    return true;

}

bool faceRecognition::deleteFacesCB(robina_faceRecognition::deleteFaces::Request &req, robina_faceRecognition::deleteFaces::Response &res)
{
    loadVectorName.nameVector.clear();
    loadVectorName.templateData.clear();
    cout<<"load has been cleared"<<endl;
    countOfPeople = 0;
    personNumber = 0;
    nPersons = 0;
    remove(npersonAddress);
    remove(xmlAdress);
    cout<<"All data has been removed!"<<endl;
    return res.response = true;
}

bool faceRecognition::recognizeOnceCB(robina_faceRecognition::recognizeFaceOnce::Request &req, robina_faceRecognition::recognizeFaceOnce::Response &res)
{
    //make subscriber
    buffer.empty();
    bufferDone = false;
    ros::Subscriber image_sub__ = nh.subscribe<sensor_msgs::Image>("/camera/rgb/image_color", 1, &faceRecognition::imageCB,this);
    //wait for buffering done
    bool ready= false;

    do
    {
        mutex.lock();
        ready=bufferDone;
        cout<<ready<<endl;
        mutex.unlock();
        loop_rate.sleep();

    }while(!ready);

    loop_rate.sleep();
    bool recognized = false;

    int confidenceOfRecognizeOnce = 0;
    int unknownConfidenceFace = 0;
    int canntDetect = 0;

    recognizeVectorFaces.nameVector.clear();
    recognizeVectorFaces.templateData.clear();

    recognizeVectorFaces = loadVectorName;
    do
    {
        loop_rate.sleep();
        mutex.lock();
        TFacePosition facePosition;
        FSDK_FaceTemplate faceTemplate;
        mutex.unlock();
        float threshold = 0;
        float similarity = 0;
        int j = 0;
        int k = 10 ;
        int similarityCount = 0;
        string person;
        if( waitForDetectFace(15) && bufferDone )
        {
            mutex.lock();
            FSDK_DetectFace(buffer.front(),&facePosition);
            FSDK_GetMatchingThresholdAtFAR(0.45f,&threshold);
            FSDK_GetFaceTemplateInRegion( buffer.front(), &facePosition,&faceTemplate );
            mutex.unlock();
            if(recognizeVectorFaces.templateData.size() == 0)
            {
                nPersons = 0;
            }
            for(int i = 0; i < nPersons; i++)
            {
                for(  ; j < k ; j++)
                {
                    FSDK_MatchFaces(&recognizeVectorFaces.templateData.at(j), &faceTemplate, &similarity);
                    cout<<"similarity is:"<<similarity*100<<endl;
                    if(similarity>threshold)
                    {
                        similarityCount++;
                        person = recognizeVectorFaces.nameVector.at(i);
                    }
                }
                k = k + 10;
            }
            if(similarityCount>3)
            {
                confidenceOfRecognizeOnce++;
                unknownConfidenceFace--;
                cout<<"confidence"<<confidenceOfRecognizeOnce<<endl;
            }
            else
            {
                confidenceOfRecognizeOnce--;
                unknownConfidenceFace++;
            }

            if( ( confidenceOfRecognizeOnce >= 1 ) && ( confidenceOfRecognizeOnce > unknownConfidenceFace ) )
            {
                unknownConfidenceFace = 0;
                res.recognizedName = person;
                recognized = true;
            }
            if( ( unknownConfidenceFace > 2 ) && ( unknownConfidenceFace > confidenceOfRecognizeOnce ) )
            {
                cout<<unknownConfidenceFace<<endl;
                confidenceOfRecognizeOnce = 0;
                recognized = true;
                res.recognizedName = "unknown";
                similarityCount = 0;
            }
            cout<<"Im Recognizing Who Are You!"<<endl;
        }
        else
        {
            mutex.unlock();
            cout<<"cannt detect anyone"<<endl;
            canntDetect++;
            if(canntDetect>=req.tryNumber)
            {
                res.recognizedName = "CANNT DETECT";
                image_sub__.shutdown();
                return false;
            }
            confidenceOfRecognizeOnce = 0;
            unknownConfidenceFace = 0;
        }
        image_sub__.shutdown();
        return true;
    }
    while(!recognized && ros::ok());
    image_sub__.shutdown();
    return recognized;
}

void faceRecognition::faceDetectionContiniouslyActionExecuteCB(const robina_faceRecognition::ac_faceRecognitionContiniouslyGoalConstPtr goal)
{
    //make subscriber
    ros::Subscriber image_sub__ = nh.subscribe<sensor_msgs::Image>("/camera/rgb/image_color", 1, &faceRecognition::imageCB,this);
    //wait for buffering done
    bool ready= false;
    do
    {
        mutex.lock();
        ready=bufferDone;
        cout<<ready<<endl;
        mutex.unlock();
        loop_rate.sleep();

    }while(!ready);
    stopContiniouslyAction = false;
    robina_faceRecognition::ac_faceRecognitionContiniouslyFeedback feedback;
    robina_faceRecognition::ac_faceRecognitionContiniouslyResult result;
    int detectFace;
    do
    {
        loop_rate.sleep();
        mutex.lock();
        TFacePosition facePosition;
        FSDK_FaceTemplate faceTemplate;
        detectFace = FSDK_DetectFace(buffer.front(),&facePosition);
        float threshold = 0;
        float similarity = 0;
        int j = 0;
        int k = 10 ;
        int similarityCount = 0;
        string person;
        if( detectFace == FSDKE_OK )
        {

            FSDK_DetectFace(buffer.front(),&facePosition);
            FSDK_GetMatchingThresholdAtFAR(0.45f,&threshold);
            FSDK_GetFaceTemplateInRegion( buffer.front(), &facePosition,&faceTemplate );
            mutex.unlock();

            if(recognizeVectorFaces.templateData.size() == 0)
            {
                nPersons = 0;
            }
            for(int i = 0; i < nPersons; i++)
            {
                for(  ; j < k ; j++)
                {
                    FSDK_MatchFaces(&recognizeVectorFaces.templateData.at(j), &faceTemplate, &similarity);
                    cout<<"similarity is:"<<similarity*100<<endl;
                    if(similarity>threshold)
                    {
                        similarityCount++;
                        person = loadVectorName.nameVector.at(i);
                    }
                }
                k = k + 10;
            }
            if(similarityCount>3)
            {
                feedback.faceDetectedName = person;
                faceRecognitionContiniously_as->publishFeedback(feedback);
            }
            else
            {
                cout<<"unknown"<<endl;
                feedback.faceDetectedName = "unknown";
                faceRecognitionContiniously_as->publishFeedback(feedback);
                similarityCount = 0;
            }
        }
        else
        {
            mutex.unlock();
            feedback.faceDetectedName = "0";
            faceRecognitionContiniously_as->publishFeedback(feedback);
            cout<<"cannt detect anyone"<<endl;
        }
        loop_rate.sleep();
    }
    while(!stopContiniouslyAction && ros::ok());
    result.finished = stopContiniouslyAction;
    faceRecognitionContiniously_as->setSucceeded(result);
    image_sub__.shutdown();
}

void faceRecognition::faceDetectionActionExecuteCB(const robina_faceRecognition::ac_faceRecognitionGoalConstPtr goal)
{
    //make subscriber
    ros::Subscriber image_sub__ = nh.subscribe<sensor_msgs::Image>("/camera/rgb/image_color", 1, &faceRecognition::imageCB,this);
    //wait for buffering done
    bool ready= false;
    do
    {
        mutex.lock();
        ready=bufferDone;
        cout<<ready<<endl;
        mutex.unlock();
        loop_rate.sleep();

    }while(!ready);
    stopContiniouslyAction = false;
    bool recognized = false;
    int detectFace;
    robina_faceRecognition::ac_faceRecognitionFeedback feedback;
    robina_faceRecognition::ac_faceRecognitionResult result;
    do
    {
        loop_rate.sleep();
        mutex.lock();
        TFacePosition facePosition;
        FSDK_FaceTemplate faceTemplate;
        detectFace = FSDK_DetectFace(buffer.front(),&facePosition);
        float threshold = 0;
        float similarity = 0;
        int j = 0;
        int k = 10 ;
        int similarityCount = 0;
        string person;
        if( detectFace == FSDKE_OK )
        {
            FSDK_DetectFace(buffer.front(),&facePosition);
            FSDK_GetMatchingThresholdAtFAR(0.45f,&threshold);
            FSDK_GetFaceTemplateInRegion( buffer.front(), &facePosition,&faceTemplate );
            mutex.unlock();
            cout<<"number of person"<<nPersons<<endl;
            cout<<"load Vector size is"<<recognizeVectorFaces.templateData.size()<<endl;
            if(recognizeVectorFaces.templateData.size() == 0)
            {
                nPersons = 0;
            }
            for(int i = 0; i < nPersons; i++)
            {
                for(  ; j < k ; j++)
                {
                    FSDK_MatchFaces(&recognizeVectorFaces.templateData.at(j), &faceTemplate, &similarity);
                    cout<<"similarity is:"<<similarity*100<<endl;
                    if(similarity>threshold)
                    {
                        similarityCount++;
                        person = loadVectorName.nameVector.at(i);
                    }
                }
                k = k + 10;
            }
            if( similarityCount>3 && goal->goalPerson == person )
            {
                result.faceRecognizedName = person;
                faceRecognition_as->setSucceeded( result );
                recognized = true;
            }

            if(similarityCount>3)
            {
                feedback.faceDetectedName = person;
                faceRecognition_as->publishFeedback(feedback);
                similarityCount = 0;
            }
            else
            {
                cout<<"unknown"<<endl;
                feedback.faceDetectedName = "unknown";
                faceRecognition_as->publishFeedback(feedback);
                similarityCount = 0;
            }
        }
        else
        {
            mutex.unlock();
            feedback.faceDetectedName = "0";
            faceRecognition_as->publishFeedback(feedback);
            cout<<"cannt detect anyone"<<endl;
        }
        loop_rate.sleep();
    }
    while(!recognized && ros::ok());
    image_sub__.shutdown();
}

bool faceRecognition::stopContiniouslyCB(robina_faceRecognition::stopContiniouslyfaceDetection::Request &req, robina_faceRecognition::stopContiniouslyfaceDetection::Response &res)
{
    stopContiniouslyAction = true;
    return stopContiniouslyAction;
}
void faceRecognition::checkXmlFile()
{
    ///////////////xml reading
    CvFileStorage *checkFile;
    checkFile = cvOpenFileStorage( npersonAddress,0,CV_STORAGE_READ );
    countOfPeople = cvReadIntByName( checkFile , 0, "nPersons");
    cout<<"Count of People Who I Know :"<<countOfPeople<<endl;
    cvReleaseFileStorage( &checkFile );
}
void faceRecognition::trainFileDataPath()
{
    ros::param::get( "train_file_path", train_file_path );
    strcpy(xmlAdress, ( string(train_file_path)+"/faceTemplateData.xml").c_str() );
    strcpy(npersonAddress, ( string(train_file_path)+"/Data.xml").c_str() );
}

bool faceRecognition::waitForDetectFaceCB(robina_faceRecognition::waitToDetectFace::Request &req, robina_faceRecognition::waitToDetectFace::Response &res)
{
    waitForDetectFace(req.trynumber);
}
bool faceRecognition::waitForDetectFace(int tryNumber)
{
    //make subscriber
      buffer.empty();
      bufferDone = false;
      ros::Subscriber image__sub__ = nh.subscribe<sensor_msgs::Image>("/camera/rgb/image_color", 1, &faceRecognition::imageCB,this);
    //wait for buffering done
    bool ready= false;
    do
    {
        mutex.lock();
        ready=bufferDone;
        cout<<ready<<endl;
        mutex.unlock();
        loop_rate.sleep();

    }while(!ready);
    int confidenceOfDetectOnce = 0;
    int confidenceOfCantDetectOnce = 0;
    int tilt = 0 ;
    bool swithcPantilt = false;
    do
    {
        int detectFace;
        loop_rate.sleep();
        mutex.lock();
        TFacePosition facePosition;
        detectFace = FSDK_DetectFace(buffer.front(),&facePosition);
        mutex.unlock();


        if( detectFace == FSDKE_OK )
        {
            cout<<"detected"<<endl;
            image__sub__.shutdown();
            return true;
            if(confidenceOfCantDetectOnce <= 0)
            {
                confidenceOfCantDetectOnce = 0;
            }
        }
        else
        {
            mutex.unlock();

            confidenceOfCantDetectOnce++;
            cout<<confidenceOfCantDetectOnce<<endl;
            if(confidenceOfCantDetectOnce>3)
            {
                cout<<"set pantilt must call"<<endl;
                cout<<"tilt value"<<tilt<<endl;

                cout<<"cannt detect anyone"<<endl;
                if(!swithcPantilt)
                {
                    if(tilt>=10)
                    {
                        swithcPantilt = true;
                    }
                    else
                    {
                        tilt = tilt +10;
                    }

                }
                else
                {   if(tilt <= -10)
                    {
                        swithcPantilt = false;

                    }
                    else
                    {
                        tilt = tilt - 10;
                    }

                }
                confidenceOfCantDetectOnce = 0;
                setPantilt(tilt);
                loop_rate.sleep();
            }
            if(confidenceOfCantDetectOnce>=tryNumber )
            {
                image__sub__.shutdown();
                return true;
            }
            if(confidenceOfDetectOnce <= 0)
            {
                confidenceOfDetectOnce = 0;
            }
            buffer.empty();
            bufferDone = false;
        }

    }
    while( ros::ok() );
    image__sub__.shutdown();
}

bool faceRecognition::setPantilt(int tilt)
{
    Pantilt *pantilt;
    pantilt = new Pantilt(&nh);
    pantilt->init(true);
    bool result = pantilt->set_pantilt(0,tilt);
    delete pantilt;
    return result;
}

bool faceRecognition::imageResponseCB(robina_faceRecognition::imageResponse::Request &req, robina_faceRecognition::imageResponse::Response &res)
{

    res.image_address = saveAddress;
    return true;

}

int main(int argc,char **argv)
{
    ros::init(argc,argv,"robina_faceRecognition");
    faceRecognition facerecognition;
    ros::Rate loop(50);
    ros::MultiThreadedSpinner spinner(2);
    while (ros::ok())
    {
        spinner.spin();
        loop.sleep();
    }
}
