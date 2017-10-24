#ifndef userConfig_h
#define userConfig_h

/////////////////////////////////////////////////
//
// userConfig: main place for user input
// define all needed stuff here
//
/////////////////////////////////////////////////

#include "configInfo.h"

// flags for testing
bool verbose = false;
bool testPlotting = false;

// draw signal as overlaying curve (or stacked on top)
bool overlaySignal = false;

// create configInfo struct
double lumi = 19712.;
TString infile = "LOCAL_COMBINED_ztauhtauh_default_LumiScaled.root";
TString ident = "ztauhtauh_default_";
//TString ident = "ztotauhtaumu_default_Cut_10_Nminus1_";
bool isLumiScaled = true; // specify if input root file is already scaled to lumi

// define user-specific config
configInfo conf(infile, isLumiScaled, ident, lumi);

#endif