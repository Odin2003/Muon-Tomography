#!/bin/bash
#SBATCH -J tomographysim_890
#SBATCH -N 1
#SBATCH --ntasks-per-node=1
#SBATCH -o /home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/jobs/log/%x.%j.out
#SBATCH -e /home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/jobs/log/%x.%j.err
#SBATCH -p nocona


cd /home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/odin_test_sim/build

singularity run --cleanenv --bind /lustre:/lustre /lustre/work/yofeng/SimulationEnv/alma9forgeant4_sbox/ <<EOF

/home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/jobs/../odin_test_sim/build/exampleB4a -b /home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/jobs/../odin_test_sim/build/batch_run_C0RRR_01.mac       -runNumber 101 -runSeq 890    -numberOfEvents 20000 -dataDIR /home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/odin_test_sim/data

EOF