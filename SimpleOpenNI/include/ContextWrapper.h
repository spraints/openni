/* ----------------------------------------------------------------------------
 * SimpleOpenNI
 * ----------------------------------------------------------------------------
 * Copyright (C) 2011 Max Rheiner / Interaction Design Zhdk
 *
 * This file is part of SimpleOpenNI.
 *
 * SimpleOpenNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version (subject to the "Classpath" exception
 * as provided in the LICENSE.txt file that accompanied this code).
 *
 * SimpleOpenNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleOpenNI.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------------
 */

#ifndef _CONTEXWRAPPER_H_
#define _CONTEXWRAPPER_H_

#include <vector>

// OpenNI
#include <XnOS.h>
#include <XnCppWrapper.h>

namespace sOpenNI{

#define		MAX_DEPTH	10000	// 10m

typedef XnPoint3D*	XnPoint3DArray;

class ContextWrapper
{
public:
	ContextWrapper();
	~ContextWrapper();

	int version();

	bool init(const char* xmlInitFile);
	bool isInit(){	return _initFlag; }
	void close();

	bool enableDepth();
	bool enableRGB();
	bool enableIR();

	bool enableScene();
	bool enableUser(int flags);


	void update();

	// depth methods
	int depthWidth();
	int	depthHeight();

	int	depthImage(int* map);		// argb 4-Bytes / alpha is not used
	void setDepthImageColor(int r,int g,int b);
	void setDepthImageColorRange(XnRGB24Pixel* colors,int count);

	int depthMapSize();
	int	depthMap(int* map);					// in milimeters
	int depthMapRealWorld(XnPoint3D map[]);	
	XnPoint3DArray depthMapRealWorldA();	

	int depthHistSize();
	int depthHistMap(float* histMap);

	// cam image
	int rgbWidth();
	int rgbHeight();

	int	rgbImage(int* map);			// argb 4-Bytes / alpha is not used

	// ir image
	int irWidth();
	int irHeight();

	int irMap(int* map);			// 16-bit value
	int irImage(int* map);			// argb 4-Bytes / alpha is not used

	// scene analyzer
	int sceneWidth();
	int	sceneHeight();

	int sceneMap(int* map);			
	int sceneImage(int* map);		// 16-bit value, with the labels, size of the depth map
	void getSceneFloor(XnVector3D* point,	
					   XnVector3D* normal);	

	// users
	int		getUsers(std::vector<int>* userList);

	bool	isCalibratedSkeleton(int user);
	void	requestCalibrationSkeleton(int user, bool force);

	bool	isTrackingSkeleton(int user);
	void	startTrackingSkeleton(int user);
	void	stopTrackingSkeleton(int user);

	void	startPoseDetection(const char* pose,int user);
	void	stopPoseDetection(int user);

	bool	getJointPositionSkeleton(int user,int joint,XnSkeletonJointPosition* jointPos);

	// access methods
	void setMirror(bool flag);
	bool mirror();

	// converter methods
	void convertRealWorldToProjective(XnVector3D* world,XnVector3D* proj);	
	void convertRealWorldToProjective(std::vector<XnVector3D>* worldArray,std::vector<XnVector3D>* projArray);

	void convertProjectiveToRealWorld(XnVector3D* proj,XnVector3D* world);	
	void convertProjectiveToRealWorld(std::vector<XnVector3D>* projArray,std::vector<XnVector3D>* worldArray);

	// callbacks
	static void XN_CALLBACK_TYPE newUserCb(xn::UserGenerator& generator, XnUserID user, void* cxt);
	static void XN_CALLBACK_TYPE lostUserCb(xn::UserGenerator& generator, XnUserID user, void* cxt);
	
	static void XN_CALLBACK_TYPE calibrationStartedCb(xn::SkeletonCapability& skeleton, XnUserID user, void* cxt);
	static void XN_CALLBACK_TYPE calibrationEndedCb(xn::SkeletonCapability& skeleton, XnUserID user, XnBool bSuccess, void* cxt);

	static void XN_CALLBACK_TYPE poseStartedCb(xn::PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user, void* cxt);
	static void XN_CALLBACK_TYPE poseEndedCb(xn::PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user, void* cxt);

protected:

	virtual void onNewUserCb(unsigned int userId);
	virtual void onLostUserCb(unsigned int userId);
	
	virtual void onCalibrationStartedCb(unsigned int userId);
	virtual void onCalibrationEndedCb(unsigned int userId,bool successFlag);

	virtual void onPoseStartedCb(const char* strPose, unsigned int user);
	virtual void onPoseEndedCb(const char* strPose, unsigned int user);

	void onNewUserCb(xn::UserGenerator& generator, XnUserID user);
	void onLostUserCb(xn::UserGenerator& generator, XnUserID user);
	
	void onCalibrationStartedCb(xn::SkeletonCapability& skeleton, XnUserID user);
	void onCalibrationEndedCb(xn::SkeletonCapability& skeleton, XnUserID user, XnBool bSuccess);

	void onPoseStartedCb(xn::PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user);
	void onPoseEndedCb(xn::PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user);
	

	void calcHistogram();
	void createDepthImage();
	void calcDepthImageRealWorld();
	void createIrImage();
	void calcSceneData();

	bool _initFlag;
	bool _generatingFlag;
	
	XnStatus			_rc;
	xn::Context			_context;

	xn::DepthGenerator	_depth;
	xn::DepthMetaData	_depthMD;
	float				_pDepthHist[MAX_DEPTH];
	XnRGB24Pixel*		_pDepthImage;
	int					_depthBufSize;
	float				_depthImageColor[3];
	XnPoint3D*			_depthMapRealWorld;

	xn::ImageGenerator	_image;
	xn::ImageMetaData	_imageMD;
	int					_rgbBufSize;

	xn::IRGenerator		_ir;
	xn::IRMetaData		_irMD;
	XnRGB24Pixel*		_pIrImage;
	int					_irBufSize;

	xn::SceneAnalyzer	_sceneAnalyzer;
	xn::SceneMetaData	_sceneMD;
	XnRGB24Pixel*		_pSceneImage;
	int					_sceneBufSize;

	xn::UserGenerator	_userGenerator;
	XnCallbackHandle	_hUserCB;
	XnCallbackHandle	_hCalibrationCB;
	XnCallbackHandle	_hPoseCB;

};

};

#endif // _CONTEXWRAPPER_H_