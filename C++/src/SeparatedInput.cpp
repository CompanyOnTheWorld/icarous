/* 
 * SeparatedInput
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Cesar Munoz, George Hagen
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#include "Units.h"
//#include "UnitSymbols.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "format.h"
#include "Quad.h"
#include "SeparatedInput.h"
#include "string_util.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

namespace larcfm {

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;

SeparatedInput::SeparatedInputException::SeparatedInputException(
		const std::string& s) :
		std::logic_error(s) {
}

SeparatedInput::SeparatedInput() :
		error("SeparatedInput") {
	header = false;
	units = false;
	caseSensitive = true;
	//parameters = map<string,Quad<std::string,double,std::string,bool> >();
	parameters = ParameterData();
	patternStr = " \t,;";
	reader = 0;
	linenum = 0;
}

// fs must already be opened!!!
SeparatedInput::SeparatedInput(std::istream* ins) :
		error("SeparatedInput") {
	reader = ins;
	header = false;
	units = false;
	caseSensitive = true;
	header_str.reserve(10);
	units_str.reserve(10);
	units_factor.reserve(10);
	line_str.reserve(10);
	linenum = 0;
	patternStr = " \t,;";

	//parameters = map<string,Quad<std::string,double,std::string,bool> >();
	parameters = ParameterData();
}

// This should never be used, it should exit
SeparatedInput::SeparatedInput(const SeparatedInput& x) :
		error("SeparatedInputError") {
	error = x.error;
	reader = x.reader;
	header = x.header;
	units = x.units;
	caseSensitive = x.caseSensitive;
	header_str = x.header_str;
	units_str = x.units_str;
	units_factor = x.units_factor;
	line_str = x.line_str;
	linenum = x.linenum;
	patternStr = x.patternStr;
	parameters = x.parameters;

//    std::cout << "SeparatedInput copy constructor failure" << std::endl;
//    exit(1);
}

// This should never be used, it should exit
SeparatedInput& SeparatedInput::operator=(const SeparatedInput& x) {
	error = x.error;
	reader = x.reader;
	header = x.header;
	units = x.units;
	caseSensitive = x.caseSensitive;
	header_str = x.header_str;
	units_str = x.units_str;
	units_factor = x.units_factor;
	line_str = x.line_str;
	linenum = x.linenum;
	patternStr = x.patternStr;
	parameters = x.parameters;

	//std::cout << "SeparatedInput assignment operator failure" << std::endl;
	//exit(1);
	return *this;  // should never get here
}

ParameterData& SeparatedInput::getParametersRef() {
	return parameters;
}

ParameterData SeparatedInput::getParameters() const {
	return parameters;
}

string SeparatedInput::getHeading(int i) const {
	if (i < 0 || (unsigned int) i >= line_str.size()) {
		error.addWarning(
				"getHeading index " + Fm0(i) + ", line " + Fm0(linenum)
						+ " out of bounds");
		return "";
	}
	return header_str[i];
}

int SeparatedInput::findHeading(const string& heading) const {
	int rtn = -1;
	for (unsigned int i = 0; i < header_str.size(); i++) {
		if (heading.compare(header_str[i]) == 0) {
			rtn = i;
			break;
		}
	}
	return rtn;
}

string SeparatedInput::getUnit(int i) const {
	if (!units || i < 0 || (unsigned int) i >= units_str.size()) {
		return "unspecified";
	}
	return units_str[i];
}

bool SeparatedInput::unitFieldsDefined() const {
	return units;
}

void SeparatedInput::setCaseSensitive(bool b) {
	caseSensitive = b;
	parameters.setCaseSensitive(b);
}

bool SeparatedInput::getCaseSensitive() const {
	return caseSensitive;
}

double SeparatedInput::getUnitFactor(int i) const {
	if (!units || i < 0 || (unsigned int) i >= units_str.size()) {
		return Units::getFactor("unspecified");
	}
	return units_factor[i];
}

//  void SeparatedInput::clearParameters() {
//    parameters.clear();
//  }
//
//  bool SeparatedInput::containsParameter(const string& str) const {
//    string key = str;
//    if (!caseSensitive) key = toLowerCase(str);
//    return parameters.find(key) != parameters.end();
//  }
//
//  string SeparatedInput::getParameterString(const string& str) const {
//    string key = str;
//    if (!caseSensitive) key = toLowerCase(str);
//    if (parameters.find(key) == parameters.end()) {
//      return "";
//    } else {
//      return parameters.find(key)->second.getFirst();
//    }
//  }
//
//  double SeparatedInput::getParameterValue(const string& str) const {
//    string key = str;
//    if (!caseSensitive) key = toLowerCase(str);
//    if (parameters.find(key) == parameters.end()) {
//      return 0.0;
//    } else {
//      return parameters.find(key)->second.getSecond();
//    }
//  }
//
//  double SeparatedInput::getParameterValue(const string& str, const string& defaultUnit) const {
//    string key = str;
//    if (!caseSensitive) key = toLowerCase(str);
//    if ( ! containsParameter(key)) {
//      return 0.0;
//    }
//    double val = getParameterValue(key);
//    if (getParameterUnit(key) == "unspecified") {
//      val = Units::from(defaultUnit,val);
//    }
//    return val;
//  }
//
//  string SeparatedInput::getParameterUnit(const string& str) const {
//    string key = str;
//    if (!caseSensitive) key = toLowerCase(str);
//    if (parameters.find(key) == parameters.end()) {
//      return "unspecified";
//    } else {
//      return parameters.find(key)->second.getThird();
//    }
//  }
//
//  bool SeparatedInput::getParameterBool(const string& str) const {
//    string key = str;
//    if (!caseSensitive) key = toLowerCase(str);
//    if (parameters.find(key) == parameters.end()) {
//      return false;
//    } else {
//      return parameters.find(key)->second.getFourth();
//    }
//  }
//
//  void SeparatedInput::setParameter(const string& key, const string& value) {
//    process_preamble(key+" = "+value);
//  }
//
//  bool SeparatedInput::setParameter(const string& s) {
//	  vector<string> fields = split(s, patternStr);
//
//      // parameter keys are lower case only
//  	if (fields.size() >= 3 && (fields[1].compare("=") == 0)) {
//  		process_preamble(s);
//  		return true;
//  	}
//  	return false;
//  }

bool SeparatedInput::columnHasValue(int i) const {
	return (i >= 0 && i < (int) line_str.size() && !equals(line_str[i], "")
			&& !equals(line_str[i], "-"));
}

string SeparatedInput::getColumnString(int i) const {
	if (i < 0 || (unsigned int) i >= line_str.size()) {
		error.addWarning(
				"getColumnString index " + Fm0(i) + ", line " + Fm0(linenum)
						+ " out of bounds");
		return "";
	}
	return line_str[i];
}

double SeparatedInput::getColumn(int i) const {
	if (i < 0 || (unsigned int) i >= line_str.size()) {
		error.addWarning(
				"getColumn index " + Fm0(i) + ", line " + Fm0(linenum)
						+ " out of bounds");
		return 0.0;
	}
	double rtn = 0.0;
	try {
		rtn = Units::from(getUnitFactor(i), Util::parse_double(line_str[i]));
	}
	catch (std::runtime_error e) {
		error.addWarning(
				"could not parse double (" + Fm0(i) + "), line " + Fm0(linenum)
						+ ": " + line_str[i]);
		rtn = 0.0;  // arbitrary value
	}
	return rtn;
}

double SeparatedInput::getColumn(int i, const std::string& default_unit) const {
	if (getUnit(i) == "unspecified") {
		return Units::from(default_unit, getColumn(i));
	}

	return getColumn(i);
}

bool SeparatedInput::readLine() {
	//char* linestr = new char[maxLineSize];
	string str;
	str.reserve(maxLineSize);
	str.clear();

	try {
		while ( ! reader->eof()) {
			//reader->getline(linestr, maxLineSize);
			getline(*reader, str);
			++linenum;

	    	// Remove comments from line
	    	int comment_num = str.find('#');
	    	if (comment_num >= 0) {
	    		str = str.substr(0,comment_num);
	    	}
	    	trim(str);
	    	// Skip empty lines
	    	if (str.size() == 0) {
				str.clear();
	    		continue;
	    	}


			if (!header) {
				header = process_preamble(str);
			} else if (!units) {
				try {
					units = process_units(str);
				}
				catch (SeparatedInputException e) {
					// use default units
					units = true;
					process_line(str);
					break;
				}
			} else {
				process_line(str);
				break;
			}
			str.clear();
		}  //while
	}
	catch (std::runtime_error e) {
		error.addError(
				"*** An IO error occurred at line " + Fm0(linenum)
						+ "The error was:" + e.what());
		// ERROR CLEANUP
	}
	if (str.size() == 0) {
		return true;
	} else {
		return false;
	}
}

int SeparatedInput::lineNumber() const {
	return linenum;
}

bool SeparatedInput::process_preamble(string str) {
//
//	  vector<string> fields = split(str, patternStr);
//
//	  // parameter keys are lower case only
//	  if (fields.size() >= 3 && (fields[1].compare("=") == 0  || fields[1].compare("==") == 0) && fields[0].length() > 0) {
//		  string id = fields[0];
//          if (!caseSensitive) id = toLowerCase(id);
//
//		  // string value
//		  string strv = fields[2];
//		  for (unsigned int i = 3; i < fields.size(); i++) {
//			  strv += " " + fields[i];
//		  } // values become a space-delineated list string
//
//		  // Double value
//		  double dbl = 0.0;
//		  string unit = "unspecified";
//		  try {
//			  dbl = getd(fields[2]);
//			  if (fields.size() >= 4) {
//				  unit = Units::clean(fields[3]);
//				  dbl = Units::from(unit, dbl);
//			  }
//		  } catch (std::runtime_error e) {
//			  dbl = 0.0;  // string is not a number so use a default value
//		  }
//
//		  // Boolean value
//		  bool bval = false;
//		  if (equalsIgnoreCase(strv,"true") || equalsIgnoreCase(strv,"T")) {
//			  bval = true;
//		  }
//		  string strv2 = str.substr(str.find(fields[2],str.find("=")));
//		  trim(strv2);
//		  Quad<string,double,string,bool> quad = Quad<string,double,string,bool>::make(strv2, dbl, unit, bval);
//		  // Quad<string,double,string,bool> quad = Quad<string,double,string,bool>::make(strv, dbl, unit, bval);
//
//		  parameters[id]=quad;
//
//		  return false;
//	  } else {
//          if (!caseSensitive) {
//            for (unsigned int i = 0; i < fields.size(); i++) fields[i] = toLowerCase(fields[i]);
//          }
//		  header_str = fields;
//		  return true;
//	  }
//  }

	vector<string> fields = split(str, "=");

	// parameter keys are lower case only
	if (fields.size() >= 2
			&& /*(fields[1].compare("=") == 0  || fields[1].compare("==") == 0) &&*/fields[0].length()
					> 0) {
		string id = fields[0];
		trim(id, " \t");
		if ( ! caseSensitive)
			id = toLowerCase(id);

		// string value
		string strv = fields[1];
		for (unsigned int i = 2; i < fields.size(); i++) {
			strv += "=" + fields[i];
		} // values become a space-delineated list string

//	  // Double value
//	  double dbl = 0.0;
//	  string unit = "unspecified";
//	  try {
//		  dbl = getd(fields[2]);
//		  if (fields.size() >= 4) {
//			  unit = Units::clean(fields[3]);
//			  dbl = Units::from(unit, dbl);
//		  }
//	  } catch (std::runtime_error e) {
//		  dbl = 0.0;  // string is not a number so use a default value
//	  }
//
//	  // Boolean value
//	  bool bval = false;
//	  if (equalsIgnoreCase(strv,"true") || equalsIgnoreCase(strv,"T")) {
//		  bval = true;
//	  }
//	  string strv2 = str.substr(str.find(fields[2],str.find("=")));
//	  trim(strv2);
//	  Quad<string,double,string,bool> quad = Quad<string,double,string,bool>::make(strv2, dbl, unit, bval);
//	  // Quad<string,double,string,bool> quad = Quad<string,double,string,bool>::make(strv, dbl, unit, bval);
//
//	  parameters[id]=quad;

		parameters.set(id, strv);

		return false;
	} else {
		fields = split(str, patternStr);
		if ( ! caseSensitive) {
			for (unsigned int i = 0; i < fields.size(); i++) {
				fields[i] = toLowerCase(fields[i]);
				trim(fields[i]);
			}
		}
		header_str = fields;
		return true;
	}
}

