/*
 * feZone.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: Makoto Sadahiro
 */

#include "feZone.h"

using namespace std;

feZone::feZone(){

	debug = true;

}

/* this is obsolete, not used now.  replaced by default constructor and feZone::config(*).
 * delete this when I have chance.
 */
feZone::feZone(ifstream &zoneStream,
		const vector<string>* const var_name_list,
		unsigned int uiN, unsigned int uiE){

	numFeVar= var_name_list->size();
	numUVWVar= numFeVar - 3;
	numFeNode= uiN;
	FeEdgeListCt= uiE;

	//pFeNode=

	cout << "feZone: numFeNode=" << numFeNode
			<< " FeEdgeListCt=" << FeEdgeListCt
			<< " col=" << numFeVar
			<< endl;

//	string s_line;
//	for(unsigned int iteratorFeNode=0; iteratorFeNode<numFeNode; iteratorFeNode++){
//		getline(zoneStream, s_line);
//		cout << s_line << endl;
//	}
//
//	for(unsigned int iteratorFeEdge=0; iteratorFeEdge<FeEdgeListCt; iteratorFeEdge++){
//		getline(zoneStream, s_line);
//		cout << s_line << endl;
//	}

	/*
	while(zoneStream){
			getline(zoneStream, s_line);
			cout << s_line << endl;
		}
		*/

}


feZone::~feZone(){

	if(debug){
		cout << "feZone::~feZone " << timeStep << " called." << endl;
		cout << "numFeNode: " << numFeNode << "  FeEdgeListCt: " << FeEdgeListCt << endl;
//		printZone();
	}

	if(var_shared){
		// do nothing for aV6[1,2]
		for(unsigned int i = 2; i < numFeVar; ++i){
			delete[] aV6[i];
		}
		delete[] aV6;
	}else{
		for(unsigned int i = 0; i < numFeVar; ++i){
			delete[] aV6[i];
		}
		delete[] aV6;
	}

	if(var_shared){
		// do nothing
	}
	else{
		for(unsigned int i = 0; i < FeEdgeListCt; ++i){
			delete[] aE8[i];
		}
		delete[] aE8;
	}

	for(unsigned int y = 0; y < ras_dim_y; ++y){
		for(unsigned int x = 0; x < ras_dim_x; ++x){
			delete[] rasUVW[y][x];
		}
		delete[] rasUVW[y];
	}
	delete[] rasUVW;

	delete[] minUVW;
	delete[] maxUVW;

	if(debug){
		cout << "exiting feZone::~feZone." << endl << endl;
	}

}


