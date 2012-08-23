/*
 *  ofxOpticalFlow.h
 *  Created by lukasz karluk on 31/07/10.
 *
 */

#pragma once

#define OPTICAL_FLOW_DEFAULT_WIDTH		320
#define OPTICAL_FLOW_DEFAULT_HEIGHT		240

#include "ofMain.h"
#include "ofxOpenCv.h"

class ofxOpticalFlowLK {
	
public :
	
	 ofxOpticalFlowLK();
	~ofxOpticalFlowLK();
	
	void setup(const ofRectangle& size);
	void setup(int width = OPTICAL_FLOW_DEFAULT_WIDTH, int height = OPTICAL_FLOW_DEFAULT_HEIGHT);
    
    void setOpticalFlowSize(int value);
    void setOpticalFlowBlur(int value);
	
	void reset();
	void destroy();
	
	void update(ofImage& source);
	void update(ofxCvColorImage& source);
	void update(ofxCvGrayscaleImage& source);
	void update(ofVideoPlayer& source);
	void update(ofVideoGrabber& source);
	void update(unsigned char* pixels, int width, int height, int imageType);
    void update(IplImage * previousImage, IplImage * currentImage, int opticalFlowSize=5, int opticalFlowBlur=0);
	
	ofPoint getVelAtNorm(float x, float y);
	ofPoint getVelAtPixel(int x, int y);
	
	void draw(int width = OPTICAL_FLOW_DEFAULT_WIDTH, int height = OPTICAL_FLOW_DEFAULT_HEIGHT, float lineScale = 10, int res = 6);
	
	void setMirror(bool mirrorHorizontally = false, bool mirrorVertically = false);
	
	bool bInitialised;
	ofRectangle sizeSml;
	ofRectangle sizeLrg;
	
	ofxCvColorImage colrImgLrg;		// full scale color image.
	ofxCvColorImage	colrImgSml;		// full scale color image.
	ofxCvGrayscaleImage	greyImgLrg;	// full scale grey image.
	ofxCvGrayscaleImage	greyImgSml;	// scaled down grey image.
	ofxCvGrayscaleImage	greyImgPrv;	// scaled down grey image - copy of previous frame.
	IplImage * opFlowVelX;          // optical flow in the x direction.
	IplImage * opFlowVelY;          // optical flow in the y direction.
	
	bool bMirrorH;
	bool bMirrorV;
	
	int opticalFlowSize;
	int opticalFlowBlur;
	float opticalFlowMin;
	float opticalFlowMax;
};