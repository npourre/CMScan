#ifndef Constants_h
#define Constants_h

#include "globals.hh"
#include "G4SystemOfUnits.hh"

//TODO prendre en compte le json
//TODO cree un singleton pour remplacer ca.
	const G4int Nb_Of_RPC = 4; // Nombre de RPC par calo
	const G4double Dist_Mid = 34.6*cm; // Distance entre les calo
	const G4double Dist_RPC = 3.6*cm; // Distance entre les RPC dans un calo
	
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

	const G4double Detector_Size_X = 1*m;
	const G4double Detector_Size_Y = 1*m;
	const G4double Detector_Size_Z = ((Nb_Of_RPC * RPC_Th) + (Dist_RPC*(Nb_Of_RPC-1)) + Dist_Mid + (Nb_Of_RPC * RPC_Th) + (Dist_RPC*(Nb_Of_RPC-1)));

	const G4double World_Size_X = Detector_Size_X*1.2;
	const G4double World_Size_Y = Detector_Size_Y*1.2;
	const G4double World_Size_Z = Detector_Size_Z*1.2;

#endif
