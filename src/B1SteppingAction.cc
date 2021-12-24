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
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class

#include "B1SteppingAction.hh"
#include "B1EventAction.hh"
#include "B1DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::B1SteppingAction(
    const B1DetectorConstruction *detectorConstruction,
    B1EventAction *eventAction)
    : G4UserSteppingAction(),
      fDetConstruction(detectorConstruction),
      fEventAction(eventAction)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step *step)
{
  // Collect energy and track length step by step

  // auto prePoint = step->GetPreStepPoint();
  // auto postPoint = step->GetPostStepPoint();
  // G4cout << "====================================================="<<std::endl;
  // G4cout << "=====================================================\n"<<std::endl;
  // std::cout<<"cout reference about stepPoint\n"
  // <<"prePoint->GetBeta() = "<<prePoint->GetBeta()<<"\t post = "<<postPoint->GetBeta()<<std::endl
  // <<"prePoint->GetCharge() = "<<prePoint->GetCharge()<<"\t post = "<<postPoint->GetCharge()<<std::endl
  // <<"prePoint->GetGamma() = "<<prePoint->GetGamma()<<"\t post = "<<postPoint->GetGamma()<<std::endl
  // <<"prePoint->GetGlobalTime() = "<<prePoint->GetGlobalTime()<<"\t post = "<<postPoint->GetGlobalTime()<<std::endl
  // <<"prePoint->GetKineticEnergy() = "<<prePoint->GetKineticEnergy()<<"\t post = "<<postPoint->GetKineticEnergy()<<std::endl
  // <<"prePoint->GetLocalTime() = "<<prePoint->GetLocalTime()<<"\t post = "<<postPoint->GetLocalTime()<<std::endl
  // <<"prePoint->GetMass() = "<<prePoint->GetMass()<<"\t post = "<<postPoint->GetMass()<<std::endl
  // <<"prePoint->GetMaterial() = "<<prePoint->GetMaterial()<<"\t post = "<<postPoint->GetMaterial()<<std::endl
  // <<"prePoint->GetMaterialCutsCouple() = "<<prePoint->GetMaterialCutsCouple()<<"\t post = "<<postPoint->GetMaterialCutsCouple()<<std::endl
  // <<"prePoint->GetMomentum() = "<<prePoint->GetMomentum()<<"\t post = "<<postPoint->GetMomentum()<<std::endl
  // <<"prePoint->GetMomentumDirection() = "<<prePoint->GetMomentumDirection()<<"\t post = "<<postPoint->GetMomentumDirection()<<std::endl
  // <<"prePoint->GetPhysicalVolume() = "<<prePoint->GetPhysicalVolume()<<"\t post = "<<postPoint->GetPhysicalVolume()<<std::endl
  // <<"prePoint->GetPolarization() = "<<prePoint->GetPolarization()<<"\t post = "<<postPoint->GetPolarization()<<std::endl
  // <<"prePoint->GetPosition() = "<<prePoint->GetPosition()<<"\t post = "<<postPoint->GetPosition()<<std::endl
  // <<"prePoint->GetPolarization() = "<<prePoint->GetPolarization()<<"\t post = "<<postPoint->GetPolarization()<<std::endl
  // <<"prePoint->GetPosition() = "<<prePoint->GetPosition()<<"\t post = "<<postPoint->GetPosition()<<std::endl
  // <<"prePoint->GetProperTime() = "<<prePoint->GetProperTime()<<"\t post = "<<postPoint->GetProperTime()<<std::endl
  // <<"prePoint->GetSafety() = "<<prePoint->GetSafety()<<"\t post = "<<postPoint->GetSafety()<<std::endl
  // <<"prePoint->GetSensitiveDetector() = "<<prePoint->GetSensitiveDetector()<<"\t post = "<<postPoint->GetSensitiveDetector()<<std::endl
  // <<"prePoint->GetStepStatus() = "<<prePoint->GetStepStatus()<<"\t post = "<<postPoint->GetStepStatus()<<std::endl
  // <<"prePoint->GetTotalEnergy() = "<<prePoint->GetTotalEnergy()<<"\t post = "<<postPoint->GetTotalEnergy()<<std::endl
  // <<"prePoint->GetWeight() = "<<prePoint->GetWeight()<<"\t post = "<<postPoint->GetWeight()<<std::endl
  // <<"prePoint->GetTouchable()->GetCopyNumber() = "<<prePoint->GetTouchable()->GetCopyNumber()<<"\t post = "<<postPoint->GetTouchable()->GetCopyNumber()<<std::endl
  // <<"prePoint->GetTouchable()->GetReplicaNumber() = "<<prePoint->GetTouchable()->GetReplicaNumber()<<"\t post = "<<postPoint->GetTouchable()->GetReplicaNumber()<<std::endl
  // <<"prePoint->GetTouchable()->GetHistory()->GetDepth() = "<<prePoint->GetTouchable()->GetHistory()->GetDepth()<<"\t post = "<<postPoint->GetTouchable()->GetHistory()->GetDepth()<<std::endl
  // <<"prePoint->GetTouchable()->GetHistory()->GetMaxDepth() = "<<prePoint->GetTouchable()->GetHistory()->GetMaxDepth()<<"\t post = "<<postPoint->GetTouchable()->GetHistory()->GetMaxDepth()<<std::endl
  // <<"prePoint->GetTouchable()->GetHistory()->GetTopVolume()->GetName() = "<<prePoint->GetTouchable()->GetHistory()->GetTopVolume()->GetName()<<"\t post = "<<postPoint->GetTouchable()->GetHistory()->GetTopVolume()<<std::endl
  // <<"prePoint->GetTouchable()->GetHistory()->GetTopVolumeType() = "<<prePoint->GetTouchable()->GetHistory()->GetTopVolumeType()<<"\t post = "<<postPoint->GetTouchable()->GetHistory()->GetTopVolumeType()<<std::endl
  // <<"prePoint->GetTouchable()->GetHistory()->GetVolume(0)->GetInstanceID() = "<<prePoint->GetTouchable()->GetHistory()->GetVolume(0)->GetInstanceID()<<"\t post = "<<postPoint->GetTouchable()->GetHistory()->GetVolume(0)->GetInstanceID()<<std::endl
  // <<"prePoint->GetTouchable()->GetHistory()->GetVolume(0)->GetName() = "<<prePoint->GetTouchable()->GetHistory()->GetVolume(0)->GetName()<<"\t post = "<<postPoint->GetTouchable()->GetHistory()->GetVolume(0)->GetName()<<std::endl
  // <<"prePoint->GetTouchable()->GetHistory()->GetVolumeType(0) = "<<prePoint->GetTouchable()->GetHistory()->GetVolumeType(0)<<"\t post = "<<postPoint->GetTouchable()->GetHistory()->GetVolumeType(0)<<std::endl
  // <<"prePoint->GetTouchable()->GetHistoryDepth() = "<<prePoint->GetTouchable()->GetHistoryDepth()<<"\t post = "<<postPoint->GetTouchable()->GetHistoryDepth()<<std::endl
  // <<"prePoint->GetTouchable()->GetSolid()->GetName() = "<<prePoint->GetTouchable()->GetSolid()->GetName()<<"\t post = "<<postPoint->GetTouchable()->GetSolid()->GetName()<<std::endl
  // <<"prePoint->GetTouchable()->GetVolume()->GetName() = "<<prePoint->GetTouchable()->GetVolume()->GetName()<<"\t post = "<<postPoint->GetTouchable()->GetVolume()->GetName()<<std::endl
  // <<"prePoint->GetTouchable()->GetTranslation() = "<<prePoint->GetTouchable()->GetTranslation()<<"\t post = "<<postPoint->GetTouchable()->GetTranslation()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetCopyNumber() = "<<prePoint->GetTouchableHandle()->GetCopyNumber()<<"\t post = "<<postPoint->GetTouchableHandle()->GetCopyNumber()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetReplicaNumber() = "<<prePoint->GetTouchableHandle()->GetReplicaNumber()<<"\t post = "<<postPoint->GetTouchableHandle()->GetReplicaNumber()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetHistory()->GetDepth() = "<<prePoint->GetTouchableHandle()->GetHistory()->GetDepth()<<"\t post = "<<postPoint->GetTouchableHandle()->GetHistory()->GetDepth()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetHistory()->GetMaxDepth() = "<<prePoint->GetTouchableHandle()->GetHistory()->GetMaxDepth()<<"\t post = "<<postPoint->GetTouchableHandle()->GetHistory()->GetMaxDepth()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetHistory()->GetTopVolume()->GetName() = "<<prePoint->GetTouchableHandle()->GetHistory()->GetTopVolume()->GetName()<<"\t post = "<<postPoint->GetTouchableHandle()->GetHistory()->GetTopVolume()->GetName()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetHistory()->GetTopVolumeType() = "<<prePoint->GetTouchableHandle()->GetHistory()->GetTopVolumeType()<<"\t post = "<<postPoint->GetTouchableHandle()->GetHistory()->GetTopVolumeType()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetHistory()->GetVolume(0)->GetInstanceID() = "<<prePoint->GetTouchableHandle()->GetHistory()->GetVolume(0)->GetInstanceID()<<"\t post = "<<postPoint->GetTouchableHandle()->GetHistory()->GetVolume(0)->GetInstanceID()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetHistory()->GetVolume(0)->GetName() = "<<prePoint->GetTouchableHandle()->GetHistory()->GetVolume(0)->GetName()<<"\t post = "<<postPoint->GetTouchableHandle()->GetHistory()->GetVolume(0)->GetName()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetHistory()->GetVolumeType(0) = "<<prePoint->GetTouchableHandle()->GetHistory()->GetVolumeType(0)<<"\t post = "<<postPoint->GetTouchableHandle()->GetHistory()->GetVolumeType(0)<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetHistoryDepth() = "<<prePoint->GetTouchableHandle()->GetHistoryDepth()<<"\t post = "<<postPoint->GetTouchableHandle()->GetHistoryDepth()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetSolid()->GetName() = "<<prePoint->GetTouchableHandle()->GetSolid()->GetName()<<"\t post = "<<postPoint->GetTouchableHandle()->GetSolid()->GetName()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetVolume()->GetName() = "<<prePoint->GetTouchableHandle()->GetVolume()->GetName()<<"\t post = "<<postPoint->GetTouchableHandle()->GetVolume()->GetName()<<std::endl
  // <<"prePoint->GetTouchableHandle()->GetTranslation() = "<<prePoint->GetTouchableHandle()->GetTranslation()<<"\t post = "<<postPoint->GetTouchableHandle()->GetTranslation()<<std::endl
  // <<std::endl;
  // G4cout << "\n====================================================="<<std::endl;
  // G4cout << "====================================================="<<std::endl;
  // std::getchar();

  if (fEventAction->primaryPoint.isDefined == false)
  {
    // 还没有记录初始作用点的坐标
    // get volume of the current step
    // auto prePoint = step->GetPreStepPoint();
    // auto postPoint = step->GetPostStepPoint();
    // auto prevolume = prePoint->GetPhysicalVolume();
    // auto postvolume = postPoint->GetPhysicalVolume();

    // std::cout << "prevolume->GetName() = " << prevolume->GetName() << "\tpostvolume->GetName() = " << postvolume->GetName() << std::endl;
    // std::cout << "prevolume->X, Y, Z = " << step->GetPreStepPoint()->GetPosition()
    //           << std::endl;

    // std::getchar(); // 这个好像不会精确停在断点附近

    // if (prevolume->GetName() == "World" && postvolume->GetName() != "World") // 不如用能量判断
    
    if (step->GetTrack()->GetTrackID() == 1 && step->GetDeltaEnergy()!=0)  
    // 每个Event第一个track，第一次沉积能量的位置是初级作用点
    {
      // 此时说明该step从world进入到Detector
      G4ThreeVector primaryCoord = step->GetPostStepPoint()->GetPosition();
      fEventAction->primaryPoint.x = primaryCoord.getX();
      fEventAction->primaryPoint.y = primaryCoord.getY();
      fEventAction->primaryPoint.z = primaryCoord.getZ();
      fEventAction->primaryPoint.isDefined = true;
    }
  }

  // if ( volume == fDetConstruction->GetAbsorberPV() ) {
  //   fEventAction->AddAbs(edep,stepLength);
  // }

  // if ( volume == fDetConstruction->GetGapPV() ) {
  //   fEventAction->AddGap(edep,stepLength);
  // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
