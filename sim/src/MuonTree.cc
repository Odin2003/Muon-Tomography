#include "MuonTree.h"

#include <iostream>  // for cout
#include <fstream>   // for input/output files
#include <sstream>   // for string stream
#include <math.h>    // for sin(x) etc.
#include <cstdlib>   // for rand() on archer.
#include <iomanip>   // for setw() in cout, 
#include <vector>
#include <ctime>
#include <chrono>   // from std::

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TDirectory.h"
#include "TGraph.h"
#include "TEllipse.h"
#include "TText.h"
#include "TPaveText.h"

#include "SC8DataStruc.h"


using namespace std;

MuonTree::MuonTree(string macFileName,int argc,char** argv)
{

   cout<<"initializing MuonTree...   macFileName:"<<macFileName<<endl;
   startTimeJob=std::chrono::steady_clock::now();

   readMacFile(macFileName);

   //  overwrite params from argc, argv...
  for ( int i=1; i<argc; i=i+2 ) {
     if(string(argv[i]) == "-b" || string(argv[i]) == "-i") continue;
     string a=argv[i];
     string b=argv[i+1];
     std::cout<<"a="<<a<<"   size:"<<a.size()<<std::endl;
     setParam(a.substr(1,a.size()-1), b);
  }

   runConfig=getParamS("runConfig");
   runNumber=getParamI("runNumber");

   string outname=getParamS("jobName")+"_run"
                 +getParamS("runNumber")+"_"
                 +getParamS("runSeq")+"_"
                 +getParamS("runConfig")+"_"
                 +getParamS("numberOfEvents");
   string outRootName=getParamS("rootPre")+"_"+outname+".root";
   string outMuonListName=getParamS("MuonListPre")+"_"+outname+".txt";

   eventCounts=0;
   eventCountsALL=0;

   createMuonList=false;
   if(getParamS("createMuonList").compare(0,4,"true")==0) {
       createMuonList=true;
       // muonListFile.open(getParamS("MuonListFile"));
       cout<<"open MuonListFile="<<outMuonListName<<endl;
       muonListFile.open(outMuonListName);
       map<string,string>::iterator it;

       muonListFile<<"### parametes ###"<<endl;
       for(it=mcParams.begin(); it!=mcParams.end(); ++it) {
         muonListFile<<"#  "<<it->first <<" => " << it->second <<endl; 
       }  
       muonListFile<<"# run event refId seq x y z px py pz p config"<<endl;
   } 
      
   // _MuonXG4OutName=getParamS("rootFile");
   _MuonXG4OutName=outRootName;;
   fout=new TFile(_MuonXG4OutName.c_str(),"recreate");

   createNtuple=false;
   if(getParamS("createNtuple").compare(0,4,"true")==0) createNtuple=true;

   histo1D["all_CryTime"]=new TH1D("all_CryTime","CRY: simullated time (sec)",1,0.0,2.0);
   histo1D["all_nCry"]=new TH1D("all_nCry","CRY: (all)  number of particles",20,0.0,20.0);
   histo1D["all_CryPid"]=new TH1D("all_CryPid","CRY: (all)  PID",40,-20.0,20.0);
   histo1D["all_CryTheta"]=new TH1D("all_CryTheta","CRY: (all)  dN/dt vs theta",20,0.0,100.0);
   histo1D["all_CryP"]=new TH1D("all_CryP","CRY: (all)  muon P  (GeV)",200,0.0,100.0);
   histo1D["all_CryThStr"]=new TH1D("all_CryThStr","CRY: (all)  dN/dStr vs theta",20,0.0,100.0);
   histo1D["all_CryVx_small"]=new TH1D("all_CryVx_small","CRY- (all) X (meter) in CRY plane",100,-5.0,5.0);
   histo1D["all_CryVy_small"]=new TH1D("all_CryVy_small","CRY- (all) Y (meter) in CRY plane",100,-5.0,5.0);
   histo1D["all_CryVz_small"]=new TH1D("all_CryVz_small","CRY- (all) Z (meter) in CRY plane",100,-5.0,5.0);
   histo1D["all_CryVx_large"]=new TH1D("all_CryVx_large","CRY- (all) X (meter) in CRY plane",100,-500.0,500.0);
   histo1D["all_CryVy_large"]=new TH1D("all_CryVy_large","CRY- (all) Y (meter) in CRY plane",100,-500.0,500.0);
   histo1D["all_CryVz_large"]=new TH1D("all_CryVz_large","CRY- (all) Z (meter) in CRY plane",100,-500.0,500.0);
   histo1D["trig_nCry"]=new TH1D("trig_nCry","CRY: (trig)  number of particles",20,0.0,20.0);
   histo1D["trig_CryP"]=new TH1D("trig_CryP","CRY: (trig)  P (GeV)",200,0.0,100.0);
   histo1D["trig_CryVx_small"]=new TH1D("trig_CryVx_small","CRY- (trig) X (meter) in CRY plane",100,-5.0,5.0);
   histo1D["trig_CryVy_small"]=new TH1D("trig_CryVy_small","CRY- (trig) Y (meter) in CRY plane",100,-5.0,5.0);
   histo1D["trig_CryVz_small"]=new TH1D("trig_CryVz_small","CRY- (trig) Z (meter) in CRY plane",100,-5.0,5.0);
   histo1D["trig_CryVx_large"]=new TH1D("trig_CryVx_large","CRY- (trig) X (meter) in CRY plane",100,-500.0,500.0);
   histo1D["trig_CryVy_large"]=new TH1D("trig_CryVy_large","CRY- (trig) Y (meter) in CRY plane",100,-500.0,500.0);
   histo1D["trig_CryVz_large"]=new TH1D("trig_CryVz_large","CRY- (trig) Z (meter) in CRY plane",100,-500.0,500.0);


   histo1D["all_nGen"]=new TH1D("all_nGen","N of Gen Particles (all)",40,0.,40.0);
   histo1D["all_GenPid"]=new TH1D("all_GenPid","Gen Particle ID (all)",40,-20.0,20.0);
   histo1D["all_GenPmu"]=new TH1D("all_GenPmu","Gen muon. P (MeV) (all)",100,0.,100.0);
   histo1D["all_GenPnonMu"]=new TH1D("all_PnonMu","Gen non-muon. P (MeV) (all)",100,0.,100.0);
   histo1D["all_GenPxPz"]=new TH1D("all_GenPxPz","slopeX, px/pz (all)",100,-2.0,2.0);
   histo1D["all_GenPyPz"]=new TH1D("all_GenPyPz","slopeY, py/pz (all)",100,-2.0,2.0);
   histo1D["all_GenPtPz"]=new TH1D("all_GenPtPz","slope, pt/pz (all)",100,0.0,2.0);

   histo1D["all_Edep"]=new TH1D("all_Edep","Tray:  Edep (trig) ",100,0.,10.0);
   histo1D["all_Step"]=new TH1D("all_Step","Tray:  Step (trig) (mm)",100,0.,50.0);

   histo1D["ProjX"]=new TH1D("ProjX","Muon- Projected X (in m at Y=0)",100,-25.0,25.0);
   histo1D["ProjZ"]=new TH1D("ProjZ","Muon- Projected Z (in m at Y=0)",250,-5.0 ,20.0);
   histo1D["ProjZ1"]=new TH1D("ProjZ1","Muon- Projected Z x=[-6,-2] (in m at Y=0)",300,-10.0 ,20.0);
   histo1D["ProjZ2"]=new TH1D("ProjZ2","Muon- Projected Z x=[-2,+2](in m at Y=0)",250,-5.0 ,20.0);
   histo1D["ProjZ3"]=new TH1D("ProjZ3","Muon- Projected Z x=[+2,+4](in m at Y=0)",250,-5.0 ,20.0);
   histo2D["ProjXZ"]=new TH2D("ProjXZ","Muon- Projected X-Z (in m at Y=0)",20,-10.,10.,16,0.0,8.);
   
   // define root tree...
   tree=new TTree("tree","Cosmic Muon Tree");

    tree->Branch("RunConfig"        ,&mRunConfig );
    tree->Branch("RunNumber"        ,&mRunNumber );
    tree->Branch("EventNumber"      ,&mEventNumber );

    tree->Branch("trigger"        ,&mTrigger  );

    tree->Branch("CryTime"        ,&mCryTime );

  if(getParamS("miniNtuple").compare(0,4,"true")==0) {
    tree->Branch("nCry"        ,&mnCry );
    tree->Branch("CryPid"       ,&mCryPid );
    tree->Branch("CryPy"       ,&mCryPy );
    tree->Branch("CryPz"       ,&mCryPz );
    tree->Branch("CryP"        ,&mCryP );
    tree->Branch("CryMass"     ,&mCryMass  );
    tree->Branch("CryVx"       ,&mCryVx );
    tree->Branch("CryVy"       ,&mCryVy );
    tree->Branch("CryVz"       ,&mCryVz );
    tree->Branch("CryLocPx"       ,&mCryLocPx );
    tree->Branch("CryLocPy"       ,&mCryLocPy );
    tree->Branch("CryLocPz"       ,&mCryLocPz );
    tree->Branch("CryLocVx"       ,&mCryLocVx );
    tree->Branch("CryLocVy"       ,&mCryLocVy );
    tree->Branch("CryLocVz"       ,&mCryLocVz );
    tree->Branch("CryTheta"     ,&mCryTheta );
    tree->Branch("CryPhi"       ,&mCryPhi );
  }

  if(getParamS("miniNtuple").compare(0,4,"true")==0) {
    tree->Branch("nGenPart"        ,&mnGen  );
    tree->Branch("GenPartId"       ,&mGenPid );
    tree->Branch("GenPartPx"       ,&mGenPx );
    tree->Branch("GenPartPy"       ,&mGenPy );
    tree->Branch("GenPartPz"       ,&mGenPz );
    tree->Branch("GenPartP"        ,&mGenP  );
    tree->Branch("GenPartMass"     ,&mGenMass );
    tree->Branch("GenPartVx"       ,&mGenVx );
    tree->Branch("GenPartVy"       ,&mGenVy );
    tree->Branch("GenPartVz"       ,&mGenVz );
  }

    tree->Branch("nHitRef"        ,&mnRef  );
    tree->Branch("RefId"         ,&mRefId  );
    tree->Branch("RefPid"        ,&mRefPId  );
    tree->Branch("RefPx"         ,&mRefPx  );
    tree->Branch("RefPy"         ,&mRefPy  );
    tree->Branch("RefPz"         ,&mRefPz  );
    tree->Branch("RefP"         ,&mRefP  );
    tree->Branch("RefMass"       ,&mRefMass  );
    tree->Branch("RefVx"         ,&mRefVx  );
    tree->Branch("RefVy"         ,&mRefVy );
    tree->Branch("RefVz"         ,&mRefVz  );
    // tree->Branch("RefLocPx"      ,&mRefLocPx  );
    // tree->Branch("RefLocPy"      ,&mRefLocPy  );
    // tree->Branch("RefLocPz"         ,&mRefLocPz  );
    // tree->Branch("RefLocVx"         ,&mRefLocVx  );
    // tree->Branch("RefLocVy"         ,&mRefLocVy  );
    // tree->Branch("RefLocVz"         ,&mRefLocVz  );
    tree->Branch("RefTheta"         ,&mRefTheta  );
    tree->Branch("RefPhi"           ,&mRefPhi  );
    tree->Branch("RefProjVx"         ,&mRefProjVx  );
    tree->Branch("RefProjVy"         ,&mRefProjVy  );
    tree->Branch("RefProjVz"         ,&mRefProjVz  );


  if(getParamS("miniNtuple").compare(0,4,"true")==0) {
    tree->Branch("nTray"         ,&mnTray  );
    tree->Branch("TrayId"        ,&mTrayId  );
    tree->Branch("TrayEdep"        ,&mTrayEdep  );
    tree->Branch("TrayStep"        ,&mTrayStep  );
  }

    tree->Branch("nCairn"         ,&mnCairn  );
    tree->Branch("CairnId"        ,&mCairnId  );
    tree->Branch("CairnEdep"        ,&mCairnEdep  );
    tree->Branch("CairnStep"        ,&mCairnStep  );

    tree->Branch("nChamber"         ,&mnChamber  );
    tree->Branch("ChamberId"        ,&mChamberId  );
    tree->Branch("ChamberEdep"        ,&mChamberEdep  );
    tree->Branch("ChamberStep"        ,&mChamberStep  );

    tree->Branch("nPassage"           ,&mnPassage  );
    tree->Branch("PassageId"          ,&mPassageId  );
    tree->Branch("PassageEdep"        ,&mPassageEdep  );
    tree->Branch("PassageStep"        ,&mPassageStep  );

  if(getParamS("miniNtuple").compare(0,4,"true")==0) {
    tree->Branch("nSimTk"       ,&mnSimTk  );
    tree->Branch("SimTkSeqID"   ,&mSimTkSeqId  );
    tree->Branch("SimTkPointId" ,&mSimTkPointId  );  // 0, 1, 111/121, 211/221, 311/321, see .h for definition. 
    tree->Branch("SimTkPx"     ,&mSimTkPx  );
    tree->Branch("SimTkPy"     ,&mSimTkPy  );
    tree->Branch("SimTkPz"     ,&mSimTkPz  );
    tree->Branch("SimTkVx"     ,&mSimTkVx  );
    tree->Branch("SimTkVy"     ,&mSimTkVy  );
    tree->Branch("SimTkVz"     ,&mSimTkVz  );
    tree->Branch("SimTkTime"   ,&mSimTkTime  );
  }
}

