/*
 * feParse.cpp
 *
 *  Created on: May 19, 2010
 *      Author: Makoto Sadahiro
 */

#include "feParse.h"

using namespace std;
using namespace boost;

feParse::feParse(int debug_flag, vector<feZone*>* zone_list, char** arg_v) :
	debug(debug_flag),
//	pZoneList(zone_list),
	nameFileIn(arg_v[1]),
	// change this to map container with pointer to shared_array
	var_name_list(new vector<string> ),
	prism_elementCount(8){

	// user defined resolutions for the fe raster field
  ufx = atoi(arg_v[2]);
  ufy = atoi(arg_v[3]);
  ufz = atoi(arg_v[4]);

	if(debug){
		cout << "debug: feParse() nameFileIn=" << nameFileIn << endl;
	}
	pZoneList = zone_list;

	// making directories for output
	stringstream outfile_strs;
	outfile_strs << nameFileIn << "_fx" << ufx << "_fy" << ufy << "_fz" << ufz << "_ts";// << "_Output/";
	outfile_str = outfile_strs.str();
//	string command_str = "mkdir " + outfile_str;
//	cout << "mkdir: " << command_str << endl;
//	system(command_str.c_str());

}

feParse::~feParse(){

	// change this to map container with pointer to shared_array
	// delete variable name list
	delete var_name_list;

	if(debug){
		cout << "debug: exiting ~feParse()" << endl;
	}
}


// util for converting scientific expression to double
double feParse::sciExp2double(const string src){

	double base, exp;

	string::size_type idx = src.find_last_of("e");
	if(idx == string::npos){
		cerr << "filename does not appear to be valid." << endl;
		return false;
	}else{
		istringstream issBase_num(src.substr(0, idx));
		istringstream issExp(src.substr(idx + 1, 3));
		issBase_num >> base;
		issExp >> exp;
	}

	return base * pow(10, exp);
}


template<class T> bool feParse::string2num(
		T& t,
		const std::string& s,
		std::ios_base& (*f)(std::ios_base&)){

	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}


