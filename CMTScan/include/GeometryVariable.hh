#ifndef Constants_h
#define Constants_h

#include "globals.hh"
#include "G4SystemOfUnits.hh"

	const G4double RPC_Chip_Th = 1.6*mm;
	const G4double RPC_PCB_Th = 1.2*mm;
	const G4double RPC_MylarAnode_Th = 0.05*mm;
	const G4double RPC_CoatingAnode_Th = 0.05*mm; 
	const G4double RPC_GlassAnode_Th = 0.7*mm;
	const G4double RPC_GasChamber_Th = 1.2*mm;
	const G4double RPC_GlassCathode_Th = 1.1*mm;
	const G4double RPC_CoatingCathode_Th = 0.05*mm; 
	const G4double RPC_MylarCathode_Th = 0.18*mm;

	const G4double RPC_K7T_Th = 5*mm;
	const G4double RPC_K7B_Th = 5*mm;
	const G4double RPC_Inner_Th = 	RPC_Chip_Th +
									RPC_PCB_Th +
									RPC_MylarAnode_Th +
									RPC_CoatingAnode_Th +
									RPC_GlassAnode_Th +
									RPC_GasChamber_Th +
									RPC_GlassCathode_Th +
									RPC_CoatingCathode_Th +
									RPC_MylarCathode_Th;
	
	const G4double RPC_Th = RPC_K7T_Th + RPC_Inner_Th + RPC_K7B_Th;

class GeometryVariable
{
private:
    GeometryVariable(): _world_size{}, _detector_size{} {;}
    ~GeometryVariable() = default;

public:
    inline void setDetectorSize(G4ThreeVector detector_size) {_detector_size = detector_size;}
    inline void setWorldSize(G4ThreeVector world_size){_world_size = world_size;}
    inline G4ThreeVector const getWorldSize(){return _world_size;}
    inline G4ThreeVector const getDetectorSize(){return _detector_size;}

    static GeometryVariable *instance(){
        if (nullptr == _singleton) {
            _singleton =  new GeometryVariable;
        }
        return _singleton;
    }
    static void kill (){
        if (nullptr != _singleton) {
            delete _singleton;
            _singleton = nullptr;
        }
    }

private:
    G4ThreeVector _world_size;
    G4ThreeVector _detector_size{};
    static GeometryVariable *_singleton;
};
#endif
