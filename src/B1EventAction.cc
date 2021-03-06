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
/// \file B1EventAction.cc
/// \brief Implementation of the B1EventAction class

#include "B1EventAction.hh"
#include "B1CalorimeterSD.hh"
#include "B1CalorHit.hh"
#include "B1Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include <fstream>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

extern G4int gX_Layers, gY_Layers, gZ_Layers;
extern std::string csvFileName; // ?????????????????????????????????

B1EventAction::B1EventAction(B1RunAction * /*runAction*/)
    : G4UserEventAction(),
      fAbsHCID(-1)
{
  primaryPoint.x = 0;
  primaryPoint.y = 0;
  primaryPoint.z = 0;
  primaryPoint.isDefined = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::~B1EventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1CalorHitsCollection *
B1EventAction::GetHitsCollection(G4int hcID,
                                 const G4Event *event) const
{
  auto hitsCollection = static_cast<B1CalorHitsCollection *>(
      event->GetHCofThisEvent()->GetHC(hcID)); // ??? hcID ???????????????

  if (!hitsCollection)
  {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("B1EventAction::GetHitsCollection()",
                "MyCode0003", FatalException, msg);
  }

  return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::PrintEventStatistics(G4double absoEdep) const
{
  // print event statistics
  G4cout
      << "   Absorber: total energy: "
      << std::setw(7) << G4BestUnit(absoEdep, "Energy")
      << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::BeginOfEventAction(const G4Event * /*event*/)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::EndOfEventAction(const G4Event *event)
{
  // Get hits collections IDs (only once)
  if (fAbsHCID == -1)
  {
    fAbsHCID = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");
  }

  // Get hits collections
  auto absoHC = GetHitsCollection(fAbsHCID, event);

  // Get hit with total values
  auto absoHit = (*absoHC)[absoHC->entries() - 1];

  // Print per event (modulo n)
  //
  // auto cellID =
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ((printModulo > 0) && (eventID % printModulo == 0))
  {
    G4cout << "---> End of event: " << eventID << G4endl;

    PrintEventStatistics(
        absoHit->GetEdep());
  }

  // Fill histograms, ntuple
  //

  // get analysis manager
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  analysisManager->FillH1(0, absoHit->GetEdep());

  // fill ntuple
  G4double cellID;

  auto hitsCollection = static_cast<B1CalorHitsCollection *>(event->GetHCofThisEvent()->GetHC(0));
  G4int NofCells = hitsCollection->entries();

  G4int x_index, y_index;
  G4double cellEdep;
  for (int index = 0; index < NofCells - 1; index++)
  {
    // ???????????????????????????????????????????????????

    cellID = (*hitsCollection)[index]->GetCellID();      // ??????????????????????????????layers????????????hit???????????????????????????
    x_index = (*hitsCollection)[index]->GetChamberNbX(); 
    y_index = (*hitsCollection)[index]->GetChamberNbY(); 
    cellEdep = (*hitsCollection)[index]->GetEdep();

    if (cellEdep == 0)
      continue;// ?????????????????????????????????

    // x_index = cellID / (gY_Layers);
    // y_index = (cellID - x_index*gY_Layers);
    // z_index = cellID - x_index*(gY_Layers*gZ_Layers) - y_index*(gZ_Layers);

    analysisManager->FillNtupleIColumn(0, eventID);
    analysisManager->FillNtupleIColumn(1, cellID);
    analysisManager->FillNtupleIColumn(2, x_index);
    analysisManager->FillNtupleIColumn(3, y_index);
    // analysisManager->FillNtupleIColumn(4,  z_index); // ????????????
    analysisManager->FillNtupleDColumn(4, primaryPoint.x);  // ??????????????????x??????
    analysisManager->FillNtupleDColumn(5, primaryPoint.y);  // ??????????????????y??????
    analysisManager->FillNtupleDColumn(6, primaryPoint.z);  // ??????????????????z?????? ?????????
    analysisManager->FillNtupleDColumn(7, cellEdep);  // ????????????
    analysisManager->AddNtupleRow(0);

    std::ofstream fout;                     //??????ofstream
    fout.open(csvFileName, std::ios::app); //??????????????????
    fout << eventID << "\t";
    fout.width(4);
    fout << cellID << "\t"
         << x_index << "\t"
         << y_index << "\t"
         << primaryPoint.x << "\t"
         << primaryPoint.y << "\t"
         << primaryPoint.z << "\t"
         << cellEdep << std::endl; //??????
    fout.close();                  //??????

    // std::cout << eventID << "\t" << cellID << "\t" << (*hitsCollection)[index]->GetEdep()<<std::endl; // ?????????
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
