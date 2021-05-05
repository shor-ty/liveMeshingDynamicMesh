/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "liveMeshingDMSolverFvMesh.H"
#include "addToRunTimeSelectionTable.H"
#include "motionSolver.H"

#include "hexMatcher.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(liveMeshingDMSolverFvMesh, 0);
    addToRunTimeSelectionTable
    (
        dynamicFvMesh,
        liveMeshingDMSolverFvMesh,
        IOobject
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::liveMeshingDMSolverFvMesh::liveMeshingDMSolverFvMesh(const IOobject& io)
:
    dynamicFvMesh(io),
    motionPtr_(motionSolver::New(*this, dynamicMeshDict())),
    velocityMotionCorrection_(*this, dynamicMeshDict())
{
    // TODO PRE-STUFF

    // We always start with the background mesh as the meshing is done inside
    // this library. Hence, we have to firstly check, if the background mesh
    // was already saved to the new location, if yes, we load that mesh and
    // store it. This means that the calculation was already started. If there
    // is no background mesh stored, the actual mesh is the base mesh used for
    // meshing. We take that and store it to the location of the background mesh

    // Load mesh in constant
    const fvMesh backgroundMesh
    (
        IOobject
        (
            "region0",
            "constant",
            time(),
            IOobject::MUST_READ
        )
    );

    // TODO old stuff not needed actually
    // Get cell types - each cell has to be hexahedral for remeshing
    hexMatcher hex;

    for (label celli = 0; celli < backgroundMesh.nCells(); ++celli)
    {
        // If it is not a hex cell stop
        if (!hex.isA(backgroundMesh, celli))
        {
            FatalErrorInFunction
                << "The background mesh in constant/polyMesh has to be a pure "
                << "\nhexahedral mesh... Stopping"
                << exit(FatalError);
        }
    }

    // Get the mesh used for the analysis (it is the fvMesh from the solver)
    const fvMesh& cfdMesh = time().lookupObject<fvMesh>("region0");

    Info << backgroundMesh.nCells() << endl;
    Info << cfdMesh.nCells() << endl;
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::liveMeshingDMSolverFvMesh::~liveMeshingDMSolverFvMesh()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::motionSolver& Foam::liveMeshingDMSolverFvMesh::motion() const
{
    return motionPtr_();
}


bool Foam::liveMeshingDMSolverFvMesh::update()
{
    // 1. Save current mesh and quantities in memory for possible restoring
    Info<< "Save old mesh and data"<< endl;

    // 2. Move the mesh according to the motion
    {
        fvMesh::movePoints(motionPtr_->newPoints());

        velocityMotionCorrection_.update();
    }

    // 3. Check the mesh quality
    // -------------------------
    // Check fails
    // 3.1 restore old data
    // 3.2 export actual position of moving surfaces or move the original STLs
    // 3.3 take the initial mesh (constant/polyMesh) that is stored in the 
    //     memory and remesh with new STL positions
    // 3.4 map the old quantities from the old mesh to the new generated mesh

    // 4. Optional
    // Do adaptive mesh refinement based on gradients etc.


    return true;
}


bool Foam::liveMeshingDMSolverFvMesh::writeObject
(
    IOstream::streamFormat fmt,
    IOstream::versionNumber ver,
    IOstream::compressionType cmp,
    const bool write
) const
{
    motionPtr_->write();
    return fvMesh::writeObject(fmt, ver, cmp, write);
}


// ************************************************************************* //
