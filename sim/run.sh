rm Muon*.root

. muonSetup.sh
make clean
make -j16

sh ./run_batch1.sh & batch1=$!
sh ./run_batch2.sh & batch2=$!
sh ./run_batch3.sh & batch3=$!
sh ./run_batch4.sh & batch4=$!
sh ./run_batch5.sh & batch5=$!
sh ./run_batch6.sh & batch6=$!
sh ./run_batch7.sh & batch7=$!
sh ./run_batch8.sh & batch8=$!
sh ./run_batch9.sh & batch9=$!
sh ./run_batch10.sh & batch10=$!
sh ./run_batch11.sh & batch11=$!
sh ./run_batch12.sh & batch12=$!
sh ./run_batch13.sh & batch13=$!
sh ./run_batch14.sh & batch14=$!
sh ./run_batch15.sh & batch15=$!
sh ./run_batch16.sh & batch16=$!

wait $batch1
wait $batch2
wait $batch3
wait $batch4
wait $batch5
wait $batch6
wait $batch7
wait $batch8
wait $batch9
wait $batch10
wait $batch11
wait $batch12
wait $batch13
wait $batch14
wait $batch15
wait $batch16

hadd -f Muon.root Muon*.root

cp Muon*.root /home/cristmor/Muon/mc_prototype1-b/ana
cd /home/cristmor/Muon/mc_prototype1-b/ana

cp Muon.root muonTree01.root
cp Muon.root cristmorMSI_proto_85.7mill_0_airProto1B_1.root #100mill_0_airProto1B_1.root
cp cristmorMSI_proto_85.7mill_0_airProto1B_1.root /home/cristmor/Muon/MCData
. runana.sh
cp muonTree01hist.root Muonh.root
cp muonTree01hist.root cristmorMSI_proto_85.7mill_0_airProto1B_1_hist.root #cristmormsi_proto_100mill_0_airProto1B_1_hist.root #proto_100mill_0_airProto1B_1_hist.root
cp cristmorMSI_proto_85.7mill_0_airProto1B_1_hist.root /home/cristmor/Muon/MCData


cd /home/cristmor/Muon/mc_prototype1-b/sim
