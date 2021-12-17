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

#include<fstream>
#include<algorithm>
#include "Randomize.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

extern G4int gX_Layers,gY_Layers,gZ_Layers;
extern G4double gdetUnit_XY;

class DetectorUnitPoint{

  public:
    DetectorUnitPoint(G4int xindex, G4int yindex, G4double length, G4int gX_Layers, G4int gY_Layers){
      counts = 0;
      XIndex = xindex;
      YIndex = yindex;
      sideLen = length;

      X = (XIndex - 1/2*gX_Layers)*sideLen;
      Y = (YIndex - 1/2*gY_Layers)*sideLen;
    }
    bool isIn(G4double xr, G4double yr){
      if( xr>=X && xr<X+sideLen && yr>=Y && yr<Y+sideLen){
        counts += 1;
        return 1;
      }
      else return 0;
    }

      public:
    G4int XIndex,YIndex;
    G4double X,Y; // 该Unit左下角的坐标
    G4double sideLen; // 每一个unit的边长
    G4int counts; // 统计撒点法求面积时，落在该Unit的数目
};




B1EventAction::B1EventAction(B1RunAction* /*runAction*/)
: G4UserEventAction(),
  fAbsHCID(-1)
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::~B1EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1CalorHitsCollection* 
B1EventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<B1CalorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID)); // 第 hcID 块灵敏体积
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B1EventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::PrintEventStatistics( G4double absoEdep) const
{
  // print event statistics
  G4cout
     << "   Absorber: total energy: " 
     << std::setw(7) << G4BestUnit(absoEdep, "Energy")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::EndOfEventAction(const G4Event* event)
{  
  // Get hits collections IDs (only once)
  if ( fAbsHCID == -1 ) {
    fAbsHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");
  }

  // Get hits collections
  auto absoHC = GetHitsCollection(fAbsHCID, event);
  
  // Get hit with total values
  auto absoHit = (*absoHC)[absoHC->entries()-1];
 
 
  // Print per event (modulo n)
  //
  // auto cellID = 
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
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
  std::ofstream fout;           //创建ofstream，使用 csv 来存储数据
  fout.open("result.csv",std::ios::app);   //关联一个文件


  // fill ntuple
  G4double cellID;

  auto hitsCollection = static_cast<B1CalorHitsCollection*>(event->GetHCofThisEvent()->GetHC(0));
  G4int NofCells = hitsCollection->entries(); 


  G4int x_index,y_index;
  G4double cellEdep;
  G4double EdepList[NofCells-1] = {0};
  // vector<G4double>EdepListv(EdepList,EdepList+NofCells-1);
  std::vector <DetectorUnitPoint> UnitPointList;

  for(int index = 0; index < NofCells-1; index++){
    // 最后一个数据是求和，在这里没有意义

    cellID = (*hitsCollection)[index]->GetCellID();// 这个只是最后一次这个layers最后一次hit的信息，不是全部的
    x_index = (*hitsCollection)[index]->GetChamberNbX();// 这个只是最后一次这个layers最后一次hit的信息，不是全部的
    y_index = (*hitsCollection)[index]->GetChamberNbY();// 这个只是最后一次这个layers最后一次hit的信息，不是全部的
    cellEdep = (*hitsCollection)[index]->GetEdep();
    EdepList[index] = cellEdep; // 沉积能量的列表

    UnitPointList.push_back(DetectorUnitPoint(x_index, y_index, gdetUnit_XY, gX_Layers, gY_Layers));

    if(cellEdep== 0) continue;

    // x_index = cellID / (gY_Layers);
    // y_index = (cellID - x_index*gY_Layers);
    // z_index = cellID - x_index*(gY_Layers*gZ_Layers) - y_index*(gZ_Layers);
    
    
      analysisManager->FillNtupleIColumn(0, eventID);
      analysisManager->FillNtupleIColumn(1,  cellID);
      analysisManager->FillNtupleIColumn(2,  x_index);
      analysisManager->FillNtupleIColumn(3,  y_index);
      // analysisManager->FillNtupleIColumn(4,  z_index); // 被折叠了
      analysisManager->FillNtupleDColumn(4, cellEdep);
      analysisManager->AddNtupleRow(0);


      fout << eventID << "\t";
      fout.width(4);
      fout << cellID << "\t" 
      << x_index << "\t" 
      << y_index << "\t" 
      // << z_index << "\t" 
      << cellEdep << std::endl;   //写入
      
    
    // std::cout << eventID << "\t" << cellID << "\t" << (*hitsCollection)[index]->GetEdep()<<std::endl; // 测试用
  }
  fout.close(); //关闭csv的数据流，不知道存储的时间比较长会不会出bug emmmm...

  G4int maxIndex = std::max_element(EdepList, EdepList + NofCells-1) - EdepList;
  bool isXOdd = gX_Layers % 2; // ==1 表示是奇数
  bool isYOdd = gY_Layers % 2; 

  G4double xr, yr; // 随机点的坐标
  G4double Radius = 10; // 要计算圆柱的半径 这里统一以mm为单位，故省略
  G4double theta, k, r; // 用于生成圆内均匀的随机数
  for(int i=0;i<1e6;i++){
    theta = G4UniformRand()*360; // 生成 [0,360]的随机数
    k = G4UniformRand();
    r = sqrt(k)*Radius;
    xr = r*sin(theta);
    yr = r*cos(theta);

    UnitPointList[1].isIn(xr,yr);

  }

}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
