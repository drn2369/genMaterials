/*
 * =====================================================================================
 *
 *       Filename:  ortho.cpp
 *
 *    Description:  Class implimentation for ortho
 *
 *        Version:  1.0
 *        Created:  01/21/2013 06:59:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#include	<iostream>
#include	<vector>
#include	<utility>

#include	<highgui.h>

#include	"ortho.h"

Ortho::Ortho(string dataT, string imageL){
	setDataType(dataT);
	setImageLoc(imageL);
}

MapOrtho::MapOrtho(string dataT, string imageL):Ortho(dataT,imageL){}  

HyperOrtho::HyperOrtho(string dataT, string imageL):Ortho(dataT,imageL){}

void Ortho::readImage(){

	//Read in image
	image = cv::imread(imageLoc,CV_LOAD_IMAGE_UNCHANGED);

	//Extract header info
	GDALAllRegister();
	GDALDatasetH hDataset = GDALOpen( imageLoc.c_str(), GA_ReadOnly);

	//Extract Forawrd and Reverse Transforms
	GDALGetGeoTransform(hDataset,forwardGeoTrans);
	GDALInvGeoTransform(forwardGeoTrans, reverseGeoTrans);
}


void MapOrtho::readImage(){
	
	string imLoc = this->getImageLoc();
	//Get x and y image
	string xImage = imLoc + "_x.tif";
	string yImage = imLoc + "_y.tif";

	//Read in images
	image = cv::imread(xImage,CV_LOAD_IMAGE_UNCHANGED);
	image2 = cv::imread(yImage,CV_LOAD_IMAGE_UNCHANGED);

	//Extract header info
	GDALAllRegister();
	GDALDatasetH hDataset = GDALOpen( xImage.c_str(), GA_ReadOnly);

	//Extract Forawrd and Reverse Transforms
	GDALGetGeoTransform(hDataset,forwardGeoTrans);
	GDALInvGeoTransform(forwardGeoTrans, reverseGeoTrans);
}

void HyperOrtho::readImage(){
	
	string imLoc = this->getImageLoc();

	//Read in images
	GDALAllRegister();
	GDALDataset *poDataset;

	poDataset = (GDALDataset *) GDALOpen(imLoc.c_str(), GA_ReadOnly);

	//Extract Forward and Reverse Transforms
	GDALGetGeoTransform(poDataset,forwardGeoTrans);
	GDALInvGeoTransform(forwardGeoTrans, reverseGeoTrans);

	dataset = poDataset;
}

cv::Point2d Ortho::intrinsicToGeographic(double x, double y){

	double xP = forwardGeoTrans[0] + x*forwardGeoTrans[1] + y*forwardGeoTrans[2];
	double yP = forwardGeoTrans[3] + x*forwardGeoTrans[4] + y*forwardGeoTrans[5];

	cv::Point2d point(xP,yP);

	return point;
}

cv::Point2d Ortho::intrinsicToGeographic(cv::Point2d pt){

	double x = pt.x;
	double y = pt.y;

	double xP = forwardGeoTrans[0] + x*forwardGeoTrans[1] + y*forwardGeoTrans[2];
	double yP = forwardGeoTrans[3] + x*forwardGeoTrans[4] + y*forwardGeoTrans[5];

	cv::Point2d point(xP,yP);

	return point;
}

cv::Point2d Ortho::geographicToIntrinsic(double xP, double yP){

	double x = reverseGeoTrans[0] + xP*reverseGeoTrans[1] + yP*reverseGeoTrans[2];
	double y = reverseGeoTrans[3] + xP*reverseGeoTrans[4] + yP*reverseGeoTrans[5];

	cv::Point2d point(x,y);

	return point;
}

cv::Point2d Ortho::geographicToIntrinsic(cv::Point2d pt){

	double xP = pt.x;
	double yP = pt.y;

	double x = reverseGeoTrans[0] + xP*reverseGeoTrans[1] + yP*reverseGeoTrans[2];
	double y = reverseGeoTrans[3] + xP*reverseGeoTrans[4] + yP*reverseGeoTrans[5];

	cv::Point2d point(x,y);

	return point;
}

cv::Point2d MapOrtho::findGeo(double x, double y){

	//Convert to map index by rounding to
	//nearest int, Map index starts at 1
	int xM = static_cast<int>(x+1.5);
	int yM = static_cast<int>(y+1.5);

	//Search X until correct value is found
	int nRows = image.rows;
	int nCols = image.cols;
	int i,j;
	int indStartI, indStartJ;
	int breakLoop=1;

	float* p;

	//Boundry checking
	if( xM < 0 || xM > nCols || yM < 0 || yM > nRows){
		throw 1;
	}

	//Search through image for first instance of value
	for( i=0; i < nRows && breakLoop; ++i){
		//Temp value to match against
		float tempV;
		//Get Pointer to row
		p = image.ptr<float>(i);
		//Loop through column
		for( j = 0; j < nCols && breakLoop; ++j){
		  tempV = p[j];
		  //Match value if left side
		  if( xM == static_cast<int>(tempV) && j < (nCols/2)){
			indStartI = i;
			indStartJ = j;
		  	breakLoop = 0;
		  }else if( xM == static_cast<int>(tempV) && j >= (nCols/2)){
		  	//Get rightmost value if on right
			while( xM == static_cast<int>(tempV)){
				indStartI = i;
				indStartJ = j;
				j++;
				tempV = p[j];
			}
			breakLoop = 0;
		  }
		}
	}


	//Extract all indicies which are the same value
	//These will be in a "seam"
	//Extraction done by looking at surrounding pixels
	
	//*Should* always start at an edge ( I think)
	
	int breakWhile=1;

	//Initialize while loop
	i = -1; j = -1;
	int prevI=-1,prevJ=-1;
	int newI=indStartI,newJ=indStartJ;
   	int bottomCount=0, rightCount=0,leftCount=0;	
	//Storage
	vector<pair<int,int> > xInds;
	xInds.push_back(make_pair(newI,newJ));

	    /*
	 cout << "t" << " " << "t" << " " << "t" << " " << "m" << " " << "m" << " "
	         << "b" << " " << "b" << " " << "b" << endl;
	 cout << "l" << " " << "m" << " " << "r" << " " << "l" << " " << "r" << " "
	         << "l" << " " << "m" << " " << "r" << endl << endl;
		*/
	while(breakWhile){
	   //Set i,j
	   i=newI;j=newJ;
	   
	   //Eight surrounding pixels
	   int tl=0,tm=0,tr=0;
	   int ml=0,mr=0;
	   int bl=0,bm=0,br=0;

	   //Check all boundry conditions
	   int topB = i-1 < 0;
	   int bottomB = i+1 > nRows;
	   int leftB = j-1 < 0;
	   int rightB = j+1 > nCols;
	  
	  //Find next pixel while enforcing boundry
	   if(!(topB)){
		tm = xM == static_cast<int>(image.at<float>(i-1,j));
	   }
	   if(!(bottomB)){
		bm = xM == static_cast<int>(image.at<float>(i+1,j));
	   }
	   if(!(leftB)){
		ml = xM == static_cast<int>(image.at<float>(i,j-1));
	   }
	   if(!(rightB)){
		mr = xM == static_cast<int>(image.at<float>(i,j+1));
	   }
	   if(!(topB||leftB)){
		tl = xM == static_cast<int>(image.at<float>(i-1,j-1));
	   }
	   if(!(topB||rightB)){
		tr = xM == static_cast<int>(image.at<float>(i-1,j+1));
	   }
	   if(!(bottomB||leftB)){
		bl  = xM == static_cast<int>(image.at<float>(i+1,j-1));
	   }
	   if(!(bottomB||rightB)){
		br  = xM == static_cast<int>(image.at<float>(i+1,j+1));
	   }
	
	   //Determine new i,j checking to make sure it's not the previous
	   //determined i,ji

	   int tlo = ( (i-1 == prevI) && (j-1 == prevJ));
	   int tmo = ( (i-1 == prevI) && (j == prevJ));
	   int tro = ( (i-1 == prevI) && (j+1 == prevJ));
	   
	   int mlo = ( (i == prevI) && (j-1 == prevJ));
	   int mro = ( (i == prevI) && (j+1 == prevJ));
	   
	   int blo = ( (i+1 == prevI) && (j-1 == prevJ));
	   int bmo = ( (i+1 == prevI) && (j == prevJ));
	   int bro = ( (i+1 == prevI) && (j+1 == prevJ));
	   
	   int tlp = tl && !tlo;
	   int tmp = tm && !tmo;
	   int trp = tr && !tro;
	   
	   int mlp = ml && !mlo;
	   int mrp = mr && !mro;
	   
	   int blp = bl && !blo;
	   int bmp = bm && !bmo;
	   int brp = br && !bro;

	  //Don't count diags, first two steps
	  if(xInds.size() < 4){
	   bottomCount = bottomCount  + bmp;
	   rightCount = rightCount + mrp;
	   leftCount = leftCount + mlp ;
	  }
	  /*  cout << tlo << " " << tmo << " " << tro << " " << mlo << " " << mro << " "
	         << blo << " " << bmo << " " << bro << endl;

	    cout << tlp << " " << tmp << " " << trp << " " << mlp << " "
	         << mrp << " " << blp << " " << bmp << " " << brp  << endl;
		*/
	   //Enforce cannot take a step back
	   //If come from that direction, do not use that direction
	   // AKA SUPER IF-STATEMENT
	   // Cardinal directions (up down left right) before diagonals
	  
	   //Take two steps (3 points)
	   if(xInds.size() < 3){

	   //If come from top left
	   if(tlo){
	   	if(mrp){
			newI=i; newJ=j+1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   }
	   //If come from top middle
	   else if(tmo){
	   	if(mlp){
			newI=i; newJ=j-1;
	   	}else if(mrp){
			newI=i; newJ=j+1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   
	   }
	   //If come from top right
	   else if(tro){
	   	if(mlp){
			newI=i; newJ=j-1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(tlp){
			newI=i-1; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   }
	   //If come from middle left
	   else if(mlo){

	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	  	}else if(mrp){
			newI=i; newJ=j+1;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   //If come from middle right
	   else if(mro){

	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(tlp){
	   		newI=i-1; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   //If come from bottom left
	   else if(blo){
	   	if(tmp){
			newI=i-1; newJ=j;
	  	}else if(mrp){
			newI=i; newJ=j+1;
	   	}else if(tlp){
	   		newI=i-1; newJ=j-1;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   //If come from bottom middle
	   else if(bmo){
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	  	 }else if(mrp){
			newI=i; newJ=j+1;
	   	}else if(tlp){
	   		newI=i-1; newJ=j-1;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   //If come from bottom right
	   else if(bro){
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else if(tlp){
	   		newI=i-1; newJ=j-1;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   //Else it is the first pixel
	   else{
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	  	 }else if(mrp){
			newI=i; newJ=j+1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
		}else if(tlp){
	   		newI=i-1; newJ=j-1;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}
	   }
	   //Direction is moving top to bottom (Choose middle first
	   //then diag, then bottom)
	   }else if( bottomCount > rightCount && bottomCount > leftCount){
           //If it comes from top left
	   if(tlo){
	   	if(mrp){
			newI=i; newJ=j+1;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   }
	   //If come from top middle
	   else if(tmo){
	   	if(mlp){
			newI=i; newJ=j-1;
	   	}else if(mrp){
			newI=i; newJ=j+1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   
	   }
	   //If come from top right
	   else if(tro){
	   	if(mrp){
			newI=i; newJ=j+1;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   }
	   //If come from middle left
	   else if(mlo){
	  	if(mrp){
			newI=i; newJ=j+1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   //If come from middle right
	   else if(mro){
	        if(mlp){
			newI=i; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }

	   //Direction is moving left to right (Top/Bottom, then diag
	   // then right)
	   }else if(rightCount > bottomCount && rightCount > leftCount){

	   if(tlo){
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else if(mrp){
			newI=i; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   }
	   //If come from top middle
	   else if(tmo){
	   	if(bmp){
			newI=i+1; newJ=j;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(mrp){
			newI=i; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   
	   }
	   //If come from middle left
	   else if(mlo){

	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	  	}else if(mrp){
			newI=i; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   
	   //If come from bottom left
	   else if(blo){
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	  	}else if(mrp){
			newI=i; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   //If come from bottom middle
	   else if(bmo){
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(trp){
			newI=i-1; newJ=j+1;
	   	}else if(brp){
			newI=i+1; newJ=j+1;
	  	 }else if(mrp){
			newI=i; newJ=j+1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	    //Direction is moving right to left (Top/Bottom, diag, left)
	   }else{
		
	   //If come from top middle
	   if(tmo){
	   	if(bmp){
			newI=i+1; newJ=j;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(tlp){
			newI=i-1; newJ=j-1;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   
	   }
	   //If come from top right
	   else if(tro){
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(tlp){
			newI=i-1; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}
	   }

	   //If come from middle right
	   else if(mro){

	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(tlp){
			newI=i-1; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}

	   }

	   //If come from bottom middle
	   else if(bmo){
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(tlp){
	   		newI=i-1; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
	   	}

	   }
	   //If come from bottom right
	   else if(bro){
	   	if(tmp){
			newI=i-1; newJ=j;
	   	}else if(bmp){
			newI=i+1; newJ=j;
	   	}else if(tlp){
			newI=i-1; newJ=j-1;
	   	}else if(blp){
			newI=i+1; newJ=j-1;
	   	}else if(mlp){
			newI=i; newJ=j-1;
	   	}else{
			//If all false, then loop ends
			breakWhile = 0;
		}

	   }
	   
	   }

	   //Store new i,j
           xInds.push_back(make_pair(newI,newJ));
	   //Store old i,j
	   prevI=i,prevJ=j;
	}

      //Search through inds in second image to find match
       vector<pair<int,int> >::iterator it;
	int iMatch,jMatch;

	for(it = xInds.begin(); it != xInds.end(); ++it){
		int i = it->first;
		int j = it->second;

		int match = yM == static_cast<int>(image2.at<float>(i,j));
		if(match){
			iMatch =i; jMatch=j;
			break;
		}
	}
	//i = row = y, j = col = x
	return this->intrinsicToGeographic(jMatch, iMatch);
}


float HyperOrtho::returnBandPix(int i, int x, int y){

	//Get Band
	GDALRasterBand *poBand;
	poBand = dataset->GetRasterBand(i);

	//Get Pixel
	float *pafPix;
	pafPix = (float *) CPLMalloc(sizeof(float));
	poBand->RasterIO(GF_Read,x,y,1,1,pafPix,1,1,GDT_Float32,0,0);

	return *pafPix;
}

int HyperOrtho::getNCols(){
	return dataset->GetRasterXSize();
}


int HyperOrtho::getNRows(){
	return dataset->GetRasterYSize();
}

int HyperOrtho::getNBands(){
	return dataset->GetRasterCount();
}

