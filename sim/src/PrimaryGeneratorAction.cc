//******************************************************************************
// PrimaryGeneratorAction.cc
//
// 1.00 JMV, LLNL, Jan-2007:  First version.
//******************************************************************************
//

#include <stdlib.h>     /* getenv */
#include <iomanip>
#include "PrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "B4DetectorConstruction.hh"
#include "MuonTree.h"

using namespace std;


#include "G4Event.hh"

//----------------------------------------------------------------------------//
PrimaryGeneratorAction::PrimaryGeneratorAction(const char *inputfile,B4DetectorConstruction* det, MuonTree* histo)
:fB4DetectorConstruction(det),fMuonTree(histo)
{
    // std::cout<<"PrimaryGeneratorAction::PrimaryGeneratorAction(constructor) is called."<<std::endl;

    InputState=-1;
    // testing paramter from mac file...
    // float aaa=histo->getParamF("worldDX");
    // cout<<"PrimaryGeneratorAction::PrimaryGeneratorAction: histo->getParamF(worldDX)="<<aaa<<endl;

    // define a particle gun
    particleGun = new G4ParticleGun();


    if(fMuonTree->getParamS("useParticleGun").compare(0,4,"true")==0) {
       initSingleParticleGun();
       //       return;
    }
    // create a vector to store the CRY particle properties
    vect=new std::vector<CRYParticle*>;

    // Create the table containing all particle names
    particleTable = G4ParticleTable::GetParticleTable();

    // Create the messenger file
    gunMessenger = new PrimaryGeneratorMessenger(this);
}

//----------------------------------------------------------------------------//
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{

  std::cout << "deleting PrimaryGeneratorAction..."<<std::endl;
  std::cout << "Total time simulated: " << gen->timeSimulated() << " seconds"<<std::endl;;
  std::cout << "   "<<std::endl;

}

//----------------------------------------------------------------------------//
void PrimaryGeneratorAction::InputCRY()
{
    InputState=1;
}

//----------------------------------------------------------------------------//
void PrimaryGeneratorAction::UpdateCRY(std::string* MessInput)
{
    // This function is executed by a command /CRY/update in mac file.

    std::string  dataDir="../data";
    char* pPath;
    pPath = getenv ("CRYDATAPATH");
    if (pPath!=NULL) { string ss(pPath); dataDir=ss;}

    std::cout<<"CRY::PrimaryGenratorAction(Update)  dataDIR="<<dataDir<<std::endl;

    XSHIFT=fMuonTree->getParamF("cryX");  // in meters
    YSHIFT=fMuonTree->getParamF("cryY");
    ZSHIFT=fMuonTree->getParamF("cryZ");
    DZSHIFT=fMuonTree->getParamF("cryDZ");

    double degrad=3.1415/180.0;
    double cryElevationAngleMin=(fMuonTree->getParamF("cryElevationAngleMin"))*degrad;
    double cryElevationAngleMax=(fMuonTree->getParamF("cryElevationAngleMax"))*degrad;
    double cryAzimuthalAngleMin=(fMuonTree->getParamF("cryAzimuthalAngleMin"))*degrad;
    double cryAzimuthalAngleMax=(fMuonTree->getParamF("cryAzimuthalAngleMax"))*degrad;

    double thetaMin=cryElevationAngleMin;
    double thetaMax=cryElevationAngleMax;
    double phiMin=cryAzimuthalAngleMin;
    double phiMax=cryAzimuthalAngleMax;

    momentumCut=fMuonTree->getParamF("cryPcut");

    std::cout<<" beam X, Y, Z shift: "<<XSHIFT<<"  "<<YSHIFT<<"  "<<ZSHIFT<<"  "<<std::endl;
    std::cout<<"G4CRY momentum cut:  "<<momentumCut<<" MeV."<<std::endl;

    std::cout<<"PrimaryGeneratorAction::UpdateCRY:   CRYSetup..."<<std::endl;
    // CRYSetup *setup=new CRYSetup(*MessInput,"../data");
    CRYSetup *setup=new CRYSetup(*MessInput,dataDir);

    // gen = new CRYGenerator(setup);
    gen = new CRYGenerator(setup,thetaMin,thetaMax,phiMin,phiMax,momentumCut);

   
    // fEngine = new CLHEP::HepRandomEngine(12345); 
   

    // set random number generator
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);

    InputState=0;
}