// ########################################################################
MuonTree::~MuonTree(){
   std::cout<<"deleting MuonTree..."<<std::endl;
}


// ########################################################################
void MuonTree::BeginEvent() {
     // This is clled from PrimaryGeneratorAction::GeneratePrimaries,
     // not from B4aEventAction.. 
     clearMuonTree();
     clearAccumulatedEdepSetpLength();
}

// ########################################################################
void MuonTree::EndEvent() {

     saveAccumulatedEdepSetpLength();
     analyze();
}

// ########################################################################
void MuonTree::EndJob() {
  cout<<"MuonTree  endjob..."<<endl;
  std::chrono::steady_clock::time_point endTime=std::chrono::steady_clock::now();
  std::chrono::duration<double> time_span=std::chrono::duration_cast<std::chrono::duration<double>>(endTime-startTimeJob);
  std::cout<<"Duration of this job: "<<time_span.count()<<" seconds"<<std::endl;
  //
  std::cout<<"Total time for PrimaryGeneratorAction: "<<totalTimePGA.count()<<" seconds"<<std::endl;
  std::cout<<"Total time for TimeB4aEventAction:   : "<<totalTimeEVT.count()<<" seconds"<<std::endl;

  //
  fout->Write();
  fout->Close();
  if(getParamS("createMuonList").compare(0,4,"true")) {
      muonListFile.close();
  }

}

