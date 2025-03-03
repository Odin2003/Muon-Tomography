// TessellatedGeometry.cc
#include "TessellatedVertices.hh"   // Contains all vertex definitions
#include "TessellatedGeometry.hh"   // Declares DefineTessellatedSolids()
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"

//
// CreateTessellatedGeometry() builds the tessellated solids (terrain and mound),
// creates logical volumes for them using the materials (Rock1 and Rock2),
// places them into a world volume (using G4_AIR) and returns the physical world.
//
G4VPhysicalVolume* CreateTessellatedGeometry() {
    // Get materials from the NIST manager
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* rock1 = nist->FindOrBuildMaterial("Rock1");
    G4Material* rock2 = nist->FindOrBuildMaterial("Rock2");
    G4Material* air   = nist->FindOrBuildMaterial("G4_AIR");

    // Create the tessellated solids using the inline function declared in TessellatedGeometry.hh
    G4TessellatedSolid* terrainSolid = nullptr;
    G4TessellatedSolid* moundSolid   = nullptr;
    DefineTessellatedSolids(terrainSolid, moundSolid);

    // Create logical volumes for the tessellated solids.
    // (Assumes that the GDML assigned terrain to Rock1 and mound to Rock2)
    G4LogicalVolume* terrainLV = new G4LogicalVolume(terrainSolid, rock1, "TerrainLV");
    G4LogicalVolume* moundLV   = new G4LogicalVolume(moundSolid, rock2, "MoundLV");

    // Create a world volume.
    // Here we use a G4Box with a large size (adjust as necessary).
    G4Box* worldSolid = new G4Box("WorldBox", 150 * m, 150 * m, 150 * m);
    G4LogicalVolume* worldLV = new G4LogicalVolume(worldSolid, air, "WorldLV");

    // Place the terrain and mound logical volumes in the world volume.
    // Since the vertex positions are defined absolutely, we place both at (0,0,0)
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), terrainLV, "Terrain", worldLV, false, 0, true);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), moundLV, "Mound", worldLV, false, 0, true);

    // Return the physical world volume.
    return new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), worldLV, "World", nullptr, false, 0, true);
}

