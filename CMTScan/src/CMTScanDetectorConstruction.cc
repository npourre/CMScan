#include "CMTScanDetectorConstruction.hh"
#include "CMTScanTrackerSD.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4SDParticleFilter.hh"

#include "filereadstream.h"
#include "document.h"

std::map<int, G4ThreeVector> CMTScanDetectorConstruction::_geometryMap={};
GeometryVariable *GeometryVariable::_singleton = nullptr;

CMTScanDetectorConstruction::CMTScanDetectorConstruction() :
		_geometryFile("../geometry/geometry.json") {
    _geometryVariable=GeometryVariable::instance();
}


G4VPhysicalVolume* CMTScanDetectorConstruction::Construct() {
    processGeometry();
	DefineMaterials();  	
	return DefineVolumes();  
}

void CMTScanDetectorConstruction::DefineMaterials() {
	G4NistManager *nist_rpc_materials = G4NistManager::Instance();

	G4double a = 1.01 * g / mole;
	G4Element *elH = new G4Element("Hydrogen", "H", 1, a);

	a = 12.01 * g / mole;
	G4Element *elC = new G4Element("Carbon", "C", 6, a);

	a = 16.00 * g / mole;
	G4Element *elO = new G4Element("Oxygen", "O", 8, a);

	a = 32.06 * g / mole;
	G4Element *elS = new G4Element("Sulfur", "S", 16, a);

	a = 19.00 * g / mole;
	G4Element *elF = new G4Element("Fluor", "F", 9, a);

	a = 55.845 * g / mole;
	G4Element *elFe = new G4Element("Fer", "Fe", 26, a);

	a = 54.938 * g / mole;
	G4Element *elMn = new G4Element("Manganese", "Mn", 25, a);

	a = 28.086 * g / mole;
	G4Element *elSi = new G4Element("Silicium", "Si", 14, a);

	a = 30.974 * g / mole;
	G4Element *elP = new G4Element("Phosphore", "P", 15, a);

	a = 51.996 * g / mole;
	G4Element *elCr = new G4Element("Chrome", "Cr", 24, a);

	a = 58.693 * g / mole;
	G4Element *elNi = new G4Element("Nickel", "Ni", 28, a);

	a = 63.546 * g / mole;
	G4Element *elCu = new G4Element("Cuivre", "Cu", 29, a);

	a = 26.982 * g / mole;
	G4Element *elAl = new G4Element("Aluminium", "Al", 13, a);

	a = 92.906 * g / mole;
	G4Element *elNb = new G4Element("Niobium", "Nb", 41, a);

	///Hexafluorure de souffre (SF6)
	G4double SF6_Density = 6.16e-3 * g / cm3;
	G4Material *SF6 = new G4Material("SF6", SF6_Density, 2);
	SF6->AddElement(elS, 1);
	SF6->AddElement(elF, 6);

	///Nid d'abeille
	G4Material *alveolar_core = new G4Material("alveolar_core", 0.764*g/cm3, 1);
	alveolar_core->AddElement(elAl, 1);

	///Isobutane
	G4double Isobutane_Density = 2.51e-3 * g / cm3;
	G4Material *Isobutane = new G4Material("Isobutane", Isobutane_Density, 2);
	Isobutane->AddElement(elC, 4);
	Isobutane->AddElement(elH, 10);

	///CO2
	G4double CO2_Density = 1.87e-3 * g / cm3;
	G4Material *CO2 = new G4Material("CO2", CO2_Density, 2);
	CO2->AddElement(elC, 1);
	CO2->AddElement(elO, 2);

	///Tétrafluoroéthane (TFE)
	G4double TFE_Density = 4.28e-3 * g / cm3;
	G4Material *TFE = new G4Material("TFE", TFE_Density, 3);
	TFE->AddElement(elC, 2);
	TFE->AddElement(elH, 2);
	TFE->AddElement(elF, 4);

	///RPCGaz
	G4double RPC_Gaz_Density = (SF6_Density*0.02+CO2_Density*0.05+TFE_Density*0.93);
	G4Material *RPC_GasChamber_Material = new G4Material("RPC_Gaz", RPC_Gaz_Density, 3);
	RPC_GasChamber_Material->AddMaterial(TFE, 0.93);
	RPC_GasChamber_Material->AddMaterial(CO2, 0.05);
	RPC_GasChamber_Material->AddMaterial(SF6, 0.02);

	///g10
	G4double epoxydensity = 1.3 * g / cm3;
	G4Material *epoxy = new G4Material("epoxy", epoxydensity, 3);
	epoxy->AddElement(elC, 15);
	epoxy->AddElement(elO, 7);
	epoxy->AddElement(elH, 44);

	G4Material *Si02 = nist_rpc_materials->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
	G4Material *Cl = nist_rpc_materials->FindOrBuildMaterial("G4_Cl");

	G4double g10density = 1.7 * g / cm3;
	G4Material *g10 = new G4Material("g10", g10density, 3);
	g10->AddMaterial(epoxy, 14.7 * perCent);
	g10->AddMaterial(Cl, 8 * perCent);
	g10->AddMaterial(Si02, 77.3 * perCent);

	///Corten-A steel
	G4double cortenSteelDensity = 7.8 * g / cm3;
	G4Material *cortenSteel = new G4Material("cortenSteel", cortenSteelDensity, 11);
	cortenSteel -> AddElement(elFe, 95.88 * perCent);
	cortenSteel -> AddElement(elC, 0.12 * perCent);
	cortenSteel -> AddElement(elMn, 0.5 * perCent);
	cortenSteel -> AddElement(elSi, 0.75 * perCent);
	cortenSteel -> AddElement(elS, 0.03 * perCent);
	cortenSteel -> AddElement(elP, 0.15 * perCent);
	cortenSteel -> AddElement(elCr, 1.25 * perCent);
	cortenSteel -> AddElement(elNi, 0.65 * perCent);
	cortenSteel -> AddElement(elCu, 0.55 * perCent);
	cortenSteel -> AddElement(elAl, 0.06 * perCent);
	cortenSteel -> AddElement(elNb, 0.06 * perCent);
}

