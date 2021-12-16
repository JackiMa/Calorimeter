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
/// \file B1CalorHit.hh
/// \brief Definition of the B1CalorHit class

#ifndef B1CalorHit_h
#define B1CalorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

/// Calorimeter hit class
///
/// It defines data members to store the the energy deposit and track lengths
/// of charged particles in a selected volume:
/// - fEdep, fTrackLength

class B1CalorHit : public G4VHit
{
public:
  B1CalorHit();
  B1CalorHit(const B1CalorHit &);
  virtual ~B1CalorHit();

  // operators
  const B1CalorHit &operator=(const B1CalorHit &);
  G4bool operator==(const B1CalorHit &) const;

  inline void *operator new(size_t);
  inline void operator delete(void *);

  // methods from base class
  virtual void Draw() {}
  virtual void Print();

  // methods to handle data
  void Add(G4double edep);

  // set methods
  void SetChamberNb(G4int chamb) { fChamberNb = chamb; }
  void SetCellID(G4int CellID) { fCellID = CellID; }

  // get methods
  G4double GetEdep() const;
  G4double GetTrackLength() const;
  G4int GetChamberNb() const { return fChamberNb; }
  G4int GetCellID() const { return fCellID; }

public:
  G4int fChamberNb;
  G4int fCellID=0;
  G4double fEdep; ///< Energy deposit in the sensitive volume
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using B1CalorHitsCollection = G4THitsCollection<B1CalorHit>;

extern G4ThreadLocal G4Allocator<B1CalorHit> *B1CalorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void *B1CalorHit::operator new(size_t)
{
  if (!B1CalorHitAllocator)
  {
    B1CalorHitAllocator = new G4Allocator<B1CalorHit>;
  }
  void *hit;
  hit = (void *)B1CalorHitAllocator->MallocSingle();
  return hit;
}

inline void B1CalorHit::operator delete(void *hit)
{
  if (!B1CalorHitAllocator)
  {
    B1CalorHitAllocator = new G4Allocator<B1CalorHit>;
  }
  B1CalorHitAllocator->FreeSingle((B1CalorHit *)hit);
}

inline void B1CalorHit::Add(G4double de)
{
  fEdep += de;
}

inline G4double B1CalorHit::GetEdep() const
{
  return fEdep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
