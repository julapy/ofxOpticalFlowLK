/*
 *  ofxOpticalFlow.cpp
 *  Created by lukasz karluk on 31/07/10.
 *  http://codeoncanvas.cc
 *
 */

#include "ofxOpticalFlowLK.h"

ofxOpticalFlowLK::ofxOpticalFlowLK() {
	opticalFlowSize	= 5;
	opticalFlowBlur = 7;
	opticalFlowMin	= 0;
	opticalFlowMax	= 0;
    
    opFlowVelX = NULL;
    opFlowVelY = NULL;
    
    bMirrorH = false;
    bMirrorV = false;
    
    cvInterpolationMethod = CV_INTER_NN;
    
    bInitialised = false;
}

ofxOpticalFlowLK::~ofxOpticalFlowLK() {
	destroy();
}

///////////////////////////////////////////
//	SETUP.
///////////////////////////////////////////

void ofxOpticalFlowLK::setup(const ofRectangle& size) {
	setup(size.width, size.height);
}

void ofxOpticalFlowLK::setup(int width, int height) {
	sizeSml.width = width;
	sizeSml.height = height;
	
	sizeLrg.width = width;		// assume input size is the same as optical flow size.
	sizeLrg.height = height;
	
	if(bInitialised) {
		destroy();
    }
	
	colrImgLrg.allocate(sizeLrg.width, sizeLrg.height);
	colrImgSml.allocate(sizeSml.width, sizeSml.height);
	greyImgLrg.allocate(sizeLrg.width, sizeLrg.height);
	greyImgSml.allocate(sizeSml.width, sizeSml.height);
	greyImgPrv.allocate(sizeSml.width, sizeSml.height);
	
	opFlowVelX = cvCreateImage(cvSize(sizeSml.width, sizeSml.height), IPL_DEPTH_32F, 1);
	opFlowVelY = cvCreateImage(cvSize(sizeSml.width, sizeSml.height), IPL_DEPTH_32F, 1);
	
	reset();
	
	bInitialised = true;
}

void ofxOpticalFlowLK::setOpticalFlowSize(int value) {
    opticalFlowSize = value;
}

void ofxOpticalFlowLK::setOpticalFlowBlur(int value) {
    opticalFlowBlur = value;
}

void ofxOpticalFlowLK::setCvInterpolationMethod(int value) {
    cvInterpolationMethod = value;
}

void ofxOpticalFlowLK::reset() {
	colrImgLrg.set(0);
	colrImgSml.set(0);
	greyImgLrg.set(0);
	greyImgSml.set(0);
	greyImgPrv.set(0);

    cvSetZero(opFlowVelX);
    cvSetZero(opFlowVelY);
}

void ofxOpticalFlowLK::destroy() {
	colrImgLrg.clear();
	colrImgSml.clear();
	greyImgLrg.clear();
	greyImgSml.clear();
	greyImgPrv.clear();
	
    if(opFlowVelX) {
        cvReleaseImage(&opFlowVelX);
    }
    if(opFlowVelY) {
        cvReleaseImage(&opFlowVelY);
    }
}

///////////////////////////////////////////
//	UPDATE.
///////////////////////////////////////////

void ofxOpticalFlowLK::update(const ofImage & source) {
	update(source.getPixels());
}

void ofxOpticalFlowLK::update(const ofxCvColorImage & source) {
	update(source.getPixels());
}

void ofxOpticalFlowLK::update(const ofxCvGrayscaleImage & source) {
	update(source.getPixels());
}

void ofxOpticalFlowLK::update(const ofVideoPlayer & source) {
	update(source.getPixels());	// assume colour image type.
}

void ofxOpticalFlowLK::update(const ofVideoGrabber & source) {
	update(source.getPixels());	// assume colour image type.
}