// ########################################################################
void MuonTree::startTimePrimaryGeneratorAction() {
   startTimePGA=std::chrono::steady_clock::now();
}

// ########################################################################
void MuonTree::endTimePrimaryGeneratorAction() {
   std::chrono::steady_clock::time_point endTime=std::chrono::steady_clock::now();
   std::chrono::duration<double> time_span=std::chrono::duration_cast<std::chrono::duration<double>>(endTime-startTimePGA);
   totalTimePGA=totalTimePGA+time_span;
}

// ########################################################################
void MuonTree::startTimeB4aEventAction() {
   startTimeEVT=std::chrono::steady_clock::now();
}

// ########################################################################
void MuonTree::endTimeB4aEventAction() {
   std::chrono::steady_clock::time_point endTime=std::chrono::steady_clock::now();
   std::chrono::duration<double> time_span=std::chrono::duration_cast<std::chrono::duration<double>>(endTime-startTimeEVT);
   totalTimeEVT=totalTimeEVT+time_span;
}

// ########################################################################
void MuonTree::clearMuonTree() {

   mRunConfig.clear();
   mRunNumber.clear();
   mEventNumber.clear();

   mTrigger.clear();
   mTrigger.push_back(0);  // for camera 0
   mTrigger.push_back(0);  // for camera 1

   mCryTime.clear();
   mnCry.clear();
   mnCry.push_back(0);  // clear the counter
   mCryPid.clear();
   mCryTheta.clear();
   mCryPhi.clear();
   mCryPx.clear();
   mCryPy.clear();
   mCryPz.clear();
   mCryP.clear();
   mCryMass.clear();
   mCryVx.clear();    
   mCryVy.clear();
   mCryVz.clear();
   mCryLocPx.clear(); 
   mCryLocPy.clear();
   mCryLocPz.clear();
   mCryLocVx.clear();
   mCryLocVy.clear();
   mCryLocVz.clear();
   mnGen.clear();
   mnGen.push_back(0);  
   mGenPid.clear();
   mGenPx.clear();
   mGenPy.clear();
   mGenPz.clear();
   mGenP.clear();
   mGenMass.clear();
   mGenVx.clear();
   mGenVy.clear();
   mGenVz.clear();

   mnRef.clear();    
   mnRef.push_back(0);
   mRefId.clear();
   mRefPId.clear();
   mRefPx.clear();
   mRefPy.clear();
   mRefPz.clear();
   mRefP.clear();
   mRefMass.clear();
   mRefVx.clear();
   mRefVy.clear();
   mRefVz.clear();
   mRefLocPx.clear();
   mRefLocPy.clear();
   mRefLocPz.clear();
   mRefLocVx.clear();
   mRefLocVy.clear();
   mRefLocVz.clear();
   mRefTheta.clear();
   mRefPhi.clear();
   mRefProjVx.clear();
   mRefProjVy.clear();
   mRefProjVz.clear();

   mnTray.clear();
   mTrayId.clear();    
   mTrayEdep.clear();   
   mTrayStep.clear();   

   mnCairn.clear();
   mCairnId.clear();    
   mCairnEdep.clear(); 
   mCairnStep.clear();  

   mnChamber.clear();
   mChamberId.clear();    
   mChamberEdep.clear(); 
   mChamberStep.clear(); 

   mnPassage.clear();
   mPassageId.clear();    
   mPassageEdep.clear();  
   mPassageStep.clear(); 

   mnSimTk.clear();
   mnSimTk.push_back(0);   
   mSimTkSeqId.clear(); 
   mSimTkPointId.clear();   
   mSimTkPx.clear();    
   mSimTkPy.clear();
   mSimTkPz.clear();
   mSimTkVx.clear();
   mSimTkVy.clear();
   mSimTkVz.clear();
   mSimTkTime.clear();

}

