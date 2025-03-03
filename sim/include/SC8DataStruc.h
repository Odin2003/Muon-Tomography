#ifndef SC8DataStruc_h
#define SC8DataStruc_h 1

struct SC8Particle {
   int pid;
   int trackid;
   double px;
   double py;
   double pz;
   double pp;
   double ma;
   double x;
   double y;
   double z;
   double loc_px;
   double loc_py;
   double loc_pz;
   double loc_x;
   double loc_y;
   double loc_z;
   double steplength;
   double edep;

};

struct SC8edep {
  double SBAR[40];
  double TRAY[4];
  double MStepBar[40]; // changed by SAS 29/11 //array to store the step lengths of muons in the scintillator bars
  double MStepTray[4]; // changed by SAS 29/11 //array to store the step lengths of muons in the trays
  double EdepTestObj;
  double StepTestObj;
//  float mEdepWater;
//  float mEdepWall;
//  float mLengthWater;
//  float mLengthWall; 

};

#endif
