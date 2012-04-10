/*
 * feParse.h
 *
 *  Created on: May 20, 2010
 *      Author: Makoto Sadahiro
 */

#ifndef FEPARSE_H_
#define FEPARSE_H_


#include <sstream>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <typeinfo>
#include <iomanip>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>

#include "feZone.h"

using namespace std;
using namespace boost;

class feParse{

 public:

  feParse(int debug_flag, vector<feZone*>* zone_list, char** arg_v);
  ~feParse();

  // function to be called to parse a input file with multiple zone information
  bool tec2fe();
  bool fe2ras();
  bool packV(string a_str, double arr[], unsigned int arr_size);
  bool packE(string a_str, unsigned int arr[], unsigned int arr_size);

 private:

  // util for scientific notation
  double sciExp2double(const string src);

  // util for string to double
  template <class T>
  bool string2num(T& t,
  		const std::string& s,
  		std::ios_base& (*f)(std::ios_base&));

  // create and initialize array with values
  bool prep_array();

  // intersection test takes ref to store Barycentric uvw coordinate
  // returns bool for intersection test
  bool pixInTriangle();
  // interpolate function will interpolate a pixel point according to uvw from pixInTriangle
  bool interpolate();

  // checks tripoint rectangle for intersection and fills interpolation values in triangle
  bool fillTriangle();

  bool debug;
  string nameFileIn;
  string outfile_str;

  unsigned int ufx, ufy, ufz;
  string tokenCategory;
  string dataTypeCategory;
  string nameProject;
	// change this to map container with pointer to shared_array
  vector<feZone*>* pZoneList;
  feZone* cFeZone;
  vector<string>* var_name_list; // X Y Z inclusive
  const unsigned int prism_elementCount;
  map<string, shared_array<double>* > var_name_map;
  double fTemp, fMIN_X, fMIN_Y, fMAX_X, fMAX_Y;
  double rasXmin, rasXmax, rasYmin, rasYmax;
  unsigned int dx, dy, dz;
  string sF, sET;
  unsigned int uiT, uiN, uiE, uiSOLUTIONTIME;
  vector<feZone*>* zone_list;

};


#endif /* FEPARSE_H_ */
