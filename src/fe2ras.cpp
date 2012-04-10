//============================================================================
// Name        : fe2ras.cpp
// Author      : Makoto Sadahiro
// Version     :
// Copyright   : Texas Advanced Computing Center
// Description : rasterizing utility for finite element mesh
//============================================================================

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "feParse.h"

/* implicit
 * #include "feZone.h"
 */

using namespace std;


void print_usage(){
  cout << "usage: fe2ras srcDataFile float:dx float:dy float:dz" << endl;
}

/* constant for version, debug and etc */
// current version number;  change it here
const string fe2ras_version= "alpha 0.1";
const string fe2ras_build_date= __DATE__;
const string fe2ras_build_time= __TIME__;

// Global debug flag: turn this on/off to enable/disable debug flag globally
bool g_debug= true;

int main(int argc, char *argv[]) {

	const int parameter_count = 5;
	// fe2ras srcDataFile float:res_x float:res_y float:res_z

	cout << "fe2ras: " << fe2ras_version << endl
			 << "     build " << fe2ras_build_date << " " << fe2ras_build_time << endl
			 << "*current version of fe2ras does not support z-slicing.  argv_4 has no effect.";

	if(argc != parameter_count){
		print_usage();
		return EXIT_FAILURE;
	}

	string argv_1 = argv[1];
	if(("-help" == argv_1)||("--help"==argv_1)){
		print_usage();
    return EXIT_FAILURE;
  }

	// vector for the handles to zones
	vector<feZone*>* zone_list = new vector<feZone*>();

  // create parser
	feParse* pFeParser = new feParse(g_debug, zone_list, argv);

  // parsing input file
  if(!(pFeParser->tec2fe())){
  	cout << "error: feParse::tec2fe" << argv[1] << endl << endl;
  }
  if(!(pFeParser->fe2ras())){
  	cout << "error: feParse::fe2ras" << argv[1] << endl << endl;
  }

  if(pFeParser != NULL){
  	cout << "deleting pFeParser" << endl;
  	delete pFeParser;
  	pFeParser= NULL;
  }

  // delete zone_list
  cout << "deleting zone - " << zone_list->size() << endl;
  while(!zone_list->empty()){
  	delete zone_list->back();
  	zone_list->pop_back();
  }
  cout << " zones deletion finished" << endl;

  cout << "exiting fe2ras ." << endl;
	return EXIT_SUCCESS;
}
