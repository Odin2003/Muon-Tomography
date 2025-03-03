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
// $Id: exampleB4a.cc 100946 2016-11-03 11:28:08Z gcosmo $
//
/// \file exampleB4a.cc
/// \brief Main program of the B4a example

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "Randomize.hh"

#include "B4DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "B4RunAction.hh"
#include "B4aEventAction.hh"
#include "B4aSteppingAction.hh"

#include "CRYGenerator.h"
#include "MuonTree.h"


#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " exampleB4a [-m macro ] [-u UIsession] [-t nThreads]" << G4endl;
    G4cerr << "   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  //
  bool batchJob=false;

  string macro;

  for ( G4int i=1; i<argc; i=i+2 ) {
    string a=argv[i];
    if      ( G4String(argv[i]) == "-b" ) { macro = argv[i+1]; batchJob=true;}
    else if ( G4String(argv[i]) == "-i" ) { macro = argv[i+1]; batchJob=false;}
    else if(a.substr(0,1)!="-") {
        std::cout<<"argument error: parameter shoudl start with -. "<<a<<std::endl;
        return 0;
     }
  }

  MuonTree*  histo = new MuonTree(macro,argc, argv);
/*
  for ( G4int i=1; i<argc; i=i+2 ) {
     if(G4String(argv[i]) == "-b" || G4String(argv[i]) == "-i") continue;
     string a=argv[i];
     string b=argv[i+1];
     std::cout<<"a="<<a<<"   size:"<<a.size()<<std::endl;
     if(a.substr(0,1)!="-") {
        std::cout<<"argument error: parameter shoudl start with -. "<<a<<std::endl;
        return 0;
     }
     if(histo->setParam(a.substr(1,a.size()-1), b)) {return 0;}
  }
*/
  // Detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = nullptr;;
  if ( !batchJob ) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Generate rndom number seeds;
  long seeds[2];
  std::chrono::system_clock::time_point now=std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(); 
  long long  micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count(); 
  std::cout<<"millis "<<millis <<std::endl; 
  std::cout<<"micros "<<micros <<std::endl; 
  long long t1=micros/10000000;
  long long t2=micros-t1*10000000;
  std::cout<<"t1="<<t1<<"   t2="<<t2<<std::endl;
  seeds[0] =long(t2);
  seeds[1] =seeds[0]+8134;
  G4Random::setTheSeeds(seeds);
  G4Random::showEngineStatus();

  // Construct a serial run manager
  //
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);

  // Set mandatory initialization classes
  //
  B4DetectorConstruction* detector = new B4DetectorConstruction(histo);
  runManager->SetUserInitialization(detector);
  //
  runManager->SetUserInitialization(new FTFP_BERT);

  // Set user action classes
  //
  PrimaryGeneratorAction* gen_action =
                          new PrimaryGeneratorAction("",detector,histo);

  runManager->SetUserAction(gen_action);
  //
  B4RunAction* run_action = new B4RunAction(histo);
  runManager->SetUserAction(run_action);
  //
  B4aEventAction* event_action = new B4aEventAction(run_action,histo);
  runManager->SetUserAction(event_action);
  //
  B4aSteppingAction* stepping_action =
                    new B4aSteppingAction(detector, event_action,histo);
  runManager->SetUserAction(stepping_action);

  // Initialize G4 kernel
  //
  runManager->Initialize();

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if ( batchJob ) {
    // batch mode
    std::cout<<"sk-debug:  batch mode"<<std::endl;
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);  // macro does not have /run/beamOn 100
    // string evtmax="100";
    string evtmax=histo->getParamS("numberOfEvents");
    string command2="/run/beamOn "+evtmax;
    UImanager->ApplyCommand(command2);
  }
  else {
    // interactive mode
    std::cout<<"sk-debug:  interactive mode"<<std::endl;
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    std::cout<<"sk-debug:  interactive mode, step 2"<<std::endl;
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  std::cout<<"Job termination..."<<std::endl;
  histo->EndJob();

  G4Random::showEngineStatus();

  // std::cout<<"deleteing visManager..."<<std::endl;
  delete visManager;
  // std::cout<<"deleteing histo..."<<std::endl;
  delete histo;
  // std::cout<<"deleteing runManager..."<<std::endl;
  delete runManager;

  return 0;
}

