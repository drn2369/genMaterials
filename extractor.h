/*
 * =====================================================================================
 *
 *       Filename:  extractor.h
 *
 *    Description:  Extractor class definitions
 *
 *        Version:  1.0
 *        Created:  01/22/2013 08:02:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David Nilosek (), drn2369@cis.rit.edu
 *        Company:  Rochester Institute of Technology
 *
 * =====================================================================================
 */
#ifndef		EXTRACTOR_H
#define		EXTRACTOR_H

#include	<string>

#include	"patch.h"
#include	"ortho.h"

using namespace std;
/*
 * =====================================================================================
 *        Class:  Extractor
 *  Description:  Extracts Ortho Values
 * =====================================================================================
 */
class Extractor
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		Extractor (Patch *ptch,string dataType, string combType, string baseImLoc );                             /* constructor */
		~Extractor();
		/* ====================  ACCESSORS     ======================================= */
		
		/* ====================  MUTATORS      ======================================= */
		void setPatch(Patch *pc){patchP = pc;}
		void setDataType(string dT){dataType = dT;}
		void setCombType(string cT){combType = cT;}
		void setBaseImLoc(string bL){baseImLoc = bL;}

		/* ====================  OPERATORS     ======================================= */
		void clear();
		void init();
		void progressBar(float percent);

		void writeData();
		void writeDataFile(vector<float> data);
		void writePLY(vector<float> data);

		void loadMaps();
		void loadOrthos();
		void extractValues();
		float interpValue(Ortho* orthoI, cv::Point2d pt, string interpType);
		vector<float> combineValues();

		/* ====================  DATA MEMBERS  ======================================= */
		Patch *patchP;
		string dataType;
		string combType;
		string baseImLoc;
		vector<MapOrtho*> maps;
		vector<Ortho*> orthos;
		vector<vector<float> > values;
	protected:

	private:

}; /* -----  end of class Extractor  ----- */
#endif
