#include "B1DetectorConstruction.hh"
#include "G4SDManager.hh"
#include "B1CalorimeterSD.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction(G4int gX_Layers,G4int gY_Layers,G4int gZ_Layers)
: G4VUserDetectorConstruction(),
  X_Layers(gX_Layers),Y_Layers(gY_Layers),Z_Layers(gZ_Layers)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();


  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;


  fNofCrystal = X_Layers* Y_Layers* Z_Layers;

  // X_Layers = 3;
  // Y_Layers = 3;
  // Z_Layers = 5;
  // fNofCrystal = X_Layers* Y_Layers* Z_Layers;

/*
                                            ↑ Z = 3000
    Z = 2500          ● source         ╗    |
                   ↙ ↓ ↘                  
                                       ║    |
                                       ║    |
                                       ║    |
                                       ║    |
                    :   :            5000mm |
                   : : : :             ║    |
                  : : : : :            ║    |
                 : : : : : :           ║    |
                : : : : : : :          ║    |
                                       ╝    |
                                            |
    Z = -2500 __ __ __ __ __ __ __ __  ╗    |
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓  ║    |     
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓  ║    |
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓  ║    |
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓       |
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ 416mm | Z = -2708
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓       |
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓  ║    |
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓  ║    |
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓  ║    |
        ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓  ║    |      
        -------     404mm     -------  ╝    |
                                            | Z = -3000
        
XYZ 从 1 开始计数
*/
  //     
  // World
  //
  G4double world_sizeXY = 3.7 * m;
  G4double world_sizeZ  = 6. * m;
  // G4double world_sizeXY = 1 * m;
  // G4double world_sizeZ  = 2. * m;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
  
  /*     
   一个探测单元                  ————> Z轴
   ▓░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░▓
   Fe C8H8   ... C8H8 ...   C8H8 Fe 
  */  
  G4String name, symbol; // a=mass of a mole;
  G4double a, z, density; // z=mean number of protons;
  G4int ncomponents, natoms;
  G4double X_pos, Y_pos, Z_pos; // XYZ coordinate
  // element define
  a = 1.01*g/mole;    G4Element* elH = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);
  a = 12.01*g/mole;   G4Element* elC = new G4Element(name="Carbon" ,symbol="C" , z= 6., a);
  
  // material define
  a = 55.85*g/mole;   density = 7.86*g/cm3;
  G4Material* Fe_mat = new G4Material(name="Iron", z=26., a, density);
  a = 207.20*g/mole;  density = 11.34*g/cm3;
  G4Material* Pb_mat = new G4Material(name="Lead" , z=82., a, density);
  density = 1.032*g/cm3;
  G4Material* Crystal_mat = new G4Material(name="Scintillator", density, ncomponents=2);
  Crystal_mat->AddElement(elC, natoms=8);
  Crystal_mat->AddElement(elH, natoms=8);
  density = 1.032*g/cm3;
  G4Material* Plastics_mat = new G4Material(name="Plastics", density, ncomponents=2);
  Plastics_mat->AddElement(elC, natoms=8);
  Plastics_mat->AddElement(elH, natoms=8);

  // size
  G4double detUnit_XY = 40.4 * mm; // 探测器单元的XY，Fe、crystal、Plastics也是这个值
  G4double Detector_XY = X_Layers*40.4 * mm;
  G4double Fe_Z = 1. * mm;
  G4double Pb_Z = 2. * mm;
  G4double Crystal_Z = 4. * mm;
  G4double Plastics_Z = 7. * mm;
  G4double Detector_Z = (Fe_Z*2 + Plastics_Z*2 + Crystal_Z*Z_Layers + Pb_Z*(Z_Layers-1)) * mm; 
                        // 1*2 + 7*2 + 4*67 + 2*66 = 416 detUnit也是这个值
  // soild & logical
  G4Box* solidFe    = new G4Box("Iron",     detUnit_XY/2, detUnit_XY/2, Fe_Z/2);
  G4Box* solidPb    = new G4Box("Lead",     detUnit_XY/2, detUnit_XY/2, Pb_Z/2);
  G4Box* solidCrystal = new G4Box("Crystal",  detUnit_XY/2, detUnit_XY/2, Crystal_Z/2);
  G4Box* solidPlastics = new G4Box("Plastics", detUnit_XY/2, detUnit_XY/2, Plastics_Z/2);
  G4LogicalVolume* logicFe =  new G4LogicalVolume(solidFe,Fe_mat,"FeLV");  
  G4LogicalVolume* logicPb =  new G4LogicalVolume(solidPb,Pb_mat,"PbLV");   
  G4LogicalVolume* logicCrystal =  new G4LogicalVolume(solidCrystal,Crystal_mat,"CrystalLV");    
  G4LogicalVolume* logicPlastics =  new G4LogicalVolume(solidPlastics,Plastics_mat,"PlasticsLV");    
  G4Box* solidDetUnit = new G4Box("DetUnit",  detUnit_XY/2, detUnit_XY/2, Detector_Z/2);
  G4LogicalVolume* logicDetUnit =  new G4LogicalVolume(solidDetUnit,world_mat,"DetUnitLV");
  Z_pos = 0*mm;
 G4int Z_index = 0;
 G4int copynumber;
   Z_pos = -1/2.*Detector_Z + 1/2.*Fe_Z;
   copynumber = Z_index; // copynmuber == -1 都是不要的
      new G4PVPlacement(0,G4ThreeVector(0,0,Z_pos),logicFe,"Iron",logicDetUnit,false,-1,checkOverlaps);  
      Z_pos = Z_pos + 1/2.*Fe_Z + 1/2.*Plastics_Z;
      new G4PVPlacement(0,G4ThreeVector(0,0,Z_pos),logicPlastics,"Plastics",logicDetUnit,false,-1,checkOverlaps);  
      Z_pos = Z_pos + 1/2.*Plastics_Z + 1/2.*Crystal_Z;
      new G4PVPlacement(0,G4ThreeVector(0,0,Z_pos),logicCrystal,"Crystal",logicDetUnit,false,copynumber,checkOverlaps);  
      for(Z_index = 1; Z_index < Z_Layers; Z_index++){ // 因为第一块Crystal编号0，这里从编号1开始
        copynumber = Z_index; // copynmuber == -1 都是不要的
        Z_pos = Z_pos + 1/2.*Crystal_Z + 1/2.*Pb_Z;
        new G4PVPlacement(0,G4ThreeVector(0,0,Z_pos),logicPb,"Lead",logicDetUnit,false,-1,checkOverlaps);  
        Z_pos = Z_pos + 1/2.*Pb_Z + 1/2.*Crystal_Z;
        new G4PVPlacement(0,G4ThreeVector(0,0,Z_pos),logicCrystal,"Crystal",logicDetUnit,false,copynumber,checkOverlaps); 
      }
      Z_pos = Z_pos + 1/2.*Crystal_Z + 1/2.*Plastics_Z;
      new G4PVPlacement(0,G4ThreeVector(0,0,Z_pos),logicPlastics,"Plastics",logicDetUnit,false,-1,checkOverlaps);  
      Z_pos = Z_pos + 1/2.*Plastics_Z + 1/2.*Fe_Z;
      new G4PVPlacement(0,G4ThreeVector(0,0,Z_pos),logicFe,"Iron",logicDetUnit,false,-1,checkOverlaps);  


  /* DetectorWall combined with 10 × 1 detUnit with index Y       
             -→ Y轴
    ▓▓▓▓▓▓▓▓
   ↓ X 轴 
  */ 
  G4Box* solidDetectorWall = new G4Box("Detector",  detUnit_XY/2, Detector_XY/2, Detector_Z/2);
  G4LogicalVolume* logicDetectorWall =  new G4LogicalVolume(solidDetectorWall,world_mat,"DetectorWallLV");
  Y_pos = -1/2.*Detector_XY + 1/2.*detUnit_XY;
  copynumber = 0;
  Z_pos = 0; X_pos = 0;
  Y_pos = -1/2.*Detector_XY + 1/2.*detUnit_XY;
 for(int Y_index = 0; Y_index < 0+Y_Layers;Y_index++){
      copynumber = Y_index ; // copynmuber == -1 都是不要的，需要的从0开始
      // place detectorUnit within a detectorWall
      new G4PVPlacement(0,G4ThreeVector(X_pos,Y_pos,Z_pos),logicDetUnit,"detUnit",logicDetectorWall,false,copynumber,checkOverlaps);  
      Y_pos = Y_pos + 1.*detUnit_XY;
    }


  /* Detector combined with 1 × 10 DetectorWall with index X
             -→ Y轴
    ▓▓▓▓▓▓▓▓
    ▓▓▓▓▓▓▓▓
    ▓▓▓▓▓▓▓▓
   ↓ X 轴 
  */ 
  G4Box* solidDetector = new G4Box("Detector",  Detector_XY/2, Detector_XY/2, Detector_Z/2);
  G4LogicalVolume* logicDetector =  new G4LogicalVolume(solidDetector,world_mat,"DetectorLV");
  X_pos = -1/2.*Detector_XY + 1/2.*detUnit_XY;
  
  copynumber = 0;
  Z_pos = 0;Y_pos=0;
  for(int X_index = 0; X_index < 0+X_Layers;X_index++){
    copynumber = X_index ; 
    new G4PVPlacement(0,G4ThreeVector(X_pos,Y_pos,Z_pos),logicDetectorWall,"DetectorWall",logicDetector,false,copynumber,checkOverlaps); 
    X_pos = X_pos + 1.*detUnit_XY;
  }

  // place detector in world
  // Z_pos = 0. * mm;
  Z_pos = -2708. * mm;
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(0,0,Z_pos),    //at position
                    logicDetector,             //its logical volume
                    "Detector",                //its name
                    logicWorld,                //its mother  volume
                    false,                   //no boolean operation
                    -1,                       //copy number
                    checkOverlaps);          //overlaps checking


  // Visualization attributes
  //
  // logicDetUnit->SetVisAttributes (G4VisAttributes::GetInvisible()); 
  logicDetector->SetVisAttributes (G4VisAttributes::GetInvisible());   

  auto visAttributes = new G4VisAttributes(G4Colour(0.7,0.7,0.7));
  logicFe->SetVisAttributes (visAttributes);     
  visAttributes = new G4VisAttributes(G4Colour(0.3,0.8,0.3));
  logicPlastics->SetVisAttributes (visAttributes);      
  visAttributes = new G4VisAttributes(G4Colour(0.5,1,0.5));
  logicCrystal->SetVisAttributes (visAttributes);   
  visAttributes = new G4VisAttributes(G4Colour(0.4,0.4,0.4));
  logicPb->SetVisAttributes (visAttributes);       
 
  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void B1DetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // 
  // Sensitive detectors
  //
  
  auto absoSD // fNofCrystal 就是 nofCells
    = new B1CalorimeterSD("AbsorberSD", "AbsorberHitsCollection",X_Layers, Y_Layers, Z_Layers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD);
  SetSensitiveDetector("CrystalLV",absoSD);
}