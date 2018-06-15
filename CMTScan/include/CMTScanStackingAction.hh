#ifndef CMTScanStackingAction_h
#define CMTScanStackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

class CMTScanStackingAction : public G4UserStackingAction
{
  public:
    CMTScanStackingAction();
    virtual ~CMTScanStackingAction();
     
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);        
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

