// Small script to plot the data files from an a-c-s-scan
// Export data with OmniPC software
// Thomas Eichhorn 2016

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TH2D.h"
#include "THnSparse.h"

using namespace std;

void plot()
{
    cout << "trying to open file";
    ifstream filestream( "Datei0003.txt" );
    if( !filestream )
    {
	cout << ": failed!" << endl;
	return;
    }
    cout << ": succeeded!" << endl;

    // the histogram
    //200mm scan direction, 60mm head / wedge width
    TH2D *histo = new TH2D("Ultrasonic A-C-S Scan","Ultrasonic A-C-S Scan",1000,0,200,60,0,60);
    histo->SetXTitle("Scan Position [mm]");
    histo->SetYTitle("Index Position [mm]");

    // normalised means cut off at 100 :)
    TH2D *histo_n = new TH2D("Ultrasonic A-C-S Scan Normalised","Ultrasonic A-C-S Scan Normalised",1000,0,200,60,0,60);
    histo_n->SetXTitle("Scan Position [mm]");
    histo_n->SetYTitle("Index Position [mm]");

    // the output file we want to save into
    TFile * outputFile = new TFile("output.root", "RECREATE");

    // linecount in reading
    int linecount = 0;

    // the vector of data for further analysis
    std::vector < std::vector < double > > totalvector;

    // distance of each scan index array in x in mm
    double scalescan = 1.0;

    // first scan position in x in mm
    double startx = 0.0;

    // read file
    while( filestream.good() && ! filestream.eof() )
    {

	// inc linecount so we start with 1
	linecount++;

	// Read file by lines:
	string rl;

	// read one line into string
	getline( filestream, rl );

	// tokenize string
	istringstream listentry(rl);

	// first 22 lines have header info
	// not all fields included yet...
	if (linecount <=22)
	{
	    // dummy string
	    std::string fail;

	    // the input string of this line
	    std::string input;
	    std::istringstream iss(rl);
	    input = iss.str();

	    // the delimiter between field description and value
	    std::string delimiter = "= ";

	    // the position we found of the delimiter
	    size_t pos = 0;

	    // the iterator for counting elements
	    std::vector<int>::iterator it;

	    // string iterator
	    std::vector<string>::iterator its;

	    // remove the description
	    pos = input.find(delimiter);
	    fail = input.substr(0, pos);
	    input.erase(0, pos + delimiter.length());

	    // for string to double
	    std::string::size_type sz;

	    // header infos considered interesting...
	    if (linecount == 9)
	    {
		scalescan = std::stod(input,&sz);
	    }

	}

	// line 23 holds the scan position in x
	// first entry is "mm" -> discard
	// second entry is the startx position, every following point is 'scalescan' mm afterwards...
	if (linecount == 23)
	{
	    double fail = 0.0;
	    listentry >> fail;
	    listentry >> startx;
	}

	if (linecount > 23)
	{

	    // the x position of the current scan
	    double xpos = startx;

	    // vector for each line
	    std::vector < double > linevector;

	    // the index we are at is stored in the first entry
	    double indexpos = 0.0;
	    listentry >> indexpos;

	    // temp to read into
	    double temp = 0.0;
	    while (listentry >> temp)
	    {
		linevector.push_back(temp);
		histo->Fill(xpos,indexpos,temp);

		// 'normalisation'
		if ( temp > 100.0 )
		{
		    temp = 100.0;
		}
		histo_n->Fill(xpos,indexpos,temp);

		// add the scale position in x
		xpos += scalescan;
	    }
	    totalvector.push_back(linevector);
	}
    }

    cout << "Read " << totalvector.size() << " lines!" << endl;

    outputFile->cd();
    histo->Write();
    histo_n->Write();

}