void feZone::config(
		const string nameFileIn,
		vector<string>* const var_name_list,
		unsigned int src_prisEleCount,
		unsigned int uiSOLUTIONTIME,
		double fMIN_X,
		double fMIN_Y,
		double fMAX_X,
		double fMAX_Y,
		bool vshared,
		feZone* src_zone,
		unsigned int uiN,
		unsigned int uiE){

	srcFileName = nameFileIn;
	listFeVer = var_name_list;
	numFeVar= listFeVer->size();
	numUVWVar= numFeVar - 3;

	cout << "*** feZone VARLIST: ";
	for(unsigned int i = 0; i < numFeVar; ++i){
		cout << var_name_list->at(i) << " ";
	}
	cout << endl;

	prism_elementCount = src_prisEleCount;
	timeStep = uiSOLUTIONTIME;
	min_x = numeric_limits<double>::max();//fMIN_X;
	min_y = numeric_limits<double>::max();//fMIN_Y;
	min_z = numeric_limits<double>::max();
	max_x = -numeric_limits<double>::max();//fMAX_X;
	max_y = -numeric_limits<double>::max();//fMAX_Y;
	max_z = -numeric_limits<double>::max();

	minUVW = new double[numUVWVar];
	maxUVW = new double[numUVWVar];
	for(unsigned int i = 0; i < numUVWVar; ++i){
		minUVW[i] = numeric_limits<double>::max();
		maxUVW[i] = -numeric_limits<double>::max();
	}

	/*
	rasmin_u = numeric_limits<double>::max();
	rasmin_v = numeric_limits<double>::max();
	rasmin_w = numeric_limits<double>::max();
	rasmax_u = -numeric_limits<double>::max();
	rasmax_v = -numeric_limits<double>::max();
	rasmax_w = -numeric_limits<double>::max();
	 */

	var_shared = vshared;
	shared_zone = src_zone;
	numFeNode= uiN;
	FeEdgeListCt= uiE;
	currPLV = 0;
	currPLE = 0;

	if(debug){
		cout << "feZone::config numFeNode: " << numFeNode << "  FeEdgeListCt: " << FeEdgeListCt << endl;
	}
//	FE6 = new double **[6][numFeNode];
//	E6.resize(numFeVar);
//	for(unsigned int i = 0; i < numFeVar; ++i){
//		E6[i].resize(numFeNode);
//	}

	if(var_shared){
		aV6 = new double*[numFeVar];
		aV6[0] = shared_zone->aV6[0];
		aV6[1] = shared_zone->aV6[1];
		min_x = shared_zone->min_x;
		min_y = shared_zone->min_y;
		max_x = shared_zone->max_x;
		max_y = shared_zone->max_y;
		for(unsigned int i = 2; i < numFeVar; ++i){
			aV6[i] = new double[numFeNode];
		}
	}else{
		aV6 = new double*[numFeVar];
		for(unsigned int i = 0; i < numFeVar; ++i){
			aV6[i] = new double[numFeNode];
		}
	}

	if(var_shared){
		aE8 = shared_zone->aE8;
	}
	else{
		aE8 = new unsigned int*[FeEdgeListCt];
		for(unsigned int i = 0; i < FeEdgeListCt; ++i){
			aE8[i] = new unsigned int[prism_elementCount];
		}
	}

}


void feZone::pushLineV(double* arrV){

	if(var_shared){

		for(unsigned int i = 2; i < numFeVar; i++){
			aV6[i][currPLV] = arrV[i - 2];
		}

	}
	else{

		for(unsigned int i = 0; i < numFeVar; i++){
			aV6[i][currPLV] = arrV[i];
		}

		if(aV6[0][currPLV] < min_x){
			min_x = aV6[0][currPLV];
		}
		if(max_x < aV6[0][currPLV]){
			max_x = aV6[0][currPLV];
		}
		if(aV6[1][currPLV] < min_y){
			min_y = aV6[1][currPLV];
		}
		if(max_y < aV6[1][currPLV]){
			max_y = aV6[1][currPLV];
		}

	}

	if(aV6[2][currPLV] < min_z){
		min_z = aV6[2][currPLV];
	}
	if(max_z < aV6[2][currPLV]){
		max_z = aV6[2][currPLV];
	}

	currPLV++;
}


void feZone::pushLineE(unsigned int* arrE){

	if(var_shared){
		// do nothing
	}else{

		for(unsigned int i = 0; i < prism_elementCount; i++){
			aE8[currPLE][i] = arrE[i];
		}
//		cout << "PL " << currPLE;
//		for(unsigned int i = 0; i < numFeVar; i++){
//			cout << " " << fixed << setprecision(8) << aV6[i][currPLE];
//		}
//		cout << endl;

	}

	currPLE++;
}