// ########################################################################
void MuonTree::analyze(){

  // cout<<"MuonTree::analyze() is called..."<<endl;

  eventCountsALL=eventCountsALL+1;

  bool trig=false;

     int tray00=0;
     int tray01=0;
     int tray10=0;
     int tray11=0;
     for(unsigned  it=0; it<mTrayEdep.size(); it++) {
        if(mTrayEdep[it]>0.5) {
           if(mTrayId[it]==0)  { tray10=1;}
           if(mTrayId[it]==1)  { tray11=1;}
           if(mTrayId[it]==10) { tray10=1;}
           if(mTrayId[it]==11) { tray11=1;}
        }
     }    
     mTrigger[0]=tray00+tray01;
     mTrigger[1]=tray10+tray11;
     
  if(mTrigger[0]==2 || mTrigger[1]==2) trig=true;

  double ecut=1.0; // cut on edep in each layer (in MeV)

  double pi=M_PI;
  double halfpi=pi*0.5;
  double raddeg=180.0/pi;

  histo1D["all_CryTime"]->SetBinContent(1,lastCryTime);
  histo1D["all_nCry"]->Fill(mnCry[0]); if(trig) histo1D["trig_nCry"]->Fill(mnCry[0]);
  for(int i=0; i<mnCry[0]; i++){
     // std::cout<<"skdebug- MuonTree Analyze:  mNCryPar="<<mNCryPar<<"  i="<<i<<"   pp="<<mCryParP[i]<<std::endl;
     histo1D["all_CryPid"]->Fill(mCryPid[i]);
     if(mCryPid[i]==13 || mCryPid[i]==-13) {
        histo1D["all_CryP"]->Fill(mCryP[i]/1000.);
    
        double a=abs(mCryPz[i])/mCryP[i];
        double theta=acos(a);
        double thetaDegree=theta*raddeg;;
        double wt=1.0/sin(theta);
        histo1D["all_CryTheta"]->Fill(thetaDegree);
        histo1D["all_CryThStr"]->Fill(thetaDegree,wt);
        histo1D["all_CryVx_small"]->Fill(mCryVx[i]);
        histo1D["all_CryVy_small"]->Fill(mCryVy[i]);
        histo1D["all_CryVz_small"]->Fill(mCryVz[i]);
        histo1D["all_CryVx_large"]->Fill(mCryVx[i]);
        histo1D["all_CryVy_large"]->Fill(mCryVy[i]);
        histo1D["all_CryVz_large"]->Fill(mCryVz[i]);
        if(trig) {
           histo1D["trig_CryP"]->Fill(mCryP[i]/1000.);
           histo1D["trig_CryVx_small"]->Fill(mCryVx[i]);
           histo1D["trig_CryVy_small"]->Fill(mCryVy[i]);
           histo1D["trig_CryVz_small"]->Fill(mCryVz[i]);
           histo1D["trig_CryVx_large"]->Fill(mCryVx[i]);
           histo1D["trig_CryVy_large"]->Fill(mCryVy[i]);
           histo1D["trig_CryVz_large"]->Fill(mCryVz[i]);
        }
     }
  }

  histo1D["all_nGen"]->Fill(mnGen[0]);
  for(int i=0; i<mnGen[0]; i++){
     histo1D["all_GenPid"]->Fill(mGenPid[i]);
     if(mGenPid[i]==13 || mGenPid[i]==-13) {
        histo1D["all_GenPmu"]->Fill(mGenP[i]);
        histo1D["all_GenPxPz"]->Fill(mGenPx[i]/mGenPz[i]);
        histo1D["all_GenPyPz"]->Fill(mGenPy[i]/mGenPz[i]);
        histo1D["all_GenPtPz"]->Fill(sqrt(mGenPx[i]*mGenPx[i]+mGenPy[i]*mGenPy[i])/abs(mGenPz[i]));
     } else {
        histo1D["all_GenPnonMu"]->Fill(mGenP[i]);
     }
  }

     for(unsigned  it=0; it<mTrayEdep.size(); it++) {
        if(mTrayEdep[it]>0.0001) histo1D["all_Edep"]->Fill(mTrayEdep[it]);
        if(mTrayStep[it]>0.0001) histo1D["all_Step"]->Fill(mTrayStep[it]);
     }     

     double xlimitPlus=getParamF("chamberSizeX")/2.0;
     double xlimitMinus=-xlimitPlus;
     for(unsigned it=0; it<mRefProjVx.size(); it++) {
        histo1D["ProjX"]->Fill(mRefProjVx[it]);
        histo1D["ProjZ"]->Fill(mRefProjVz[it]);
        histo2D["ProjXZ"]->Fill(mRefProjVx[it],mRefProjVz[it]);
        if(mRefProjVx[it]>-6.0 && mRefProjVx[it]<-2.0) histo1D["ProjZ1"]->Fill(mRefProjVz[it]);
        // if(mRefProjVx[it]>-2.0 && mRefProjVx[it]<2.0) histo1D["ProjZ2"]->Fill(mRefProjVz[it]);
        if(mRefProjVx[it]>xlimitMinus && mRefProjVx[it]<xlimitPlus) histo1D["ProjZ2"]->Fill(mRefProjVz[it]);
        if(mRefProjVx[it]>2.0  && mRefProjVx[it]<6.0) histo1D["ProjZ3"]->Fill(mRefProjVz[it]);
     }

     //  send firns N-events to tree output for monitoring purpose.
     if(eventCountsALL<getParamI("monitorEvents")) trig=true;

     // random sampling...
     

     if(trig) { 
        eventCounts=eventCounts+1;
        mRunConfig.push_back(runConfig);
        mRunNumber.push_back(runNumber);
        mEventNumber.push_back(eventCounts);
        if(createNtuple) {
           tree->Fill();   //  write this event to ntuple.
        }
        if(createMuonList) {
           writeMuonList();
        } 
     }  // end of if(trig)

}

