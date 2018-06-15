#ifndef CMTScanTrackerHit_h
#define CMTScanTrackerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"


class CMTScanTrackerHit : public G4VHit
{
	public:
		CMTScanTrackerHit();
		CMTScanTrackerHit(const CMTScanTrackerHit&);
		virtual ~CMTScanTrackerHit();

		// operators
		const CMTScanTrackerHit& operator=(const CMTScanTrackerHit&);
		G4int operator==(const CMTScanTrackerHit&) const;

		inline void* operator new(size_t);
		inline void  operator delete(void*);

		// Set methods
		void SetChamberNb	(G4int chamb)	{fChamberNb = chamb; };
		void SetEnergy  	(G4double energy)	{fEnergy = energy; };
		void SetEdep     	(G4double de)		{fEdep = de; };
		void SetPos      	(G4ThreeVector xyz)	{fPos = xyz; };
		void SetMom      	(G4ThreeVector uvw)	{fMom = uvw; };
		void SetTime 		(G4double time)		{fTime = time;};

		// Get methods
		G4int GetChamberNb() const  	{return fChamberNb;};
		G4double GetEnergy() const 		{return fEnergy;};
		G4double GetEdep() const    	{return fEdep;};
		G4ThreeVector GetPos() const	{return fPos;};
		G4ThreeVector GetMom() const	{return fMom;};
		G4double GetTime() const		{return fTime;};

	private:

		G4int       	fChamberNb;
		G4double    	fEnergy;
		G4double    	fEdep;
		G4ThreeVector	fPos;
		G4ThreeVector 	fMom;
		G4double 		fTime;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<CMTScanTrackerHit> CMTScanTrackerHitsCollection;

extern G4ThreadLocal G4Allocator<CMTScanTrackerHit>* CMTScanTrackerHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* CMTScanTrackerHit::operator new(size_t)
{
  if(!CMTScanTrackerHitAllocator)
      CMTScanTrackerHitAllocator = new G4Allocator<CMTScanTrackerHit>;
  return (void *) CMTScanTrackerHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void CMTScanTrackerHit::operator delete(void *hit)
{
  CMTScanTrackerHitAllocator->FreeSingle((CMTScanTrackerHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
