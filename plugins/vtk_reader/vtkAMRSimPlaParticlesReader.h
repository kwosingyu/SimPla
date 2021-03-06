/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vtkAMRSimPlaParticlesReader.h

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
// .NAME vtkAMRSimPlaParticlesReader.h -- Reads AMR SimPla Particle datasets
//
// .SECTION Description
//  A concrete instance of the vtkAMRBaseParticlesReader which provides
//  functionality for loading ENZO AMR particle datasets from ENZO.
//
// .SECTION See Also
//  vtkAMRBaseParticlesReader

#ifndef VTKAMRENZOPARTICLESREADER_H_
#define VTKAMRENZOPARTICLESREADER_H_

#include <vtkAMRBaseParticlesReader.h>
#include <bits/unique_ptr.h>


class vtkPolyData;

class vtkDataArray;

class vtkIntArray;

class vtkSimPlaReaderInternal;

class vtkAMRSimPlaParticlesReader :
        public vtkAMRBaseParticlesReader
{
public:
    static vtkAMRSimPlaParticlesReader *New();

    vtkTypeMacro(vtkAMRSimPlaParticlesReader, vtkAMRBaseParticlesReader);

    void PrintSelf(ostream &os, vtkIndent indent);

    // Description:
    // Returns the requested particle type.
    vtkSetMacro(ParticleType, int);

    vtkGetMacro(ParticleType, int);

    // Description:
    // See vtkAMRBaseParticlesReader::GetTotalNumberOfParticles.
    int GetTotalNumberOfParticles();

protected:
    vtkAMRSimPlaParticlesReader();

    virtual ~vtkAMRSimPlaParticlesReader();

    // Description:
    // Read the m_fluid_sp_ from the given m_fluid_sp_ file for the block
    // corresponding to the given block index.
    vtkPolyData *GetParticles(const char *file, const int blockIdx);

    // Description:
    // See vtkAMRBaseParticlesReader::ReadMetaData()
    void ReadMetaData();

    // Description:
    // See vtkAMRBaseParticlesReader::SetupParticleDataSelections
    void SetupParticleDataSelections();

    // Description:
    // Filter's by particle type, iff particle_type is included in
    // the given file.
    bool CheckParticleType(const int pIdx, vtkIntArray *ptypes);

    // Description:
    // Returns the ParticlesType Array
    vtkDataArray *GetParticlesTypeArray(const int blockIdx);

    // Description:
    // Reads the m_fluid_sp_.
    vtkPolyData *ReadParticles(const int blkidx);

    int ParticleType;


private:
    vtkAMRSimPlaParticlesReader(const vtkAMRSimPlaParticlesReader &) = delete; // Not implemented
    void operator=(const vtkAMRSimPlaParticlesReader &) = delete; // Not implemented

    struct pimpl_s;
    std::unique_ptr<pimpl_s> m_pimpl_;
};

#endif /* VTKAMRENZOPARTICLESREADER_H_ */
