/*
 * =====================================================================================
 *
 *       Filename:  points.h
 *
 *    Description:  Point class
 *
 *        Version:  1.0
 *        Created:  04/26/2013 01:20:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#ifndef POINTS_H
#define	POINTS_H

#include	<string>
#include	<vector>

using namespace std;

/*
 * =====================================================================================
 *        Class:  Points
 *  Description:  Class definitions for points
 * =====================================================================================
 */
class Points
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		Points ( string plyF );                             /* constructor */

		/* ====================  ACCESSORS     ======================================= */

		/* ====================  MUTATORS      ======================================= */
		void setPlyF(string pF){plyFile=pF;}
		/* ====================  OPERATORS     ======================================= */
		void readPLYFile();

		vector< cv::Point2d > projectPoints( cv::Mat cam );
		/* ====================  DATA MEMBERS  ======================================= */
	protected:

	private:
		string plyFile;
		vector< cv::Mat > xyz;
		vector< cv::Mat > norm;
		vector< vector<int> > rgb;

}; /* -----  end of class Points  ----- */
#endif