//  =============================================================================
void MuonTree::addCryParticle(SC8Particle aParticle){
    mCryTime.push_back(aParticle.steplength);  // aParticle.steplength is used for Cry time.
    lastCryTime=aParticle.steplength;
    mCryPid.push_back(aParticle.pid);
    mCryPx.push_back(aParticle.px);
    mCryPy.push_back(aParticle.py);
    mCryPz.push_back(aParticle.pz);
    mCryP.push_back(aParticle.pp);
    mCryVx.push_back(aParticle.x);
    mCryVy.push_back(aParticle.y);
    mCryVz.push_back(aParticle.z);

    double degrad=180.0/M_PI;
    double theta=atan2(-aParticle.pz,-aParticle.px)*degrad;
    double phi=atan2(-aParticle.py,-aParticle.px)*degrad;
    mCryTheta.push_back(theta);
    mCryPhi.push_back(phi);

    mCryLocPx.push_back(aParticle.loc_px);
    mCryLocPy.push_back(aParticle.loc_py);
    mCryLocPz.push_back(aParticle.loc_pz);
    mCryLocVx.push_back(aParticle.loc_x);
    mCryLocVy.push_back(aParticle.loc_y);
    mCryLocVz.push_back(aParticle.loc_z);
    mnCry[0]=mCryPid.size();
    // std::cout<<"MuonTree::addCryParticle   mNCryPar="<<mnCry[0]<<std::endl;
}