void ofxOpticalFlowLK::update(const ofPixels & pixels) {

    int pixelsW = pixels.getWidth();
    int pixelsH = pixels.getHeight();
    int pixelsType = pixels.getImageType();
	bool rightSize = (sizeSml.width == pixelsW && sizeSml.height == pixelsH);
	
	//-- making the input the right size for optical flow to work with.
	
	if(rightSize == true) {
		if(pixelsType == OF_IMAGE_COLOR) {
			colrImgSml.setFromPixels(pixels.getData(), sizeSml.width, sizeSml.height);
			greyImgSml.setFromColorImage(colrImgSml);
		} else if(pixelsType == OF_IMAGE_GRAYSCALE) {
			greyImgSml.setFromPixels(pixels.getData(), sizeSml.width, sizeSml.height);
		} else {
			return;		// wrong image type.
		}
	} else {

		bool sizeLrgChanged = (sizeLrg.width != pixelsW || sizeLrg.height != pixelsH);
		
		if(sizeLrgChanged) {		// size of input has changed since last update.
			sizeLrg.width = pixelsW;
			sizeLrg.height = pixelsH;
			
			colrImgLrg.clear();
			greyImgLrg.clear();
			
			colrImgLrg.allocate(sizeLrg.width, sizeLrg.height);
			greyImgLrg.allocate(sizeLrg.width, sizeLrg.height);
			
			colrImgLrg.set(0);
			greyImgLrg.set(0);
		}
		
		if(pixelsType == OF_IMAGE_COLOR) {
			colrImgLrg.setFromPixels(pixels);
			colrImgSml.scaleIntoMe(colrImgLrg, cvInterpolationMethod);
			greyImgSml.setFromColorImage(colrImgSml);
		} else if(pixelsType == OF_IMAGE_GRAYSCALE) {
			greyImgLrg.setFromPixels(pixels);
			greyImgSml.scaleIntoMe(greyImgLrg, cvInterpolationMethod);
		} else {
			return;		// wrong image type.
		}
	}
	
	if(bMirrorH || bMirrorV) {
		greyImgSml.mirror(bMirrorV, bMirrorH);
    }
	
    update(greyImgPrv.getCvImage(), greyImgSml.getCvImage(), opticalFlowSize, opticalFlowBlur);
	
	greyImgPrv = greyImgSml;
}

void ofxOpticalFlowLK::update(IplImage * previousImage, IplImage * currentImage, int opticalFlowSize, int opticalFlowBlur) {
    
    if((previousImage->width != currentImage->width) || (previousImage->height != currentImage->height)) {
        return; // images do not match.
    }
    
    int w = currentImage->width;
    int h = currentImage->height;

    if(opFlowVelX) {
        if((opFlowVelX->width != w) || (opFlowVelX->height != h)) {
            cvReleaseImage(&opFlowVelX);
            opFlowVelX = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1);
        }
    } else {
        opFlowVelX = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1);
    }
    
    if(opFlowVelY) {
        if((opFlowVelY->width != w) || (opFlowVelY->height != h)) {
            cvReleaseImage(&opFlowVelY);
            opFlowVelY = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1);
        }
    } else {
        opFlowVelY = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1);
    }
    
    int opFlowSize = opticalFlowSize;	// value must be 1, 3, 5, 7... etc.
    opFlowSize /= 2;
    opFlowSize *= 2;
    opFlowSize += 1;
	
	cvCalcOpticalFlowLK(previousImage, currentImage, cvSize(opFlowSize, opFlowSize), opFlowVelX, opFlowVelY);
	
	int opFlowBlur = opticalFlowBlur;	// value must be 1, 3, 5, 7... etc.
    opFlowBlur /= 2;
	opFlowBlur *= 2;
	opFlowBlur += 1;
	
	cvSmooth(opFlowVelX, opFlowVelX, CV_BLUR, opFlowBlur);
	cvSmooth(opFlowVelY, opFlowVelY, CV_BLUR, opFlowBlur);
}

///////////////////////////////////////////
//	OP.FLOW VELOCITY GETTERS.
///////////////////////////////////////////

ofVec3f ofxOpticalFlowLK::getVelAtNorm(float x, float y) {
	int px = x * (opFlowVelX->width - 1);
	int py = y * (opFlowVelX->height - 1);
    return getVelAtPixel(px, py);
}

ofVec3f ofxOpticalFlowLK::getVelAtPixel(int x, int y) {
	x = ofClamp(x, 0, opFlowVelX->width - 1);
	y = ofClamp(y, 0, opFlowVelX->height - 1);
	
	ofVec3f p;
	p.x = cvGetReal2D(opFlowVelX, y, x);
	p.y = cvGetReal2D(opFlowVelY, y, x);

	return p;
}

///////////////////////////////////////////
//	DRAW.
///////////////////////////////////////////

void ofxOpticalFlowLK::draw(int width, int height,  float lineScale, int res) {
	bool rightSize = (sizeSml.width == width && sizeSml.height == height);
	
	ofVec3f vel;
	
	for(int x=0; x<width; x+=res) {
		for(int y=0; y<height; y+=res) {
			if(rightSize) {
				vel = getVelAtPixel(x, y);
            } else {
				vel = getVelAtNorm(x / (float)width, y / (float)height);
            }
			
			if(vel.length() < 1) {  // smaller then 1 pixel, no point drawing.
				continue;
            }
			
			ofDrawLine(x, y, x + vel.x * lineScale, y + vel.y * lineScale);
		}
	}
}

///////////////////////////////////////////
//	CONFIG.
///////////////////////////////////////////

void ofxOpticalFlowLK::setMirror(bool mirrorHorizontally, bool mirrorVertically) {
	bMirrorH = mirrorHorizontally;
	bMirrorV = mirrorVertically;
}
