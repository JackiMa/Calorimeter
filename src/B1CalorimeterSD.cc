//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1CalorimeterSD.cc
/// \brief Implementation of the B1CalorimeterSD class

#include "B1CalorimeterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1CalorimeterSD::B1CalorimeterSD(
                            const G4String& name, 
                            const G4String& hitsCollectionName,
                            G4int gX_Layers,G4int gY_Layers,G4int gZ_Layers)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr),
   X_Layers(gX_Layers),Y_Layers(gY_Layers),Z_Layers(gZ_Layers),
  //  fNofCells(X_Layers*Y_Layers*Z_Layers)
   fNofCells(gX_Layers*gY_Layers) // 压缩掉Z轴
{
  collectionName.insert(hitsCollectionName);
  std::cout << "Detector is constucted with XLayers = " << X_Layers
            << " YLayers = " << Y_Layers << " ZLayers = " << Z_Layers;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1CalorimeterSD::~B1CalorimeterSD() 
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1CalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection 
    = new B1CalorHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce
  auto hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 

    for (G4int i=0; i<fNofCells+1; i++ ) {
    // 在该 fHitsCollection 中创建hits，在此 fHitsCollection是一个存放hits到数组
    fHitsCollection->insert(new B1CalorHit());
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// 探测器灵敏区域每一个Step完成时被调用，相当于SteppingAction
G4bool B1CalorimeterSD::ProcessHits(G4Step* step, 
                                     G4TouchableHistory*)
{  
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();
  
  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  if ( edep==0. && stepLength == 0. ) return false;      

  auto touchable = (step->GetPreStepPoint()->GetTouchable());
    
  // Get calorimeter cell id， 压缩掉Z轴，应当取 depth=1，2 的层
  // cellID = touchable->GetCopyNumber(1); // 得到这一step所处物理体的copynumber，但没有意义。
  auto layerNumberY = touchable->GetReplicaNumber(1); // 好的，实验表明，这个就是copynumber！！
  auto layerNumberX = touchable->GetReplicaNumber(2); // 好的，实验表明，这个就是copynumber！！
  cellID = layerNumberX*Y_Layers+layerNumberY; // 一维的cellID

 // 得看看这个地方，GetCopyNumber(1) 是否还等于 GetReplicaNumber(1)，好吧，还是一样的
// std::cout << "\n---------\n" << "GetCopyNumber(1) = " << cellID  
//           << "\tGetReplicaNumber(1) = " << layerNumber << std::endl;
// getchar(); // 暂停以调试

  // Get hit accounting data for this cell
  auto hit = (*fHitsCollection)[cellID];
  if ( ! hit ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << layerNumberX; 
    G4Exception("B1CalorimeterSD::ProcessHits()",
      "MyCode0004", FatalException, msg);
  }         


    hit->SetChamberNbX(layerNumberX); // 返回X坐标，
    hit->SetChamberNbY(layerNumberY); // 返回Y坐标，
    hit->SetCellID(cellID); // 返回一维的坐标


  // Get hit for total accounting
  auto hitTotal 
    = (*fHitsCollection)[fHitsCollection->entries()-1]; // 索引最后一个元素，即hitTotal的值
  
  // Add values
  hit->Add(edep);
  hitTotal->Add(edep);  // 这次Event的总能量，这一次Event中不论是触碰到哪一个SD，都会执行该行
      
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1CalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     auto nofHits = fHitsCollection->entries();
     G4cout
       << G4endl 
       << "-------->Hits Collection: in this event they are " << nofHits 
       << " hits in the tracker chambers: " << G4endl;
     for ( std::size_t i=0; i<nofHits; ++i ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
