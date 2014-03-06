/*
 * =====================================================================================
 *
 *       Filename:  extractor.cpp
 *
 *    Description:  Class implimentation for extractior
 *
 *        Version:  1.0
 *        Created:  01/22/2013 08:10:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#include	<iostream>
#include	<sstream>
#include	<fstream>

#include	"extractor.h"
#include	"points.h"

Extractor::Extractor(){}

Extractor::Extractor(Patch *ptch, string dataType, string combType, string baseImLoc){
	setPatch(ptch);
	setDataType(dataType);
	setCombType(combType);
	setBaseImLoc(baseImLoc);
}

Extractor::~Extractor(){

	for(unsigned int i=0; i < maps.size();i++){
		delete maps[i];
	}
	for(unsigned int i=0;i<orthos.size();i++){
		delete orthos[i];
	}
}

void Extractor::clear(){
	for(unsigned int i=0; i < maps.size();i++){
		delete maps[i];
	}
	for(unsigned int i=0;i<orthos.size();i++){
		delete orthos[i];
	}
}

void Extractor::init(){
	cout << "	[- Loading Images -]" << endl;
	loadMaps();
	loadOrthos();
	cout << "	[- Extracting Ortho Values -]" << endl;
	extractValues();
	cout << "	[- Writing Data -" << endl;
	writeData();
}

void Extractor::writeData(){

	cout << "	[- Combining Values -] " << endl;
	vector<float> data = combineValues();
	
	cout << "	[- Writing Data -] " << endl;
	writeDataFile(data);

	cout << "	[- Writing photoscaled PLY -] " << endl;
	writePLY(data);
}

void Extractor::writeDataFile(vector<float> data){
	
	string patchFile = patchP->getPatchFile();
	string dataFile = patchFile +"-"+dataType+".dat";

	fstream dataOut(dataFile.c_str(), ios::out);
	dataOut.precision(8);
	for(unsigned int i=0; i<data.size();i++){

		dataOut << data[i] << "\n";
	}

	dataOut.close();

}

void Extractor::writePLY(vector<float> data){

	string patchFile = patchP->getPatchFile();
	string plyFile = patchFile +"-"+dataType+".ply";

	//Arbirarty scaling for this dataset, needs fixed
	if(dataType.compare("MWIR") == 0){
		for(unsigned int i=0; i<data.size();i++){
			if( data[i] < 2.3){
			   data[i] = 2.3;
			}else if(data[i] > 4.25){
			   data[i] = 4.25;
			}
			data[i] = ((data[i]-2.3)/1.95)*255;
		}
	}else if(dataType.compare("LWIR") == 0){
		for(unsigned int i=0; i<data.size();i++){
			if( data[i] < 6.2){
			   data[i] = 6.2;
			}else if(data[i] > 7.75){
			   data[i] = 7.75;
			}
			data[i] = ((data[i]-6.2)/1.55)*255;
		}	
	}


	fstream plyOut(plyFile.c_str(), ios::out);


	//Write header out
	plyOut << "ply\n";
	plyOut << "format ascii 1.0\n";
	plyOut << "element vertex " << data.size() <<"\n";
	plyOut << "property float x\n";
	plyOut << "property float y\n";
	plyOut << "property float z\n";
	plyOut << "property uchar red\n";
	plyOut << "property uchar green\n";
	plyOut << "property uchar blue\n";
	plyOut << "property uchar alpha\n";
	plyOut << "end_header\n";

		for(unsigned int i=0; i<data.size();i++){
			cv::Mat wP = patchP->getWorldPoint(i);
			plyOut << wP.at<double>(0,0) << " ";
			plyOut << wP.at<double>(1,0) << " ";
			plyOut << wP.at<double>(2,0) << " ";
			
			int dataPV = static_cast<int>(data[i]+0.5);
			//Three times for rgb
			plyOut << dataPV << " ";
			plyOut << dataPV << " ";
			plyOut << dataPV << " ";

			//Alpha channel
			plyOut << "255\n";

		}
	plyOut << "0 0 0 0";


	plyOut.close();



}

void Extractor::loadMaps(){

	vector<string> imList = patchP->getImageNums();
	
	for(unsigned int i=0;i<imList.size();++i){
	  string mapPath = baseImLoc + "VNIR/mapVNIR" + imList[i] + "_flatfield";
	  MapOrtho* mapIm = new MapOrtho(dataType,mapPath);
	  mapIm->readImage();
	  maps.push_back(mapIm);
	}
}

void Extractor::loadOrthos(){

	vector<string> imList = patchP->getImageNums();
	
	for(unsigned int i=0;i<imList.size();++i){
	  string orthoPath = baseImLoc + dataType + "/ortho" + dataType + imList[i] + "_PreProcCube.tif";
	  Ortho* orthoIm = new Ortho(dataType,orthoPath);
	  orthoIm->readImage();
	  orthos.push_back(orthoIm);
	}
}

void Extractor::progressBar(float percent){

	static int x = 0;
	string slash[4];
	slash[0] = "\\";
	slash[1] = "-";
	slash[2] = "/";
	slash[3] = "|";
	cout.precision(2);
	cout << "\r"; // carriage return back to beginning of line
	cout << slash[x] << " " << fixed << percent << " %"; // print the bars and percentage
	x++; // increment to make the slash appear to rotate
	if(x == 4)
	x = 0; // reset slash animation

}
void Extractor::extractValues(){

	int numPoints = patchP->getNumPoints();

    for(int i=0; i<numPoints;i++){
    	//Calc precent complete and display
	float perc = static_cast<float>(i)/static_cast<float>(numPoints)*100;
	progressBar(perc);
	//Storage declare
	vector<float> valueV;

	//This part should be multithreaded if I ever learn how	
	vector< cv::Point > imP = patchP->getImagePoint(i);
	vector<int> imIn = patchP->getImageInds(i);

	for(unsigned int j=0; j<imP.size(); j++){
		//Enforce boundry (even though it shouldent need to be done... it has to)
		int nCols = maps[imIn[j]]->image.cols;
		int nRows = maps[imIn[j]]->image.rows;

		if( imP[j].x < 0 || imP[j].x > nCols || imP[j].y < 0 || imP[j].y > nRows){
			continue;
		}else{
		//Use Map to find ortho location
		cv::Point2d orthoPt = maps[imIn[j]]->findGeo(imP[j].x,imP[j].y);
		try{
		  float vT = interpValue(orthos[imIn[j]], orthoPt, "nearest");
		  valueV.push_back(vT);
		}catch(int e){
		  continue;
		}
		}
	}
	values.push_back(valueV);
    }
}

void Extractor::extractValues(MapOrtho orthoMap, HyperOrtho ortho, Points pts,  cv::Mat cam){

	vector< cv::Point2d > imPts = pts.projectPoints(cam);

	vector< cv::Point2d >::iterator it;
	for(it = imPts.begin(); it != imPts.end(); ++it){
		//Use Map to find ortho location
	    try{
		cv::Point2d orthoPt = orthoMap.findGeo(it->x,it->y);
		vector<float> prof = spectralProfile(ortho, orthoPt);
		values.push_back(prof);
	    }catch(int e){ // if caught, fill profile with 0s
		vector<float> profile;
		profile.resize(ortho.getNBands());
		fill(profile.begin(),profile.end(),0);
		values.push_back(profile);
		continue;
	    }
	}
}

vector<float> Extractor::spectralProfile(HyperOrtho ortho, cv::Point2d pt){

	vector<float> profile;

	cv::Point2d ptN = ortho.geographicToIntrinsic(pt);

	int xP = static_cast<int>(ptN.x+0.5);
	int yP = static_cast<int>(ptN.y+0.5);

	//Enforce boundry
	int nCols = ortho.getNCols();
	int nRows = ortho.getNRows();
	int bands = ortho.getNBands();

	if( xP < 0 || xP > nCols || yP < 0 || yP > nRows){
		profile.resize(bands);
		fill(profile.begin(),profile.end(),0);
	}else{
		for(int i=1; i<bands+1; i++){
			float value = ortho.returnBandPix(i,xP,yP);
			profile.push_back(value);
		}
	}
	return profile;
}

float Extractor::interpValue(Ortho* orthoI, cv::Point2d pt, string interpType){

	float intpValue;

	  if( interpType.compare("nearest") == 0){

	  	cv::Point2d ptN = orthoI->geographicToIntrinsic(pt);
		int xP = static_cast<int>(ptN.x+0.5);
		int yP = static_cast<int>(ptN.y+0.5);
		
		//Enforce boundry (even though it shouldent need to be done... it has to)
		int nCols = orthoI->image.cols;
		int nRows = orthoI->image.rows;

		if( xP < 0 || xP > nCols || yP < 0 || yP > nRows){
			throw -1;
		}else{
			intpValue = orthoI->image.at<float>(yP,xP);
	   	}
	   }

	return intpValue;

}


vector<float> Extractor::combineValues(){

	vector<float> combV;

	if( combType.compare("avg") == 0){
		for(unsigned int i=0; i<values.size(); i++){
		   int avgSize = 0;
		   float tempV = 0;
		  for(unsigned int j=0;j<values[i].size();j++){
			if(values[i][j]>0){	
			tempV = tempV + values[i][j];	
			avgSize++;
			}
		  }
		  tempV = tempV/values[i].size();

		  combV.push_back(tempV);
		}

	}else{
		cerr << "Invalid combType" << endl;
	}

	return combV;
}