bool feParse::tec2fe(){

	ifstream ifstreamFileIn;
	ifstreamFileIn.open(nameFileIn.c_str());

	string s_line;
	string s_token;
	char_separator<char> sep_char(",=\" ");

	tokenCategory = "";
	dataTypeCategory = "newDataLine";

	while(ifstreamFileIn){
		if(dataTypeCategory == "newDataLine"){
			getline(ifstreamFileIn, s_line);
			tokenCategory = "newLine";
			tokenizer<char_separator<char> > tok(s_line, sep_char);

			for(tokenizer<char_separator<char> >::iterator beg = tok.begin();
					beg != tok.end();
					++beg){

				s_token = *beg;
				if(tokenCategory == "newLine"){
					if((s_token == "TITLE") || (s_token == "VARIABLES")
							|| (s_token == "DATASETAUXDATA") || (s_token == "ZONE")
							|| (s_token == "VARSHARELIST") || (s_token == "CONNECTIVITYSHAREZONE")){
						tokenCategory = s_token;

						if(tokenCategory == "ZONE"){
							cFeZone = new feZone();
							pZoneList->push_back(cFeZone);
						}

						cout << "Category: " << tokenCategory << endl;
					}else{
						cout << "Encountered an unsupported meta data type as below." << endl
								<< "***" << s_token << "***" << endl;
						sleep(1);
						return false;
					}
				} // if(tokenCategory == "newLine")
				else{
					if((tokenCategory == "TITLE") || (tokenCategory == "DATASETAUXDATA")
					    || (tokenCategory == "ZONE")){
						cout << "." << tokenCategory << ": " << s_token << endl;
						tokenCategory = s_token;
					}
					else{
						if(tokenCategory == "VARIABLES"){
							cout << "." << tokenCategory << ": " << s_token << endl;
							// change this to map container with pointer to shared_array
							var_name_list->push_back(s_token);
						}
						else if(tokenCategory == "MIN_X"){
//							if(string2num<typeof(fTemp)> (fTemp, s_token, std::dec)){
								fMIN_X = atof(s_token.c_str());//fTemp;
//							}
//							else{
//								cerr << "string to double conversion failed" << endl;
//								return false;
//							}
						}
						else if(tokenCategory == "MAX_X"){
							if(string2num<typeof(fTemp)> (fTemp, s_token, std::dec)){
								fMAX_X = atof(s_token.c_str());//fTemp;
							}
//							else{
//								cerr << "string to double conversion failed" << endl;
//								return false;
//							}
						}else if(tokenCategory == "MIN_Y"){
//							if(string2num<typeof(fTemp)> (fTemp, s_token, std::dec)){
								fMIN_Y = atof(s_token.c_str());//fTemp;
//							}
//								else{
//								cerr << "string to double conversion failed" << endl;
//								return false;
//							}
						}else if(tokenCategory == "MAX_Y"){
//							if(string2num<typeof(fTemp)> (fTemp, s_token, std::dec)){
								fMAX_Y = atof(s_token.c_str());//fTemp;
//							}
//								else{
//								cerr << "string to double conversion failed" << endl;
//								return false;
//							}
//						}else if(tokenCategory == "MIN_Z"){
//							if(string2num<typeof(fTemp)> (fTemp, s_token, std::dec)){
//								fMIN_Z = fTemp;
//							}
//								else{
//								cerr << "string to double conversion failed" << endl;
//								return false;
//							}
//						}
//								else if(tokenCategory == "MAX_Z"){
//							if(string2num<typeof(fTemp)> (fTemp, s_token, std::dec)){
//								fMAX_Z = fTemp;
//							}
//								else{
//								cerr << "string to double conversion failed" << endl;
//								return false;
//							}
						}else if(tokenCategory == "F"){
							sF = s_token;
							cout << ".." << tokenCategory << ": " << sF << endl;
							tokenCategory = "ZONE";
						}else if(tokenCategory == "ET"){
							sET = s_token;
							cout << ".." << tokenCategory << ": " << sET << endl;
							tokenCategory = "ZONE";
						}else if(tokenCategory == "T"){
							cout << ".." << tokenCategory << ": ";
							tokenCategory = s_token;
						}else if(tokenCategory == "Time"){
							uiT = (unsigned int)(atoi(s_token.c_str()));
							cout << uiT << endl;
							tokenCategory = "ZONE";
						}else if(tokenCategory == "N"){
							uiN = (unsigned int)(atoi(s_token.c_str()));
							cout << ".." << tokenCategory << ": " << uiN << endl;
							tokenCategory = "ZONE";
						}else if(tokenCategory == "E"){
							uiE = (unsigned int)(atoi(s_token.c_str()));
							cout << ".." << tokenCategory << ": " << uiE << endl;
							tokenCategory = "ZONE";
						}else if(tokenCategory == "SOLUTIONTIME"){
							uiSOLUTIONTIME = (unsigned int)(atoi(s_token.c_str()));
							cout << ".." << tokenCategory << ": " << uiSOLUTIONTIME << endl;
							dataTypeCategory = "zoneData";
						}
						// mandetory assignment of curr category
						if(tokenCategory == "VARIABLES"){
							// do nothing to retain tokenCatogory as "VARIABLES" while there are
						}
						else{
							tokenCategory = s_token;
						}
					}
				} // else(tokenCategory == "newLine")
			}

			cout << "--\n";
		} //if(dataTypeCategory == "newDataLine")
		else if(dataTypeCategory == "zoneData"){
			getline(ifstreamFileIn, s_line);
			tokenCategory = "newLine";
			tokenizer < char_separator<char> > tok(s_line, sep_char);
			tokenizer<char_separator<char> >::iterator bt = tok.begin();

			std::istringstream inpStream(*bt);
			double inpValue = 0.0;
			if(inpStream >> inpValue){
				cout << "a_number" << endl;
			}else{
				cout << "NOT_a_number" << endl;
			}

			double tarrV [var_name_list->size()];
			unsigned int tarrE [prism_elementCount];

			if((*bt) == "VARSHARELIST"){
				cout << "VARSHARELIST" << endl;
				cFeZone->config(
						outfile_str,
						var_name_list,
						prism_elementCount,
						uiSOLUTIONTIME,
						fMIN_X, fMIN_Y,
						fMAX_X, fMAX_Y,
						true,
						pZoneList->front(),
						uiN, uiE);
				for(tokenizer<char_separator<char> >::iterator beg = tok.begin(); beg
				    != tok.end(); ++beg){
					s_token = *beg;
					cout << " " << s_token;
				}
				cout << endl;
				for(unsigned int i = 0; i < uiN; i++){
					getline(ifstreamFileIn, s_line);
					if(packV(s_line, tarrV, var_name_list->size())){
						cFeZone->pushLineV(tarrV);
					}
					else{
						cout << "failed packV" << endl;
					}
//				cout << s_line << endl;
				}
			}else{
				cout << "NON-VARSHARELIST" << endl;
				cFeZone->config(
						outfile_str,
						var_name_list,
						prism_elementCount,
						uiSOLUTIONTIME,
						fMIN_X, fMIN_Y,
						fMAX_X, fMAX_Y,
						false,
						pZoneList->front(),
						uiN, uiE);
//				cout << 0 << "  : " << s_line << endl;
				if(packV(s_line, tarrV, var_name_list->size())){
					cFeZone->pushLineV(tarrV);
				}
				else{
					cout << "failed packV" << endl;
				}
//				cout << "0: " << s_line << endl;
				for(unsigned int i = 0; i < uiN - 1; i++){
					getline(ifstreamFileIn, s_line);
//					cout << i + 1 << "  : " << s_line << endl;
					if(packV(s_line, tarrV, var_name_list->size())){
						cFeZone->pushLineV(tarrV);
					}
					else{
						cout << "failed packV" << endl;
					}
				}
//				cout << "---" << endl;
				for(unsigned int i = 0; i < uiE; i++){
					getline(ifstreamFileIn, s_line);
					if(packE(s_line, tarrE, prism_elementCount)){
						cFeZone->pushLineE(tarrE);
					}
					else{
						cout << "failed packE" << endl;
					}
//					cout << i << ": " << s_line << endl;
				}
//				cout << "===" << endl;
			}
			dataTypeCategory = "newDataLine";
			//					return false;
			cout << endl;
		} //else if(dataTypeCategory == "zoneData")

	} //while(ifstreamFileIn)

	ifstreamFileIn.close();

	// test section
	if(debug){
		cout << "var name list size: " << var_name_list->size() << endl;
		for(unsigned int i = 0; i < var_name_list->size(); i++){
			cout << " " << var_name_list->at(i);
		}
		cout << endl << "exiting feParse::tec2fe" << endl << endl;
	}

	return true;
}


