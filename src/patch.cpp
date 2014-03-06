/*
 * =====================================================================================
 *
 *       Filename:  patch.cpp
 *
 *    Description:  Class implimentation for patch class
 *
 *        Version:  1.0
 *        Created:  01/21/2013 01:54:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#include	<iostream>
#include	<fstream>
#include	<sstream>

#include	"patch.h"

Patch::Patch(cameraInfo *cInfo, string patchDir, string patchF){
	//Store data members
	setCamInfo(cInfo);
	setPatchDir(patchDir);
	setPatch(patchF);

	//Read in tImage data
	readTImages();
	//Read patch data
	readPatch();
}

void Patch::readTImages(){
	
	//Open tImages file
	string tPath = patchDirectory + "tImages.txt";
       	fstream tIm(tPath.c_str(),ios::in);

	while(tIm.good()){
		string patchTemp;
		int nCams;
		vector<int> camIndex;

		//Read in patch and values
		tIm >> patchTemp >> nCams;
		  //Read in each index
		  for(int i=0; i<nCams; i++){
			int index;
			tIm >> index;
			camIndex.push_back(index);
		  }
		//Store if correct and exit loop
		if( patchFile.compare(patchTemp) == 0){
			tImages = camIndex;
			break;
		}
	}
	tIm.close();

}

vector<string> Patch::getImageNums(){
	vector<string> imList;
	for(unsigned int i =0 ; i < tImages.size(); ++i){
		//Get image file name base (number)
		string base = camInfo->getBase(tImages[i]);
		imList.push_back(base);
	}
	return imList;
}

void Patch::readPatch(){
	//Declarations
	string patchStr;
	double t;

	int numPatch;
	string patchPath = patchDirectory + patchFile + ".patch";
	
	//Open patch file
	fstream patch(patchPath.c_str(),ios::in);

	//Read in number of patches
	patch >> patchStr >> numPatch;

	  //Big Ass loop to read it all in
	 for(int i=0;i<numPatch;i++){
	  //Declarations
	  int nCams;
	  double x,y,z,h;
	  vector<cv::Point> imPoints;
	  vector<int> imInds;

	  //Toss header
	  patch >> patchStr;
	  //Read Point
	  patch >> x >> y >> z >> h;

	  //Store as a 4x1 vec
	  cv::Mat wpVec = (cv::Mat_<double>(4,1) << x,y,z,h);
	   worldPoints.push_back(wpVec);
	 
	  //Toss next seven
	  patch >> t >> t >> t >> t >> t >> t >> t;

	  //Next is number of cameras
	  patch >> nCams;
	  //Loop through nCams 
	  	for(int j=0;j<nCams;j++){
		//	cout << j << endl;
			//Declarations
			int camIdx;
			//Get index
			patch >> camIdx;
			//Store index
			imInds.push_back(camIdx);
			//Get Projection matrix and project point
			cv::Mat P = camInfo->getCam(tImages[camIdx]);
			cv::Mat imageProj = P*wpVec;
			//Dehomogeniousize (spelling ftw)
			double x = imageProj.at<double>(0,0)/imageProj.at<double>(2,0);
			double y = imageProj.at<double>(1,0)/imageProj.at<double>(2,0);

			//Store image point
			cv::Point2d imP(x,y);
			imPoints.push_back(imP);
		}
           //Store point and index vectors
	   imagePoints.push_back(imPoints);
	   imageInds.push_back(imInds);

	   //Next number is number of pooly correleated images
	   int poor;
	   patch >> poor;
	   //If greater than zero, then read the extras and toss them
	   if(poor > 0){
		for(int j=0;j<poor;j++){
		   patch >> t;
		}
	   }
	 }
	patch.close();

}

