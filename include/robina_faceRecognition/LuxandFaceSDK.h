///////////////////////////////////////////////////
//
//         Luxand FaceSDK Library 
//
//  Copyright(c) 2005-2011 Luxand, Inc.
//         http://www.luxand.com
//
///////////////////////////////////////////////////

#ifndef _LUXANDFACESDK_
#define _LUXANDFACESDK_

#if defined( _WIN32 ) || defined ( _WIN64 )
	#define _FSDKIMPORT_ __declspec(dllimport) __cdecl
    #include <windows.h>
#else
	#define _FSDKIMPORT_
	#define __cdecl
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Error codes

#define FSDKE_OK				0
#define FSDKE_FAILED			-1
#define FSDKE_NOT_ACTIVATED		-2
#define FSDKE_OUT_OF_MEMORY		-3
#define FSDKE_INVALID_ARGUMENT  -4
#define FSDKE_IO_ERROR			-5
#define FSDKE_IMAGE_TOO_SMALL  -6
#define FSDKE_FACE_NOT_FOUND  -7
#define FSDKE_INSUFFICIENT_BUFFER_SIZE  -8
#define FSDKE_UNSUPPORTED_IMAGE_EXTENSION 	-9
#define FSDKE_CANNOT_OPEN_FILE -10
#define FSDKE_CANNOT_CREATE_FILE -11
#define FSDKE_BAD_FILE_FORMAT -12
#define FSDKE_FILE_NOT_FOUND -13
#define FSDKE_CONNECTION_CLOSED		-14
#define FSDKE_CONNECTION_FAILED		-15
#define FSDKE_IP_INIT_FAILED	-16
#define FSDKE_NEED_SERVER_ACTIVATION -17

// Facial feature count

#define FSDK_FACIAL_FEATURE_COUNT	66

// Types

typedef enum {
	FSDK_MJPEG
} FSDK_VIDEOCOMPRESSIONTYPE;

typedef enum {
	FSDK_IMAGE_GRAYSCALE_8BIT,
	FSDK_IMAGE_COLOR_24BIT,
	FSDK_IMAGE_COLOR_32BIT	
} FSDK_IMAGEMODE; 

typedef unsigned int HImage;
	
typedef struct { 
	int x, y; 
} TPoint;

typedef TPoint TPointArray [FSDK_FACIAL_FEATURE_COUNT];

typedef TPoint FSDK_Features [FSDK_FACIAL_FEATURE_COUNT];

typedef float FSDK_ConfidenceLevels [FSDK_FACIAL_FEATURE_COUNT];

typedef struct {
	int xc, yc, w;
	double angle;
} TFacePosition;

typedef struct { 
	char ftemplate[16384];  
} FSDK_FaceTemplate; 



typedef void (__cdecl *FSDK_ProgressCallbackFunction)(int);

// Facial features

