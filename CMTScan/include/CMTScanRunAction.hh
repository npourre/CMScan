#ifndef CMTScanRunAction_h
#define CMTScanRunAction_h

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class CMTScanRunAction : public G4UserRunAction {
public:
	CMTScanRunAction();
	~CMTScanRunAction() override;
	void BeginOfRunAction(const G4Run* run) override;
	void   EndOfRunAction(const G4Run* run) override;
};

#endif
