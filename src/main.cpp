/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  Main function to run OrthoRegister
 *
 *        Version:  1.0
 *        Created:  01/21/2013 10:38:32 AM
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
#include	<ctime>

#include	<cv.h>
#include	<highgui.h>
#include	<limits>

#include	"points.h"
#include	"ortho.h"
#include	"extractor.h"
#include	"IO.h"

using namespace std;

cv::Mat readCamMtrix(string camFile){
	//Declare varibles
	double p00,p01,p02,p03,p10,p11,p12,p13,p20,p21,p22,p23;
	string header;
	string camPath = camFile;
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

int main ( int argc, char *argv[] ){ 

	clock_t init,final;
	init=clock();

	string camPath;
	string mapImLoc;
	string hyperImLoc;
	string dataType;
	string plyF;

	//Extract options from file
	fstream opt(argv[1],ios::in);
		opt >> camPath 
		    >> mapImLoc 
		    >> hyperImLoc 
		    >> dataType
		    >> plyF;
	opt.close();

	
	MapOrtho map( dataType, mapImLoc );
	map.readImage();
	HyperOrtho hyper( dataType, hyperImLoc);
	hyper.readImage();
	Points pts( plyF );
	
	//Read camera info
	cv::Mat cam = readCamMtrix(camPath);
	//Extract Ortho Data
	cout << "[- Extracting " << dataType << " Data -]" << endl;
	Extractor extract;
	extract.extractValues(map, hyper, pts, cam);

	vector< vector<float> > values = extract.getValues();
	vector<vector<float> >::iterator it;

	for(it=values.begin(); it != values.end(); ++it){
		for(unsigned int i=0; i < it->size(); i++){
			cout << (*it)[i] << " ";
		}
		cout << endl;
	}

	final=clock()-init;
	double seconds = (double)final / (double)CLOCKS_PER_SEC;
	cout << "[- Done  ( "<<seconds<<" seconds ) -]" << endl;
	
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