#define FSDKP_LEFT_EYE	0
#define FSDKP_RIGHT_EYE	1
#define FSDKP_LEFT_EYE_INNER_CORNER	24
#define FSDKP_LEFT_EYE_OUTER_CORNER	23
#define FSDKP_LEFT_EYE_LOWER_LINE1	38
#define FSDKP_LEFT_EYE_LOWER_LINE2	27
#define FSDKP_LEFT_EYE_LOWER_LINE3	37
#define FSDKP_LEFT_EYE_UPPER_LINE1	35
#define FSDKP_LEFT_EYE_UPPER_LINE2	28
#define FSDKP_LEFT_EYE_UPPER_LINE3	36
#define FSDKP_LEFT_EYE_LEFT_IRIS_CORNER	29
#define FSDKP_LEFT_EYE_RIGHT_IRIS_CORNER	30
#define FSDKP_RIGHT_EYE_INNER_CORNER	25
#define FSDKP_RIGHT_EYE_OUTER_CORNER	26
#define FSDKP_RIGHT_EYE_LOWER_LINE1	41
#define FSDKP_RIGHT_EYE_LOWER_LINE2	31
#define FSDKP_RIGHT_EYE_LOWER_LINE3	42
#define FSDKP_RIGHT_EYE_UPPER_LINE1	40
#define FSDKP_RIGHT_EYE_UPPER_LINE2	32
#define FSDKP_RIGHT_EYE_UPPER_LINE3	39
#define FSDKP_RIGHT_EYE_LEFT_IRIS_CORNER	33
#define FSDKP_RIGHT_EYE_RIGHT_IRIS_CORNER	34
#define FSDKP_LEFT_EYEBROW_INNER_CORNER	13
#define FSDKP_LEFT_EYEBROW_MIDDLE	16
#define FSDKP_LEFT_EYEBROW_MIDDLE_LEFT	18
#define FSDKP_LEFT_EYEBROW_MIDDLE_RIGHT	19
#define FSDKP_LEFT_EYEBROW_OUTER_CORNER	12
#define FSDKP_RIGHT_EYEBROW_INNER_CORNER	14
#define FSDKP_RIGHT_EYEBROW_MIDDLE	17
#define FSDKP_RIGHT_EYEBROW_MIDDLE_LEFT	20
#define FSDKP_RIGHT_EYEBROW_MIDDLE_RIGHT	21
#define FSDKP_RIGHT_EYEBROW_OUTER_CORNER	15
#define FSDKP_NOSE_TIP	2
#define FSDKP_NOSE_BOTTOM	49
#define FSDKP_NOSE_BRIDGE	22
#define FSDKP_NOSE_LEFT_WING	43
#define FSDKP_NOSE_LEFT_WING_OUTER	45
#define FSDKP_NOSE_LEFT_WING_LOWER	47
#define FSDKP_NOSE_RIGHT_WING	44
#define FSDKP_NOSE_RIGHT_WING_OUTER	46
#define FSDKP_NOSE_RIGHT_WING_LOWER	48
#define FSDKP_MOUTH_RIGHT_CORNER	3
#define FSDKP_MOUTH_LEFT_CORNER	4
#define FSDKP_MOUTH_TOP	54
#define FSDKP_MOUTH_TOP_INNER	61
#define FSDKP_MOUTH_BOTTOM	55
#define FSDKP_MOUTH_BOTTOM_INNER	64
#define FSDKP_MOUTH_LEFT_TOP	56
#define FSDKP_MOUTH_LEFT_TOP_INNER	60
#define FSDKP_MOUTH_RIGHT_TOP	57
#define FSDKP_MOUTH_RIGHT_TOP_INNER	62
#define FSDKP_MOUTH_LEFT_BOTTOM	58
#define FSDKP_MOUTH_LEFT_BOTTOM_INNER	63
#define FSDKP_MOUTH_RIGHT_BOTTOM	59
#define FSDKP_MOUTH_RIGHT_BOTTOM_INNER	65
#define FSDKP_NASOLABIAL_FOLD_LEFT_UPPER	50
#define FSDKP_NASOLABIAL_FOLD_LEFT_LOWER	52
#define FSDKP_NASOLABIAL_FOLD_RIGHT_UPPER	51
#define FSDKP_NASOLABIAL_FOLD_RIGHT_LOWER	53
#define FSDKP_CHIN_BOTTOM	11
#define FSDKP_CHIN_LEFT	9
#define FSDKP_CHIN_RIGHT	10
#define FSDKP_FACE_CONTOUR1	7
#define FSDKP_FACE_CONTOUR2	5
#define FSDKP_FACE_CONTOUR12	6
#define FSDKP_FACE_CONTOUR13	8	


// Initialization functions
int _FSDKIMPORT_ FSDK_ActivateLibrary(char * LicenseKey);
int _FSDKIMPORT_ FSDK_GetHardware_ID(char * HardwareID);
int _FSDKIMPORT_ FSDK_GetLicenseInfo(char * LicenseInfo);
int _FSDKIMPORT_ FSDK_SetNumThreads(int Num);
int _FSDKIMPORT_ FSDK_GetNumThreads(int * Num);
int _FSDKIMPORT_ FSDK_Initialize(char * DataFilesPath);
int _FSDKIMPORT_ FSDK_Finalize();

