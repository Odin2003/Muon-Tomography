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
// $Id: B4aSteppingAction.cc 100946 2016-11-03 11:28:08Z gcosmo $
// 
/// \file B4aSteppingAction.cc
/// \brief Implementation of the B4aSteppingAction class

#include "B4aSteppingAction.hh"
#include "B4RunAction.hh"
#include "B4aEventAction.hh"
#include "B4DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "Randomize.hh"
#include <iomanip>

#include "MuonTree.h"
#include "SC8DataStruc.h"
#include "TTree.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aSteppingAction::B4aSteppingAction(
                      B4DetectorConstruction* detectorConstruction,
                      B4aEventAction* eventAction,
                      MuonTree* histo)
  : G4UserSteppingAction(),
    fDetConstruction(detectorConstruction),
    fEventAction(eventAction),
    fMuonTree(histo)
{
     trackStopZ=fMuonTree->getParamF("trackStopZ")*1000.0;  // stop the tracking of partilce at this Z (in mm).
     
     trackMuonOnly=false;
     if(fMuonTree->getParamS("trackMuonOnly").compare(0,4,"true")==0) trackMuonOnly=true; 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aSteppingAction::~B4aSteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// ============================================================================
void B4aSteppingAction::UserSteppingAction(const G4Step* step)
{

//  std::cout<<"B4aSteppingAction::UserSteppingAction starting"<<std::endl;

     G4Track* track = step ->GetTrack();
     double zz=step->GetPreStepPoint()->GetPosition().z();
  if(zz<trackStopZ) {track->SetTrackStatus(fStopAndKill); return;}

     const G4DynamicParticle* dynamicParticle= track ->GetDynamicParticle();
     G4ParticleDefinition* particle = dynamicParticle->GetDefinition();
     G4String particleName= particle ->GetParticleName();
     int pdgID=particle->GetPDGEncoding();  // 11 elec, 13 muon, 22 photon

  if(trackMuonOnly) {
     if(abs(pdgID) != 13) {
        track->SetTrackStatus(fStopAndKill);
//		std::cout<<"UserSteppingAction starting   before frist return "<<std::endl;
        return;
     }
  }

  
  auto charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
//		std::cout<<"UserSteppingAction starting   rpdebug1... "<<std::endl;
  if (charge==0.) return; 

  auto preStepPoint = step->GetPreStepPoint();
//  auto materialName = preStep->GetMaterial()->GetName();

  auto touchable = step->GetPreStepPoint()->GetTouchable();
  auto depth = touchable->GetHistory()->GetDepth();
  if(depth==0) return;
  
     //  active this for debugging....

  //if(depth<1) return;  // 0) World, 1) SC8,  2) Station1  3) Tray1,  4) sBar
                       //                                 3) RefPlane1
                       //
//		std::cout<<"UserSteppingAction starting   rpdebug2... "<<std::endl;
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  if(edep<0.00000001) return;

  auto stepl = step->GetStepLength();  //Changed by SAS 29/11
  auto motherPhysical = touchable->GetVolume(1); // mother
  auto motherCopyNo = motherPhysical->GetCopyNo();
  auto motherName = motherPhysical->GetName();

  auto thisPhysical = touchable->GetVolume(); // mother
  auto thisCopyNo = thisPhysical->GetCopyNo();
  auto thisName = thisPhysical->GetName();

  // save energy depsit and step length...
  if(abs(pdgID)==13) {
     string newname=thisName+to_string(motherCopyNo)+to_string(thisCopyNo);
     fMuonTree->accumulateEdepSetpLength(newname,edep,stepl);
  }

  //
  //  - entering a new volume.   fGeomBoundary is 1 ...
  //
  if(preStepPoint->GetStepStatus()==fGeomBoundary) {
      // PrintStep(step);
      if(thisName.compare(0,8,"RefPlane")==0) {
          SC8Particle ref_p=FillSC8Particle(step);
          ref_p.trackid=motherCopyNo;
          fMuonTree->addRefParticle(ref_p);
      }
  }

//		std::cout<<"UserSteppingAction starting   rpdebug-end... "<<std::endl;
}  // end of B4aSteppingAction::UserSteppingAction

// ============================================================================
void B4aSteppingAction::PrintStep(const G4Step* step) {

  if(step->GetTotalEnergyDeposit()<1.0E-10) return;
  // auto charge = step->GetTrack()->GetDefinition()->GetPDGCharge();

  auto preStepPoint = step->GetPreStepPoint();

  auto touchable = step->GetPreStepPoint()->GetTouchable();
  auto depth = touchable->GetHistory()->GetDepth();

  auto motherPhysical = touchable->GetVolume(1); // mother
  auto motherCopyNo = motherPhysical->GetCopyNo();
  auto motherName = motherPhysical->GetName();

  auto thisPhysical = touchable->GetVolume(); // mother
  auto thisCopyNo = thisPhysical->GetCopyNo();
  auto thisName = thisPhysical->GetName();

  auto worldPos = preStepPoint->GetPosition();
  auto localPos
    = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPos);

  auto just_enterd=preStepPoint->GetStepStatus();

  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  G4StepPoint* point1 = step->GetPreStepPoint();
  G4ThreeVector pos1 = point1->GetPosition();
  double xx=pos1.x();
  double yy=pos1.y();
  double zz=pos1.z();

  G4Track* track = step ->GetTrack();
  G4int steps = track ->GetCurrentStepNumber();

  const G4DynamicParticle* dynamicParticle= track ->GetDynamicParticle();
  G4ParticleDefinition* particle = dynamicParticle->GetDefinition();
  G4String particleName= particle ->GetParticleName();
  G4double kinEnergy=dynamicParticle->GetKineticEnergy();

  std::cout<<" tk "<<track->GetTrackID();
  std::cout<<" step "<<steps;;
  std::cout<<"    "<<particleName;
  std::cout<<" ke "<<kinEnergy;
  std::cout<<" edep "<<edep;
  std::cout<<" xyz "<<xx<<"  "<<yy<<"  "<<zz;
  std::cout<<" depth "<<depth;
  std::cout<<"   "<<thisName;
  std::cout<<"   "<<thisCopyNo;;
  std::cout<<" ( "<<motherName;
  std::cout<<"  "<<motherCopyNo<<" )";;
  std::cout<<"  status="<<just_enterd;
  std::cout<<"  fGeomBoundary="<<fGeomBoundary;
  std::cout<<" "<<std::endl;
  return ;
}

