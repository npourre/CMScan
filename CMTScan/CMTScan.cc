//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: exampleCMTScan.cc 86065 2014-11-07 08:51:15Z gcosmo $
//
/// \file exampleCMTScan.cc
/// \brief Main program of the CMTScan example

#include "CMTScanDetectorConstruction.hh"
#include "CMTScanActionInitialization.hh"

#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "G4StepLimiterPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

int main(int argc,char** argv)
{

	// Detect interactive mode (if no arguments) and define UI session
  	G4UIExecutive* ui = nullptr;
  	if ( argc == 1 ) 
	{
	    ui = new G4UIExecutive(argc, argv );
  	}

  	// Choose the Random engine
  	G4Random::setTheEngine(new CLHEP::RanecuEngine);

	///////////////////////////////////////////////////////////////////////////////////////////
  	// Construct the default run manager
	///////////////////////////////////////////////////////////////////////////////////////////

    auto* runManager = new G4RunManager;

	///////////////////////////////////////////////////////////////////////////////////////////
	// Set mandatory initialization classes
  	///////////////////////////////////////////////////////////////////////////////////////////

  	// Detector construction
  	runManager->SetUserInitialization(new CMTScanDetectorConstruction());

  	// Physics list
  	G4VModularPhysicsList* physicsList = new FTFP_BERT;
  	physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  	runManager->SetUserInitialization(physicsList);
    
  	// User action initialization
  	runManager->SetUserInitialization(new CMTScanActionInitialization());

  	///////////////////////////////////////////////////////////////////////////////////////////

	G4cout<<"OK => declaration et initialisation\n"<<G4endl;
	// in case something is wrong
	if ( G4RunManager::GetRunManager()->GetUserPhysicsList() == nullptr)
	{
		G4cerr << G4endl << " ***** ERROR ***** The PHYSICS LIST is not set ***** ERROR ***** " << G4endl;
		exit (-1);
	}
	if ( G4RunManager::GetRunManager()->GetUserDetectorConstruction() == nullptr)
	{
		G4cerr << G4endl << " ***** ERROR ***** The DETECTOR is not built ***** ERROR ***** " << G4endl;
		exit (-1);
	}
	if ( G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction() == nullptr)
	{
		G4cerr << G4endl << " ***** ERROR ***** The Primary generator is not defined ***** ERROR ***** " << G4endl;
		exit (-1);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	/// Initialize visualization
	///////////////////////////////////////////////////////////////////////////////////////////

	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// Process macro or start UI session
	if (ui != nullptr) {
		// interactive mode
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
		delete ui;
	} else {
		// batch mode
		G4String command = "/control/execute ";
		std::string fileName = argv[1];
		UImanager->ApplyCommand(command + fileName);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	// owned and deleted by the run manager, so they should not be deleted 
	// in the main() program !
  	///////////////////////////////////////////////////////////////////////////////////////////

	delete visManager;
	delete runManager;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
