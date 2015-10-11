/*
 *  ofxOpticalFlow.h
 *  Created by lukasz karluk on 31/07/10.
 *  http://codeoncanvas.cc
 *
 */

#pragma once

#define OPTICAL_FLOW_DEFAULT_WIDTH		320
#define OPTICAL_FLOW_DEFAULT_HEIGHT		240

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "opencv2/legacy/legacy.hpp"

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
	
	void update(const ofImage & source);
	void update(const ofxCvColorImage & source);
	void update(const ofxCvGrayscaleImage & source);
	void update(const ofVideoPlayer & source);
	void update(const ofVideoGrabber & source);
	void update(const ofPixels & pixels);
    void update(IplImage * previousImage, IplImage * currentImage, int opticalFlowSize=5, int opticalFlowBlur=0);
	
	ofVec3f getVelAtNorm(float x, float y);
	ofVec3f getVelAtPixel(int x, int y);
	
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