bool feZone::prefe2ras(unsigned int src_dx, unsigned int src_dy, unsigned int src_dz){

//	unsigned int min_dim = 100;

//	if(min_dim * def_dx > abs(max_x - min_x)){
//		cout << "error: dx is too large" << endl;
//		cout << def_dx << " " << min_x << endl;
//		return false;
//	}
//	if(min_dim * def_dy > abs(max_y - min_y)){
//		cout << "error: dy is too large" << endl;
//		cout << def_dy << " " << min_y << endl;
//		return false;
//	}
//	if(min_dim * def_dz > abs(max_z - min_z)){
//		cout << "warning: dz is too large: " << def_dz << endl;
//		cout << "range: " << min_z << ".." << max_z << endl;
//		// take care of this !!!
//		//			return false;
//	}

//	// test of flooring in negative numbers
//	double top = -10.234;
//	unsigned int down = 2;
//	double result = top/down;
//	double floor_result = floor(result);
//	cout << top << " " << down << " " << result << " " << floor_result << endl;
//	// end test

	def_dx = src_dx;
	def_dy = src_dy;
	def_dz = src_dz;

	cout << "def_dx:"
			<< fixed
			<< def_dx << " range_x:[" << min_x << " .. " << max_x << "], abs= "
			<< max_x - min_x << endl;

	cout << "def_dy:"
			<< fixed
			<< def_dy << " range_y:[" << min_y << " .. " << max_y << "], abs= "
			<< max_y - min_y << endl;

	cout << "def_dz:"
			<< fixed
			<< def_dz << " range_z:[" << min_z << " .. " << max_z << "], abs= "
			<< max_z - min_z << endl;

	// aligns rasmin_* values to create a box that BARELY
	// contains the area according to user definned resolutin.
	rasmin_x = floor(min_x / def_dx) * def_dx;
	rasmax_x = (floor(max_x / def_dx) + 1) * def_dx;
	rasmin_y = floor(min_y / def_dy) * def_dy;
	rasmax_y = (floor(max_y / def_dy) + 1) * def_dy;
	rasmin_z = floor(min_z / def_dz) * def_dz;
	rasmax_z = (floor(max_z / def_dz) + 1) * def_dz;

	cout << "rasterizing range from data sample:" << endl
			<< fixed
			<< "x " << rasmin_x << " (" << min_x << ") .. "
			<< rasmax_x << " (" << max_x << ")" << endl
			<< "y " << rasmin_y << " (" << min_y << ") .. "
			<< rasmax_y << " (" << max_y << ")" << endl
			<< "z " << rasmin_z << " (" << min_z << ") .. "
			<< rasmax_z << " (" << max_z << ")" << endl
			<< endl;

	ras_dim_x = (unsigned int)((rasmax_x - rasmin_x) / def_dx + 1);
	ras_dim_y = (unsigned int)((rasmax_y - rasmin_y) / def_dy + 1);
	ras_dim_z = (unsigned int)((rasmax_z - rasmin_z) / def_dz + 1);

	cout << "raster array: [" << ras_dim_x << "][" << ras_dim_y << "]" << endl;

	rasUVW = new double**[ras_dim_y];
	for(unsigned int y = 0; y < ras_dim_y; ++y){
		rasUVW[y] = new double*[ras_dim_x];
		for(unsigned int x = 0; x < ras_dim_x; ++x){
			rasUVW[y][x] = new double[numUVWVar];

			for(unsigned int a = 0; a < numUVWVar; ++a){
				rasUVW[y][x][a] = 0.0;
			}

		}
	}

	cout << "ending feZone::prefe2ras" << endl
			<< endl;
	return true;
}


