#ifndef plottingHelpers_h
#define plottingHelpers_h

/////////////////////////////////////////////////
//
// plotting helper functions
// to be used in plotting macro
//
/////////////////////////////////////////////////

#include <vector>
#include "TMath.h"
#include "THStack.h"
#include "TSystem.h"

// include struct definitions
#include "sample.h"
#include "plotInfo.h"
#include "configInfo.h"
#include "userConfig.h"

// some global settings
int errorBandFillStyle = 3013;
int errorBandColor = kGray + 2;

// test definiton of structs
void testInputs(configInfo config, std::vector<sample> samples, std::vector<plotInfo> plots){
	bool isValid = true;
	isValid &= testConfigInfo(config, verbose);
	isValid &= testSamples(samples);
	isValid &= testPlots(plots);
	if(!isValid){
		std::cout << "========== WARNING: Your inputs (configInfo, plotInfo or sample) seem to be corrupted! ==========" << std::endl;
	}
}

void manipulateHisto(TH1D* hist, const plotInfo plot){
	hist->Rebin(plot.rebin);
	if(plot.xRangeLow != -9.9) hist->GetXaxis()->SetRangeUser(plot.xRangeLow, hist->GetXaxis()->GetXmax());
	if(plot.xRangeHigh != -9.9) hist->GetXaxis()->SetRangeUser(hist->GetXaxis()->GetXmin(), plot.xRangeHigh);
}

TH1D* getHisto(configInfo config, TString name){
	if(verbose) std::cout << "--> getHisto(configInfo, TString): " << config.prefix+name << std::endl;
	TH1D* hist = ((TH1D*)config.file->Get(config.prefix+name));
	return hist;
}

TH1D* getHisto(configInfo config, TString name, double scale, int color){
	if(verbose) std::cout << "--> getHisto(configInfo, TString, double, int): " << name << std::endl;
	TH1D* hist = getHisto(config, name);
	hist->Scale(scale);
	hist->SetFillColor(color);
	hist->SetLineColor(1);
	hist->SetLineWidth(1);
	return hist;
}

TH1D* getHisto(configInfo config, TString name, double scale, int color, double systematic){
	if(verbose) std::cout << "--> getHisto(configInfo, TString, double, int, double)" << std::endl;
	TH1D* hist = getHisto(config, name, scale, color);
	for(int i=1;i<hist->GetNbinsX();i++){
		hist->SetBinError(i,TMath::Sqrt(TMath::Power(hist->GetBinError(i),2)+TMath::Power(systematic*hist->GetBinContent(i),2)));
	}
	if(name.Contains("DY_emu")){ // todo: generalize. introduce "isSignal" in sample struct?
	//if(name.Contains("emu_DY")){
		hist->SetFillStyle(0);
		hist->SetLineStyle(7);
		hist->SetLineColor(kBlack);
		hist->SetLineWidth(3);
	}
	return hist;
}

//todo: not adapted yet
/*
std::vector<TH1D*> getHistos(std::vector<TString> names, std::vector<double> scale, std::vector<int> color, TFile* file){
	std::vector<TH1D*> histos;
	for(unsigned i=0;i<names.size();i++){
		histos.push_back(getHisto(names.at(i),scale.at(i),color.at(i),file));
	}
	return histos;
}
*/

TH1D* getDataMC(TH1D* datahist, std::vector<TH1D*> MChists){
	if(verbose) std::cout << "--> getDataMC(...)" << std::endl;
	int nbins = datahist->GetNbinsX();
	double xlow = datahist->GetXaxis()->GetXmin();
	double xhigh = datahist->GetXaxis()->GetXmax();
	TString name = (TString)datahist->GetName()+"_MCRatio";
	TH1D* hist = new TH1D(name.Data(),";;Data/MC",nbins,xlow,xhigh);

	for(int i=1;i<=nbins;i++){
		double data = datahist->GetBinContent(i);
		double dataerror = datahist->GetBinError(i);
		double mc = 0;
		double mcerr = 0;
		for(unsigned j=0;j<MChists.size();j++){
			mc+=MChists.at(j)->GetBinContent(i);
			mcerr+=TMath::Power(MChists.at(j)->GetBinError(i),2);
		}
		double mcerror = TMath::Sqrt(mcerr);
		if(mc>0){
			hist->SetBinContent(i,data/mc);
			//hist->SetBinError(i,TMath::Sqrt(pow(dataerror/mc,2)+pow(data*mcerror/pow(mc,2),2)));
			hist->SetBinError(i,dataerror/mc);
		}
	}
	double min = 999;
	for(int i=1;i<nbins;i++){
		if(hist->GetBinContent(i)>0){
			if(hist->GetBinContent(i)<min)min=hist->GetBinContent(i);
		}
	}
	double max = hist->GetMaximum();
	if(max>2)max=1.5;
	if(min<0.5)min=0.5;
	//hist->GetYaxis()->SetRangeUser(min-0.2,max+0.2);
	hist->GetYaxis()->SetRangeUser(0.7,1.3);
	hist->GetXaxis()->SetTitle(datahist->GetXaxis()->GetTitle());
	return hist;
}

