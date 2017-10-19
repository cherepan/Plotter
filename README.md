PlottingTool
============

**Small tool to create nice plots from ROOT output files created by AnalysisTools**

This tool is intended to be used with the output .root files as created by the [AnalysisTools](https://github.com/inugent/AnalysisTools) package.
Its major goal is to provide publication-quality plots for your thesis, analysis note, or whatever, while remaining small and flexible.
Please feel free to fork this repository and collaborate!

## Instructions
### Running the tool
To run the tool, just call  
`root runPlotting.cc+`  
on your machine. Be aware that the code must be compiled in root, thus the `+` at the end is mandatory.  
As output the PlottingTool will create `eps` as well as `pdf` files for each individual plot.

### Define what to plot
There are two files that define what is plotted, namely `userConfig.h` and `defineSamplesAndPlots.h`.
##### userConfig.h
In this file the user specifies general information about what to plot:
* `infile`: input root file
* `ident`: the identifier of the histograms within the root file (often `myClassName_default_`)
* `isLumiScaled`: bool to specify if input root file contains histograms that have been scaled to luminosity already  

Some additional variables should be self-explanatory. If the boolean `testPlotting` is set, only one plot will be created, which can be helpful for testing purposes.

##### defineSamplesAndPlots.h
This is the file to specify which samples you want to include in your plots, and which variables you would like to plot.

In the function `defineSamples()` you can create [samples](#sample) using various constructors, and combine them using the `+=` syntax. Which samples (or combination of samples) will be plotted by the tool is specified by adding them to the vector `samples`. Elements added first will be drawn first, and thus appear at the bottom of your plot, and vice versa.

The one thing you still need to specify is which variables to plot. This is done in the function `definePlots()`. You can create your plots using various constructors of the [plotInfo](#plotInfo) struct, and add the individual plotInfo objects to the vector `plots`.

#### Special plots
In case you want to create individual, fancy plots that are not normal stacks of histograms, you can create those in the file `runPlotting.cc` directly. Make use of the various functions defined in `plottingHelpers.h` (and also add any new features to this collection as a function).

## Understanding what's happening
The PlottingTool is based on structs (basically a thin class). We will take a quick tour through all the structs and helper files.

### `sample` struct <a name="sample"></a>
The `sample` struct holds all information on one specific Monte-Carlo sample, which is to be plotted as a histogram. It is mandatory to define the following members of each `sample` object:
* `color`: Color to be used for this sample
* `legName`: Name of the sample to appear in the legend
* `identifier`: String used by the framework to identify the sample in the root file

In case your root input file is not already scaled by luminosity, one additional object is mandatory:
* `mcScale`: scale factor to normalize the sample to luminosity

The `sample` struct can hold information on various sub-samples. For example you can create a sample `top` which combines top-pair and single-top samples. Use the `+=` syntax to combine samples. Information on various sub-samples is stored as elements of a vector in the `sample` struct; thus, the `identifier` and `mcScale` members (as well as some others) are vectors.

The file `sample.h` defines the struct and various constructors for it. You can specify the individual members by hand, but there are also some helper functions available. The function `readSkimSummary` will obtain the number of events before skimming of a given sample by its DataMCType from the SkimSummary.log.

### `plotInfo` struct <a name="plotInfo"></a>
The `plotInfo` struct holds all information for a plot that you want to create. The mandatory members for each object are:
* `identifier`: String used by the framework to identify the variable to plot
* `unit`: String which holds the unit of the variable

The additional members allow you to manipulate the histograms before plotting and should be self-explanatory.

### `configInfo` struct
This struct holds information about the individual configuration. One instance of it, named `conf`,  is created in `userConfig.h` and used throughout the framework. The struct takes care of opening the input root file and other important stuff.

### Testing functionality
Each struct used in the framework defines a function `testXYZ`, which tests the setup of this struct and produces warning messages. The function `testInputs` calls these functions and creates a warning, if something in your configuration seems to be strange. The tool will not stop, so the crash message might give you some additional information what is happening.

### `plottingHelpers.h` file
This file is a dump for all the functionality of the tool. It contains functionality to
* obtain histograms from the root file
* manipulate histograms
* create stacked histograms
* produce Data-MC comparison plots
* create legends
* draw plots

It also defines some colors (corporate design, Higgs legacy result) which can be used to create nice plots.

### Additional helper files
There are some additional files present, namely
* `CMS_lumi.h/.cc`: Script to add luminosity information to plots
* `tdrstyle.C`: style of CMS plots

These are copies of the original files by CMS with some slight modifications to make them compatibel with the framework.
