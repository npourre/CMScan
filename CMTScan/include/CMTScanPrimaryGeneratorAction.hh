#ifndef CMTScanPrimaryGeneratorAction_h
#define CMTScanPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "CRYSetup.h"
#include "CRYGenerator.h"
#include "CRYParticle.h"
#include "CRYUtils.h"
#include "RNGWrapper.hh"
#include <vector>

class G4Event;

class CMTScanPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    explicit CMTScanPrimaryGeneratorAction(const char * filename);
  ~CMTScanPrimaryGeneratorAction() override;

public:
  void GeneratePrimaries(G4Event* anEvent) override;
  void InputCRY();
  void UpdateCRY(std::string* MessInput);
  void CRYFromFile(G4String newValue);
  static inline G4double GetDuration(){return TimeSimulated;};


private:
  std::vector<CRYParticle*> *vect;
  G4ParticleTable* particleTable;
  G4ParticleGun* particleGun;
  CRYGenerator* gen;
  G4int InputState;
//  CMTScanPrimaryGeneratorMessenger* gunMessenger;

  static G4double TimeSimulated;
};

#endif