// ============================================================================
void B4aSteppingAction::FillRefPlane(const G4Step* step, int iflag) {
  //  iflag:  0 fill GenPar,  1 fill RefPlane

//  std::cout<<"rpdebug-A1"<<std::endl;
  G4Track* trackA = step->GetTrack();
  int stepNo = trackA->GetCurrentStepNumber();

  // generted partilce
//  std::cout<<"rpdebug-A2"<<std::endl;
  if(iflag==0) {
     int parentID = trackA->GetParentID();
     //  std::cout<<"  stepNo "<<stepNo ;
     //  std::cout<<"  parentID "<<parentID ;
     // std::cout<<std::endl;
     if(parentID==0 && stepNo==1) {
        SC8Particle sc8part=FillSC8Particle(step);
        fEventAction->AddGenParticle(sc8part);
        //std::cout<<"sc8part.pid= "<<sc8part.pid<<std::endl;
     }
     if(parentID==0) {
     if(step->GetTotalEnergyDeposit()>1.0E-10) {
        auto touchable = step->GetPreStepPoint()->GetTouchable();
        auto thisPhysical = touchable->GetVolume();
        auto thisName = thisPhysical->GetName();
        // std::cout<<"  thisName="<<thisName<<std::endl;
          // if(thisName.compare(0,4,"Box1")==0)  {
          if((thisName.compare(0,4,"Tank")==0) || (thisName.compare(0,8,"TnkWater")==0)) {
              SC8Particle sc8part=FillSC8Particle(step);
              fEventAction->UpdateMuontk(sc8part);
          }
     }
     }
  }

//  std::cout<<"rpdebug-A3"<<std::endl;
  //   hit in RefPlan1...
  if(iflag==1) {
     SC8Particle sc8part=FillSC8Particle(step);
//  std::cout<<"rpdebug-A3a"<<std::endl;
     if(sc8part.pid==13 || sc8part.pid==-13) {
        // fill only muons...
//  std::cout<<"rpdebug-A3b"<<std::endl;
        fEventAction->AddHitsRef1(sc8part);
//  std::cout<<"rpdebug-A3c"<<std::endl;
     }
  }

//  std::cout<<"rpdebug-Aend"<<std::endl;
  return;
}
//===============================================================================
void B4aSteppingAction::FillTray1Plane(const G4Step* step, int iflag) {
 
//  std::cout<<"rpdebug-A3"<<std::endl;
  //   hit in RefPlan1...
  if(iflag==1) {
     SC8Particle sc8part=FillSC8Particle(step);
//  std::cout<<"rpdebug-A3a"<<std::endl;
     if(sc8part.pid==13 || sc8part.pid==-13) {
        // fill only muons...
//  std::cout<<"rpdebug-A3b"<<std::endl;
        fEventAction->AddHits1Tray(sc8part);
//  std::cout<<"rpdebug-A3c"<<std::endl;
     }
  }

//  std::cout<<"rpdebug-Aend"<<std::endl;
  return;
}