THStack* produceHistStack(std::vector<TH1D*> histos){
	if(verbose) std::cout << "--> produceHistStack(...)" << std::endl;
	THStack* stack = new THStack("stack","stack");
	for(unsigned i=0;i<histos.size();i++){ //
		stack->Add(histos.at(i));
	}
	return stack;
}

//todo: should in principal be obsolete, since total histo can be accessed from stack
TH1D* produceTotal(std::vector<TH1D*> histos){
	if(verbose) std::cout << "--> produceTotal(...)" << std::endl;
	TString name = (TString)histos.at(0)->GetName() + "_TotalMC";
	TH1D* total = new TH1D(name.Data(),"total",histos.at(0)->GetNbinsX(),histos.at(0)->GetXaxis()->GetXmin(),histos.at(0)->GetXaxis()->GetXmax());
	total->Sumw2();
	for(unsigned i=0;i<histos.size();i++){
		total->Add(histos.at(i));
	}
	total->SetFillStyle(errorBandFillStyle);
	total->SetFillColor(errorBandColor);
	total->SetLineColor(18);
	total->SetMarkerColor(1);
	total->SetMarkerSize(0.001);
	return total;
}

//todo: add systematics
TH1D* getHistoFromSample(configInfo conf, plotInfo p, sample s){
	if(verbose) std::cout << "--> getHistoFromSample(...)" << std::endl;
	TH1D* tmp = getHisto(conf, p.identifier + s.identifier.at(0), 1, s.color);
	TH1D* histo = new TH1D(s.legName+"_"+p.identifier,s.legName,tmp->GetNbinsX(),tmp->GetXaxis()->GetXmin(),tmp->GetXaxis()->GetXmax());
	histo->Sumw2();
	// add up subsamples of this sample
	for(unsigned ss = 0; ss < s.identifier.size(); ss++){
		histo->Add(getHisto(conf, p.identifier + s.identifier.at(ss), s.mcScale.at(ss), s.color));
	}
	histo->SetFillColor(s.color);
	// todo: is this a good place to do the rebinning and xRange setting? or move to getHisto?
	manipulateHisto(histo, p);
	//histo->SetLineColor(1);
	return histo;
}

//todo: add systematics
std::vector<TH1D*> getHistos(configInfo conf, plotInfo p, std::vector<sample> samples, bool getSignals){
	if(verbose) std::cout << "--> getHistos(configInfo, plotInfo, std::vector<sample>, bool)" << std::endl;
	std::vector<TH1D*> histos;

	// merge signal histos into BG histos when signal is stacked
	if (!overlaySignal && getSignals) return histos;

	for(unsigned s = 0; s < samples.size(); s++){
		if (overlaySignal){
			// if getSignals = true, only signal samples are returned
			// if getSignals = false, only background samples are returned
			if (getSignals != samples.at(s).isSignal) continue;
		}

		histos.push_back(getHistoFromSample(conf, p, samples.at(s)));
	}
	return histos;
}

// needs to be adapted
TLegend* createLegend(TH1D* histo1, TH1D* histo2, TString name1, TString name2){
	if(verbose) std::cout << "--> createLegend(TH1D*, TH1D*, TString, TString)" << std::endl;
	TLegend* legend = new TLegend(0.7,0.77,0.90,0.87);
	legend->SetFillColor(0);
	legend->SetBorderSize(0);
	legend->SetFillStyle(0);
	legend->AddEntry(histo1,name1,"pe");
	legend->AddEntry(histo2,name2,"F");
	return legend;
}

