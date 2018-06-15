#ifndef CMTScanTrackerSD_h
#define CMTScanTrackerSD_h 1

#include "G4VSensitiveDetector.hh"
#include "CMTScanTrackerHit.hh"
#include <vector>

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CMTScanTrackerSD : public G4VSensitiveDetector	
{
	public:
		CMTScanTrackerSD(const G4String& name, const G4String& hitsCollectionName);
		virtual ~CMTScanTrackerSD();
  
		// methods from base class
		virtual void   Initialize(G4HCofThisEvent* hitCollection);
		virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
		virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

	private:
		CMTScanTrackerHitsCollection* fHitsCollection;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
