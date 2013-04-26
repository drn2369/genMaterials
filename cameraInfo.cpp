/*
 * =====================================================================================
 *
 *       Filename:  cameraInfo.cpp
 *
 *    Description:  Implimentation of cameraInfo class
 *
 *        Version:  1.0
 *        Created:  01/21/2013 12:41:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#include	<fstream>

#include	"cameraInfo.h"

cameraInfo::cameraInfo(string camDir){
	setCamDir(camDir);
	genCamInfo();
}

//Format of idx file is:
// nCams
// baseNumber pmvsCameraFile indexNumber
void cameraInfo::genCamInfo(){
	//Declare varibles
	int nCams;
	string idxFile = "listInds.txt";
	string idxPath = camDirectory+idxFile;

	//Open index file
	fstream idx(idxPath.c_str(),ios::in);

	idx >> nCams;
	for(int i=0;i<nCams;i++){
		string baseN;
		string camF;
		int index;
		// Read in line
		idx >> baseN >> camF >> index;

		// Extract projection matrix
		cv::Mat projM = readCamMatrix(camF);
	
		// Store in map
		camInfo[index] = make_pair(baseN,projM);
	}
	idx.close();
}

cv::Mat cameraInfo::readCamMatrix(string camFile){
	//Declare varibles
	double p00,p01,p02,p03,p10,p11,p12,p13,p20,p21,p22,p23;
	string header;
	string camPath = camDirectory+camFile;
	//Read in from pmvs camera file
	fstream cam(camPath.c_str(),ios::in);

	cam >> header >> p00 >> p01 >> p02 >> p03 >> p10 >> p11 
	    >> p12 >> p13 >> p20 >> p21 >> p22 >> p23;
	
	cam.close();
	//Set up Projection matrix and return
	cv::Mat proj = (cv::Mat_<double>(3,4) << p00, p01, p02, p03,
					p10, p11, p12, p13,
					p20, p21, p22, p23);
	
	return proj;
}
