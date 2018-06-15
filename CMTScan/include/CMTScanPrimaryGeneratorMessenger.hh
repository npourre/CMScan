/*#ifndef CMTScanPrimaryGeneratorMessenger_h
#define CMTScanPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class CMTScanPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CMTScanPrimaryGeneratorMessenger: public G4UImessenger
{
  	public:
    	CMTScanPrimaryGeneratorMessenger(CMTScanPrimaryGeneratorAction*);
   		~CMTScanPrimaryGeneratorMessenger();
    
    	void SetNewValue(G4UIcommand*, G4String);
    
  	private:
    	CMTScanPrimaryGeneratorAction*      Action;
    	G4UIdirectory*               CRYDir;
    	G4UIcmdWithAString*          FileCmd; 
    	G4UIcmdWithAString*          InputCmd;
    	G4UIcmdWithoutParameter*     UpdateCmd;
    	std::string* MessInput;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

*/