#ifndef CMTScanRunAction_h
#define CMTScanRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CMTScanRunAction : public G4UserRunAction
{
	public:
		CMTScanRunAction();
		virtual ~CMTScanRunAction();

		virtual void BeginOfRunAction(const G4Run* run);
		virtual void   EndOfRunAction(const G4Run* run);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
