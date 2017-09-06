// This ROOT macro *should* work without these includes,
// but I have them here to get autocomplete type functionality
// in my editor...
#include <iostream>
#include "GeoMagnetic.h"
#include "TCanvas.h"
#include "AntarcticaBackground.h"
#include "UsefulAdu5Pat.h"
#include "RampdemReader.h"


/** 
 * A function to test the GeoMagnetic namespace.
 * 
 * To run this code in your terminal, do 
 * 
 * $ root testGeoMagnetic.C
 * 
 * ROOT expects to find a void function with the same name as the file
 * without the .C extension
 */
void testGeoMagnetic(){

  // ROOT uses cling (https://root.cern.ch/cling) as an interactive c++ interpreter
  // it basically allows you to use c++ like you would python,
  // each line is compiled after it's entered into the interpreter.
  // For example, you can...
  std::cout << "Print stuff to the terminal with cout statements " << std::endl;
  // just as you would in properly compiled C++ code.
  // If you're familiar with c++ compilers and at all interested with
  // what's going on under the hood there's a talk at Google by one of the developers
  // https://www.youtube.com/watch?v=f9Xfh8pv3Fs
  // It's by no means required info though, and probably a bit dry if you don't care.


  // Moving on.
  

  // You can call functions from our libraries like so.
  // This one is for visualizing the very simple atmospheric model
  // in the GeoMagnetic namespace. 
  TCanvas* c0 = GeoMagnetic::plotAtmosphere();


  // In addition to being a c++ interpreter, ROOT is an enormous collection
  // of c++ classes for data analysis and visualization.
  // For that reason it's used by lots of particle physics experiments,
  // including ANITA. Many plots you will see in a physics are made with ROOT.
  //  Familiarity is essential for doing useful work.

  // So lets get familiar.
  // Here we make a 1D histogram, fill it with some random numbers
  // Notice that all ROOT classes begin with the letter T,
  // which is useful for figuring out where to look for documentation
  TCanvas* c1 = new TCanvas();
  TH1D* hTest = new TH1D("hTest", "Test Histogram", 50, -5, 5);
  hTest->FillRandom("gaus");
  hTest->Draw();
  // 4 lines of code and we got a histogram on the screen!
  // That's why we use it.  
  // Aside:
  //       Modern c++ guidelines recommend against using new/delete to allocate memory
  //       and to use smart pointers, etc.
  //       But ROOT has a long history since before that time, and has it's own
  //       pointer ownership scheme... 
  

  
  TCanvas* c2 = new TCanvas();
  TH1D* hTest2 = new TH1D("hTest2", "Another test histogram filled with numbers we chose", 200, -100, 100);
  for(int i=-100; i < 100; i++){
    for(int j=i; j < 100; j++){
      hTest2->Fill(i);
    }
  }  
  hTest2->Draw();

  
  
  // Finally, we'll do something a little closer to what I think you'll be working on initially.
  // Here we make a UsefulAdu5Pat object, and set the position up so ANITA is hovering above the south pole.
  // (When we get to the stage of working with real data, ANITA used some GPS sensors called "ADU5", hence the name)
  // We then loop over source positions in lon/lat, getting the direction in payload coordinates a plane wave
  // would arrive at ANITA from a 5k altitude, and then ask the question: what would the polarisation angle be
  // for a cosmic ray coming from that direction?
  
  TCanvas* c3 = new TCanvas();
  UsefulAdu5Pat anitaPosition;
  anitaPosition.latitude = -90;
  anitaPosition.longitude = 0;
  anitaPosition.altitude = 40e3;


  const int nBinsLon = 20;
  const int nBinsLat = 20;
  // make a 2D histogram
  TH2D* hExpectedPolAngle = new TH2D("hExpectedPolAngle", "",
                                     nBinsLon, 0, 360,  // x-axis is longitude
                                     nBinsLat, -90, -60); // y-axis is latitude
  // T == ROOT class
  // H == Histogram
  // 2 == two dimensional
  // D == stores the numbers as doubles (as opposed to float/int/short/char)

  for(int by=1; by <= hExpectedPolAngle->GetNbinsY(); by++){
    double lon = hExpectedPolAngle->GetYaxis()->GetBinCenter(by);
    for(int bx=1; bx <= hExpectedPolAngle->GetNbinsX(); bx++){
      double lat = hExpectedPolAngle->GetYaxis()->GetBinCenter(bx);
      Double_t thetaWave, phiWave;
      anitaPosition.getThetaAndPhiWave(lon, lat, 5e2, thetaWave, phiWave);
      double expectedPol = GeoMagnetic::getExpectedPolarisation(anitaPosition, phiWave, thetaWave);
      lon = lon < 0 ? lon + 360 : lon; // Make sure the longitude is between 0, 360
      hExpectedPolAngle->Fill(lon, lat, expectedPol);
    }
  }
  hExpectedPolAngle->SetTitle("Expected geomagnetic polarization at ANITA (above south pole) vs. source longitude/latitude");
  hExpectedPolAngle->GetXaxis()->SetTitle("Longitude (Degrees)");
  hExpectedPolAngle->GetYaxis()->SetTitle("Latitude (Degrees)");
  hExpectedPolAngle->Draw("colz");
  hExpectedPolAngle->GetZaxis()->SetTitle("Expected pol angle (Degrees)");

  // There's some interesting structure in there...
  // Now, does it make sense?
  // Try playing around with this macro or write your own.
  // What dials can you turn to change the answer?  

}
