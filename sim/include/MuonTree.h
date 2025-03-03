#ifndef MuonTree_h
#define MuonTree_h 1

#include <fstream>   // for input/output files
#include <sstream>   // for string stream
#include <math.h>    // for sin(x) etc.
#include <cstdlib>   // for rand() on archer.
#include <iomanip>   // for setw() in cout, 
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"

#include "SC8DataStruc.h"

#define MAXGENPAR  1000
#define MAXCHAN   100

using namespace std;

class MuonTree{
   public:
      MuonTree(string,int argc,char** argv);  // string outname
      ~MuonTree();  // string outname
      void BeginEvent();
      void EndEvent();
      void EndJob();

      void addCryParticle(SC8Particle);
      void addGenParticle(SC8Particle);
      void addRefParticle(SC8Particle);
      void clearAccumulatedEdepSetpLength();
      void accumulateEdepSetpLength(string _name,double _edep, double _stepL);
      void saveAccumulatedEdepSetpLength();
      void updateSimTK(SC8Particle); 

      // for cpu timing monitoring.
      void startTimePrimaryGeneratorAction();
      void   endTimePrimaryGeneratorAction();
      void startTimeB4aEventAction();
      void   endTimeB4aEventAction();

      bool  setParam(string key,string val);
      float getParamF(string key);
      int   getParamI(string key);
      string  getParamS(string key);

   private:
      // for timing monitoring.
      std::chrono::steady_clock::time_point startTimeJob;
      std::chrono::steady_clock::time_point startTimePGA;  // PrimaryGeneratorAction
      std::chrono::duration<double> totalTimePGA;
      std::chrono::steady_clock::time_point startTimeEVT;  // B4aEventAction
      std::chrono::duration<double> totalTimeEVT;

      // private functions.
      void readMacFile(string); 
      void clearMuonTree();
      void analyze();
      void writeMuonList();
      vector<string> parse_line(string line);


      string runConfig;
      int runNumber;

      int eventCounts;
      int eventCountsALL;

      bool createNtuple;
     
      ofstream muonListFile;
      bool createMuonList;

      // CRY duration...
      float lastCryTime;

      // for edep and step length accumulation...
      std::map<string, double> edepSum;
      std::map<string, double> stepSum;  // step length
      std::map<string, double>::iterator itr_edepSum;
      std::map<string, double>::iterator itr_stepSum;  // step length

      //  MC run time parameters...
      std::map<std::string, std::string> mcParams;

      // for histogrming...
      std::string title;
      std::map<std::string, TH1D*> histo1D;
      std::map<std::string, TH1D*>::iterator histo1Diter;
      std::map<std::string, TH2D*> histo2D;
      std::map<std::string, TH2D*>::iterator histo2Diter;

      // ntuple file definition...
      TFile *fout;

      TTree *tree;
      std::string _MuonXG4OutName;

      std::vector<string>  mRunConfig;
      std::vector<int>   mRunNumber;
      std::vector<int>   mEventNumber; 

      std::vector<int>   mTrigger;

      std::vector<float> mCryTime;
      std::vector<int>   mnCry;
      std::vector<int>   mCryPid;
      std::vector<float> mCryPx;
      std::vector<float> mCryPy;
      std::vector<float> mCryPz;
      std::vector<float> mCryP;
      std::vector<float> mCryMass;
      std::vector<float> mCryVx;     // in global coordinate
      std::vector<float> mCryVy;
      std::vector<float> mCryVz;
      std::vector<float> mCryLocPx;  // in CRY plane
      std::vector<float> mCryLocPy;
      std::vector<float> mCryLocPz;
      std::vector<float> mCryLocVx;  // in CRY plane
      std::vector<float> mCryLocVy;
      std::vector<float> mCryLocVz;
      std::vector<float> mCryTheta;  // elevation angle in degree
      std::vector<float> mCryPhi;    // azimuthal angle in degree

      std::vector<int>   mnGen;
      std::vector<int>   mGenPid;
      std::vector<float> mGenPx;
      std::vector<float> mGenPy;
      std::vector<float> mGenPz;
      std::vector<float> mGenP;
      std::vector<float> mGenMass;
      std::vector<float> mGenVx;
      std::vector<float> mGenVy;
      std::vector<float> mGenVz;

      //   Reference plane is defined at the center of each camera.
      std::vector<int>   mnRef;     //  number of hits in each camera. 
      std::vector<int>   mRefId;      
      std::vector<int>   mRefTkId;
      std::vector<int>   mRefPId;
      std::vector<float> mRefPx;
      std::vector<float> mRefPy;
      std::vector<float> mRefPz;
      std::vector<float> mRefP;
      std::vector<float> mRefMass;
      std::vector<float> mRefVx;
      std::vector<float> mRefVy;
      std::vector<float> mRefVz;
      std::vector<float> mRefLocPx;
      std::vector<float> mRefLocPy;
      std::vector<float> mRefLocPz;
      std::vector<float> mRefLocVx;
      std::vector<float> mRefLocVy;
      std::vector<float> mRefLocVz;
      std::vector<float> mRefTheta;   // elevation angle in theta
      std::vector<float> mRefPhi;     // azimuthal angle in degree
      std::vector<float> mRefProjVx;  // in m. projected to a vertical plan at x=0
      std::vector<float> mRefProjVy;
      std::vector<float> mRefProjVz;


      std::vector<int>   mnTray;
      std::vector<int>   mTrayId;       // i=1,2
      std::vector<float> mTrayEdep;   // total edep in in Tray[i]
      std::vector<float> mTrayStep;   // total step length in Tray[i]

      std::vector<int>   mnCairn;
      std::vector<int>   mCairnId;    //  i=1 for now
      std::vector<float> mCairnEdep;  // Edep in Carin
      std::vector<float> mCairnStep;  // step lengh in Carin
      
      std::vector<int>   mnChamber;
      std::vector<int>   mChamberId;    //  i=1 for noe
      std::vector<float> mChamberEdep;  // Edep in Chamber
      std::vector<float> mChamberStep;  // step lengh in Chamber

      std::vector<int>   mnPassage;
      std::vector<int>   mPassageId;    //  i=1 for now
      std::vector<float> mPassageEdep;  // Edep in Passage
      std::vector<float> mPassageStep;  // step lengh in Passage

      std::vector<int>   mnSimTk;   // number of sim (muon) trakcs 
      std::vector<int>   mSimTkSeqId;  // sequential id number, corresponds to GenPar.
      std::vector<int>   mSimTkPointId;  //  0=not defined,  1=GenVtx.  
                                       //  110/120=entrance/exit Cairn
                                       //  211/221=entrance/exit Chamber 1
                                       //  311/321=entrance/exit Passage 1
      std::vector<float> mSimTkPx;    // Muon Px, Py, Pz, X, Y, Z
      std::vector<float> mSimTkPy;
      std::vector<float> mSimTkPz;
      std::vector<float> mSimTkVx;
      std::vector<float> mSimTkVy;
      std::vector<float> mSimTkVz;
      std::vector<float> mSimTkTime;

};

#endif