bool feParse::packV(string a_str, double arr[], unsigned int arr_size){

	unsigned int ic = 0;
	char_separator<char> sep_char(",=\" ");
	tokenizer < char_separator<char> > tok(a_str, sep_char);

//	cout << "packV: ";
	for(tokenizer<char_separator<char> >::iterator beg = tok.begin(); beg
	    != tok.end(); ++beg){
		if(ic >= arr_size){
			return false;
		}
		// this is original before switching to boost
		//		arr[ic++] = sciExp2double(*beg);
		arr[ic++] = boost::lexical_cast<double>(*beg);
//		cout << "  " << fixed << setprecision( 8 ) << arr[ic-1];
	}
//	cout << endl;
	return true;
}


bool feParse::packE(string a_str, unsigned int arr[], unsigned int arr_size){

	unsigned int ic = 0;
	char_separator<char> sep_char(",=\" ");
	tokenizer < char_separator<char> > tok(a_str, sep_char);

//	cout << "packE: ";
	for(tokenizer<char_separator<char> >::iterator beg = tok.begin(); beg
	    != tok.end(); ++beg){
		if(ic >= arr_size){
			return false;
		}
		arr[ic++] = atoi((*beg).c_str());
//		cout << "  " << fixed << arr[ic-1];
	}
//	cout << endl;
	return true;
}


bool feParse::fe2ras(){
	if(debug){
		cout << "debug: feParse::fe2ras: " << endl;
	}

	if(pZoneList->empty()){
		cout << "zone list is empty" << endl;
	}
	else{
		cout << "zone size: " << pZoneList->size() << endl;
	}

	for(unsigned int i = 0; i < pZoneList->size(); ++i){
		if((pZoneList->at(i))->prefe2ras(ufx, ufy, ufz)){
			(pZoneList->at(i))->fe2ras();
			(pZoneList->at(i))->spoolAllDrasBin();
		}
		else{
			return false;
		}
	}

	cout << "fe2ras parser finished." << endl;

	return true;
}

