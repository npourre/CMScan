#include "CMTScanTrackerHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<CMTScanTrackerHit>* CMTScanTrackerHitAllocator=0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanTrackerHit::CMTScanTrackerHit() : G4VHit()
,fChamberNb(-1)
,fEnergy(0.)
,fEdep(0.)
,fPos(G4ThreeVector())
,fMom(G4ThreeVector())
,fTime(0.)

{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanTrackerHit::~CMTScanTrackerHit() 
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanTrackerHit::CMTScanTrackerHit(const CMTScanTrackerHit& right) : G4VHit()
{
	fChamberNb 	= right.fChamberNb;
	fEnergy	 	= right.fEnergy;
	fEdep  	        = right.fEdep;
	fPos		= right.fPos;
	fMom		= right.fMom;
	fTime		= right.fTime;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const CMTScanTrackerHit& CMTScanTrackerHit::operator=(const CMTScanTrackerHit& right)
{
	fChamberNb 	= right.fChamberNb;
	fEnergy	 	= right.fEnergy;
	fEdep 	    = right.fEdep;
	fPos		= right.fPos;
	fMom		= right.fMom;
	fTime		= right.fTime;

	return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int CMTScanTrackerHit::operator==(const CMTScanTrackerHit& right) const
{
	return ( this == &right ) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

