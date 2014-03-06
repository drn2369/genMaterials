/*
 * =====================================================================================
 *
 *       Filename:  points.cpp
 *
 *    Description:  Implimentation
 *
 *        Version:  1.0
 *        Created:  04/26/2013 01:35:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#include	"IO.h"
#include	"points.h"

Points::Points(string plyF){
	setPlyF(plyF);
	readPLYFile();
}

void Points::readPLYFile(){
	PLYML_Reader reader(plyFile.c_str());
	xyz = *reader.getPointCloud();
	norm = *reader.getPointCloudN();
	rgb = *reader.getPointCloudC();
}

vector< cv::Point2d > Points::projectPoints( cv::Mat cam){

	//Point Vector
	vector< cv::Point2d > imPts;

	//Vector iterator
	vector< cv::Mat >::iterator it;
	for(it = xyz.begin(); it != xyz.end(); ++it){
		cv::Mat pointH = (cv::Mat_<double>(4,1) << it->at<double>(0,0),
							   it->at<double>(1,0),
							   it->at<double>(2,0),
							   1.0);
	
		cv::Mat imPtH = cam*pointH;
		double x = imPtH.at<double>(0,0)/imPtH.at<double>(2,0);
		double y = imPtH.at<double>(1,0)/imPtH.at<double>(2,0);

		cv::Point2d pt(x,y);
		imPts.push_back(pt);
	}
	return imPts;

}