// needs to be adapted
TLegend* createLegend(TH1D* data, TString name){
	if(verbose) std::cout << "--> createLegend(TH1D*, TString)" << std::endl;
	TLegend* legend = new TLegend(0.7,0.77,0.90,0.87);
	legend->SetFillColor(0);
	legend->SetBorderSize(0);
	legend->SetFillStyle(0);
	legend->AddEntry(data,name,"F");
	return legend;
}

TLegend* createLegend(TH1D* data, TH1D* total, std::vector<TH1D*> backgrounds, std::vector<TH1D*> signals, std::vector<sample> samples){
	if(verbose) std::cout << "--> createLegend(TH1D*, TH1D*, vector<TH1D*>, vector<TH1D*>, vector<sample>)" << std::endl;
	TLegend* legend = new TLegend(0.73,0.37,0.93,0.87);
	legend->SetFillColor(0);
	legend->SetBorderSize(0);
	legend->SetFillStyle(0);
	legend->AddEntry(data,"Data","pe");
	legend->AddEntry(total, "Bkg uncertainty", "F");

	if(backgrounds.size() + signals.size() != samples.size())
		std::cout << "ERROR: createLegend: histos and samples have unequal size!" << std::endl;
	for(int i=backgrounds.size()-1;i>=0;i--){
		legend->AddEntry(backgrounds.at(i),samples.at(i).legName,"F");
	}
	for(int i=signals.size()-1;i>=0;i--){
		int i_sample = i + backgrounds.size();
		legend->AddEntry(signals.at(i),samples.at(i_sample).legName,"l");
	}
	return legend;
}


// get a TString from the current time
// stolen from http://www.cplusplus.com/reference/ctime/strftime/
static TString timeNow = "";
TString timeStamp(bool Date, bool Time){
	// use same timestamp for all plots
	if (timeNow != "")
		return timeNow;

	if (!Date && !Time){
		return "";
	}

	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	// output pattern
	TString pattern = "";
	if (Date)			pattern += "%F";
	if (Date && Time)	pattern += "_";
	if (Time) 			pattern += "%H-%M";

	strftime (buffer,80,pattern.Data(),timeinfo);
	timeNow = TString(buffer);
	return timeNow;
}

void savePlot(TCanvas* canvas, TString plotName, bool useTimedFolders = true){
	TString fileName = plotName + ".eps";
	TString fileNameps = plotName + ".ps";
	TString fileNamepng = plotName + ".png";
	TString fileNamepdf = plotName + ".pdf";

	// store plots in individual folder
	TString pwd = gSystem->WorkingDirectory();
	if (useTimedFolders){
		TString dirName = timeStamp(true, true);
		if (not gSystem->ChangeDirectory(dirName)){
			gSystem->MakeDirectory(dirName);
			gSystem->ChangeDirectory(dirName);
		}
	}

	canvas->cd();
	//canvas->SaveAs(fileName.Data());
	//canvas->SaveAs(fileNameps.Data());
	canvas->SaveAs(fileNamepng.Data());
	canvas->SaveAs(fileNamepdf.Data());

	if (useTimedFolders){
		gSystem->ChangeDirectory(pwd);
	}
}

void convertAllEpsToPdf(){
	std::cout << "Convert all eps into pdf..." << std::endl;

	const char* pwd = gSystem->pwd();
	if (timeNow != ""){
		gSystem->ChangeDirectory(timeNow);
		std::cout << "Plots are saved in folder " << timeNow << std::endl;
	}

	gSystem->Exec("for i in `ls -1 *.ps`; do ps2pdf $i; done");

	gSystem->ChangeDirectory(pwd);
}

