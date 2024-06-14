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
	 sample ztt_taumu		         ("Z#rightarrow #tau_{3#mu}#tau_{#mu}"					, col_rwth_lightblue, "MC3"); 



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
	 samples.push_back(ztt_taumu); 


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

  plots.push_back( plotInfo("PostSelection_Vertex_Chi2", "GeV") );  
  plots.push_back( plotInfo("PostSelection_Tau3MuRelativeIsolation", "") );
  plots.push_back( plotInfo("PostSelection_OppositeMuRelativeIsolation", "") );
  plots.push_back( plotInfo("PostSelection_VisibleDiTauMass", "") );
  plots.push_back( plotInfo("PostSelection_TripletPt", "") );
  plots.push_back( plotInfo("PostSelection_TripletEta", "") );
  plots.push_back( plotInfo("PostSelection_MET_Et", "") );
  plots.push_back( plotInfo("PostSelection_MET_Phi", "") );
  plots.push_back( plotInfo("PostSelection_Mu1_Pt", "") );
  plots.push_back( plotInfo("PostSelection_Mu1_Eta", "") );
  plots.push_back( plotInfo("PostSelection_Mu2_Pt", "") );
  plots.push_back( plotInfo("PostSelection_Mu2_Eta", "") );
  plots.push_back( plotInfo("PostSelection_Mu3_Pt", "") );
  plots.push_back( plotInfo("PostSelection_Mu3_Eta", "") );
  plots.push_back( plotInfo("PostSelection_mu_Pt", "") );
  plots.push_back( plotInfo("PostSelection_mu_Eta", "") );
  plots.push_back( plotInfo("PostSelection_Pair_Pt", "") );
  plots.push_back( plotInfo("PostSelection_Pair_Eta", "") );
  plots.push_back( plotInfo("PostSelection_FLSignificance", "") );
  plots.push_back( plotInfo("PostSelection_SVPVTauDirAngle", "") );
  plots.push_back( plotInfo("PostSelection_VertexChi2KF", "") );
  plots.push_back( plotInfo("PostSelection_MinDistToIsoTrack", "") );
  plots.push_back( plotInfo("PostSelection_Kinematics_MissingTrMass_cos", "") );
  plots.push_back( plotInfo("PostSelection_VisibleDiTauMass_Collinear", "") );
  

	return plots;
}


#endif
