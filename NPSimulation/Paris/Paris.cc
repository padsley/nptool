/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien MATTA  contact address: matta@ipno.in2p3.fr       *
 *                                                                           *
 * Creation Date  : November 2012                                            *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class describe the Paris Silicon array                              *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *****************************************************************************/

// C++ headers
#include <sstream>
#include <cmath>
#include <limits>
using namespace std;

//Geant4
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4SDManager.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4Colour.hh"

// NPS
#include "Paris.hh"
using namespace PARIS;

#include "CalorimeterScorers.hh"
#include "MaterialManager.hh"
// NPL
#include "NPOptionManager.h"
#include "RootOutput.h"

// CLHEP header
#include "CLHEP/Random/RandGauss.h"
using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Paris Specific Method
Paris::Paris(){
  m_Event = new TParisData();

  // Orange 
  m_NaIVisAtt = new G4VisAttributes(G4Colour(1, 0.5, 0))  ;

  // Blue
  m_LaBr3VisAtt = new G4VisAttributes(G4Colour(0, 0.5, 1));

  // Dark Grey
  m_PMTVisAtt = new G4VisAttributes(G4Colour(0.1, 0.1, 0.1));

  // Grey wireframe
  m_PhoswichCasingVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5,0.2));

  // White wireframe
  m_ClusterCasingVisAtt = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7)); 

  m_LogicalPhoswich = 0;
  m_LogicalCluster = 0;  
  m_NaIScorer = 0 ;
  m_LaBr3Scorer = 0 ;
}

