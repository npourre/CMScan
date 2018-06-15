#ifndef CMTScanDetectorConstruction_h
#define CMTScanDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "GeometryVariable.hh"
#include <map>

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Detector construction class to define materials and geometry.
class CMTScanDetectorConstruction : public G4VUserDetectorConstruction {
public:
    CMTScanDetectorConstruction();
    ~CMTScanDetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
    void processGeometry();
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

private:
    static std::map<int, G4ThreeVector> _geometryMap;
    std::string _geometryFile;
    GeometryVariable *_geometryVariable;
};
#endif