//----------------------------------------------------------------------------//
void PrimaryGeneratorAction::CRYFromFile(G4String newValue)
{
    // Read the cry input file
    std::ifstream inputFile;
    inputFile.open(newValue,std::ios::in);
    char buffer[1000];

    if (inputFile.fail()) {
        G4cout << "Failed to open input file " << newValue << G4endl;
        G4cout << "Make sure to define the cry library on the command line" << G4endl;
        InputState=-1;
    }else{
        std::string setupString("");
        while ( !inputFile.getline(buffer,1000).eof()) {
            setupString.append(buffer);
            setupString.append(" ");
        }


    double degrad=3.1415/180.0;
    double cryElevationAngleMin=(fMuonTree->getParamF("cryElevationAngleMin"))*degrad;
    double cryElevationAngleMax=(fMuonTree->getParamF("cryElevationAngleMax"))*degrad;
    double cryAzimuthalAngleMin=(fMuonTree->getParamF("cryAzimuthalAngleMin"))*degrad;
    double cryAzimuthalAngleMax=(fMuonTree->getParamF("cryAzimuthalAngleMax"))*degrad;

    double thetaMin=cryElevationAngleMin;
    double thetaMax=cryElevationAngleMax;
    double phiMin=cryAzimuthalAngleMin;
    double phiMax=cryAzimuthalAngleMax;

    momentumCut=fMuonTree->getParamF("cryPcut");


    std::cout<<"PrimaryGeneratorAction::CRYFromFile:   CRYSetup..."<<std::endl;

        CRYSetup *setup=new CRYSetup(setupString,"../data");

        gen = new CRYGenerator(setup,thetaMin,thetaMax,phiMin,phiMax, momentumCut);

        // set random number generator
        RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
        setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
        InputState=0;
    }
}
//This part of the code is what needs change!! - sas Nov 25
//----------------------------------------------------------------------------//
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
    // cout<<"PrimaryGeneratorAction::GeneratePrimaries is called..."<<endl;

    fMuonTree->startTimePrimaryGeneratorAction();   // for cpu timing monitoring

    // double myrandom=G4UniformRand();
    // cout<<"myrandom="<<myrandom<<endl;

    fMuonTree->BeginEvent();    // initialize ntuple and event data analysis here.

    if(fMuonTree->getParamS("useParticleGun").compare(0,4,"true")==0) {
       executeSingleParticleGun(anEvent);
       fMuonTree->endTimePrimaryGeneratorAction();
       return;
    }

    double degrad=3.1415/180.0;
    double cryElevationAngleMin=(fMuonTree->getParamF("cryElevationAngleMin"))*degrad;
    double cryElevationAngleMax=(fMuonTree->getParamF("cryElevationAngleMax"))*degrad;
    double cryAzimuthalAngleMin=(fMuonTree->getParamF("cryAzimuthalAngleMin"))*degrad;
    double cryAzimuthalAngleMax=(fMuonTree->getParamF("cryAzimuthalAngleMax"))*degrad;

    double randomZhift=ZSHIFT+(G4UniformRand()*2.0-1.0)*DZSHIFT;
    // cout<<"randZhift="<<randomZhift<<"    ZSHIFT="<<ZSHIFT<<endl;

            // Size of World volume.
    double worldSizeX = fB4DetectorConstruction->getWorldSizeX();
    double worldSizeY = fB4DetectorConstruction->getWorldSizeY();
    // double  worldSizeZ = fB4DetectorConstruction->getWorldSizeZ();  
    double worldSizeZ = fMuonTree->getParamF("cryWorldZ")*m; 
    double worldSizeR = fMuonTree->getParamF("cryWorldR")*m; 

    if (InputState != 0) {
        G4String* str = new G4String("CRY library was not successfully initialized");
        //G4Exception(*str);
        G4Exception("PrimaryGeneratorAction", "1",
                RunMustBeAborted, *str);
    }
    G4String particleName;

    //  loop until we find a particle with momentum above threshold...
    //  this is practically infinit loop...
    for(int ijk=0; ijk<100000000; ijk++) {

        vect->clear();
        gen->genEvent(vect);

        // std::cout<<"CRY::Simulation time in seconds: "<<gen->timeSimulated()<<std::endl; //SAS 2O20

        // apply momentum and angle  cut... Now this is done in CRY. Accept all muons.
        vector<int> acceptedMuonsIndex;
        for ( unsigned j=0; j<vect->size(); j++) {
            if(abs((*vect)[j]->PDGid())==13) {
               // double a1=atan2(-((*vect)[j]->w()),-((*vect)[j]->u())); // elevationAngle
               // double a2=atan2(-((*vect)[j]->v()),-((*vect)[j]->u())); // azimuthalAngle
               // if( (*vect)[j]->ke() > momentumCut 
               //   && a1>cryElevationAngleMin && a1<cryElevationAngleMax
               //  && a2>cryAzimuthalAngleMin && a2<cryAzimuthalAngleMax) 
               //{ 
                    acceptedMuonsIndex.push_back(j);
               //}
             }  // end of muon selection 
        }
        if(acceptedMuonsIndex.size()==0) continue;

        //....debug output
        /* 
         G4cout << "\nEvent=" << anEvent->GetEventID() << " "
            << "CRY generated nparticles=" << vect->size()
            << " after ijk tries "<<ijk
            << G4endl;        
        */

        float cryTimeSimulated=gen->timeSimulated();
        // std::cout<<"PrimaryGeneratorAction  cryTimeSimulated="<<cryTimeSimulated<<std::endl;
        // std::cout<<"acceptedMuonsIndex.size()="<<acceptedMuonsIndex.size()<<endl;

        // for ( unsigned j=0; j<vect->size(); j++) {
        for ( unsigned kk=0; kk<acceptedMuonsIndex.size(); kk++) {
            int j=acceptedMuonsIndex[kk];
            particleName=CRYUtils::partName((*vect)[j]->id());

            //....debug output  
            /*
                 cout << "  "          << particleName << " "
                  << "charge="      << (*vect)[j]->charge() << " "
                  << setprecision(4)
                  << "energy (MeV)=" << (*vect)[j]->ke()*MeV << " "
                  << "pos (m)"
                  << G4ThreeVector((*vect)[j]->x(), (*vect)[j]->y(), (*vect)[j]->z())
                  << " " << "direction cosines "
                  << G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w())
                  << " " << endl;
            */
    

            // calculate the starting point of particles which end up near the detector

            // Units in G4:   MeV, mm,  nsec
            // Units in CRY:  MeV,  m,  sec
            // Factors from G4:  MeV=1.0,  GeV=1000.0,  m=1000.

            G4double xCRY=(*vect)[j]->x()+XSHIFT;   // Unit in G4 is mm.
            G4double yCRY=(*vect)[j]->y()+YSHIFT;
            // G4double zCRY=(*vect)[j]->z()+ZSHIFT;
            G4double zCRY=(*vect)[j]->z()+randomZhift;

            G4double xvtx=xCRY*m;   // Unit in G4 is mm.
            G4double yvtx=yCRY*m;
            G4double zvtx=zCRY*m;

            G4ParticleDefinition * pDef= particleTable->FindParticle((*vect)[j]->PDGid());
            particleGun->SetParticleDefinition(pDef);
            double mass=pDef->GetPDGMass();               // in MeV
            double energy=(*vect)[j]->ke()*MeV+mass;      // in MeV
            double pp=sqrt(energy*energy-mass*mass);      // in MeV

            SC8Particle cryPartilce;
            cryPartilce.pid=(*vect)[j]->PDGid();
            cryPartilce.steplength=cryTimeSimulated;  // keep timeSimulated here.
            cryPartilce.px=((*vect)[j]->u())*pp;
            cryPartilce.py=((*vect)[j]->v())*pp;
            cryPartilce.pz=((*vect)[j]->w())*pp;
            cryPartilce.pp=pp;   
            cryPartilce.ma=mass;
            cryPartilce.x=xCRY;   // in meters
            cryPartilce.y=yCRY;
            cryPartilce.z=zCRY;
            cryPartilce.loc_px=((*vect)[j]->u())*pp;
            cryPartilce.loc_py=((*vect)[j]->v())*pp;
            cryPartilce.loc_pz=((*vect)[j]->w())*pp;
            cryPartilce.loc_x=(*vect)[j]->x();   // in meters
            cryPartilce.loc_y=(*vect)[j]->y();
            cryPartilce.loc_z=(*vect)[j]->z();

            fMuonTree->addCryParticle(cryPartilce);

            // move particle back to the world boundary...
            double deltaL=100.0;   // 100 mm step
            for (int i=0; i<100000; i++) {
                double x= xvtx-(*vect)[j]->u() * deltaL;
                double y= yvtx-(*vect)[j]->v() * deltaL;
                double z= zvtx-(*vect)[j]->w() * deltaL;
                if(abs(x) > worldSizeX ) break;
                if(abs(y) > worldSizeY ) break;
                if(z > worldSizeZ ) break;
                if((x*x+y*y > worldSizeR*worldSizeR) && z>0.0) break;
                xvtx=x; yvtx=y; zvtx=z;
            }


            particleGun->SetParticleEnergy(energy);
            particleGun->SetParticlePosition(G4ThreeVector(xvtx, yvtx, zvtx));
            particleGun->SetParticleMomentumDirection(G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w()));
            particleGun->SetParticleTime((*vect)[j]->t());
            particleGun->GeneratePrimaryVertex(anEvent);

            SC8Particle genParticle;
            genParticle.pid=(*vect)[j]->PDGid();
            genParticle.px=((*vect)[j]->u())*pp;
            genParticle.py=((*vect)[j]->v())*pp;
            genParticle.pz=((*vect)[j]->w())*pp;
            genParticle.pp=pp;   
            genParticle.ma=mass;           
            genParticle.x=xvtx;
            genParticle.y=yvtx;
            genParticle.z=zvtx;
            fMuonTree->addGenParticle(genParticle);
 
            // delete (*vect)[j];
        }

        //  exist from infinit loop...
        break;

    }   // end of ijk loop...

    fMuonTree->endTimePrimaryGeneratorAction();
    return;
}