G4VPhysicalVolume* CMTScanDetectorConstruction::DefineVolumes() {
	G4NistManager *nistManager = G4NistManager::Instance();
	/////////////////////////////////////////////////////
	///Simple materials
	/////////////////////////////////////////////////////

	G4Material *RPC_Mylar_Material = nistManager->FindOrBuildMaterial("G4_MYLAR");
	G4Material *RPC_Coating_Material = nistManager->FindOrBuildMaterial("G4_GRAPHITE");
	G4Material *RPC_Glass_Material = nistManager->FindOrBuildMaterial("G4_Pyrex_Glass");
	G4Material *air = nistManager->FindOrBuildMaterial("G4_AIR");
	G4Material *Target_Material = nistManager->FindOrBuildMaterial("G4_Pb");

	/////////////////////////////////////////////////////
	/// World
	/////////////////////////////////////////////////////

	G4Box* WorldSol = new G4Box("WorldSol",
								_geometryVariable->getWorldSize().getX()*0.5,
                                _geometryVariable->getWorldSize().getY()*0.5,
                                _geometryVariable->getWorldSize().getZ()*0.5);

	G4LogicalVolume* WorldLog = new G4LogicalVolume(WorldSol,
                                                    air,
                                                    "WorldLog");

	G4VPhysicalVolume* WorldPhy = new G4PVPlacement(nullptr,
                                                    G4ThreeVector(),
                                                    WorldLog,
                                                    "WorldPhy",
                                                    nullptr,
                                                    false,
                                                    0,
                                                    true);

	/////////////////////////////////////////////////////
	/// Logical volume : RPC
	/////////////////////////////////////////////////////

    G4double Detector_Size_X = 1*m;
    G4double Detector_Size_Y = 1*m;
	G4Box* RPC_Sol = new G4Box("RPC_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_Th*0.5);
	G4LogicalVolume* RPC_Log = new G4LogicalVolume(RPC_Sol, air, "RPC_Log");

	/////////////////////////////////////////////////////
	/// RPC_layer
	/////////////////////////////////////////////////////

	G4Box* RPC_K7T_Sol = 			new G4Box("RPC_K7T_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_K7T_Th*0.5);  
	G4Box* RPC_Chip_Sol = 			new G4Box("RPC_Chip_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_Chip_Th*0.5);
	G4Box* RPC_PCB_Sol = 			new G4Box("RPC_PCB_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_PCB_Th*0.5);
	G4Box* RPC_MylarAnode_Sol = 	new G4Box("RPC_MylarAnode_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_MylarAnode_Th*0.5);
	G4Box* RPC_CoatingAnode_Sol = 	new G4Box("RPC_CoatingAnode_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_CoatingAnode_Th*0.5);
	G4Box* RPC_GlassAnode_Sol = 	new G4Box("RPC_GlassAnode_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_GlassAnode_Th*0.5);
	G4Box* RPC_GasChamber_Sol = 	new G4Box("RPC_GasChamber_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_GasChamber_Th*0.5);
	G4Box* RPC_GlassCathode_Sol = 	new G4Box("RPC_GlassCathode_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_GlassCathode_Th*0.5);
	G4Box* RPC_CoatingCathode_Sol = new G4Box("RPC_CoatingCathode_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_CoatingCathode_Th*0.5);
	G4Box* RPC_MylarCathode_Sol = 	new G4Box("RPC_MylarCathode_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_MylarCathode_Th*0.5);
	G4Box* RPC_K7B_Sol = 			new G4Box("RPC_K7B_Sol", Detector_Size_X*0.5, Detector_Size_Y*0.5, RPC_K7B_Th*0.5);

	G4LogicalVolume* RPC_K7T_Log = 				new G4LogicalVolume(RPC_K7T_Sol, nistManager->FindOrBuildMaterial("alveolar_core"), "RPC_K7T_Log");
	G4LogicalVolume* RPC_Chip_Log = 			new G4LogicalVolume(RPC_Chip_Sol, nistManager->FindOrBuildMaterial("g10"), "RPC_Chip_Log");
	G4LogicalVolume* RPC_PCB_Log = 				new G4LogicalVolume(RPC_PCB_Sol, nistManager->FindOrBuildMaterial("g10"), "RPC_PCB_Log");
	G4LogicalVolume* RPC_MylarAnode_Log = 		new G4LogicalVolume(RPC_MylarAnode_Sol, RPC_Mylar_Material, "RPC_MylarAnode_Log");
	G4LogicalVolume* RPC_CoatingAnode_Log= 		new G4LogicalVolume(RPC_CoatingAnode_Sol, RPC_Coating_Material, "RPC_CoatingAnode_Log");
	G4LogicalVolume* RPC_GlassAnode_Log = 		new G4LogicalVolume(RPC_GlassAnode_Sol, RPC_Glass_Material, "RPC_GlassAnode_Log");
	G4LogicalVolume* RPC_GasChamber_Log = 		new G4LogicalVolume(RPC_GasChamber_Sol, nistManager->FindOrBuildMaterial("RPC_Gaz"), "RPC_GasChamber_Log");
	G4LogicalVolume* RPC_GlassCathode_Log = 	new G4LogicalVolume(RPC_GlassCathode_Sol, RPC_Glass_Material, "RPC_GlassCathode_Log");
	G4LogicalVolume* RPC_CoatingCathode_Log = 	new G4LogicalVolume(RPC_CoatingCathode_Sol, RPC_Coating_Material, "RPC_CoatingCathode_Log");
	G4LogicalVolume* RPC_MylarCathode_Log = 	new G4LogicalVolume(RPC_MylarCathode_Sol, RPC_Mylar_Material, "RPC_MylarCathode_Log");
	G4LogicalVolume* RPC_K7B_Log = 				new G4LogicalVolume(RPC_K7B_Sol,  nistManager->FindOrBuildMaterial("alveolar_core"), "RPC_K7B_Log");

	G4double Position_Z = RPC_Th*0.5;
	Position_Z = Position_Z - RPC_K7T_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_K7T_Log, "RPC_K7T_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z - RPC_K7T_Th*0.5 - RPC_Chip_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_Chip_Log, "RPC_Chip_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_Chip_Th*0.5-RPC_PCB_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_PCB_Log, "RPC_PCB_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_PCB_Th*0.5-RPC_MylarAnode_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_MylarAnode_Log, "RPC_MylarAnode_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_MylarAnode_Th*0.5-RPC_CoatingAnode_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_CoatingAnode_Log, "RPC_CoatingAnode_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_CoatingAnode_Th*0.5-RPC_GlassAnode_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_GlassAnode_Log, "RPC_GlassAnode_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_GlassAnode_Th*0.5-RPC_GasChamber_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_GasChamber_Log, "RPC_GasChamber_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_GasChamber_Th*0.5-RPC_GlassCathode_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_GlassCathode_Log, "RPC_GlassCathode_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_GlassCathode_Th*0.5-RPC_CoatingCathode_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_CoatingCathode_Log, "RPC_CoatingCathode_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_CoatingCathode_Th*0.5-RPC_MylarCathode_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_MylarCathode_Log, "RPC_MylarCathode_Phy", RPC_Log, false, 0, true);
	Position_Z =Position_Z-RPC_MylarCathode_Th*0.5-RPC_K7B_Th*0.5;
	new G4PVPlacement (nullptr, G4ThreeVector(0, 0, Position_Z), RPC_K7B_Log, "RPC_K7B_Phy", RPC_Log, false, 0, true);

	/////////////////////////////////////////////////////
	/// RPC positioning
	/////////////////////////////////////////////////////

	for (auto& it : _geometryMap) {
		G4String name = "RPC_";
		name.append(std::to_string(it.first));
		new G4PVPlacement(nullptr,
						  it.second,
						  RPC_Log,
						  name,
						  WorldLog,
						  false,
						  static_cast<G4int>(it.first),
						  true);
	}

/*	/////////////////////////////////////////////////////
	/// Container
	/////////////////////////////////////////////////////

	G4double steelThickness = 3 * mm;

	G4Box* outerContainer = new G4Box(	"OuterContainer", 
			Detector_Size_X*0.5, 
			Detector_Size_Y*0.5, 
			Dist_Mid*0.5);

	G4Box* innerContainer = new G4Box(	"InnerContainer", 
			Detector_Size_X*0.5 - steelThickness, 
			Detector_Size_Y*0.5 - steelThickness, 
			Dist_Mid*0.5 - steelThickness);	

	G4SubtractionSolid* Container = new G4SubtractionSolid(	"Container", 
			outerContainer, 
			innerContainer);

	G4LogicalVolume* Container_Log = new G4LogicalVolume(	Container, 
			nistManager->FindOrBuildMaterial("cortenSteel"), 
			"Container_Log");

	new G4PVPlacement (	0, 
			G4ThreeVector(), 
			Container_Log, 
			"Container_Phy", 
			WorldLog, 
			false, 
			0, 
			true);
*/
	/////////////////////////////////////////////////////
	/// Target
	/////////////////////////////////////////////////////

    G4Box* Target = new G4Box("Target",
                              5*cm,
                              5*cm,
                              2.5*cm);

	G4LogicalVolume* Target_Log = new G4LogicalVolume(Target,
                                                      Target_Material,
                                                      "Target_Log");

	new G4PVPlacement (nullptr,
                       G4ThreeVector(0,0,0),
                       Target_Log,
                       "Target_Phy",
                       WorldLog,
                       false,
                       0,
                       true);

	/////////////////////////////////////////////////////

	CMTScanTrackerSD* SDetector = new CMTScanTrackerSD("CalorimeterSD","TrackerHitsCollection");	
	G4SDParticleFilter* MuonFilter = new G4SDParticleFilter("MuonFilter");
	MuonFilter->add("mu+");
	MuonFilter->add("mu-");
	SDetector->SetFilter(MuonFilter);

	G4SDManager::GetSDMpointer()->AddNewDetector(SDetector);
	SetSensitiveDetector("RPC_GasChamber_Log", SDetector, true);

	return WorldPhy;
}

void CMTScanDetectorConstruction::processGeometry() {
	_geometryMap.clear();
	FILE* fp = fopen(_geometryFile.c_str(), "r");
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(is);
	fclose(fp);

    assert(d.HasMember("chambers"));
    assert(d["chambers"].IsArray());
    const rapidjson::Value& chambers = d["chambers"];
    for (rapidjson::SizeType i = 0; i < chambers.Size(); i++)
    {
        if(chambers[i].HasMember("slot"))
        {
            auto it = _geometryMap.insert( {chambers[i]["slot"].GetInt(), G4ThreeVector(chambers[i]["position_x"].GetDouble()*cm,
																						chambers[i]["position_y"].GetDouble()*cm,
																						chambers[i]["position_z"].GetDouble()*cm)} ) ;
            if(!it.second)
                std::cout <<"ERROR : dif"<<chambers[i]["slot"].GetInt()<<"already exists"<<std::endl;
        }
        else
            std::cout <<"ERROR : json file has no slot member"<<std::endl;


    }

    G4double minZ=1e9;
    G4double maxZ=-1e9;
	std::cout<<"-------------------------------------------------"<<std::endl;
    for ( const auto& it : _geometryMap ){
        if(it.second.getZ()>maxZ)
            maxZ=it.second.getZ();
        if(it.second.getZ()<minZ)
            minZ=it.second.getZ();
        std::cout<<"Slot "<<it.first<<":\t"<<
				 it.second.getX()/cm<<" cm\t"<<
				 it.second.getY()/cm<<" cm\t"<<
				 it.second.getZ()/cm<<" cm"<<std::endl;
    }
    std::cout<<"-------------------------------------------------"<<std::endl;
    G4ThreeVector detector_size(1*m,1*m,maxZ-minZ+RPC_Th);
    G4ThreeVector world_size(detector_size*1.2);
    _geometryVariable->setDetectorSize(detector_size);
    _geometryVariable->setWorldSize(world_size);
}

CMTScanDetectorConstruction::~CMTScanDetectorConstruction() {
    GeometryVariable::kill();
}

