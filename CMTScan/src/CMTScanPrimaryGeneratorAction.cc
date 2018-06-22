#include "CMTScanPrimaryGeneratorAction.hh"
#include "GeometryVariable.hh"
#include "G4Event.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//TODO virer le messsenger
CMTScanPrimaryGeneratorAction::CMTScanPrimaryGeneratorAction(const char *inputfile){
	// define a particle gun
	particleGun = new G4ParticleGun();

	// Read the cry input file
	std::ifstream inputFile;
	char buffer[1000];

	inputFile.open(inputfile, std::ios::in);
    if (!inputFile.fail()) {
        std::string setupString;

        while (!inputFile.getline(buffer, 1000).eof()) {
            setupString.append(buffer);
            setupString.append(" ");
        }

        CRYSetup *setup = new CRYSetup(setupString, "../data");
				GeometryVariable *_geometryVariable = GeometryVariable::instance();
				double Detector_Size_X = _geometryVariable->getDetectorSize().getX()/1000.;
				double Detector_Size_Y = _geometryVariable->getDetectorSize().getY()/1000.;
				if (Detector_Size_X>=Detector_Size_Y)
					setup->setParam(CRYSetup::subboxLength,Detector_Size_X);
				if (Detector_Size_X<Detector_Size_Y)
					setup->setParam(CRYSetup::subboxLength,Detector_Size_Y);
        gen = new CRYGenerator(setup);

        // set random number generator
        RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
        setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
        InputState = 0;
    }
    else {
        if (*inputfile != 0) { //....only complain if a filename was given
            G4cout << "PrimaryGeneratorAction: Failed to open CRY input file= " << inputfile << G4endl;
        }
        InputState = -1;
    }

	// create a vector to store the CRY particle properties
	vect = new std::vector<CRYParticle *>;

	// Create the table containing all particle names
	particleTable = G4ParticleTable::GetParticleTable();

	// Create the messenger file
//	gunMessenger = new CMTScanPrimaryGeneratorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanPrimaryGeneratorAction::~CMTScanPrimaryGeneratorAction()
{
//	delete gunMessenger;
	delete particleGun;
	delete gen;
	delete vect;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// The folowing functions, is provided by CRY package no info give here
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanPrimaryGeneratorAction::InputCRY()
{
  InputState = 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// The folowing functions, is provided by CRY package no info give here
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanPrimaryGeneratorAction::UpdateCRY(std::string *MessInput)
{
  CRYSetup *setup = new CRYSetup(*MessInput, "../data");
  gen = new CRYGenerator(setup);
  // set random number generator
  RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
  setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
  InputState = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// The folowing functions, is provided by CRY package no info give here
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanPrimaryGeneratorAction::CRYFromFile(G4String newValue)
{
  // Read the cry input file
  std::ifstream inputFile;
  inputFile.open(newValue, std::ios::in);
  char buffer[1000];

  if (inputFile.fail())
  {
    G4cout << "Failed to open input file " << newValue << G4endl;
    G4cout << "Make sure to define the cry library on the command line" << G4endl;
    InputState = -1;
  }
  else
  {
    std::string setupString;
    while (!inputFile.getline(buffer, 1000).eof())
    {
      setupString.append(buffer);
      setupString.append(" ");
    }

    CRYSetup *setup = new CRYSetup(setupString, "../data");

    gen = new CRYGenerator(setup);

    // set random number generator
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    InputState = 0;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double CMTScanPrimaryGeneratorAction::TimeSimulated = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// This function is called at the very begining of an event
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void CMTScanPrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
    GeometryVariable *_geometryVariable = GeometryVariable::instance();
    G4double Detector_Size_X = _geometryVariable->getDetectorSize().getX();
    G4double Detector_Size_Y = _geometryVariable->getDetectorSize().getY();
    G4double Detector_Size_Z = _geometryVariable->getDetectorSize().getZ();

	if (InputState != 0){
        auto *str = new G4String("CRY library was not successfully initialized");
		G4Exception("PrimaryGeneratorAction", "1", RunMustBeAborted, *str);
		delete str;
	}

	// Clear current vect of generated particles
	// Generate a new event from CRY package
	// NB : this loop can take a WHILE, especially if fired area is small

	bool ReadyToLaunch = false;
	std::vector<CRYParticle *> GoodMuons;
	GoodMuons.clear(); //Removes all elements from the vector (which are destroyed), leaving the container with a size of 0.

	do {
		for (auto &i : *vect)
		    delete i;
		vect->clear();
    	gen->genEvent(vect);

		if ((*vect)[0]->w() >= 0 && (*vect)[0]->ke() <= 10.0)
		    continue; // (Mev) Avoid generating particles that can't be detected

		G4ThreeVector pos((*vect)[0]->x()*m, (*vect)[0]->y()*m, (*vect)[0]->z()*m + Detector_Size_Z*0.5);
		G4ThreeVector mom((*vect)[0]->u(), (*vect)[0]->v(), (*vect)[0]->w());
		G4double ft = (Detector_Size_Z) / fabs(mom[2]);

		if((fabs(pos[0]) < Detector_Size_X*0.5)&&
           (fabs(pos[1]) < Detector_Size_Y*0.5)&&
           (fabs(mom[0] * ft + pos[0]) < Detector_Size_X*0.5)&&
           (fabs(mom[1] * ft + pos[1]) < Detector_Size_Y*0.5)){
		    GoodMuons.push_back((*vect)[0]);
		    ReadyToLaunch = true;
		}
	}while(!ReadyToLaunch);

	for (auto &GoodMuon : GoodMuons) {
		particleGun->GeneratePrimaryVertex(anEvent);
		particleGun->SetParticleDefinition(particleTable->FindParticle(GoodMuon->PDGid()));
		particleGun->SetParticleEnergy(GoodMuon->ke() * MeV);
		particleGun->SetParticlePosition(G4ThreeVector(GoodMuon->x()*m,
                                                       GoodMuon->y()*m,
                                                       GoodMuon->z()*m  + Detector_Size_Z*0.5 + 2*cm));
		particleGun->SetParticleMomentumDirection(G4ThreeVector(GoodMuon->u(),
                                                                GoodMuon->v(),
                                                                GoodMuon->w()));
		particleGun->SetParticleTime(GoodMuon->t() * s);
	}
	for (auto &i : *vect)
	    delete i;
	vect->clear();

	TimeSimulated = gen->timeSimulated();
}
