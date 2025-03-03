submissiontext = """#!/bin/bash
#SBATCH -J JOBNAME
#SBATCH -N 1
#SBATCH --ntasks-per-node=1
#SBATCH -o LOGDIR/%x.%j.out
#SBATCH -e LOGDIR/%x.%j.err
#SBATCH -p nocona
"""

singularity_cmd = "singularity run --cleanenv --bind /lustre:/lustre /lustre/work/yofeng/SimulationEnv/alma9forgeant4_sbox/"
#run_cmd = "BUILDDIR/exampleB4b -b BUILDDIR/paramBatch03_single.mac      -jobName JOBNAME -runNumber RUNNUM -runSeq RUNSEQ      -numberOfEvents NEVENTS  -eventsInNtupe NEVENTS -gun_particle PARTICLE -gun_energy_min ENERGY_MIN -gun_energy_max ENERGY_MAX     -sipmType 1"

run_cmd = "BUILDDIR/exampleB4a -b BUILDDIR/batch_run_C3RAA_01.mac       -runNumber RUNNUM -runSeq RUNSEQ    -numberOfEvents NEVENTS -dataDIR /home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/odin_test_sim/data"
#run_cmd = "BUILDDIR/exampleB4a -b BUILDDIR/batch_run_C0RRR_01.mac       -runNumber RUNNUM -runSeq RUNSEQ    -numberOfEvents NEVENTS -dataDIR /home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/odin_test_sim/data"



import os

current_dir = os.getcwd()
print("Current directory: ", current_dir)

#
# change from here
#
sim_build_dir = f"{current_dir}/../odin_test_sim/build"
log_dir = f"{current_dir}/log"
njobs = 1000
nevents_per_job = 20000
runnumber = 103
jobname_prefix = "tomographysim" 

if not os.path.exists(log_dir):
    print(f"Creating log directory: {log_dir}")
    os.makedirs(log_dir)

##replace("JOBNAME", jobname).\

fnames = []
for i in range(njobs):
    jobname = f"{jobname_prefix}_{i}"
    run_cmd_tmp = run_cmd.replace("BUILDDIR", sim_build_dir).\
        replace("RUNNUM", str(runnumber)).\
        replace("RUNSEQ", str(i)).\
        replace("NEVENTS", str(nevents_per_job))
        
    #run_cmd_tmp = singularity_cmd + " " + run_cmd_tmp

        
    submissiontext_tmp = submissiontext.replace("JOBNAME", jobname).replace("LOGDIR", log_dir)
    
    # write the submission script
    fname = f"{log_dir}/submit_{jobname}.sh"
    with open(fname, "w") as f:
        f.write(submissiontext_tmp)
        f.write("\n\n")
        f.write("cd /home/odschnei/MuonX/g4sim/ICSIMtest/icsim_03_01b/odin_test_sim/build")
        f.write("\n\n")
        f.write(singularity_cmd)
        f.write(" <<EOF")
        f.write("\n\n")
        f.write(run_cmd_tmp)
        f.write("\n\n")
        f.write("EOF")
        
    fnames.append(fname)
    
submit_sh = f"{current_dir}/submit_all.sh"
with open(submit_sh, "w") as f:
    f.write("#!/bin/bash\n")
    for fname in fnames:
        f.write(f"sbatch {fname}\n")
        
os.system(f"chmod +x {submit_sh}")

print(f"Submission script written to {submit_sh}")
print("To submit jobs, run:")
print(f"bash {submit_sh}")
        