// Face detection functions
int _FSDKIMPORT_ FSDK_DetectEyes(HImage Image, FSDK_Features * FacialFeatures);
int _FSDKIMPORT_ FSDK_DetectEyesInRegion(HImage Image, TFacePosition * FacePosition, FSDK_Features * FacialFeatures);
int _FSDKIMPORT_ FSDK_DetectFace(HImage Image, TFacePosition* FacePosition);
int _FSDKIMPORT_ FSDK_DetectMultipleFaces(HImage Image, int * DetectedCount, TFacePosition * FaceArray, int MaxSize);
int _FSDKIMPORT_ FSDK_DetectFacialFeatures(HImage Image, FSDK_Features * FacialFeatures);
int _FSDKIMPORT_ FSDK_DetectFacialFeaturesInRegion(HImage Image, TFacePosition * FacePosition, FSDK_Features * FacialFeatures);
int _FSDKIMPORT_ FSDK_DetectFacialFeaturesEx(HImage Image, FSDK_Features * FacialFeatures, FSDK_ConfidenceLevels * ConfidenceLevels);
int _FSDKIMPORT_ FSDK_DetectFacialFeaturesInRegionEx(HImage Image, TFacePosition * FacePosition, FSDK_Features * FacialFeatures, FSDK_ConfidenceLevels * ConfidenceLevels);
int _FSDKIMPORT_ FSDK_SetFaceDetectionParameters(bool HandleArbitraryRotations, bool DetermineFaceRotationAngle, int InternalResizeWidth);
int _FSDKIMPORT_ FSDK_SetFaceDetectionThreshold(int Threshold);

// Image manipulation functions
int _FSDKIMPORT_ FSDK_CreateEmptyImage(HImage * Image);
int _FSDKIMPORT_ FSDK_LoadImageFromFile(HImage * Image, char * FileName);
int _FSDKIMPORT_ FSDK_LoadImageFromBuffer(HImage * Image, unsigned char * Buffer, int Width, int Height, int ScanLine, FSDK_IMAGEMODE ImageMode);
int _FSDKIMPORT_ FSDK_LoadImageFromJpegBuffer(HImage * Image, unsigned char * Buffer, unsigned int BufferLength);
int _FSDKIMPORT_ FSDK_LoadImageFromPngBuffer(HImage * Image, unsigned char * Buffer, unsigned int BufferLength);
int _FSDKIMPORT_ FSDK_FreeImage(HImage Image);
int _FSDKIMPORT_ FSDK_SaveImageToFile(HImage Image, char * FileName);

#if defined( _WIN32 ) || defined ( _WIN64 )
int _FSDKIMPORT_ FSDK_LoadImageFromFileW(HImage * Image, wchar_t * FileName);
int _FSDKIMPORT_ FSDK_SaveImageToFileW(HImage Image, wchar_t * FileName);

int _FSDKIMPORT_ FSDK_LoadImageFromHBitmap(HImage * Image, HBITMAP BitmapHandle);
int _FSDKIMPORT_ FSDK_SaveImageToHBitmap(HImage Image, HBITMAP * BitmapHandle);
#endif

int _FSDKIMPORT_ FSDK_GetImageBufferSize(HImage Image, int * BufSize, FSDK_IMAGEMODE ImageMode);
int _FSDKIMPORT_ FSDK_SaveImageToBuffer(HImage Image, unsigned char * Buffer, FSDK_IMAGEMODE ImageMode);
int _FSDKIMPORT_ FSDK_SetJpegCompressionQuality(int Quality);
int _FSDKIMPORT_ FSDK_CopyImage(HImage SourceImage, HImage DestImage);
int _FSDKIMPORT_ FSDK_ResizeImage(HImage SourceImage, double ratio, HImage DestImage);
int _FSDKIMPORT_ FSDK_RotateImage90(HImage SourceImage, int Multiplier, HImage DestImage);
int _FSDKIMPORT_ FSDK_RotateImage(HImage SourceImage, double angle, HImage DestImage);
int _FSDKIMPORT_ FSDK_RotateImageCenter(HImage SourceImage, double angle, double xCenter, double yCenter, HImage DestImage);
int _FSDKIMPORT_ FSDK_CopyRect(HImage SourceImage, int x1, int y1, int x2, int y2, HImage DestImage);
int _FSDKIMPORT_ FSDK_CopyRectReplicateBorder(HImage SourceImage, int x1, int y1, int x2, int y2, HImage DestImage);
int _FSDKIMPORT_ FSDK_MirrorImage(HImage Image, bool UseVerticalMirroringInsteadOfHorizontal);
int _FSDKIMPORT_ FSDK_GetImageWidth(HImage SourceImage, int * Width);
int _FSDKIMPORT_ FSDK_GetImageHeight(HImage SourceImage, int * Height);
int _FSDKIMPORT_ FSDK_ExtractFaceImage(HImage Image, FSDK_Features * FacialFeatures, int Width, int Height, HImage * ExtractedFaceImage, FSDK_Features * ResizedFeatures);
int _FSDKIMPORT_ FSDK_GetImageData(HImage Image, unsigned char ** Data, int * Width, int * Height, int * ScanLine, FSDK_IMAGEMODE * ColorMode);


