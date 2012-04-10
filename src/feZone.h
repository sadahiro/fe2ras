/*
 * feZone.h
 *
 *  Created on: Jun 4, 2010
 *      Author: Makoto Sadahiro
 */

#ifndef FEZONE_H_
#define FEZONE_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <iomanip>
#include <limits>
#include <cmath>
#include <algorithm>

using namespace std;

class feZone{

 public:

	feZone();

  feZone(ifstream& zoneStream,
  		const vector<string>* const var_name_list,
  		unsigned int uiN, unsigned int uiE);

  ~feZone();

  void config(
  		const string nameFileIn,
  		vector<string>* const var_name_list,
  		unsigned int ee_count,
  		unsigned int uiSOLUTIONTIME,
  		double fMIN_X,
  		double fMIN_Y,
  		double fMAX_X,
  		double fMAX_Y,
  		bool vshared,
  		feZone* src_zone,
  		unsigned int uiN,
  		unsigned int uiE);

  void pushLineV(double* arrV);
  void pushLineE(unsigned int* arrE);
  bool prefe2ras(unsigned int def_dx, unsigned int def_dy, unsigned int def_dz);
  void fe2ras();
  void spoolAllDrasBin();
  void writeDrasBin(unsigned int feID);
	void printZone();

  unsigned int timeStep;
  double min_x, min_y, min_z, max_x, max_y, max_z;
  double rasmin_x, rasmax_x, rasmin_y, rasmax_y, rasmin_z, rasmax_z;

  // trying to replace rasmin_* to minUVW and maxUVW.
  //  double rasmin_u, rasmin_v, rasmin_w, rasmax_u, rasmax_v, rasmax_w;
  double* minUVW;
  double* maxUVW;

  unsigned int ras_dx, ras_dy, ras_dz;
//  unsigned int rasmin_idx_x, rasmin_idx_y, rasmin_idx_z;
//  unsigned int rasmax_idx_x, rasmax_idx_y, rasmax_idx_z;
  //double** getMesh(string id);
  //double** getGrid(string id);

  bool debug;
  string srcFileName;
  vector<string>* listFeVer;
	unsigned int numFeVar;
	unsigned int numUVWVar;
	unsigned int prism_elementCount;
	bool var_shared;
	feZone* shared_zone;
	unsigned int numFeNode;
	unsigned int FeEdgeListCt;

//	double** FE6;
//	vector<vector<double> > E6;
//	string s_line;

	double** aV6;
	unsigned int currPLE;
	unsigned int currPLV;
	unsigned int** aE8;
	double*** rasUVW;

//	double** pEdgeList;

 private:

	bool BarycentricTest(
			double x0, double y0,
			double x1, double y1,
			double x2, double y2,
			double xT, double yT,
			double &wU, double &wV);

	unsigned int ras_dim_x, ras_dim_y, ras_dim_z;
	unsigned int def_dx, def_dy, def_dz;

	//double** addMesh(string id);
  //double** addGrid(string id);

};

#endif /* FEZONE_H_ */