//todo: Should be fine as is. But doesn't compile yet. Adapt.
/*
void drawDataOnlyPlot(TH1D* data, TString name, TString title, TString unit){
	if(verbose) std::cout << "--> drawDataOnlyPlot(...)" << std::endl;
	//todo: why is this set here??
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetPalette(1);
	gROOT->ForceStyle(true);

	TCanvas* can = new TCanvas();
	TPad* Pad1 = new TPad("Pad1","Pad1",0.,0.,1.,1.);
	Pad1->SetTopMargin(0.07);
	Pad1->SetLeftMargin(0.15);
	Pad1->SetRightMargin(0.05);
	Pad1->SetBottomMargin(0.15);
	Pad1->Draw();
	Pad1->cd();

	data->GetYaxis()->SetTitleOffset(1.5);
	data->SetFillColor(2345);
	TString ytit = "Events / %.2f ";
	TString yTitle = ytit+unit;
	data->GetYaxis()->SetTitle(Form(yTitle.Data(),data->GetBinWidth(1)));
	data->SetTitle(title);
	data->SetTitle("CMS preliminary, #sqrt{s}=8 TeV, L=19.7 fb^{-1}");
	data->Draw("hist");
	TLegend* legend = createLegend(data,name);
	legend->Draw("same");
	can->cd();
	can->SetWindowSize(800,800);
	CMS_lumi(Pad1,4,0);
}
*/

void drawPlot(configInfo conf, plotInfo plot, TH1D* data, std::vector<sample> samples, bool doRatio){
	if(verbose) std::cout << "--> drawPlot(configInfo, plotInfo, TH1D*, vector<sample>, TString)" << std::endl;

	// Set tick marks on all 4 sides of pad
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetPalette(1);
	gROOT->ForceStyle(true);

	// read histograms from input file
	std::vector<TH1D*> backgrounds = getHistos(conf, plot, samples, false);
	std::vector<TH1D*> signals = getHistos(conf, plot, samples, true);
	TH1D* ratio = getDataMC(data,backgrounds);

	//get correct xAxis range
	double xLow = (plot.xRangeLow != -9.9) ? plot.xRangeLow : data->GetXaxis()->GetXmin();
	double xHigh = (plot.xRangeHigh != -9.9) ? plot.xRangeHigh : data->GetXaxis()->GetXmax();

	TCanvas* can = new TCanvas();

	THStack* stack = produceHistStack(backgrounds);
	TH1D* total = produceTotal(backgrounds);


	

	TPad* Pad1 = new TPad("Pad1","Pad1",0.,  doRatio ? 0.3 : 0.  ,1.,1.);
	Pad1->SetFrameLineWidth(2);

	Pad1->SetTopMargin(0.07);
	Pad1->SetLeftMargin( (doRatio) ? 0.15 : 0.1);
	Pad1->SetRightMargin(0.05);
	Pad1->SetBottomMargin( (doRatio) ? 0.015 : 0.15);

	double yAxisScaleFactor = 1.5; // for linear y-axis
	if(plot.log){
		Pad1->SetLogy();
		yAxisScaleFactor = 100; // for log y-axis
	}

	Pad1->Draw();
	Pad1->cd();
	CMS_lumi(Pad1,1,0);
	Pad1->Update();

	if (plot.yRangeHigh >= 0){
		data->SetMaximum(plot.yRangeHigh);
	}
	else {
		// set default values
		if(data->GetMaximum()>=total->GetMaximum()){
			data->SetMaximum(data->GetMaximum()*yAxisScaleFactor);
		}else{
			data->SetMaximum(total->GetMaximum()*yAxisScaleFactor);
		}
	}

	if (plot.yRangeLow >= 0){
		data->SetMinimum(plot.yRangeLow);
	}
	else {
		if(!plot.log) data->SetMinimum(0);
	}
	//zmutau_default_METMC_DY_tautau->GetYaxis()->SetLabelSize(0.07);
	data->GetXaxis()->SetRangeUser(xLow, xHigh);
	data->GetYaxis()->SetLabelSize(0.04);
	data->GetXaxis()->SetLabelSize( (doRatio )  ? 0 : 0.04);
	data->GetYaxis()->SetTitleSize( (doRatio) ? 0.07 : 0.04 );
	data->GetXaxis()->SetTitleSize( (doRatio) ? 0.07 : 0.04 );
	data->GetYaxis()->SetTitleOffset( (doRatio) ? 1.15 : 1.0);
	data->GetXaxis()->SetTitleOffset( (doRatio) ? 1.15 : 1.15);
	data->SetMarkerColor(kBlack);
	data->SetMarkerStyle(20);
	TString ytit = "Events / %.2f ";
	TString yTitle = ytit+plot.unit;
	data->GetYaxis()->SetTitle(Form(yTitle.Data(),data->GetBinWidth(1)));

	//	if(!doRatio){
	//	  total->GetXaxis()->SetTitle(data->GetXaxis()->GetTitle());
	//	  total->GetXaxis()->SetTitleOffset(1.15);
	//	}

	data->Draw("E");
	stack->Draw("Histsame");
	total->Draw("E2same");




	// draw signal samples overlayed
	for (unsigned s = 0; s < signals.size(); s++){
		signals.at(s)->SetFillStyle(0);
		signals.at(s)->SetLineStyle(9);
		signals.at(s)->SetLineWidth(3);
		signals.at(s)->SetLineColor(signals.at(s)->GetFillColor());
		signals.at(s)->Draw("Histsame");
	}

	data->Draw("Esame");
	data->Draw("axissame");
	TLegend* legend = createLegend(data, total, backgrounds,signals,samples);
	legend->Draw("same");





	if(doRatio){

	  // draw data-MC ratio plot
	  can->cd();
	  TH1D* ratioband = (TH1D*) total->Clone(plot.identifier + "_ratioband");
	  for(int i=1;i<=ratioband->GetNbinsX();i++){
	    ratioband->SetBinContent(i,1);
	    ratioband->SetBinError(i,total->GetBinError(i)/total->GetBinContent(i));
	  }
	  ratioband->SetFillStyle(errorBandFillStyle);
	  ratioband->SetFillColor(errorBandColor);
	  ratioband->SetLineColor(18);
	  ratioband->SetMarkerColor(1);
	  ratioband->SetMarkerSize(0.001);
	  TPad* Pad2 = new TPad("Pad2","Pad2",0.,0.,1.,0.3);
	  Pad2->SetTopMargin(0.1);
	  Pad2->SetLeftMargin(0.15);
	  Pad2->SetRightMargin(0.05);
	  Pad2->SetBottomMargin(0.4);
	  Pad2->SetTickx(kTRUE);
	  Pad2->SetGridx();
	  Pad2->SetGridy();
	  Pad2->Draw();
	  Pad2->cd();
	  
	  ratio->GetXaxis()->SetTitleSize(0.15);
	  ratio->GetXaxis()->SetLabelSize(0.1);
	  ratio->GetXaxis()->SetTickLength(0.075);
	  ratio->GetXaxis()->SetRangeUser(xLow, xHigh);
	  ratio->GetYaxis()->SetTitleSize(0.15);
	  ratio->GetYaxis()->SetLabelSize(0.1);
	  ratio->GetYaxis()->SetTitleOffset(0.35);
	  ratio->GetYaxis()->CenterTitle();
	  ratio->GetYaxis()->SetNdivisions(4,5,0,kTRUE);
	  
	  ratio->SetMarkerStyle(20);
	  ratio->SetMarkerSize(0.7);
	  ratio->SetLineColor(kBlack);
	  
	  // line at ratio = 1
	  TLine* line = new TLine(xLow,1,xHigh,1);
	  
	  ratio->Draw("E");
	  ratioband->Draw("E2same");
	  line->Draw("same");
	  ratio->Draw("Esame");
	  
	}



	can->cd();
	can->SetWindowSize(800,800);
	CMS_lumi(Pad1,1,0);
	
	savePlot(can, plot.identifier);
}

