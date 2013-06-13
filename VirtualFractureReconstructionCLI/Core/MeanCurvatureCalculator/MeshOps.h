/*=========================================================================

  Program:   SparseFieldLevelSetContour
  Module:    $HeadURL$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef MESHOPS
#define MESHOPS

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <valarray>
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkDebugLeaks.h"
#include "vtkTestUtilities.h"
#include "vtkFieldData.h"
#include "Utils.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataMapper.h"
#include <vector>
#include <algorithm>

struct AdjData {
int myIdx;
vector<int> myNeighbs;
};


struct MeshDataCustom {
valarray<double> MeanCurv;
valarray<double> dkdx;
valarray<double> dkdy;
valarray<double> dkdz;
valarray<double> nx;
valarray<double> ny;
valarray<double> nz;

// derivatives w.r.t. intrinsic basis on manifold
valarray<double> dkde1;
valarray<double> dkde2;
valarray<double> dkmag;

// extra color maps to use for various purposes
valarray<double> cmap0;
valarray<double> cmap1;
valarray<double> cmap2;
valarray<double> cmap3;

vtkPolyData* polydata;
vtkPolyDataMapper *mapper;
vector<AdjData> adj; // neighbors N steps into tree
vector<AdjData> adjimm; // immediate neighbors; not yet computed

valarray<double> kappa;

int adj_levels;
int smoothH_its;
int showLS;
int rightHandMesh;
};

void ComputeCurvatureData( MeshDataCustom* meshdata ); // main function

// d'oh, make MeshData a class and these guys as methods

// subroutines:

void ComputeAdjacency( MeshDataCustom* meshdata );

void ComputeNormals( MeshDataCustom* meshdata );

void ComputeCurvature( MeshDataCustom* meshdata );

int CountVertsOnMesh( vtkPolyData* poly );

void SmoothCurvature( MeshDataCustom* meshdata );

void ComputeGradCurvatureTangentPlane( MeshDataCustom* meshdata );

vector<int> InitPath( MeshDataCustom* meshdata, vector<int> pts);

#endif
