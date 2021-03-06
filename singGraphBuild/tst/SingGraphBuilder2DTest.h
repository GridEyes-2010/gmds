/*----------------------------------------------------------------------------*/
#include <gtest/gtest.h>
#include <unit_test_config.h>
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
#include <gmds/ig/MeshDoctor.h>
#include <gmds/io/IGMeshIOService.h>
#include <gmds/io/VTKReader.h>
#include <gmds/io/VTKWriter.h>
#include <gmds/igalgo/BoundaryOperator.h>
/*----------------------------------------------------------------------------*/
#include <gmds/singGraphBuild/SingularityGraphBuilder2D.h>
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <vector>
#include <chrono>
#include <unistd.h>
typedef std::chrono::high_resolution_clock Clock;
/*----------------------------------------------------------------------------*/
using namespace gmds;
/*----------------------------------------------------------------------------*/
TEST(SingGraphBuilder2DTest, halfDisk)
{
	MeshModel model(DIM3 | F | E | N | F2N | E2N | F2E | E2F | N2F | N2E);
	Mesh mesh(model);
  
	//==================================================================
	// MESH READING
	//==================================================================
    std::string dir(TEST_SAMPLES_DIR);
    std::string vtk_file = dir+"/half_disk.vtk";

	gmds::IGMeshIOService ioService(&mesh);
	gmds::VTKReader vtkReader(&ioService);
	vtkReader.setCellOptions(gmds::N|gmds::F);
	vtkReader.setDataOptions(gmds::N|gmds::F);
	vtkReader.read(vtk_file);   

	//==================================================================
	// MESH TOPOLOGY PREPARATION
	//==================================================================
	MeshDoctor doctor(&mesh);
	doctor.buildEdgesAndX2E();
	doctor.updateUpwardConnectivity();
	doctor.orient2DFaces();

	EXPECT_TRUE(mesh.getNbFaces()!=0);
	EXPECT_TRUE(mesh.getNbEdges()!=0);
	EXPECT_TRUE(mesh.getNbNodes()!=0); 

	//==================================================================
	// INIT MARKs FOR BOUNDARY NODES
	//==================================================================	
	int edge_curve_mark = mesh.newMark<gmds::Edge>();
	int node_curve_mark = mesh.newMark<gmds::Node>();
	int node_point_mark = mesh.newMark<gmds::Node>();

	BoundaryOperator boundaryOp(&mesh); 
	boundaryOp.markCellsOnCurves(edge_curve_mark, node_curve_mark);
	boundaryOp.markNodesOnPoint(edge_curve_mark, node_curve_mark, node_point_mark);
  
	//==================================================================
	// CROSS FIELD EXTRACTION FROM THE IMPORTED FILE
	//==================================================================	
	Variable<math::Vector3d>* field_X = mesh.getVariable<math::Vector3d,gmds::GMDS_NODE>("cross_X");
	Variable<math::Cross2D>* field = mesh.newVariable<math::Cross2D,gmds::GMDS_NODE>("c");

	math::Vector3d OX(1,0,0);
	for(auto n_id: mesh.nodes()){
		Node n = mesh.get<Node>(n_id);    
		math::Vector3d vx = (*field_X)[n.id()];
		(*field)[n.id()]= math::Cross2D(4*vx.angle(OX));    
	}

	//==================================================================
	// SINGULARITY GRAPH EXTRACTION
	//==================================================================
	const bool BuildGeomSing = true;
	SingularityGraphBuilder2D algo(&mesh, field, BuildGeomSing);
	algo.setDebugPrefix("HalfDisk");

	algo.initMarks(node_point_mark, node_curve_mark, edge_curve_mark);
	/* possible approaches: original, simultaneousStartHeun, simultaneousStartRK4, shortestPaths, testPrescribedSing
	*/
	//User must choose the number of control Points for the Bezier curve computation (last step)
	//WARNING if the number chosen exceeds the original number of points for the curve, the latter will be chosen
	unsigned int number_of_control_points = 8; 
	auto t0 = Clock::now();
	algo.execute(SingularityGraphBuilder2D::shortestPaths, number_of_control_points);
	auto t1 = Clock::now();
	cout << "total execution time " 
		<< std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
		<< " milliseconds" << std::endl;
	//==================================================================
	// MARKS CLEANING
	//==================================================================	
	mesh.unmarkAll<Node>(node_curve_mark);
	mesh.unmarkAll<Node>(node_point_mark);
	mesh.unmarkAll<Edge>(edge_curve_mark);
	mesh.freeMark<Node>(node_curve_mark);
	mesh.freeMark<Node>(node_point_mark);
	mesh.freeMark<Edge>(edge_curve_mark);

	const::SingularityGraph& g = algo.graph();
    ASSERT_EQ(g.getNbPoints(),8);
    ASSERT_EQ(g.getNbSurfacePoints(),2);
    ASSERT_EQ(g.getNbCurvePoints(),4);
    ASSERT_EQ(g.getNbVertexPoints(),2);
    ASSERT_EQ(g.getNbLines(),11);
    ASSERT_EQ(g.getNbSurfaceLines(),5);
    ASSERT_EQ(g.getNbCurveLines(),6);
    ASSERT_EQ(g.getNbSurfacePatches(),4);
}