//todo: needs more reasonable name like "drawHistoComparison". Needs adaption?
//todo: avoid all this duplication of plotting style within drawHisto functions
void drawPlot(TH1D* histo1, TH1D* histo2, TH1D* ratio, TString name1, TString name2, TString title, TString unit){
	if(verbose) std::cout << "--> drawPlot(TH1D*, TH1D*, TH1D*, TString, TString, TString, TString)" << std::endl;
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetPalette(1);
	gROOT->ForceStyle(true);

	TH1D* total = (TH1D*)histo2->Clone();
	total->SetFillStyle(3005);
	total->SetFillColor(1);
	total->SetLineColor(18);
	total->SetMarkerColor(1);
	total->SetMarkerSize(0.001);

	TCanvas* can = new TCanvas();
	TLine* line = new TLine(histo1->GetXaxis()->GetXmin(),1,histo1->GetXaxis()->GetXmax(),1);
	TPad* Pad1 = new TPad("Pad1","Pad1",0.,0.3,1.,1.);
	Pad1->SetTopMargin(0.07);
	Pad1->SetLeftMargin(0.15);
	Pad1->SetRightMargin(0.05);
	Pad1->SetBottomMargin(0);
	Pad1->Draw();
	Pad1->cd();

	if(histo1->GetMaximum()>=histo2->GetMaximum()){
		histo1->GetYaxis()->SetRangeUser(0.001,histo1->GetMaximum()*1.2);
	}else{
		histo1->GetYaxis()->SetRangeUser(0.001,histo2->GetMaximum()*1.2);
	}
	histo1->GetYaxis()->SetLabelSize(0.07);
	histo1->GetYaxis()->SetTitleSize(0.07);
	histo1->GetYaxis()->SetTitleOffset(1.15);
	TString ytit = "Events / %.2f ";
	TString yTitle = ytit+unit;
	histo1->GetYaxis()->SetTitle(Form(yTitle.Data(),histo1->GetBinWidth(1)));
	histo1->SetTitle(title);
	histo1->SetTitle("CMS simulation, #sqrt{s}=8 TeV");
	histo1->SetMarkerStyle(20);
	histo1->SetMarkerSize(0.7);
	histo1->Draw("E");
	histo2->Draw("Histsame");
	total->Draw("E2same");
	histo1->Draw("Esame");
	histo1->Draw("axissame");
	histo1->SetMinimum(1.001);
	TLegend* legend = createLegend(histo1,histo2,name1,name2);
	legend->Draw("same");
	can->cd();
	TPad* Pad2 = new TPad("Pad1","Pad1",0.,0.,1.,0.3);
	Pad2->SetTopMargin(0);
	Pad2->SetLeftMargin(0.15);
	Pad2->SetRightMargin(0.05);
	Pad2->SetBottomMargin(0.4);
	Pad2->SetTickx(kTRUE);
	Pad2->SetGridx();
	Pad2->SetGridy();
	Pad2->Draw();
	Pad2->cd();

	ratio->GetXaxis()->SetTitleSize(0.15);
	ratio->GetXaxis()->SetLabelSize(0.15);
	ratio->GetXaxis()->SetTickLength(0.075);
	ratio->GetYaxis()->SetTitleSize(0.15);
	ratio->GetYaxis()->SetLabelSize(0.15);
	ratio->GetYaxis()->SetTitleOffset(0.35);
	ratio->GetYaxis()->CenterTitle();

	ratio->GetYaxis()->SetNdivisions(4,5,0,kTRUE);
	ratio->GetXaxis()->Set(histo1->GetXaxis()->GetNbins(),histo1->GetXaxis()->GetXmin(),histo1->GetXaxis()->GetXmax());
	ratio->SetMarkerStyle(20);
	ratio->SetMarkerSize(0.6);
	ratio->SetLineColor(kBlack);
	ratio->Draw("E");
	line->Draw("same");
	can->cd();
	can->SetWindowSize(800,800);
	CMS_lumi(Pad1,1,0);
}

