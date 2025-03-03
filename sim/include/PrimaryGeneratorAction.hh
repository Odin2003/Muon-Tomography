//******************************************************************************
// PrimaryGeneratorAction.hh
//
// This class is a class derived from G4VUserPrimaryGeneratorAction for 
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
// 
#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "CRYSetup.h"
#include "CRYGenerator.h"
#include "CRYParticle.h"
#include "CRYUtils.h"
#include "vector"
#include "RNGWrapper.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "B4aEventAction.hh"

class G4Event;

class B4DetectorConstruction;
class MuonTree;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(const char*,  B4DetectorConstruction*, MuonTree*);
    ~PrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event* anEvent);
    void InputCRY();
    void UpdateCRY(std::string* MessInput);
    void CRYFromFile(G4String newValue);
    
  private:
    B4DetectorConstruction* fB4DetectorConstruction;
    MuonTree* fMuonTree;

    void initSingleParticleGun();
    void executeSingleParticleGun(G4Event* anEvent);

    std::vector<CRYParticle*> *vect; // vector of generated particles
    G4ParticleTable* particleTable;
    G4ParticleGun* particleGun;
    CRYGenerator* gen;
    G4int InputState;
    PrimaryGeneratorMessenger* gunMessenger;
    //    momentum cut  (in MeV)
    G4double momentumCut;
    G4double XSHIFT;
    G4double YSHIFT;
    G4double ZSHIFT;
    G4double DZSHIFT;

    // CLHEP::HepRandomEngine &  fEngine;
 };

#endif

