/*
 * =====================================================================================
 *
 *       Filename:  cameraInfo.h
 *
 *    Description:  Class definition for cameraInfo class. This reads in all the camera 
 *    			data and indicies required for reprojection
 *
 *        Version:  1.0
 *        Created:  01/21/2013 12:23:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#ifndef		CAMERAINFO_H
#define		CAMERAINFO_H

#include	<map>
#include	<string>
#include	<utility>

#include	<cv.h>

using namespace std;
/*
 * =====================================================================================
 *        Class:  cameraInfo
 *  Description:  Contains camera information 
 * =====================================================================================
 */
class cameraInfo{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		cameraInfo (string camDir);                             /* constructor */
		/* ====================  ACCESSORS     ======================================= */
		string getBase(int idx){ return camInfo[idx].first;}
		cv::Mat getCam(int idx){ return camInfo[idx].second;}
		/* ====================  MUTATORS      ======================================= */
		void setCamDir(string cDir){camDirectory = cDir;}
		/* ====================  OPERATORS     ======================================= */
		void genCamInfo();
		cv::Mat readCamMatrix(string camFile);
		/* ====================  DATA MEMBERS  ======================================= */
	protected:

	private:
	string	camDirectory;
	map<int, pair<string,cv::Mat> > camInfo;

}; /* -----  end of class CameraInfo  ----- */

#endif