// Matching
int _FSDKIMPORT_ FSDK_GetFaceTemplate(HImage Image, FSDK_FaceTemplate * FaceTemplate);
int _FSDKIMPORT_ FSDK_GetFaceTemplateInRegion(HImage Image, TFacePosition * FacePosition, FSDK_FaceTemplate * FaceTemplate);
int _FSDKIMPORT_ FSDK_GetFaceTemplateUsingFeatures(HImage Image, FSDK_Features * FacialFeatures, FSDK_FaceTemplate * FaceTemplate);
int _FSDKIMPORT_ FSDK_GetFaceTemplateUsingEyes(HImage Image, FSDK_Features * eyeCoords, FSDK_FaceTemplate * FaceTemplate);
int _FSDKIMPORT_ FSDK_MatchFaces(FSDK_FaceTemplate * FaceTemplate1, FSDK_FaceTemplate * FaceTemplate2, float * Similarity);
int _FSDKIMPORT_ FSDK_GetMatchingThresholdAtFAR(float FARValue, float * Threshold);
int _FSDKIMPORT_ FSDK_GetMatchingThresholdAtFRR(float FRRValue, float * Threshold);
int _FSDKIMPORT_ FSDK_GetDetectedFaceConfidence(int * Confidence);


// Webcam usage
int _FSDKIMPORT_  FSDK_SetHTTPProxy(char * ServerNameOrIPAddress, unsigned short Port, char * UserName, char * Password);

int _FSDKIMPORT_  FSDK_OpenIPVideoCamera(FSDK_VIDEOCOMPRESSIONTYPE CompressionType, char * URL, char * Username, char * Password, int TimeoutSeconds, int * CameraHandle);
int _FSDKIMPORT_  FSDK_CloseVideoCamera(int CameraHandle);
int _FSDKIMPORT_  FSDK_GrabFrame(int CameraHandle, HImage * Image);


int _FSDKIMPORT_  FSDK_InitializeCapturing(void);
int _FSDKIMPORT_  FSDK_FinalizeCapturing(void);

#if defined( _WIN32 ) || defined ( _WIN64 )
typedef struct {
	int Width;
	int Height;
	int BPP;
} FSDK_VideoFormatInfo;

int _FSDKIMPORT_  FSDK_SetCameraNaming(bool UseDevicePathAsName);
int _FSDKIMPORT_  FSDK_GetCameraList(wchar_t *** CameraList, int * CameraCount);
int _FSDKIMPORT_  FSDK_GetCameraListEx(wchar_t *** CameraNameList, wchar_t *** CameraDevicePathList, int * CameraCount);
int _FSDKIMPORT_  FSDK_FreeCameraList(wchar_t ** CameraList, int CameraCount);
int _FSDKIMPORT_  FSDK_GetVideoFormatList(wchar_t * CameraName, FSDK_VideoFormatInfo ** VideoFormatList, int * VideoFormatCount);
int _FSDKIMPORT_  FSDK_FreeVideoFormatList(FSDK_VideoFormatInfo * VideoFormatList);
int _FSDKIMPORT_  FSDK_SetVideoFormat(wchar_t * CameraName, FSDK_VideoFormatInfo VideoFormat);
int _FSDKIMPORT_  FSDK_OpenVideoCamera(wchar_t * CameraName, int * CameraHandle);
#endif


#ifdef __cplusplus
}
#endif


#endif
