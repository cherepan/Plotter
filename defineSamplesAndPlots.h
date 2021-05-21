#ifndef defineSamplesAndPlots_h
#define defineSamplesAndPlots_h


/////////////////////////////////////////////////
//
// defineSamplesAndPlots:
// declare which plots you want to draw using which samples
// this cannot go into userConfig.h, as it would cause circular dependencies
//
/////////////////////////////////////////////////

#include "userConfig.h"
#include "sample.h"
#include "plotInfo.h"
#include "plottingHelpers.h"

// define samples to use
std::vector<sample> defineSamples(){
	 sample s_ds		         ("D_{s}#rightarrow #tau_{3#mu}#nu"					, col_htt_qcd, "MC1"); 
	 sample s_b0	            	 ("B^{0}#rightarrow #tau_{3#mu}#nu + X"					, col_rwth_green, "MC2"); 
	 sample s_bp		         ("B^{#pm}#rightarrow #tau_{3#mu}#nu + X"					, col_rwth_yellow, "MC3"); 
	 sample s_etamumugamma           ("D_{s}#rightarrow#eta_{#mu#mu#gamma}#mu#nu"			        , col_htt_tt, "MC6"); 
	 sample s_phimumugamma           ("D_{s}#rightarrow#phi_{#mu#mu#gamma}#mu#nu"			        , col_htt_W, "MC10"); 
	 sample s_etamumu                ("D_{s}#rightarrow#eta_{#mu#mu}#mu#nu"			                , col_rwth_magenta, "MC7"); 
	 sample s_etaprimemumugamma      ("D_{s}#rightarrow#eta'_{#mu#mu}#mu#nu"		                , col_rwth_red, "MC8"); 
	 sample s_omega                  ("D_{s}#rightarrow#omega_{#mu#mu#pi}#mu#nu"			        , col_rwth_orange, "MC9"); 



	// *** examples for sample creation when scaling manually ***
	// std::map<int, int> MnEvents = readSkimSummary("mySkimSummary.log");
	// sample s_ggHiggs(11,"H_{gg}", 1.233664, lumi, 632, "MC_ggFHTauTauM125", MnEvents );
	// sample s_vbfHigg(12,"H_{VBF}", 0.0997296, lumi, 633, "MC_VBFHTauTauM125", MnEvents );
	// sample s_apHiggs(13,"H_{AP}", 0.0771792, lumi, 634, "MC_WHZHTTHTauTauM125", MnEvents );


	// combine samples for plotting

	 //sample s_dytt(s_ztt, "DY(#tau#tau)", col_htt_Ztt); 
	 //sample s_dyll(s_zll, "DY(ee/#mu#mu)", col_rwth_lightblue);
	/* s_dy += s_zll; */
	 //	 sample s_ewk(s_ww, "di-bosons", col_htt_W); 
	 // s_ewk += s_zz4l;  
	 //s_ewk += s_zz2l2nu;  
	 //s_ewk += s_zz2l2q;  
	  //	  s_ewk += s_Wlnu;  
	 /* s_ewk += s_Wtaunu;  */
	 //sample s_top(s_ttbar, "t#bar{t}", col_htt_tt); 





	// define which samples to plot and in which order:
	// sample pushed back later will be drawn later, i.e. on top.
	// All signal samples MUST be added after all background samples! Add them last.
	 std::vector<sample> samples;
	 samples.push_back(s_ds); 
	 samples.push_back(s_b0); 
	 samples.push_back(s_bp); 
	 samples.push_back(s_etamumugamma); 
	 samples.push_back(s_phimumugamma); 

	 samples.push_back(s_etamumu); 
	 samples.push_back(s_etaprimemumugamma); 
	 samples.push_back(s_omega); 

	if (verbose){
		std::cout << "###>-- The following samples will be plotted:" << std::endl;
		for(unsigned i = 0; i<samples.size(); i++){
			printSample(samples.at(i), conf);
		}
	}

	return samples;
}


// define which plots to draw
// look at plotInfo.h to see which constructors exist
std::vector<plotInfo> definePlots(){
  std::vector<plotInfo> plots;
  //plots.push_back( plotInfo("HiggsGenMass", "GeV", true, 1, 108, 112));
  //	plots.push_back( plotInfo("DataMCType", "", true, 1, 11010., 16013.));
  //	plots.push_back( plotInfo("DataMCTypeFromNtuple", "", true, 1, 0., 20000.));
  //  plots.push_back( plotInfo("TransverseMass", "GeV", false, 1));
  
  plots.push_back( plotInfo("TauMassRefitABC1", "GeV") );  
  plots.push_back( plotInfo("TauMassRefitABC2", "GeV") );  
  
  plots.push_back( plotInfo("TauMassRefitABC1_BDSeparateTrain", "GeV") );  
  plots.push_back( plotInfo("TauMassRefitABC2_BDSeparateTrain", "GeV") );  




  //	plots.push_back( plotInfo("Cut_10_Nminus1_MT_", "GeV") );
//	plots.push_back( plotInfo("MtAfterOppCharge", "GeV", false, 2) );
//	plots.push_back( plotInfo("MetPt", "GeV", false, 1, 0., 80.));
//
//	plots.push_back( plotInfo("HiggsPt", "GeV") );
//	plots.push_back( plotInfo("HiggsPhi", "GeV") );
//	plots.push_back( plotInfo("TauSelEta", "GeV") );
//	plots.push_back( plotInfo("VtxRho", "", true, 1, 0., 1.) );
//	plots.push_back( plotInfo("VtxZ", "") );

	return plots;
}


#endif
