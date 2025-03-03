# Muon-Tomography
Code to run muon tomography Geant4 simulations on the HPCC.

## Environment:

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

## Directory organization:

"sim" directory: 
-> "build" 
