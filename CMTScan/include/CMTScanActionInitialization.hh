#ifndef CMTScanActionInitialization_h
#define CMTScanActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class CMTScanActionInitialization : public G4VUserActionInitialization
{
  public:
    CMTScanActionInitialization();
    virtual ~CMTScanActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
