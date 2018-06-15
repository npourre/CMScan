#include "CMTScanActionInitialization.hh"
#include "CMTScanPrimaryGeneratorAction.hh"
#include "CMTScanRunAction.hh"
#include "CMTScanEventAction.hh"
#include "CMTScanStackingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanActionInitialization::CMTScanActionInitialization()
: G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanActionInitialization::~CMTScanActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanActionInitialization::BuildForMaster() const
{
	CMTScanRunAction* runAction = new CMTScanRunAction;
	SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanActionInitialization::Build() const
{
	SetUserAction(new CMTScanPrimaryGeneratorAction("/home/vallois/thesis/CMTScan/CMTScan/cmake-build-debug/setup.file"));

	CMTScanRunAction* runAction = new CMTScanRunAction;
	SetUserAction(runAction);

	CMTScanEventAction* eventAction = new CMTScanEventAction;
	SetUserAction(eventAction);

	CMTScanStackingAction* stackingAction = new CMTScanStackingAction;
	SetUserAction(stackingAction);
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