//  =============================================================================
void MuonTree::addGenParticle(SC8Particle aParticle) {
     mGenPid.push_back(aParticle.pid);
     mGenPx.push_back(aParticle.px);
     mGenPy.push_back(aParticle.py);
     mGenPz.push_back(aParticle.pz);;
     mGenP.push_back(sqrt(aParticle.px*aParticle.px
                +aParticle.py*aParticle.py+aParticle.pz*aParticle.pz));
     mGenMass.push_back(aParticle.ma);
     mGenVx.push_back(aParticle.x);
     mGenVy.push_back(aParticle.y);
     mGenVz.push_back(aParticle.z);

     mnGen[0]=mGenPid.size();
     // std::cout<<"MuonTree::addGenParticle   mnGen="<<mnGen[0]<<std::endl;
}
//  =============================================================================
void MuonTree::clearAccumulatedEdepSetpLength() {

     if(edepSum.size()>0) edepSum.clear();
     if(stepSum.size()>0) stepSum.clear();

     edepSum["Cairn00"]=0.0;
     edepSum["Chamber00"]=0.0;
     edepSum["Passage00"]=0.0;
     edepSum["Tray00"]=0.0;
     edepSum["Tray01"]=0.0;
     edepSum["Tray10"]=0.0;
     edepSum["Tray11"]=0.0;

     stepSum["Cairn00"]=0.0;
     stepSum["Chamber00"]=0.0;
     stepSum["Passage00"]=0.0;
     stepSum["Tray00"]=0.0;
     stepSum["Tray01"]=0.0;
     stepSum["Tray10"]=0.0;
     stepSum["Tray11"]=0.0;


}