//----------------------------------------------------------------------------//
void PrimaryGeneratorAction::initSingleParticleGun() {

  G4ParticleDefinition* particleDefinition
    = G4ParticleTable::GetParticleTable()->FindParticle(fMuonTree->getParamS("pgunParticle"));

  particleGun->SetNumberOfParticles(1);
  particleGun->SetParticlePosition(G4ThreeVector(fMuonTree->getParamF("pgunX")*m,
                                                 fMuonTree->getParamF("pgunY")*m,
                                                 fMuonTree->getParamF("pgunZ")*m));
  particleGun->SetParticleDefinition(particleDefinition);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(
    fMuonTree->getParamF("pgunXdir"),fMuonTree->getParamF("pgunYdir"),fMuonTree->getParamF("pgunZdir")));
  particleGun->SetParticleMomentum(fMuonTree->getParamF("pgunP")*GeV);
}
//----------------------------------------------------------------------------//
void PrimaryGeneratorAction::executeSingleParticleGun(G4Event* anEvent) {

  particleGun->GeneratePrimaryVertex(anEvent);

            double pp=particleGun->GetParticleMomentum();
            double en=particleGun->GetParticleEnergy();
            double mass=sqrt(en*en-pp*pp);

            SC8Particle genParticle;
            genParticle.pid=particleGun->GetParticleDefinition()->GetPDGEncoding();
            genParticle.px=((particleGun->GetParticleMomentumDirection()).x())*pp;;
            genParticle.py=((particleGun->GetParticleMomentumDirection()).y())*pp;
            genParticle.pz=((particleGun->GetParticleMomentumDirection()).z())*pp;
            genParticle.pp=pp;
            genParticle.ma=mass;
            genParticle.x=fMuonTree->getParamF("pgunX")*m;
            genParticle.y=fMuonTree->getParamF("pgunY")*m;
            genParticle.z=fMuonTree->getParamF("pgunZ")*m;
            fMuonTree->addGenParticle(genParticle);
}
