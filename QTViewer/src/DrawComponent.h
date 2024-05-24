#pragma once
#include "pmp/surface_mesh.h"
class DrawComponent
{
public:
    void Init();
    void InitLoadFile();
    void InitMyMesh();
    void InitProperties(pmp::SurfaceMesh* mesh);

    void Draw();
    void DrawFace(pmp::Face f, bool isWire = false);
    void AddAttributes();

    pmp::SurfaceMesh mesh;
};

