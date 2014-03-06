/*
 * =====================================================================================
 *
 *       Filename:  ortho.h
 *
 *    Description:  Class implimentaiton for Ortho imaes
 *
 *        Version:  1.0
 *        Created:  01/21/2013 03:30:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#ifndef ORTHO_H
#define ORTHO_H

#include	<string>

#include	<gdal.h>
#include	<gdal_priv.h>
#include	<cv.h>

using namespace std;
/*
 * =====================================================================================
 *        Class:  Ortho
 *  Description:  Base class for ortho images
 * =====================================================================================
 */
class Ortho{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		Ortho (string dataT, string imageLoc);                             /* constructor */

		/* ====================  ACCESSORS     ======================================= */
		string getImageLoc(){return imageLoc;}
		string getDataType(){return dataType;}
		int    getRowNum(){return image.rows;}
		int    getColNum(){return image.cols;}
		/* ====================  MUTATORS      ======================================= */
		void setDataType(string DT){dataType = DT;}
		void setImageLoc(string IL){imageLoc = IL;}
		void setImage(cv::Mat im){image = im;}
		/* ====================  OPERATORS     ======================================= */
		void readImage();
		cv::Point2d intrinsicToGeographic(double x, double y);
		cv::Point2d intrinsicToGeographic(cv::Point2d pt);
		
		cv::Point2d geographicToIntrinsic(double xP, double yP);
		cv::Point2d geographicToIntrinsic(cv::Point2d pt);
		/* ====================  DATA MEMBERS  ======================================= */
		double forwardGeoTrans[6];
		double reverseGeoTrans[6];
		cv::Mat image;
	protected:

	private:
		string dataType;
		string imageLoc;

}; /* -----  end of class Orthos  ----- */


/*
 * =====================================================================================
 *        Class:  MapOrthos
 *  Description:  MapOrtho subclass
 * =====================================================================================
 */
class MapOrtho : public Ortho{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		MapOrtho (string dataT, string imageL);                            /* constructor */

		/* ====================  ACCESSORS     ======================================= */

		/* ====================  MUTATORS      ======================================= */

		/* ====================  OPERATORS     ======================================= */
		// Use map to find projected point as long/lat value
		cv::Point2d findGeo(double x, double y);
	
		// Overdefine readImage
	 	using Ortho::readImage;
		void readImage();
		/* ====================  DATA MEMBERS  ======================================= */
	protected:

	private:
	     cv::Mat image2;
}; /* -----  end of class MapOrthos  ----- */


/*
 * =====================================================================================
 *        Class:  HyperOrtho
 *  Description:  HyperOrtho subclass
 * =====================================================================================
 */
class HyperOrtho : public Ortho{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		HyperOrtho (string dataT, string imageL);                       /* constructor */

		/* ====================  ACCESSORS     ======================================= */
		int getNCols();
		int getNRows();
		int getNBands();
		/* ====================  MUTATORS      ======================================= */

		/* ====================  OPERATORS     ======================================= */
		float returnBandPix(int i, int x, int y);

		// Overdefine readImage
		using Ortho::readImage;
		void readImage();
		/* ====================  DATA MEMBERS  ======================================= */
	protected:

	private:
		GDALDataset* dataset;		
}; /* -----  end of class HyperOrtho  ----- */


#endif
