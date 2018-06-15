#include "CMTScanRunAction.hh"
#include "CMTScanPrimaryGeneratorAction.hh"

#include "g4root.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanRunAction::CMTScanRunAction()
 : G4UserRunAction()
{
	// set printing event number per each 1000 event
	G4RunManager::GetRunManager()->SetPrintProgress(1);

	// Create analysis manager
	auto analysisManager = G4AnalysisManager::Instance();
	G4cout << "Using " << analysisManager->GetType() << " analysis manager" << G4endl;

	// Create directories //??????
	analysisManager->SetVerboseLevel(1);
	analysisManager->SetNtupleMerging(true);    // Note: merging ntuples is available only with Root output

	// Creating ntuple
	analysisManager->CreateNtuple("Tracker", "TrackerMu");
	analysisManager->CreateNtupleIColumn("chamberNb");
	analysisManager->CreateNtupleDColumn("X");
	analysisManager->CreateNtupleDColumn("Y");
	analysisManager->CreateNtupleDColumn("Time");
	analysisManager->FinishNtuple();    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CMTScanRunAction::~CMTScanRunAction()
{
	delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanRunAction::BeginOfRunAction(const G4Run* run)
{
	G4cout << "### Run " << run->GetRunID() << " start." << G4endl; 

	//inform the runManager to save random number seed //useless
	G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	auto analysisManager = G4AnalysisManager::Instance();
	// Open an output file
	//
	G4String fileName = "data";
	analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CMTScanRunAction::EndOfRunAction(const G4Run* )
{
	G4cout << "RunTime = " <<CMTScanPrimaryGeneratorAction::GetDuration()<< " seconds" << G4endl; 

	// save histograms & ntuple
	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->Write();
	analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
