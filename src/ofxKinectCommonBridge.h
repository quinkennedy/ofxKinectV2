#pragma once

#include "ofMain.h"

#include "KCBv2LIB.h"
#pragma comment (lib, "KCBv2.lib") // add path to lib additional dependency dir $(TargetDir)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// not sure this is right
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Kv2Joint
{
  public:
	Kv2Joint(){}
	Kv2Joint(const _Joint& kcbPosition, const _JointOrientation& kcbOrientation)
	{
		jointOrientation.set(kcbOrientation.Orientation.x, kcbOrientation.Orientation.y, kcbOrientation.Orientation.z, kcbOrientation.Orientation.w);
		jointPosition.set(kcbPosition.Position.X, kcbPosition.Position.Y, kcbPosition.Position.Z);
		type = kcbPosition.JointType;
	}

	ofVec3f getPosition()
	{
		return jointPosition;
	}

	ofQuaternion getOrientation()
	{
		return jointOrientation;
	}

	TrackingState getTrackingState()
	{
		return trackingState;
	}

  protected:
	ofVec3f jointPosition;
	ofQuaternion jointOrientation;
	JointType type;
	TrackingState trackingState;
};

class Kv2Skeleton
{
  public:
	bool tracked;
	map<JointType, Kv2Joint> joints;
};

//partially duplicated from ColorImageFormat as defined in Kinect.h
typedef enum _KCBColorImageFormat KCBColorImageFormat;
enum _KCBColorImageFormat
{
    KCBColorImageFormat_Rgba	= 1,
    KCBColorImageFormat_Bgra	= 3,
    KCBColorImageFormat_Yuy2	= 5
} ;

class ofxKinectCommonBridge : protected ofThread {
  public:
	
	ofxKinectCommonBridge();
	~ofxKinectCommonBridge();

	// new API
	bool initSensor( int id = 0 );
	bool initDepthStream( bool mapDepthToColor = false );
	bool initIRStream();
	bool initColorStream(bool mapColorToDepth = false, KCBColorImageFormat format = KCBColorImageFormat_Rgba);
	bool initSkeletonStream( bool seated );
	bool start();

	void stop();

  	/// is the current frame new?
	bool isFrameNew();
	bool isFrameNewVideo();
	bool isFrameNewDepth();
	bool isNewSkeleton();
	bool initBodyIndexStream();

	void setDepthClipping(float nearClip=500, float farClip=4000);
	
	/// updates the pixel buffers and textures
	///
	/// make sure to call this to update to the latest incoming frames
	void update();
	ofPixels& getColorPixelsRef();
	ofPixels & getDepthPixelsRef();       	///< grayscale values
	ofShortPixels & getRawDepthPixelsRef();	///< raw 11 bit values

	/// enable/disable frame loading into textures on update()
	void setUseTexture(bool bUse);
	/// In the programmable renderer, this will switch the raw texture over to using GL_R32F as opposed to GL_LUMINANCE16UI_EXT
	/// This was because I was experiencing issues on some cards and the unsigned int extensions
	void setRawTextureUsesFloats(bool bUseRawFloat);

	/// draw the video texture
	void draw(float x, float y, float w, float h);
	void draw(float x, float y);
	void draw(const ofPoint& point);
	void draw(const ofRectangle& rect);

	/// draw the grayscale depth texture
	void drawRawDepth(float x, float y, float w, float h);
	void drawRawDepth(float x, float y);
	void drawRawDepth(const ofPoint& point);
	void drawRawDepth(const ofRectangle& rect);

	/// draw the grayscale depth texture
	void drawDepth(float x, float y, float w, float h);
	void drawDepth(float x, float y);
	void drawDepth(const ofPoint& point);
	void drawDepth(const ofRectangle& rect);

	void drawIR( float x, float y, float w, float h );

	void drawBodyIndex(float x, float y);

	//vector<Skeleton> &getSkeletons();
	void drawSkeleton(int index);
	void drawSkeleton(int index, ofVec2f scale);

	void drawAllSkeletons();
	void drawAllSkeletons(ofVec2f scale);

	ofVec3f mapDepthToSkeleton(ofPoint depthPoint);
	ofVec3f mapDepthToSkeleton(ofPoint depthPoint, ofShortPixels& depthImage);
	vector<ofVec3f> mapDepthToSkeleton(vector<ofPoint>& depthPoints);
	vector<ofVec3f> mapDepthToSkeleton(vector<ofPoint>& depthPoints, ofShortPixels& depthImage);
	
