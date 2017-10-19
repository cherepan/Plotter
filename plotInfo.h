#ifndef plotInfo_h
#define plotInfo_h

/////////////////////////////////////////////////
//
// plotInfo: struct to hold information on each plot to be produced
// to be used in plotting macro
//
/////////////////////////////////////////////////

struct plotInfo{
	TString identifier;
	TString unit;
	bool log;
	unsigned rebin;
	double xRangeLow;
	double xRangeHigh;
	double yRangeLow;
	double yRangeHigh;

	/* ideas for further members:
	int nLegColumn; //number of columns in legend
	double xLegPos; //position of legend
	double yLegPos;
	*/
	
	// constructors
	// default constructor
	plotInfo() {};
	// main constructor
	plotInfo(TString Identifier, TString Unit, bool Log = false, int Rebin = 1, double XRangeLow = -9.9, double XRangeHigh = -9.9){
		identifier = Identifier;
		unit = Unit;
		log = Log;
		rebin = Rebin;
		xRangeLow = XRangeLow;
		xRangeHigh = XRangeHigh;

		// members set to default values have to be set manually
		yRangeLow = -9;
		yRangeHigh = -9;
	}
};

// test validity of plotInfo vector
bool testPlots(std::vector<plotInfo> plots){
	if(plots.size() < 1){
		std::cout << "ERROR: No plots to draw." << std::endl;
		return false;
	}
	for(unsigned p = 0; p < plots.size(); p++){
		plotInfo plot = plots.at(p);
		if(plot.identifier == ""){
			std::cout << "ERROR: Plot has no identifier." << std::endl;
			return false;
		}
	}

	return true;
}

#endif

