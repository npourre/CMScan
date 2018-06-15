#ifndef CMTScanEventAction_h
#define CMTScanEventAction_h 1

#include "G4UserEventAction.hh"
#include "CMTScanTrackerHit.hh"
#include "globals.hh"

/// Event action class

class CMTScanEventAction : public G4UserEventAction {
public:
    CMTScanEventAction();
    ~CMTScanEventAction() override;

    void BeginOfEventAction(const G4Event* ) override;
    void EndOfEventAction(const G4Event* ) override;
	CMTScanTrackerHitsCollection* GetHitsCollection(G4int HCID, const G4Event* event) const;

private:
	G4int  fHCID;
};

#endif
