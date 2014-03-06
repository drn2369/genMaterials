/*
 * =====================================================================================
 *
 *       Filename:  patch.h
 *
 *    Description:  Class definitions for a patch file. Patches are extracted from 
 *    			PMVS output
 *
 *        Version:  1.0
 *        Created:  01/21/2013 01:20:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#include	<string>
#include	<vector>
#include	<utility>

#include	<cv.h>
#include	"cameraInfo.h"

using namespace std;

/*
 * =====================================================================================
 *        Class:  Patch
 *  Description:  Class definitions for a patch file
 * =====================================================================================
 */
#ifndef PATCH_H
#define PATCH_H

class Patch{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		Patch (cameraInfo *cInfo,string patchDir,string patchF );                             /* constructor */

		/* ====================  ACCESSORS     ======================================= */
		int getNumPoints(){ return worldPoints.size();}
		vector<string> getImageNums();
		
		vector< cv::Point > getImagePoint(int idx){return imagePoints[idx];}
		vector<int> getImageInds(int idx){return imageInds[idx];}
		cv::Mat getWorldPoint(int idx){return worldPoints[idx];}

		string getPatchFile(){return patchFile;}
		/* ====================  MUTATORS      ======================================= */
		void setCamInfo(cameraInfo *cInfo){camInfo = cInfo;}
		void setPatchDir(string pDir){patchDirectory = pDir;}
		void setPatch(string pF){patchFile = pF;}
		/* ====================  OPERATORS     ======================================= */
		void readTImages();
		void readPatch();

		/* ====================  DATA MEMBERS  ======================================= */
	protected:

	private:
		string patchFile;
		string patchDirectory;
		cameraInfo *camInfo;
		vector< cv::Mat > worldPoints;
		vector< vector< cv::Point > > imagePoints;
		vector< vector<int> > imageInds;
		vector<int> tImages;

}; /* -----  end of class Patch  ----- */
#endif