void feZone::fe2ras(){

	cout << "starting feZone::fe2ras()" << endl;

	/*
	 * I may need to pick best 3 vertices here from 8 vertices manually
	 * The assumption we have is that first 3 vertices represents top surface, but may not be true.
	 */
	unsigned int cv_idx_0;
	unsigned int cv_idx_1;
	unsigned int cv_idx_2;
	double x0, y0, z0, x1, y1, z1, x2, y2, z2;
	for(unsigned int FeEdgeList_itr = 0; FeEdgeList_itr < FeEdgeListCt; ++FeEdgeList_itr){
		cv_idx_0 = aE8[FeEdgeList_itr][0];
		cv_idx_1 = aE8[FeEdgeList_itr][1];
		cv_idx_2 = aE8[FeEdgeList_itr][2];
		cout << "index: " << cv_idx_0 << " " << cv_idx_1 << " " << cv_idx_2 << endl;
		x0 = aV6[0][cv_idx_0 -1];
		y0 = aV6[1][cv_idx_0 -1];
		z0 = aV6[2][cv_idx_0 -1];
		x1 = aV6[0][cv_idx_1 -1];
		y1 = aV6[1][cv_idx_1 -1];
		z1 = aV6[2][cv_idx_1 -1];
		x2 = aV6[0][cv_idx_2 -1];
		y2 = aV6[1][cv_idx_2 -1];
		z2 = aV6[2][cv_idx_2 -1];
		// element edge list start with 1 not 0 for tecplot, so use -1 above
		cout << x0 << "," << y0 << " " << x1 << "," << y1 << " " << x2 << "," << y2 << endl;


		// find index value of x and y that barely covers the triangle
		double x_min = min(min(x0, x1), x2);
		double y_min = min(min(y0, y1), y2);
		double z_min = min(min(z0, z1), z2);
		double x_max = max(max(x0, x1), x2);
		double y_max = max(max(y0, y1), y2);
		double z_max = max(max(z0, z1), z2);
		cout << "x:" << x_min << ".." << x_max << " " << "y:"
		    << y_min << ".." << y_max << " " << "z:" << z_min << ".." << z_max
		    << endl;

		unsigned int rasmin_idx_x = (unsigned int)(floor((x_min - rasmin_x) / def_dx));
		unsigned int rasmin_idx_y = (unsigned int)(floor((y_min - rasmin_y) / def_dy));
		unsigned int rasmin_idx_z = (unsigned int)(floor((z_min - rasmin_z) / def_dz));
		unsigned int rasmax_idx_x = (unsigned int)(floor((x_max - rasmin_x) / def_dx) + 1);
		unsigned int rasmax_idx_y = (unsigned int)(floor((y_max - rasmin_y) / def_dy) + 1);
		unsigned int rasmax_idx_z = (unsigned int)(floor((z_max - rasmin_z) / def_dz) + 1);
  	cout << "idx_x:" << rasmin_idx_x << ".." << rasmax_idx_x << endl
  			<< "idx_y:" << rasmin_idx_y << ".." << rasmax_idx_y << endl
  			<< "idx_z:" << rasmin_idx_z << ".." << rasmax_idx_z << endl
  			<< endl
  			<< "x " << rasmin_idx_x * def_dx << ".." << rasmax_idx_x * def_dx << endl
  			<< "y " << rasmin_idx_y * def_dy << ".." << rasmax_idx_y * def_dy << endl
  			<< "z " << rasmin_idx_z * def_dz << ".." << rasmax_idx_z * def_dz << endl
  			<< endl;

  	// Bary-centric test for all text point vectors within current vector pair
  	// This section will test all raster points that covers current triangle as rectangle
  	double wU = 0.0;
  	double wV = 0.0;
  	double wO = 0.0;
  	for(unsigned int y = rasmin_idx_y; y <= rasmax_idx_y; ++y){
  		for(unsigned int x = rasmin_idx_x; x <= rasmax_idx_x; ++x){

  			/*  Barycentric Test */
  			if(BarycentricTest(x0, y0, x1, y1, x2, y2,
  					x * def_dx + rasmin_x, y * def_dy + rasmin_y, wU, wV)){
  				// use wU and wV to interpolate UVW on the raster points
  				wO = 1.0 - ( wU + wV);
//  				cout << "BC balance: " << setw(5) << wO << wU << wV << endl;

  				// now interpolate
//  				rasUVW[y][x][0] = wO * aV6[3][cv_idx_0 -1] + wU * aV6[3][cv_idx_1 -1] + wV * aV6[3][cv_idx_2 -1];
//  				rasUVW[y][x][1] = wO * aV6[4][cv_idx_0 -1] + wU * aV6[4][cv_idx_1 -1] + wV * aV6[4][cv_idx_2 -1];
//  				rasUVW[y][x][2] = wO * aV6[5][cv_idx_0 -1] + wU * aV6[5][cv_idx_1 -1] + wV * aV6[5][cv_idx_2 -1];
  				for(unsigned int i = 0; i < numUVWVar; ++i){
  					rasUVW[y][x][i] =
  							wO * aV6[i + 3][cv_idx_0 -1] +
  							wU * aV6[i + 3][cv_idx_1 -1] +
  							wV * aV6[i + 3][cv_idx_2 -1];
  					if(rasUVW[y][x][i] != 0.0){
  						minUVW[i] = min(minUVW[i], rasUVW[y][x][i]);
  						maxUVW[i] = max(maxUVW[i], rasUVW[y][x][i]);
  					}
  				}

//  				cout << "weight:" << wO << " " << wU << " " << wV << endl
//  				<< "aV6.i3:" << aV6[3][cv_idx_0 - 1] << " " << aV6[3][cv_idx_1 - 1] << " " << aV6[3][cv_idx_2 - 1] << endl
//  				<< "rasUVW.i3:" << rasUVW[y][x][0] << endl << endl;

//  				for(unsigned int i = 0; i < numUVWVar; ++i){
//  					if(rasUVW[y][x][i] != 0.0){
//  							minUVW[i] = min(minUVW[i], rasUVW[y][x][i]);
//  							maxUVW[i] = max(maxUVW[i], rasUVW[y][x][i]);
//  					}
//  				}

//  				cout << "B";
  			}
  			else{
  				if((wU > 0)&&(wV > 0)){
  					// case the vector of the point overshoot between the vector pair
//  					cout << "O";
  				}
  				else{
  					// case the vector of the point lands outside the vector pair
//  					cout << "N";
  				}
  			}
//  			if(timeStep == 3600){
//  				cout << "key: rasmax_idx_y " << scientific << rasmax_idx_y << " rasmin_idx_y " << rasmin_idx_y << " ras_dim_y " << ras_dim_y << endl;
//  				cout << "key: rasmax_idx_x " << scientific << rasmax_idx_x << " rasmin_idx_x " << rasmin_idx_x << " ras_dim_x " << ras_dim_x << endl;
//  			}
  		}
//  		cout << endl;// for B and N pics
  	}
// I probably want this, but disabling for now for simplicity of min/maxUVW[]
//  	cout << "u:" << rasmin_u << ".." << rasmax_u << "  "
//  			<< "v:" << rasmin_v << ".." << rasmax_v << "  "
//  			<< "w:" << rasmin_w << ".." << rasmax_w << endl;
  	cout << endl;

	}// for(unsigned int FeEdgeList_itr

	cout << "var range (right after BC interpolation)" << endl;
	for(unsigned int i = 0; i < numUVWVar; ++i){
		cout << listFeVer->at(i+3) << ": " << scientific << minUVW[i] << " .. " << maxUVW[i] << endl;
	}

	double tmin = numeric_limits<double>::max();
	double tmax = -numeric_limits<double>::max();

	for(unsigned int y = 0; y < ras_dim_y; ++y){
		for(unsigned int x = 0; x < ras_dim_x; ++x){
			if(0.0 != rasUVW[y][x][0]){
				tmin = min(rasUVW[y][x][0], tmin);
				tmax = max(tmax, rasUVW[y][x][0]);
			}
		}
	}
	cout << " reverification U_range: " << scientific << tmin << " .. " << tmax << endl
			<< endl;

}