bool SeparatedInput::process_units(const string& str) {
	vector<string> fields = split(str, patternStr);

	// if units are optional, we need to determine if any were read in...
	// a unit line is considered true if AT LEASE HALF of the fields read in are interpreted as valid units
	unsigned int notFound = 0;

	for (unsigned int i = 0; i < fields.size(); i++) {
		std::string u = Units::clean(fields[i]);
		double factor = Units::getFactor(u);
		if (u == "unspecified") {
			notFound++;
			units_str.push_back("unspecified");
			units_factor.push_back(Units::getFactor("unspecified"));
		} else {
			units_str.push_back(u);
			units_factor.push_back(factor);
		}
	}
	if (notFound > fields.size() / 2) {
		throw SeparatedInputException("default units");
	}
	return true;
}

void SeparatedInput::process_line(const string& str) {
	vector<string> fields = split(str, patternStr);
	for (int i = 0; i < (int) fields.size(); i++) {
		trim(fields[i]);
	}
	line_str = fields;
}

string SeparatedInput::getLine() const {
	string s = "";
	if (line_str.size() > 0) {
		s = line_str[0];
	}
	for (int i = 0; i < (int) line_str.size(); i++) {
		s += ", "+line_str[i];
	}
	return s;
}

//  vector<string> SeparatedInput::getParameterList() const {
//    paramtype::const_iterator pos;  //aaaarrrrggggg!!!
//    vector<string> v(parameters.size());
//    // TODO: check for empty entries in this list
//    for (pos = parameters.begin(); pos != parameters.end(); ++pos) {
//      v.push_back(pos->first);
//    }
//    return v;
//  }
//
//  vector<string> SeparatedInput::unrecognizedParameters(vector<string> v) const {
//    vector<string> p = getParameterList();
//    vector<string>::iterator pos;
//    unsigned int i = 0;
//    while (i < v.size()) {
//        if (!caseSensitive) {
//        	v[i] = toLowerCase(v[i]);
//        }
//        pos = find(p.begin(), p.end(), v[i]);
//        if (pos != p.end()) {
//        	p.erase(pos);
//        } else {
//        	i++;
//        }
//    }
//
//    return p;
//  }

}