//  =============================================================================
void MuonTree::accumulateEdepSetpLength(string name,double edep, double stepLength) {
       edepSum[name]=edepSum[name]+edep;
       stepSum[name]=stepSum[name]+stepLength;
}

//  =============================================================================
void MuonTree::saveAccumulatedEdepSetpLength() {
     /*     
     cout<<"MuonTree::saveAccumulatedEdepSetpLength():    printing map"<<endl;
     for (const auto& [key,value]: edepSum) {
        cout<<key<<"  "<<value<<endl;
     }
     */
     
     mCairnEdep.push_back(edepSum["Cairn00"]); 
     mChamberEdep.push_back(edepSum["Chamber00"]);
     mPassageEdep.push_back(edepSum["Passage00"]);
     mTrayEdep.push_back(edepSum["Tray00"]);
     mTrayEdep.push_back(edepSum["Tray01"]);
     mTrayEdep.push_back(edepSum["Tray10"]);
     mTrayEdep.push_back(edepSum["Tray11"]);

     mCairnStep.push_back(stepSum["Cairn00"]);
     mChamberStep.push_back(stepSum["Chamber00"]);
     mPassageStep.push_back(stepSum["Passage00"]);
     mTrayStep.push_back(stepSum["Tray00"]);
     mTrayStep.push_back(stepSum["Tray01"]);
     mTrayStep.push_back(stepSum["Tray10"]);
     mTrayStep.push_back(stepSum["Tray11"]);
     
     mCairnId.push_back(0); 
     mChamberId.push_back(0);
     mPassageId.push_back(0);
     mTrayId.push_back(0);
     mTrayId.push_back(1);
     mTrayId.push_back(10);
     mTrayId.push_back(11);

     mnCairn.push_back(mCairnId.size());
     mnChamber.push_back(mChamberId.size());
     mnPassage.push_back(mPassageId.size());
     mnTray.push_back(mTrayId.size());

}

//  =============================================================================
void MuonTree::addRefParticle(SC8Particle aParticle) {

     // keep only muons...     
     if(abs(aParticle.pid)!=13) return;

     mRefId.push_back(aParticle.trackid);  // camera copy number is in trackid
     mRefTkId.push_back(0);
     mRefPId.push_back(aParticle.pid);
     mRefPx.push_back(aParticle.px);
     mRefPy.push_back(aParticle.py);
     mRefPz.push_back(aParticle.pz);;
     mRefP.push_back(sqrt(aParticle.px*aParticle.px
                +aParticle.py*aParticle.py+aParticle.pz*aParticle.pz));
     mRefMass.push_back(aParticle.ma);
     mRefVx.push_back(aParticle.x);
     mRefVy.push_back(aParticle.y);
     mRefVz.push_back(aParticle.z);
   
     double raddeg=180.0/M_PI;
     double theta=atan2(-aParticle.pz,-aParticle.px)*raddeg;
     double phi=atan2(-aParticle.py,-aParticle.px)*raddeg;
     mRefTheta.push_back(theta);
     mRefPhi.push_back(phi);
 
     mRefLocPx.push_back(0.0);  
     mRefLocPy.push_back(0.0);  
     mRefLocPz.push_back(0.0);  
     mRefLocVx.push_back(0.0);  
     mRefLocVy.push_back(0.0);  
     mRefLocVz.push_back(0.0);  

     double dy=abs(aParticle.y);
     mRefProjVx.push_back(dy*aParticle.px/aParticle.py+aParticle.x);
     mRefProjVy.push_back(0.0);
     mRefProjVz.push_back(dy*aParticle.pz/aParticle.py+aParticle.z);

     if(mnRef.size()==0) {
       mnRef.push_back(1);
     } else {
       mnRef[0]=mnRef[0]+1;
     }
}