// ============================================================================
void B4aSteppingAction::FillTray2Plane(const G4Step* step, int iflag) {
  
//  std::cout<<"rpdebug-A3"<<std::endl;
  //   hit in RefPlan1...
  if(iflag==1) {
     SC8Particle sc8part=FillSC8Particle(step);
//  std::cout<<"rpdebug-A3a"<<std::endl;
     if(sc8part.pid==13 || sc8part.pid==-13) {
        // fill only muons...
//  std::cout<<"rpdebug-A3b"<<std::endl;
        fEventAction->AddHits2Tray(sc8part);
//  std::cout<<"rpdebug-A3c"<<std::endl;
     }
  }

//  std::cout<<"rpdebug-Aend"<<std::endl;
  return;
}

// ============================================================================
SC8Particle B4aSteppingAction::FillSC8Particle(const G4Step* step) {
     G4Track* trackA = step->GetTrack();
     G4StepPoint* pointA = step->GetPreStepPoint();  
     G4ThreeVector posA = pointA->GetPosition();  // world position
     auto touchable = step->GetPreStepPoint()->GetTouchable();
     auto localPos=touchable->GetHistory()->GetTopTransform().TransformPoint(posA);
     SC8Particle sc8part;
     sc8part.x=posA.x()/1000.;   // in m
     sc8part.y=posA.y()/1000.;
     sc8part.z=posA.z()/1000.;
     G4ThreeVector pxyz=trackA->GetMomentum();
     sc8part.px=pxyz.x()/1000.0;  // in GeV
     sc8part.py=pxyz.y()/1000.0;
     sc8part.pz=pxyz.z()/1000.0;
     const G4DynamicParticle* dynamicParticleA= trackA ->GetDynamicParticle();
     G4ParticleDefinition* particleA = dynamicParticleA->GetDefinition();
     sc8part.ma=particleA->GetPDGMass();
     sc8part.pid=particleA->GetPDGEncoding();
     sc8part.trackid=trackA->GetTrackID();
     sc8part.steplength=step->GetStepLength();
     sc8part.edep=step->GetTotalEnergyDeposit();

     sc8part.loc_px=1000.0;
     sc8part.loc_py=1000.0;
     sc8part.loc_pz=1000.0;
     sc8part.loc_x=localPos.x();
     sc8part.loc_y=localPos.y();
     sc8part.loc_z=localPos.z();
  return sc8part;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