Paris::~Paris(){
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void Paris::AddCluster(G4ThreeVector Pos1, G4ThreeVector Pos2, G4ThreeVector Pos3, G4ThreeVector Pos4){
  G4ThreeVector Pos=(Pos1+Pos2+Pos3+Pos4)/4.;
  G4ThreeVector u = Pos1-Pos2;
  G4ThreeVector v = Pos1-Pos4;
  u = u.unit(); v = v.unit();
  G4ThreeVector w = Pos.unit();
  Pos = Pos + w*Length*0.5;

  m_Type.push_back(1); 
  m_Pos.push_back(Pos);
  m_Rot.push_back(new G4RotationMatrix(u,v,w));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void Paris::AddCluster(G4ThreeVector Pos, double beta_u, double beta_v, double beta_w){
  double Theta = Pos.theta();
  double Phi = Pos.phi();

  // vector parallel to one axis of silicon plane
  G4double ii = cos(Theta / rad) * cos(Phi / rad);
  G4double jj = cos(Theta / rad) * sin(Phi / rad);
  G4double kk = -sin(Theta / rad);
  G4ThreeVector Y = G4ThreeVector(ii, jj, kk);

  G4ThreeVector w = Pos.unit();
  G4ThreeVector u = w.cross(Y);
  G4ThreeVector v = w.cross(u);
  v = v.unit();
  u = u.unit();

  G4RotationMatrix* r = new G4RotationMatrix(u,v,w);
  r->rotate(beta_u,u);
  r->rotate(beta_v,v);
  r->rotate(beta_w,w);

  m_Type.push_back(1);
  m_Pos.push_back(Pos);
  m_Rot.push_back(r);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void Paris::AddPhoswich(G4ThreeVector Pos1, G4ThreeVector Pos2, G4ThreeVector Pos3, G4ThreeVector Pos4){
  G4ThreeVector Pos=(Pos1+Pos2+Pos3+Pos4)/4.;
  G4ThreeVector u = Pos1-Pos2;
  G4ThreeVector v = Pos1-Pos4;
  u = u.unit(); v = v.unit();
  G4ThreeVector w = Pos.unit();
  Pos = Pos + w*Length*0.5;

  m_Type.push_back(0); 
  m_Pos.push_back(Pos);
  m_Rot.push_back(new G4RotationMatrix(u,v,w));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void Paris::AddPhoswich(G4ThreeVector Pos, double beta_u, double beta_v, double beta_w){
  double Theta = Pos.theta();
  double Phi = Pos.phi();

  // vector parallel to one axis of silicon plane
  G4double ii = cos(Theta / rad) * cos(Phi / rad);
  G4double jj = cos(Theta / rad) * sin(Phi / rad);
  G4double kk = -sin(Theta / rad);
  G4ThreeVector Y = G4ThreeVector(ii, jj, kk);

  G4ThreeVector w = Pos.unit();
  G4ThreeVector u = w.cross(Y);
  G4ThreeVector v = w.cross(u);
  v = v.unit();
  u = u.unit();

  G4RotationMatrix* r = new G4RotationMatrix(u,v,w);
  r->rotate(beta_u,u);
  r->rotate(beta_v,v);
  r->rotate(beta_w,w);

  m_Type.push_back(0);
  m_Pos.push_back(Pos);
  m_Rot.push_back(r);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Virtual Method of VDetector class
// Read stream at Configfile to pick-up parameters of detector (Position,...)
// Called in DetecorConstruction::ReadDetextorConfiguration Method
void Paris::ReadConfiguration(string Path){
  ifstream ConfigFile;
  ConfigFile.open(Path.c_str());
  string LineBuffer, DataBuffer; 

  // A,B,C,D are the four corner of the detector

  G4double Ax , Bx , Cx , Dx , Ay , By , Cy , Dy , Az , Bz , Cz , Dz          ;
  G4ThreeVector A , B , C , D                                                 ;
  G4double Theta = 0 , Phi = 0 , R = 0 , beta_u = 0 , beta_v = 0 , beta_w = 0 ;
  bool Type = true;

  bool ReadingStatus = false;

  bool check_A = false;
  bool check_C = false;
  bool check_B = false;
  bool check_D = false;

  bool check_Theta = false;
  bool check_Phi   = false;
  bool check_R     = false;

  while (!ConfigFile.eof()) {
    getline(ConfigFile, LineBuffer);
    if (LineBuffer.compare(0, 12, "ParisCluster") == 0) {
      G4cout << "///" << G4endl           ;
      G4cout << "Cluster element found: " << G4endl   ;
      ReadingStatus = true ;
      Type = true;
    }

    else if (LineBuffer.compare(0, 13, "ParisPhoswich") == 0) {
      G4cout << "///" << G4endl           ;
      G4cout << "Phoswich element found: " << G4endl   ;
      ReadingStatus = true ;
      Type = false;
    }


    while (ReadingStatus) {
      ConfigFile >> DataBuffer;
      // Comment Line 
      if (DataBuffer.compare(0, 1, "%") == 0) {/*do nothing */;}

      // Position method
      else if (DataBuffer == "A=") {
        check_A = true;
        ConfigFile >> DataBuffer ;
        Ax = atof(DataBuffer.c_str()) ;
        Ax = Ax * mm ;
        ConfigFile >> DataBuffer ;
        Ay = atof(DataBuffer.c_str()) ;
        Ay = Ay * mm ;
        ConfigFile >> DataBuffer ;
        Az = atof(DataBuffer.c_str()) ;
        Az = Az * mm ;

        A = G4ThreeVector(Ax, Ay, Az);
        G4cout << "Corner A position : " << A << G4endl;
      }
      else if (DataBuffer == "B=") {
        check_B = true;
        ConfigFile >> DataBuffer ;
        Bx = atof(DataBuffer.c_str()) ;
        Bx = Bx * mm ;
        ConfigFile >> DataBuffer ;
        By = atof(DataBuffer.c_str()) ;
        By = By * mm ;
        ConfigFile >> DataBuffer ;
        Bz = atof(DataBuffer.c_str()) ;
        Bz = Bz * mm ;

        B = G4ThreeVector(Bx, By, Bz);
        G4cout << "Corner B position : " << B << G4endl;
      }

      else if (DataBuffer == "C=") {
        check_C = true;
        ConfigFile >> DataBuffer ;
        Cx = atof(DataBuffer.c_str()) ;
        Cx = Cx * mm ;
        ConfigFile >> DataBuffer ;
        Cy = atof(DataBuffer.c_str()) ;
        Cy = Cy * mm ;
        ConfigFile >> DataBuffer ;
        Cz = atof(DataBuffer.c_str()) ;
        Cz = Cz * mm ;

        C = G4ThreeVector(Cx, Cy, Cz);
        G4cout << "Corner C position : " << C << G4endl;
      }
      else if (DataBuffer == "D=") {
        check_D = true;
        ConfigFile >> DataBuffer ;
        Dx = atof(DataBuffer.c_str()) ;
        Dx = Dx * mm ;
        ConfigFile >> DataBuffer ;
        Dy = atof(DataBuffer.c_str()) ;
        Dy = Dy * mm ;
        ConfigFile >> DataBuffer ;
        Dz = atof(DataBuffer.c_str()) ;
        Dz = Dz * mm ;

        D = G4ThreeVector(Dx, Dy, Dz);
        G4cout << "Corner D position : " << D << G4endl;
      }

      // Angle method
      else if (DataBuffer=="Theta=") {
        check_Theta = true;
        ConfigFile >> DataBuffer ;
        Theta = atof(DataBuffer.c_str()) ;
        Theta = Theta * deg;
        G4cout << "Theta:  " << Theta / deg << G4endl;
      }
      else if (DataBuffer=="Phi=") {
        check_Phi = true;
        ConfigFile >> DataBuffer ;
        Phi = atof(DataBuffer.c_str()) ;
        Phi = Phi * deg;
        G4cout << "Phi:  " << Phi / deg << G4endl;
      }
      else if (DataBuffer=="R=") {
        check_R = true;
        ConfigFile >> DataBuffer ;
        R = atof(DataBuffer.c_str()) ;
        R = R * mm;
        G4cout << "R:  " << R / mm << G4endl;
      }
      else if (DataBuffer=="Beta=") {
        ConfigFile >> DataBuffer ;
        beta_u = atof(DataBuffer.c_str()) ;
        beta_u = beta_u * deg   ;
        ConfigFile >> DataBuffer ;
        beta_v = atof(DataBuffer.c_str()) ;
        beta_v = beta_v * deg   ;
        ConfigFile >> DataBuffer ;
        beta_w = atof(DataBuffer.c_str()) ;
        beta_w = beta_w * deg   ;
        G4cout << "Beta:  " << beta_u / deg << " " << beta_v / deg << " " << beta_w / deg << G4endl  ;
      }

      else G4cout << "WARNING: Wrong Token, ParisCluster: Cluster Element not added" << G4endl;

      // Add The previously define telescope
      // With position method
      if ((check_A && check_B && check_C && check_D) && 
          !(check_Theta && check_Phi && check_R)) {
        ReadingStatus = false;
        check_A = false;
        check_C = false;
        check_B = false;
        check_D = false;

        if(Type)
          AddCluster(A, B, C, D);
        else 
          AddPhoswich(A, B, C, D);
      }

      // With angle method
      if ((check_Theta && check_Phi && check_R ) && 
          !(check_A && check_B && check_C && check_D)) {
        ReadingStatus = false;
        check_Theta = false;
        check_Phi   = false;
        check_R     = false;

        G4ThreeVector Pos(R*sin(Theta)*cos(Phi),R*sin(Theta)*sin(Phi),R*cos(Theta));

        if(Type) 
          AddCluster(Pos, beta_u, beta_v, beta_w);
        else
          AddPhoswich(Pos, beta_u, beta_v, beta_w);
      }
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Construct detector and inialise sensitive part.
// Called After DetecorConstruction::AddDetector Method
void Paris::ConstructDetector(G4LogicalVolume* world){
  unsigned int mysize = m_Pos.size();
  for(unsigned int i = 0 ; i < mysize ; i++){
    if(m_Type[i])
      new G4PVPlacement(G4Transform3D(*m_Rot[i], m_Pos[i]), ConstructCluster(),  "ParisCluster", world, false, i+1);
    else
      new G4PVPlacement(G4Transform3D(*m_Rot[i], m_Pos[i]), ConstructPhoswich(),  "ParisPhoswich", world, false, i+1); 
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4LogicalVolume* Paris::ConstructPhoswich(){

  if(!m_LogicalPhoswich){
    G4Material* Vacuum = MaterialManager::getInstance()->GetMaterialFromLibrary("Vacuum");
    G4Material* Alu = MaterialManager::getInstance()->GetMaterialFromLibrary("Al");
    G4Material* NaI = MaterialManager::getInstance()->GetMaterialFromLibrary("NaI");
    G4Material* LaBr3 = MaterialManager::getInstance()->GetMaterialFromLibrary("LaBr3");

    G4Box* solidParisPhoswich = new G4Box("SolidParisPhoswich", 0.5*PhoswichCasingWidth, 0.5*PhoswichCasingWidth, 0.5*PhoswichCasingLenth+0.5*PMTLength);
    m_LogicalPhoswich = new G4LogicalVolume(solidParisPhoswich, Vacuum, "LogicParisPhoswich", 0, 0, 0);
    m_LogicalPhoswich->SetVisAttributes(G4VisAttributes::Invisible);

    // Phoswich construction
    // Casing
    G4Box* solidCasing = new G4Box("SolidParisPhoswichCasing", 0.5*PhoswichCasingWidth, 0.5*PhoswichCasingWidth, 0.5*PhoswichCasingLenth);
    G4LogicalVolume* LogicalCasing = new G4LogicalVolume(solidCasing, Alu, "LogicParisPhoswichCasing", 0, 0, 0);
    LogicalCasing->SetVisAttributes(m_PhoswichCasingVisAtt);

    new G4PVPlacement(0, 
        G4ThreeVector(0,0,-PMTLength), 
        LogicalCasing, 
        "ParisPhoswich_Casing", 
        m_LogicalPhoswich, 
        false, 
        0);

    // PMT
    G4Tubs* solidPMT = new G4Tubs("SolidPMT",0,PMTRadius,0.5*PMTLength,0,360*deg);
    G4LogicalVolume* LogicalPMT = new G4LogicalVolume(solidPMT, Alu, "LogicPMT" ,0,0,0);
    new G4PVPlacement(0, 
        G4ThreeVector(0,0,0.5*Length-PMTLength), 
        LogicalPMT, 
        "ParisPhoswich_PMT", 
        m_LogicalPhoswich, 
        false, 
        0);
    LogicalPMT->SetVisAttributes(m_PMTVisAtt);

    // LaBr3
    G4Box*           solidLaBr3Stage = new G4Box("solidLaBr3Stage", 0.5*LaBr3Face, 0.5*LaBr3Face, 0.5*LaBr3Thickness);
    G4LogicalVolume* logicLaBr3Stage = new G4LogicalVolume(solidLaBr3Stage, LaBr3, "logicLaBr3Stage", 0, 0, 0);
    G4ThreeVector  positionLaBr3Stage = G4ThreeVector(0, 0, LaBr3Stage_PosZ);

    new G4PVPlacement(0, 
        positionLaBr3Stage, 
        logicLaBr3Stage, 
        "ParisPhoswich_LaBr3Stage", 
        LogicalCasing, 
        false,
        0);

    // Set LaBr3 sensible
    logicLaBr3Stage->SetSensitiveDetector(m_LaBr3Scorer);

    // Visualisation of LaBr3 stage 
    logicLaBr3Stage->SetVisAttributes(m_LaBr3VisAtt);

    // NaI
    G4ThreeVector  positionNaIStage = G4ThreeVector(0, 0, NaIStage_PosZ);

    G4Box*           solidNaIStage = new G4Box("solidNaIStage", 0.5*NaIFace, 0.5*NaIFace, 0.5*NaIThickness);
    G4LogicalVolume* logicNaIStage = new G4LogicalVolume(solidNaIStage, NaI, "logicNaIStage", 0, 0, 0);

    new G4PVPlacement(0, 
        positionNaIStage, 
        logicNaIStage, 
        "ParisPhoswich_NaIStage", 
        LogicalCasing, 
        false, 
        0);

    // Set NaI sensible
    logicNaIStage->SetSensitiveDetector(m_NaIScorer);

    // Visualisation of the NaI stage 
    logicNaIStage->SetVisAttributes(m_NaIVisAtt);
  }

  return m_LogicalPhoswich;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4LogicalVolume* Paris::ConstructCluster(){

  if(!m_LogicalCluster){
    G4Material* Vacuum =  MaterialManager::getInstance()->GetMaterialFromLibrary("Vacuum");
    G4Material* Alu =  MaterialManager::getInstance()->GetMaterialFromLibrary("Al");

    // Mother Volume
    G4Box* solidParisCluster1 = new G4Box("SolidParisCluster1", 0.5*FaceFront, 0.5*FaceFront, 0.5*Length);
    G4Box* solidParisCluster2 = new G4Box("SolidParisCluster2", 0.5*ClusterFrameWidth, 0.5*ClusterFrameWidth, 0.5*ClusterFrameLength);     

    // The cluster is made of a metal frame that hold the phoswich via the pmt
    G4ThreeVector FramePos(0,0,Length*0.5-PMTLength+ClusterFrameLength*0.5+ClusterOffset);
    G4UnionSolid* solidParisCluster = new G4UnionSolid("solidParisCluster",solidParisCluster1,solidParisCluster2,0,FramePos);
    m_LogicalCluster = new G4LogicalVolume(solidParisCluster, Vacuum, "LogicSolidParisCluster", 0, 0, 0);
    m_LogicalCluster->SetVisAttributes(G4VisAttributes::Invisible);

    if(!m_LogicalPhoswich) ConstructPhoswich();

    // The frame is a plate with hole for the PMT
    G4VSolid* solidClusterFrame = solidParisCluster2;
    G4ThreeVector Origin(-PhoswichCasingWidth,-PhoswichCasingWidth,0.5*PMTLength);
    G4Tubs* solidPMT = new G4Tubs("SolidPMT",0,PMTRadius,10*PMTLength,0,360*deg);

    // A cluster is a 3 by 3 aggregat of phoswich
    unsigned int PhoswichNbr = 1;
    for(unsigned int i = 0 ; i < 3 ; i++){
      for(unsigned int j = 0 ; j <3 ; j++){
        G4ThreeVector Pos = Origin + G4ThreeVector(i*PhoswichCasingWidth,j*PhoswichCasingWidth,0);
        new G4PVPlacement(0, 
            Pos, 
            m_LogicalPhoswich, 
            "Paris_Phoswich", 
            m_LogicalCluster, 
            false, 
            PhoswichNbr++);

        // make room for the PMT in the cluster frame
        solidClusterFrame = new G4SubtractionSolid("ClusterFrame",solidClusterFrame,solidPMT,0,Pos); 
      }
    }

    G4LogicalVolume* LogicalClusterFrame = new G4LogicalVolume(solidClusterFrame, Alu, "LogicSolidClusterFrame", 0, 0, 0);
    LogicalClusterFrame->SetVisAttributes(m_ClusterCasingVisAtt);

    new G4PVPlacement(0, 
        FramePos, 
        LogicalClusterFrame, 
        "Paris_ClusterFrame", 
        m_LogicalCluster, 
        false, 
        0);

  }
  return m_LogicalCluster;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Add Detector branch to the EventTree.
// Called After DetecorConstruction::AddDetector Method
void Paris::InitializeRootOutput(){
  RootOutput *pAnalysis = RootOutput::getInstance();
  TTree *pTree = pAnalysis->GetTree();
  pTree->Branch("Paris", "TParisData", &m_Event) ;
  pTree->SetBranchAddress("Paris", &m_Event) ;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Read sensitive part and fill the Root tree.
// Called at in the EventAction::EndOfEventAvtion
void Paris::ReadSensitive(const G4Event* event){
  m_Event->Clear();

  ///////////
  // LaBr3
  G4THitsMap<G4double*>* LaBr3HitMap;
  std::map<G4int, G4double**>::iterator LaBr3_itr;

  G4int LaBr3CollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("Paris_LaBr3Scorer/ParisLaBr3");
  LaBr3HitMap = (G4THitsMap<G4double*>*)(event->GetHCofThisEvent()->GetHC(LaBr3CollectionID));

  // Loop on the LaBr3 map
  for (LaBr3_itr = LaBr3HitMap->GetMap()->begin() ; LaBr3_itr != LaBr3HitMap->GetMap()->end() ; LaBr3_itr++){

    G4double* Info = *(LaBr3_itr->second);

    double Energy = Info[0];

    if(Energy>EnergyThreshold){
      double Time = Info[1];
      int PhoswichNbr = (int) Info[2];
      int ClusterNbr = (int) Info[3];
      
      m_Event->SetParisLaBr3E(ClusterNbr,PhoswichNbr,Energy);
      m_Event->SetParisLaBr3T(ClusterNbr,PhoswichNbr,Time);
    }
  }
  // clear map for next event
  LaBr3HitMap->clear();

  ///////////
  // NaI
  G4THitsMap<G4double*>* NaIHitMap;
  std::map<G4int, G4double**>::iterator NaI_itr;

  G4int NaICollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("Paris_NaIScorer/ParisNaI");
  NaIHitMap = (G4THitsMap<G4double*>*)(event->GetHCofThisEvent()->GetHC(NaICollectionID));

  // Loop on the NaI map
  for (NaI_itr = NaIHitMap->GetMap()->begin() ; NaI_itr != NaIHitMap->GetMap()->end() ; NaI_itr++){

    G4double* Info = *(NaI_itr->second);

    double Energy = Info[0];

    if(Energy>EnergyThreshold){
      double Time = Info[1];
      int PhoswichNbr = (int) Info[2];
      int ClusterNbr = (int) Info[3];
     
       m_Event->SetParisNaIE(ClusterNbr,PhoswichNbr,Energy);
      m_Event->SetParisNaIT(ClusterNbr,PhoswichNbr,Time);
    }
  }

  // clear map for next event
  NaIHitMap->clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void Paris::InitializeScorers(){
  vector<G4int> NestingLevel;
  NestingLevel.push_back(2);
  NestingLevel.push_back(3);

  //   LaBr3 Associate Scorer
  bool already_exist = false;
  m_LaBr3Scorer = CheckScorer("Paris_LaBr3Scorer",already_exist);

  // if the scorer were created previously nothing else need to be made
  if(already_exist) return;

  G4VPrimitiveScorer* LaBr3Scorer =
    new  CALORIMETERSCORERS::PS_Calorimeter("ParisLaBr3",NestingLevel);
  //and register it to the multifunctionnal detector
  m_LaBr3Scorer->RegisterPrimitive(LaBr3Scorer);

  //   Add All Scorer to the Global Scorer Manager
  G4SDManager::GetSDMpointer()->AddNewDetector(m_LaBr3Scorer) ;


  //   NaI Associate Scorer
  already_exist = false;
  m_NaIScorer = CheckScorer("Paris_NaIScorer",already_exist);

  // if the scorer were created previously nothing else need to be made
  if(already_exist) return;

  G4VPrimitiveScorer* NaIScorer =
    new  CALORIMETERSCORERS::PS_Calorimeter("ParisNaI",NestingLevel);
  //and register it to the multifunctionnal detector
  m_NaIScorer->RegisterPrimitive(NaIScorer);

  //   Add All Scorer to the Global Scorer Manager
  G4SDManager::GetSDMpointer()->AddNewDetector(m_NaIScorer) ;

}