//todo: What is this for? Do we need it? Needs adaption.
/*
void drawPlot(TH1D* data, std::vector<TH1D*> histos, TH1D* ratio, std::vector<TString> names, TString title, TString unit){
	if(verbose) std::cout << "--> drawPlot(TH1D*, vector<TH1D*>, TH1D*, vector<TString>, TString, TString)" << std::endl;
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	gStyle->SetPalette(1);
	gROOT->ForceStyle(true);

	TCanvas* can = new TCanvas();
	THStack* stack = produceHistStack(histos);
	TH1D* total = produceTotal(histos);
	TLine* line = new TLine(data->GetXaxis()->GetXmin(),1,data->GetXaxis()->GetXmax(),1);
	TPad* Pad1 = new TPad("Pad1","Pad1",0.,0.3,1.,1.);
	Pad1->SetTopMargin(0.07);
	Pad1->SetLeftMargin(0.15);
	Pad1->SetRightMargin(0.05);
	Pad1->SetBottomMargin(0);
	Pad1->Draw();
	Pad1->cd();

	if(data->GetMaximum()>=total->GetMaximum()){
		data->GetYaxis()->SetRangeUser(0,data->GetMaximum()*1.2);
	}else{
		data->GetYaxis()->SetRangeUser(0,total->GetMaximum()*1.2);
	}
	data->GetYaxis()->SetLabelSize(0.07);
	data->GetYaxis()->SetTitleSize(0.07);
	data->GetYaxis()->SetTitleOffset(1.15);
	TString ytit = "Events / %.2f ";
	TString yTitle = ytit+unit;
	data->GetYaxis()->SetTitle(Form(yTitle.Data(),data->GetBinWidth(1)));
	data->SetTitle(title);
	data->SetTitle("CMS preliminary, #sqrt{s}=8 TeV, L=19.7 fb^{-1}");
	data->Draw("E");
	stack->Draw("Histsame");
	total->Draw("E2same");
	int bla = histos.size()-1;
	TH1D* signal = (TH1D*) histos.at(bla)->Clone();
	//signal->SetFillColor(10);
	//signal->SetFillStyle(3004);
	//signal->SetLineStyle(9);
	//signal->SetFillStyle(0);
	//signal->SetLineWidth(2);
	//signal->SetLineColor(kBlack);
	signal->Scale(1);
	signal->Draw("Histsame");
	data->Draw("Esame");
	data->Draw("axissame");
	data->SetMinimum(1.001);
	histos.push_back(total);
	names.push_back("Bkg uncertainty");
	TLegend* legend = createLegend(data,histos,names);
	legend->Draw("same");
	can->cd();
	TPad* Pad2 = new TPad("Pad1","Pad1",0.,0.,1.,0.3);
	Pad2->SetTopMargin(0);
	Pad2->SetLeftMargin(0.15);
	Pad2->SetRightMargin(0.05);
	Pad2->SetBottomMargin(0.4);
	Pad2->SetTickx(kTRUE);
	Pad2->SetGridx();
	Pad2->SetGridy();
	Pad2->Draw();
	Pad2->cd();

	ratio->GetXaxis()->SetTitleSize(0.15);
	ratio->GetXaxis()->SetLabelSize(0.15);
	ratio->GetXaxis()->SetTickLength(0.075);
	ratio->GetYaxis()->SetTitleSize(0.15);
	ratio->GetYaxis()->SetLabelSize(0.15);
	ratio->GetYaxis()->SetTitleOffset(0.35);
	ratio->GetYaxis()->CenterTitle();

	ratio->GetYaxis()->SetNdivisions(4,5,0,kTRUE);
	ratio->GetXaxis()->Set(data->GetXaxis()->GetNbins(),data->GetXaxis()->GetXmin(),data->GetXaxis()->GetXmax());
	ratio->Draw("E");
	line->Draw("same");
	can->cd();
	can->SetWindowSize(800,800);
	CMS_lumi(Pad1,4,0);
}
*/

