/*****************************************************************************
 * Copyright (C) 2009-2016   this file is part of the NPTool Project       *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Mohamad Moukaddam  contact address: m.moukaddam@surrey.ac.uk                        *
 *                                                                           *
 * Creation Date  : November 2016                                           *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class hold FPDTamu Spectra                                     *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *   
 *                                                                           *
 *****************************************************************************/

// class header 
#include "TFPDTamuSpectra.h"

// STL
#include <iostream>  
#include <string>
using namespace std;

// NPTool header
#include "NPOptionManager.h"



////////////////////////////////////////////////////////////////////////////////
TFPDTamuSpectra::TFPDTamuSpectra() 
   : fNumberOfDetectors(0) {
  SetName("FPDTamu");
}



////////////////////////////////////////////////////////////////////////////////
TFPDTamuSpectra::TFPDTamuSpectra(unsigned int NumberOfDetectors) {
  if(NPOptionManager::getInstance()->GetVerboseLevel()>0)
    cout << "************************************************" << endl
      << "TFPDTamuSpectra : Initalizing control spectra for " 
      << NumberOfDetectors << " Detectors" << endl
      << "************************************************" << endl ;
  SetName("FPDTamu");
  fNumberOfDetectors = NumberOfDetectors;

  InitRawSpectra();
  InitPreTreatedSpectra();
  InitPhysicsSpectra();
}



////////////////////////////////////////////////////////////////////////////////
TFPDTamuSpectra::~TFPDTamuSpectra() {
}



////////////////////////////////////////////////////////////////////////////////
void TFPDTamuSpectra::InitRawSpectra() {
  static string name;
  for (unsigned int i = 0; i < fNumberOfDetectors; i++) { // loop on number of detectors
    // Energy 
    name = "FPDTamu"+NPL::itoa(i+1)+"_ENERGY_RAW";
    AddHisto1D(name, name, 4096, 0, 16384, "FPDTamu/RAW");
    // Time 
    name = "FPDTamu"+NPL::itoa(i+1)+"_TIME_RAW";
    AddHisto1D(name, name, 4096, 0, 16384, "FPDTamu/RAW");
  } // end loop on number of detectors
}



////////////////////////////////////////////////////////////////////////////////
void TFPDTamuSpectra::InitPreTreatedSpectra() {
  static string name;
  for (unsigned int i = 0; i < fNumberOfDetectors; i++) { // loop on number of detectors
    // Energy 
    name = "FPDTamu"+NPL::itoa(i+1)+"_ENERGY_CAL";
    AddHisto1D(name, name, 500, 0, 25, "FPDTamu/CAL");
    // Time
    name = "FPDTamu"+NPL::itoa(i+1)+"_TIME_CAL";
    AddHisto1D(name, name, 500, 0, 25, "FPDTamu/CAL");

  
  }  // end loop on number of detectors
}



////////////////////////////////////////////////////////////////////////////////
void TFPDTamuSpectra::InitPhysicsSpectra() {
  static string name;
  // Kinematic Plot 
  name = "FPDTamu_ENERGY_TIME";
  AddHisto2D(name, name, 500, 0, 500, 500, 0, 50, "FPDTamu/PHY");
}



////////////////////////////////////////////////////////////////////////////////
void TFPDTamuSpectra::FillRawSpectra(TFPDTamuData* RawData) {
  static string name;
  static string family;

  // Energy 
  unsigned int sizeE = RawData->GetMultEnergy();
  for (unsigned int i = 0; i < sizeE; i++) {
    name = "FPDTamu"+NPL::itoa(RawData->GetE_DetectorNbr(i))+"_ENERGY_RAW";
    family = "FPDTamu/RAW";

    GetHisto(family,name) -> Fill(RawData->Get_Energy(i));
  }

  // Time
  unsigned int sizeT = RawData->GetMultTime();
  for (unsigned int i = 0; i < sizeT; i++) {
    name = "FPDTamu"+NPL::itoa(RawData->GetT_DetectorNbr(i))+"_TIME_RAW";
    family = "FPDTamu/RAW";

    GetHisto(family,name) -> Fill(RawData->Get_Time(i));
  }
}



////////////////////////////////////////////////////////////////////////////////
void TFPDTamuSpectra::FillPreTreatedSpectra(TFPDTamuData* PreTreatedData) {
  static string name;
  static string family;
  
  // Energy 
  unsigned int sizeE = PreTreatedData->GetMultEnergy();
  for (unsigned int i = 0; i < sizeE; i++) {
    name = "FPDTamu"+NPL::itoa(PreTreatedData->GetE_DetectorNbr(i))+"_ENERGY_CAL";
    family = "FPDTamu/CAL";

    GetHisto(family,name) -> Fill(PreTreatedData->Get_Energy(i));
  }

  // Time
  unsigned int sizeT = PreTreatedData->GetMultTime();
  for (unsigned int i = 0; i < sizeT; i++) {
    name = "FPDTamu"+NPL::itoa(PreTreatedData->GetT_DetectorNbr(i))+"_TIME_CAL";
    family = "FPDTamu/CAL";

    GetHisto(family,name) -> Fill(PreTreatedData->Get_Time(i));
  }
}



////////////////////////////////////////////////////////////////////////////////
void TFPDTamuSpectra::FillPhysicsSpectra(TFPDTamuPhysics* Physics) {
  static string name;
  static string family;
  family= "FPDTamu/PHY";

  // Energy vs time
  unsigned int sizeE = Physics->Energy.size();
  for(unsigned int i = 0 ; i < sizeE ; i++){
    name = "FPDTamu_ENERGY_TIME";
    GetHisto(family,name) -> Fill(Physics->Energy[i],Physics->Time[i]);
  }
}

