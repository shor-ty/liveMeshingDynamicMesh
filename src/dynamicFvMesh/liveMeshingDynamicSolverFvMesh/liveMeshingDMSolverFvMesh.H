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

Class
    Foam::liveMeshingDMSolverFvMesh

Description
    The liveMeshingDMSolverFvMesh handles IC engines. The idea here is to use a
    common motion solver such as a laplacian motion solver and move the mesh
    for each time step accordingly. Before the mesh is moved, a copy is stored
    and after the mesh motion the mesh is checked. If the mesh quality
    parameters are not any longer fulfilled, take the last mesh, export the
    surfaces, rebuild the background mesh and remesh.

SourceFiles
    liveMeshingDMSolverFvMesh.C

\*---------------------------------------------------------------------------*/

#ifndef liveMeshingDMSolverFvMesh_H
#define liveMeshingDMSolverFvMesh_H

#include "dynamicFvMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class motionSolver;

/*---------------------------------------------------------------------------*\
                  Class liveMeshingDMSolverFvMesh Declaration
\*---------------------------------------------------------------------------*/

class liveMeshingDMSolverFvMesh
:
    public dynamicFvMesh
{
    // Private Data

        autoPtr<motionSolver> motionPtr_;

        //- Optional list of vectorFields to update for mesh motion
        //  For modern solvers using Uf and correctPhi to update the flux
        //  after motion it is not necessary to specify a "velocityFields" list
        velocityMotionCorrection velocityMotionCorrection_;

public:

        //- Stores a copy of the actual mesh (before mesh motion) which is
        //  the mesh quality parameters are failing after the mesh motion
        //  Copy, faster if we could use some swap of mesh object pointers
        //fvMesh backUpMesh_;


public:

    //- Runtime type information
    TypeName("liveMeshingDMSolverFvMesh");


    // Constructors

        //- Construct from IOobject
        liveMeshingDMSolverFvMesh(const IOobject& io);

        //- Disallow default bitwise copy construction
        liveMeshingDMSolverFvMesh(const liveMeshingDMSolverFvMesh&) = delete;


    //- Destructor
    ~liveMeshingDMSolverFvMesh();


    // Member Functions

        //- Return the motionSolver
        const motionSolver& motion() const;

        //- Update the mesh for both mesh motion and topology change
        virtual bool update();


        // Write

            //- Write the underlying polyMesh and other data
            virtual bool writeObject
            (
                IOstream::streamFormat fmt,
                IOstream::versionNumber ver,
                IOstream::compressionType cmp,
                const bool write = true
            ) const;


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const liveMeshingDMSolverFvMesh&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