	/*
	ofVec2f mapColorToDepth(ofPoint colorPoint);
	ofVec2f mapColorToDepth(ofPoint colorPoint, ofShortPixels& depthImage);
	vector<ofVec2f> mapColorToDepth(vector<ofPoint>& colorPoints);
	vector<ofVec2f> mapColorToDepth(vector<ofPoint>& colorPoints, ofShortPixels& depthImage);
	
	ofVec3f mapColorToSkeleton(ofPoint colorPoint);
	ofVec3f mapColorToSkeleton(ofPoint colorPoint, ofShortPixels& depthImage);
	vector<ofVec3f> mapColorToSkeleton(vector<ofPoint>& colorPoints);
	vector<ofVec3f> mapColorToSkeleton(vector<ofPoint>& colorPoints, ofShortPixels& depthImage);
	*/

	//will either be GL_R32F or GL_LUMINANCE16UI_EXT depending on texture setting
	ofTexture &getRawDepthTexture() {
		return rawDepthTex;
	}
	ofTexture &getDepthTexture() {
		return depthTex;
	}
	ofTexture &getColorTexture() {
		return videoTex;
	}

  protected:

    KCBHANDLE hKinect;
	ColorImageFormat colorFormat;

  	bool bInited;
	bool bStarted;
	vector<Kv2Skeleton> skeletons;
	vector<Kv2Skeleton> backSkeletons;

	//quantize depth buffer to 8 bit range
	vector<unsigned char> depthLookupTable;
	void updateDepthLookupTable();
	void updateDepthPixels();
	void updateIRPixels();
	bool bNearWhite;
	float nearClipping, farClipping;

  	bool bUseTexture;
	bool bUseFloatTexture;

	ofTexture depthTex; ///< the depth texture
	ofTexture rawDepthTex; ///<
	ofTexture videoTex; ///< the RGB texture
	ofTexture bodyIndexTex;

	ofPixels videoPixels;
	ofPixels videoPixelsBack;			///< rgb back
	ofPixels depthPixels;
	ofPixels depthPixelsBack;
	ofShortPixels depthPixelsRaw;
	ofShortPixels depthPixelsRawBack;	///< depth back
	ofShortPixels depthPixelsRawFront;
	ofFloatPixels depthPixelsNormalized;
	
	ofShortPixels irPixelsRaw;
	ofShortPixels irPixelsBackRaw;
	ofPixels irPixels;
	ofPixels irPixelsBack;

	ofPixels bodyIndexPixelsBack;
	ofPixels bodyIndexPixels;

	bool bIsFrameNewVideo;
	bool bNeedsUpdateVideo;
	bool bIsFrameNewDepth;
	bool bNeedsUpdateDepth;
	bool bNeedsUpdateSkeleton;
	bool bIsSkeletonFrameNew;
	bool bProgrammableRenderer;

	bool bNeedsUpdateBodyIndex;

	bool bVideoIsColor;
	bool bVideoIsInfrared;
	bool bUsingSkeletons;
	bool bUsingDepth;
	bool bUsingBodyIndex;

	BYTE *irPixelByteArray;

	void threadedFunction();

	bool mappingColorToDepth;
	bool mappingDepthToColor;
	bool beginMappingColorToDepth;

	KCBDepthFrame *pDepthFrame, *pDepthFrameBack;
	KCBColorFrame *pColorFrame, *pColorFrameBack;
	KCBInfraredFrame *pInfraredFrame, *pInfraredFrameBack;
	//KCBBodyFrame* pBodyFrame; // not using this yet

	JointOrientation jointOrients[JointType_Count];
	Joint joints[JointType_Count];

	KCBBodyIndexFrame *pBodyIndexFrame, *pBodyIndexFrameBack;

	KCBFrameDescription colorFrameDescription;
	KCBFrameDescription depthFrameDescription;
	KCBFrameDescription irFrameDescription;
	KCBFrameDescription bodyIndexFrameDescription;

	pair<JointType, JointType> skeletonDrawOrder[JointType_Count];

	void checkOpenGLError(string function);
	int getGLFormat(KCBColorImageFormat format);
	int getGLFormat(ColorImageFormat format);
	ColorImageFormat getColorImageFormat(KCBColorImageFormat format);

};