bool feZone::BarycentricTest(
		double x0, double y0,
		double x1, double y1,
		double x2, double y2,
		double xT, double yT,
		double &wU, double &wV){

	double x01 = x1 - x0;
	double y01 = y1 - y0;
	double x02 = x2 - x0;
	double y02 = y2 - y0;
	double x0t = xT - x0;
	double y0t = yT - y0;

	double dot11 = x01 * x01 + y01 * y01;
	double dot12 = x01 * x02 + y01 * y02;
	double dot1t = x01 * x0t + y01 * y0t;
	double dot22 = x02 * x02 + y02 * y02;
	double dot2t = x02 * x0t + y02 * y0t;

	double invDiv = 1 / (dot11 * dot22 - dot12 * dot12);
	wU = (dot22 * dot1t - dot12 * dot2t) * invDiv;
	wV = (dot11 * dot2t - dot12 * dot1t) * invDiv;

	return (wU > 0) && (wV > 0) && (wU + wV < 1);
}


void feZone::spoolAllDrasBin(){
	cout << "sppling all double raster binary files: " << numUVWVar << "files" << endl;
	for(unsigned int i = 0; i < numUVWVar; ++i){
		writeDrasBin(i);
	}
}

void feZone::writeDrasBin(unsigned int feID){

	double tmin = numeric_limits<double>::max();
	double tmax = -numeric_limits<double>::max();

	for(unsigned int y = 0; y < ras_dim_y; ++y){
		for(unsigned int x = 0; x < ras_dim_x; ++x){
			if(0.0 != rasUVW[y][x][feID]){
				tmin = min(rasUVW[y][x][feID], tmin);
				tmax = max(tmax, rasUVW[y][x][feID]);
			}
		}
	}
	cout << " measured UVW_range: " << scientific << tmin << " .. " << tmax << endl
			<< endl;

	int rasorig_idx_x = (int)(floor(rasmin_x / def_dx));
	int rasorig_idx_y = (int)(floor(rasmin_y / def_dy));

		stringstream ss;
		ss << setw(8) << setfill('0') << timeStep;
		string fname = srcFileName + ss.str() + "_" + listFeVer->at(feID + 3) + ".drb";
		//                                                        + 3 for x, y, and z column
		cout << "output file name: " << fname << endl;
		FILE* pFile;
		pFile = fopen(fname.c_str(), "wb");

		cout << "dim:" << ras_dim_x << " " << ras_dim_y << endl;

		// write metadata
		int tmpI;
		tmpI = (int)(rasorig_idx_x);
		fwrite(&tmpI, sizeof(int), 1, pFile);
		tmpI = (int)(rasorig_idx_y);
		fwrite(&tmpI, sizeof(int), 1, pFile);

		tmpI = ras_dim_x;
		fwrite(&tmpI, sizeof(int), 1, pFile);
		tmpI = ras_dim_y;
		fwrite(&tmpI, sizeof(int), 1, pFile);

		double rasD;
		rasD = tmax;//maxUVW[feID];
		cout << "UVW_range max:" << rasD << endl;
		fwrite(&rasD, sizeof(double), 1, pFile);
		rasD = tmin;//minUVW[feID];
		cout << "UVW_range min:" << rasD << endl;
		fwrite(&rasD, sizeof(double), 1, pFile);

		// write array
		double tmpD;
		for(unsigned int y = 0; y < ras_dim_y; ++y){
			for(unsigned int x = 0; x < ras_dim_x; ++x){
				tmpD = rasUVW[y][x][feID];
				fwrite(&tmpD, sizeof(double), 1, pFile);
			}
		}
		fclose(pFile);

		cout << "DrasBin file written." << fname << endl
				<< " orig:" << rasorig_idx_x << "," << rasorig_idx_y << endl
				<< " dim:" << ras_dim_x << "," << ras_dim_y << endl
				<< " theoretical UVW_range: " << scientific << minUVW[feID] << " .. " << maxUVW[feID] << endl;

}


void feZone::printZone(){

	cout << "min: " << fixed << min_x << " " << min_y << " " << min_z << endl
			<< "max: " << fixed << max_x << " " << max_y << " " << max_z << endl;
	for(unsigned int i = 0; i < numFeNode; ++i){
		for(unsigned int j = 0; j < numFeVar; ++j){
			cout << " " << fixed << setprecision( 8 )<< aV6[j][i];
		}
		cout << endl;
	}
	cout << "---" << endl;

	for(unsigned int i = 0; i < FeEdgeListCt; ++i){
		for(unsigned int j = 0; j < prism_elementCount; ++j){
			cout << " " << fixed << aE8[i][j];
		}
		cout << endl;
	}
	cout << "===" << endl;
}
