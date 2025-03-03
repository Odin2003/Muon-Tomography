Irish Cirn GEANT4 Monte Carlo Simulation
- SK   01-July-201    (v01_03)

[1] Directory structure

   icSim/icsim_01_03/sim
   icSim/icsim_01_03/sim/src
   icSim/icsim_01_03/sim/include
   icSim/icsim_01_03/sim/scripts   (example mac and sh files)
   icSim/icsim_01_03/sim/build     (build area with cmake)

[2] Required libraries
   CRY, GEANT4, ROOT

[3] Files

   - files for cmake and environment setup [sim]
   muonSetupMac.sh  - define ENV for CRY, GEANT4, ROOT on Mac.
   muonSetup.sh     - define ENV for CRY, GEANT4, ROOT on Linux.  (need clean-up).

   CMakeLists.txt   - setup for cmake, including a list of files to copy to [build] directory
   CMakeCache.txt   - modify CMAKE_CACHEFILE_DIR:INTERNAL=... etc.

   - C++ code
  exampleB4a.cc     - main program in [sim]
  .cc files         - in [src]
  .h  files         - in [include]

   - scripts  
   The followin files will be copied from [sim] to [build] during cmake process.
   run_batch.sh     - to run the program in batch mode.
   batch_run.mac    - run time paramters for icSim, Cry, G4.

   There are mac files for 8 configurations in "scripts" directory.
   C0, C1, C2, C3 are for the location of camera, Z=0.5, -5, -10, -15 m, respectively.
   RAA is for material, (Rock, Air, Air) in (Cairn, Chamber, Passage), respectivly, and
   RRR is for material, (Rock, Rock,Rock, Air) in (Cairn, Chamber, Passage), respectivly 

   batch_run_C0RAA_01.mac		batch_run_C2RAA_01.mac
   batch_run_C0RRR_01.mac		batch_run_C2RRR_01.mac
   batch_run_C1RAA_01.mac		batch_run_C3RAA_01.mac
   batch_run_C1RRR_01.mac		batch_run_C3RRR_01.mac
   
[4] How to build with cmake (example on Mac)

$ source muonSetupMac.sh
$ mkdir build
$ cd  build
$ cmake ..
$ make

[5] How to run in [build] (example on Mac)

$ source muonSetupMac.sh
$  vi batch_run.mac     # set the number of events and other run time parameters
$ source run_batch.sh

The following is an example of running jobs for 8 configurations.
(copy from scripts/run_batch_C0123_example.sh)

source muonSetupMac.sh

./exampleB4a  -b batch_run_C0RAA_01.mac -numberOfEvents 1000 -runNumber 987 -runSeq 99
./exampleB4a  -b batch_run_C0RRR_01.mac -numberOfEvents 1000 -runNumber 987 -runSeq 99
./exampleB4a  -b batch_run_C1RAA_01.mac -numberOfEvents 1000 -runNumber 987 -runSeq 99
./exampleB4a  -b batch_run_C1RRR_01.mac -numberOfEvents 1000 -runNumber 987 -runSeq 99
./exampleB4a  -b batch_run_C2RAA_01.mac -numberOfEvents 1000 -runNumber 987 -runSeq 99
./exampleB4a  -b batch_run_C2RRR_01.mac -numberOfEvents 1000 -runNumber 987 -runSeq 99
./exampleB4a  -b batch_run_C3RAA_01.mac -numberOfEvents 1000 -runNumber 987 -runSeq 99
./exampleB4a  -b batch_run_C3RRR_01.mac -numberOfEvents 1000 -runNumber 987 -runSeq 99