double QuadraticSum(int nval, double values[]){
	if(verbose) std::cout << "--> QuadraticSum(...)" << std::endl;
	double sum = 0.;
	for(int i=0;i<nval;i++){
		sum+=TMath::Power(values[i],2);
	}
	return TMath::Sqrt(sum);
}

// === some useful color definitions ===
// htautau colors
int col_htt_qcd = TColor::GetColor(250,202,255);
int col_htt_W = TColor::GetColor(222,90,106);
int col_htt_ZJ = TColor::GetColor(222,90,106);
int col_htt_tt = TColor::GetColor(155,152,204);
int col_htt_Ztt = TColor::GetColor(248,206,104);

// RWTH colors
// http://www9.rwth-aachen.de/global/show_document.asp?id=aaaaaaaaaadpbhq
int col_rwth_darkblue	= TColor::GetColor(0,84,159);
int col_rwth_lightblue	= TColor::GetColor(142,186,229);
int col_rwth_magenta	= TColor::GetColor(227,0,102);
int col_rwth_yellow		= TColor::GetColor(255,237,0);
int col_rwth_petrol		= TColor::GetColor(0,97,101);
int col_rwth_turquoise	= TColor::GetColor(0,152,161);
int col_rwth_green		= TColor::GetColor(87,171,39);
int col_rwth_maygreen	= TColor::GetColor(189,205,0);
int col_rwth_orange		= TColor::GetColor(246,168,0);
int col_rwth_red		= TColor::GetColor(204,7,30);
int col_rwth_violett	= TColor::GetColor(97,33,88);
int col_rwth_purple		= TColor::GetColor(122,111,172);

#endif