//  =============================================================================

void MuonTree::updateSimTK(SC8Particle aParticle) {
}

//  =============================================================================
void MuonTree::writeMuonList() {

    // muonListFile<<"# run event refId seq x y z px py pz p config"<<endl;
    for(unsigned i=0; i<mRefPx.size(); i++) {
       muonListFile<<runNumber<<" "
                 <<eventCounts<<" "
                 <<mRefId[i]<<" "
                 <<i<<" "
                 <<mRefVx[i]<<" "<<mRefVy[i]<<" "<<mRefVz[i]<<" "
                 <<mRefPx[i]<<" "<<mRefPy[i]<<" "<<mRefPz[i]<<" "<<mRefP[i]<<" "
                 <<mRunConfig[0]
                 <<endl;
   }
}

//  =============================================================================
//  === user run time parameter handling
//  =============================================================================
void MuonTree::readMacFile(string fileName){
   // read parameters from G4 mac file...
   ifstream macfile(fileName);

   if(macfile.is_open()) {
      string line;
      while (getline(macfile,line))  {
          // std::cout<<"line="<<line<<"=endline="<<std::endl;
          vector<string> tokens=parse_line(line);         
          if(tokens.size()>2) {
             for (unsigned i=0; i<tokens.size(); i++) {
                 if(tokens[0].compare("#$$$")==0) {
                    mcParams[tokens[1]]=tokens[2];
                 }
             }
          }
      }
      macfile.close();
   } 
   else{
      cout<<"MuonTree::readParamFile: error to open mac file, "<<fileName<<endl;
   }

   cout<<" "<<endl;
   cout<<"=== Parameters from "<<fileName<<" (MuonTree::readMacFile) ==="<<endl;
   map<string,string>::iterator it;
   for(it=mcParams.begin(); it!=mcParams.end(); ++it) {
       cout<<it->first <<" => " << it->second <<endl;
   }

}

// =======================================================================
bool  MuonTree::setParam(string key,string val) {
   map<string,string>::iterator it;
   bool keyfound=false;
   for(it=mcParams.begin(); it!=mcParams.end(); ++it) {
       cout<<it->first <<" => " << it->second <<endl;
       if(it->first == key) {
          std::cout<<"MuonTree::setParam:  key is found."<<std::endl;
          keyfound=true;
          break;;
       }
   }
   if(keyfound) {
      std::cout<<"MuonTree::setParam: (overwrite)  kew="<<key<<"  val="<<val<<std::endl;   
mcParams[key]=val;}
   
   return !keyfound;  // return code:  true is error.
}

// =======================================================================
float MuonTree::getParamF(string key) {
   float val=98765.0;
   if(mcParams.find(key) != mcParams.end()) {
         val=std::stof(mcParams[key]);
   } else
   {
      std::cout<<"  "<<std::endl;
      std::cout<<"MuonTree::getParamF: Parameter key ("<<key<<") does not exist in the mac file. Exit.."<<std::endl;
      std::cout<<"    note:  key word is case sensitive."<<std::endl;
      std::cout<<"  "<<std::endl;
      std::exit(0);
   }
   return val;
}

// =======================================================================
int MuonTree::getParamI(string key) {
   int val=98765;
   if(mcParams.find(key) != mcParams.end()) {
      val=std::stoi(mcParams[key]);
   } else
   {  
      std::cout<<"  "<<std::endl;
      std::cout<<"MuonTree::getParamI: Parameter key ("<<key<<") does not exist in the mac file. Exit.."<<std::endl;
      std::cout<<"    note:  key word is case sensitive."<<std::endl;
      std::cout<<"  "<<std::endl;
      std::exit(0);
   }

   return val;
}

// =======================================================================
string MuonTree::getParamS(string key) {
   string val="aaa";
   if(mcParams.find(key) != mcParams.end()) {
      val=mcParams[key];
   } else
   {
      std::cout<<"  "<<std::endl;
      std::cout<<"MuonTree::getParamI: Parameter key ("<<key<<") does not exist in the mac file. Exit.."<<std::endl;
      std::cout<<"    note:  key word is case sensitive."<<std::endl;
      std::cout<<"  "<<std::endl;
      std::exit(0);
   }

   return val;
}

// =======================================================================
vector<string>  MuonTree::parse_line(string line){

    string buf; // Have a buffer string
    stringstream ss(line); // Insert the string into a stream
    vector<string> tokens; // Create vector to hold our words

    if(line.size()>0) {
        while (ss >> buf)
           tokens.push_back(buf);
    }
    return tokens;;
};
