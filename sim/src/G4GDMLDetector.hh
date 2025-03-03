#ifndef G4GDMLDETECTOR_HH
#define G4GDMLDETECTOR_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4GDMLParser.hh"

class G4GDMLDetector : public G4VUserDetectorConstruction {
public:
    explicit G4GDMLDetector(const G4String& gdmlFile) : fGDMLFile(gdmlFile) {}
    
    virtual G4VPhysicalVolume* Construct() override {
        G4GDMLParser parser;
        parser.Read(fGDMLFile, false); // Read GDML file without schema validation
        return parser.GetWorldVolume(); // Return the parsed world volume
    }

private:
    G4String fGDMLFile;
};

#endif // G4GDMLDETECTOR_HH

