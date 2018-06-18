#include "CMTScanEventAction.hh"
#include "CMTScanTrackerSD.hh"
#include "g4root.hh"

#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanEventAction::CMTScanEventAction()
: G4UserEventAction()
,fHCID(-1)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanEventAction::~CMTScanEventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanEventAction::BeginOfEventAction(const G4Event *event)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanEventAction::EndOfEventAction(const G4Event *event)
{
	if ( fHCID == -1 ) 
	{
		fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerHitsCollection");
	}

	auto analysisManager = G4AnalysisManager::Instance();

	auto HC = GetHitsCollection(fHCID, event);
	G4int nofHits = HC->entries();
	for ( G4int i=0; i<nofHits; i++ )
	{
		auto Hit = (*HC)[i];
		G4ThreeVector Pos = Hit->GetPos();
		G4ThreeVector Mom = Hit->GetMom();

        analysisManager->FillNtupleIColumn(0, event->GetEventID());
		analysisManager->FillNtupleIColumn(1, Hit->GetChamberNb());
		analysisManager->FillNtupleDColumn(2, Pos.x()/mm);
	    analysisManager->FillNtupleDColumn(3, Pos.y()/mm);
        analysisManager->FillNtupleDColumn(4, Pos.z()/mm);
	    analysisManager->FillNtupleDColumn(5, Hit->GetTime()/s);
	    analysisManager->AddNtupleRow();
	}
	G4cout<<event->GetEventID()<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanTrackerHitsCollection* CMTScanEventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
	auto hitsCollection = dynamic_cast<CMTScanTrackerHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
  
	if ( ! hitsCollection ) 
	{
	G4ExceptionDescription msg;
	msg << "Cannot access hitsCollection ID " << hcID; 
	G4Exception("B2EventAction::GetHitsCollection()", "MyCode0003", FatalException, msg);
	}         

	return hitsCollection;
}    



