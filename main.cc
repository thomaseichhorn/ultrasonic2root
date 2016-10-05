// Small script to plot the data files from an a-c-s-scan
// Export data with OmniPC software
// Thomas Eichhorn 2016

// compile with g++ -I `root-config --incdir` -o ultrasonic.exe main.cc `root-config --libs` -Wall -std=c++0x -pedantic -Wextra
// run in ROOT with root -l main.cc+g

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

#include "TROOT.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TH2D.h"
#include "THnSparse.h"

using namespace std;

// user can pass the file to be opened as an argument
int main(int argc, char** argv)
{
    std::string filename = "fail2";
    std::stringstream astream;
    if (argc>1)
    {
	astream << argv[1];
	filename = astream.str();
	cout << "Trying to open file " << filename << endl;
	filename = argv[1];
    } else {
	cout << "Please enter a file to open:" << endl;
	cin >> filename;
    }

    ifstream filestream( filename.c_str() );
    if( !filestream )
    {
	cout << "Opening file " << filename << " failed!" << endl;
	return 0;
    }
    cout << "Opening file " << filename << " succeeded!" << endl;

    // colors:
    // 55 - 'rainbow' palette (discouraged)
    // 53 - dark body radiator
    // 56 - inverted dark body radiator
    gStyle->SetPalette(55);

    // the histogram
    //200mm scan direction, 60mm head / wedge width
    TH2D *histo = new TH2D("Ultrasonic A-C-S Scan","Ultrasonic A-C-S Scan",1000,0,200,60,0,60);
    histo->SetXTitle("Scan Position [mm]");
    histo->SetYTitle("Index Position [mm]");
    histo->SetStats(0000);

    // normalised means cut off at 100 :)
    TH2D *histo_n = new TH2D("Ultrasonic A-C-S Scan Normalised","Ultrasonic A-C-S Scan Normalised",1000,0,200,60,0,60);
    histo_n->SetXTitle("Scan Position [mm]");
    histo_n->SetYTitle("Index Position [mm]");
    histo_n->SetStats(0000);

    // differential in scan direction
    TH2D *histo_dx = new TH2D("Ultrasonic A-C-S Scan Differential X","#partialA / #partialX",1000,0,200,60,0,60);
    histo_dx->SetXTitle("Scan Position [mm]");
    histo_dx->SetYTitle("Index Position [mm]");
    histo_dx->SetStats(0000);

    // differential in index direction
    TH2D *histo_di = new TH2D("Ultrasonic A-C-S Scan Differential I","#partialA / #partialI",1000,0,200,60,0,60);
    histo_di->SetXTitle("Scan Position [mm]");
    histo_di->SetYTitle("Index Position [mm]");
    histo_di->SetStats(0000);

    // the output file we want to save into
    TFile* outputFile = new TFile("output.root", "RECREATE");

    // linecount in reading
    int linecount = 0;

    // the vector of data for further analysis
    std::vector < std::vector < double > > totalvector;

    // distance of each scan index array in x in mm
    double scalescan = 1.0;

    // first scan position in x in mm
    double startx = 0.0;

    // position in mm of each index
    double indexlist[100] = {0.0};

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

	    // header infos considered interesting...
	    if (linecount == 9)
	    {
		scalescan = atof(input.c_str());
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

	// now the data
	if (linecount > 23)
	{

	    // the x position of the current scan
	    double xpos = startx;

	    // vector for each line
	    std::vector < double > linevector;

	    // the index we are at is stored in the first entry
	    double indexpos = 0.0;
	    listentry >> indexpos;

	    // save this for later
	    indexlist[linecount] = indexpos;

	    // temp to read into
	    double temp = 0.0;

	    // value at previous scan position
	    double prevtemp = 0.0;

	    // as long as there is data
	    while (listentry >> temp)
	    {
		// save into vector
		linevector.push_back(temp);

		// histo for the 'raw' data
		histo->Fill(xpos,indexpos,temp);

		// difference to previous scan position, *1.0 for double
		double diff_x = (temp-prevtemp)*1.0;

		// histo for differential in x
		histo_dx->Fill(xpos,indexpos,fabs(diff_x));

		// save value for next position
		prevtemp = temp;

		// 'normalisation' -> FIXME
		if ( temp > 100.0 )
		{
		    temp = 100.0;
		}
		histo_n->Fill(xpos,indexpos,temp);

		// add the scale position in x
		xpos += scalescan;
	    }
	    // vector of this x position is added to the index vector
	    totalvector.push_back(linevector);

	} // done linecount

    } // done reading file

    cout << "Read " << totalvector.size() << " lines!" << endl;

    // same range as histo
    double prev_i[1000] = {0.0};

    // look at data
    // iterator for index
    vector< vector<double> >::iterator i;

    // iterator for scan position
    vector<double>::iterator j;

    // loop both dimensions with iterators
    for (i = totalvector.begin() ; i != (totalvector.end() -1); i++)
    {
	for (j = i->begin(); j != i->end(); j++)
	{
	    // j points to the position in the vector, get the correct element from the array for previous index
	    double temp = 0.0;
	    temp = *j - prev_i[j - i->begin()];

	    // save current value into previous array
	    prev_i[j - i->begin()] = *j;

	    // fill histo
	    histo_di->Fill((j - i->begin())*scalescan,indexlist[(i-totalvector.begin())+25],fabs(temp));

	}
    }

    // let there be output
    outputFile->cd();
    histo->Write();
    histo_n->Write();
    histo_dx->Write();
    histo_di->Write();

    // free memory
    delete outputFile;
    delete histo;
    delete histo_n;
    delete histo_di;
    delete histo_dx;

}
