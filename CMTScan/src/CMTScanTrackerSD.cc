#include "CMTScanTrackerSD.hh"
#include "CMTScanPrimaryGeneratorAction.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "CMTScanEventAction.hh"
#include "CMTScanTrackerHit.hh"
#include "g4root.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"

#include "G4EventManager.hh"

#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanTrackerSD::CMTScanTrackerSD(const G4String& name, const G4String& hitsCollectionName) : G4VSensitiveDetector(name)
,fHitsCollection(nullptr)
{
	collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanTrackerSD::~CMTScanTrackerSD() 
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanTrackerSD::Initialize(G4HCofThisEvent* hce)
{
	// Create hits collection
	fHitsCollection = new CMTScanTrackerHitsCollection (SensitiveDetectorName, collectionName[0]); 

	// Add this collection in hce
	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CMTScanTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
	G4double edep = aStep->GetTotalEnergyDeposit();
	if (edep==0.) return false;

	if ((aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "RPC_GasChamber_Phy") && (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() != "RPC_GasChamber_Phy"))
	{
		CMTScanTrackerHit* newHit = new CMTScanTrackerHit();
		auto touchable = (aStep->GetPreStepPoint()->GetTouchable());
		auto chamberNumber = touchable->GetReplicaNumber(1);	
		newHit->SetChamberNb(chamberNumber);
		newHit->SetEnergy(aStep->GetTrack()->GetDynamicParticle()->GetKineticEnergy());  
		newHit->SetEdep(edep);
		newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());
		newHit->SetMom (aStep->GetPostStepPoint()->GetMomentumDirection());
		newHit->SetTime(CMTScanPrimaryGeneratorAction::GetDuration());
		fHitsCollection->insert( newHit );
		return true;
	}
	
	else
		return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanTrackerSD::EndOfEvent(G4HCofThisEvent*)
{
	if ( verboseLevel>1 ) 
	{ 
		G4int nofHits = fHitsCollection->entries();
		G4cout << G4endl << "-------->Hits Collection: in this event they are " << nofHits << " hits in the tracker chambers: " << G4endl;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
