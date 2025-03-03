# Muon-Tomography
Code to run muon tomography Geant4 simulations on the HPCC.

## Environment

Follow same setup for DREAM simulation:

**On HPCC**, everything (ROOT and GEANT4) is compiled inside the singularity environment. Log into the interactive node ([more information](https://www.depts.ttu.edu/hpcc/userguides/Job_User_Guide.pdf)) with e.g.

```
interactive -p nocona
```
From there run the singularity container with the following command:
```
singularity run --cleanenv --bind /lustre:/lustre /lustre/work/yofeng/SimulationEnv/alma9forgeant4_sbox/
```
The corresponding docker image can be found [here](https://hub.docker.com/repository/docker/yongbinfeng/alma9geant/general), with the build file [here](https://github.com/TTU-HEP/SimulationEnv).

**If running with Docker** on your local machine, firstly pull the docker image
```
docker pull yongbinfeng/alma9geant:latest
```
Then run the container with the following command:
```
docker run -it --rm -h dreamsim -v /path/to/DREAMSIM:/DREAMSIM yongbinfeng/alma9geant:latest
```
**Note** if you have conda installed, exit the conda environment before running the singularity container, otherwise it might cause conflicts with different ROOT versions etc.

## Running the Simulation

### Structure of software:

- `sim/exampleB4a.cc`: main program
- `sim/src/B4DetectorConstruction.cc`:setup of the cairn, hill, and camera
- `sim/include/TessellatedVertices.hh`: Definition of vertices for cairn and hill
- `sim/include/TessellatedGeometry.hh`: Arranging vertices to form cairn and hill
- `sim/src/TessellatedGeometry.cc`: Used in B4DetectorConstruction.cc to implement cairn and hill
- `sim/src/B4aSteppingAction.cc`:access hits at each step
- `sim/src/MuonTree.cc`:analysis and hit handling

### Compiling:

Inside the singularity environment, build program in "build" area,
```
cd /path/to/sim/directory
cd sim
mkdir build
cd build
cmake ..
make -j 4
```

### Running the code
Move a macro file from `sim/scripts' to the build area. C0 corresponds to a detector position on the surface just outside of the cairn.mac 

