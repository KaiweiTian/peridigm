/*
 * utPimpMatrix_np2_11x1x1.cxx
 *
 *  Created on: Mar 16, 2010
 *      Author: jamitch
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include <tr1/memory>
#include "PdZoltan.h"
#include "PdQuickGrid.h"
#include "PdQuickGridParallel.h"
#include "PdutMpiFixture.h"
#include <iostream>

using namespace PdQuickGrid;
using namespace Pdut;
using std::tr1::shared_ptr;
using namespace boost::unit_test;
using std::cout;

static int myRank;
static int numProcs;
const int nx = 1;
const int ny = 1;
const int nz = 11;
const double xStart =  0.0;
const double xLength = 1.0;
const double yStart =  0.0;
const double yLength = 1.0;
const double zStart =  0.0;
const double zLength = 1.0;
const PdQPointSet1d xSpec(nx,xStart,xLength);
const PdQPointSet1d ySpec(ny,yStart,yLength);
const PdQPointSet1d zSpec(nz,zStart,zLength);
const double crossSectionalArea = xSpec.getCellSize()*ySpec.getCellSize();
const double _cellVolume = crossSectionalArea*zSpec.getCellSize();
const int numCells = nx*ny*nz;
static PdGridData gridData;


void p0()
{
	BOOST_CHECK(0 == myRank);
	/*
	 * problem dimension is 3
	 */
	BOOST_CHECK(3 == gridData.dimension);

	/*
	 * Total number of cells in test
	 */
	BOOST_CHECK(nx*ny*nz == gridData.globalNumPoints);

	/*
	 * Number of cells on this processor
	 */
	int myNumPoints = gridData.numPoints;


}

void p1()
{
	BOOST_CHECK(1 == myRank);
	/*
	 * problem dimension is 3
	 */
	BOOST_CHECK(3 == gridData.dimension);

	/*
	 * Total number of cells in test
	 */
	BOOST_CHECK(nx*ny*nz == gridData.globalNumPoints);

	/*
	 * Number of cells on this processor
	 */
	int myNumPoints = gridData.numPoints;

}

bool init_unit_test_suite()
{
	// Add a suite for each processor in the test
	bool success=true;
	if(0 == myRank){
		test_suite* proc = BOOST_TEST_SUITE( "utPimpMatrix_np2_11x1x1p0" );
		proc->add(BOOST_TEST_CASE( &p0 ));
		framework::master_test_suite().add( proc );
		return success;
	}
	if(1 == myRank){
		test_suite* proc = BOOST_TEST_SUITE( "utPimpMatrix_np2_11x1x1p1" );
		proc->add(BOOST_TEST_CASE( &p1 ));
		framework::master_test_suite().add( proc );
		return success;
	}
	return success;
}


bool init_unit_test()
{
	init_unit_test_suite();
	return true;
}

int main
(
		int argc,
		char* argv[]
)
{

	// Initialize MPI and timer
	PdutMpiFixture myMpi = PdutMpiFixture(argc,argv);

	// These are static (file scope) variables
	myRank = myMpi.rank;
	numProcs = myMpi.numProcs;
	/**
	 * This test only make sense for numProcs == 2
	 */
	if(2 != numProcs){
		std::cerr << "Unit test runtime ERROR: utPimpMatrix_np2_11x1x1 only makes sense on 2 processors" << std::endl;
		std::cerr << "\t Re-run unit test $mpiexec -np 2 ./utPimpMatrix_np2_11x1x1" << std::endl;
		myMpi.PdutMpiFixture::~PdutMpiFixture();
		std::exit(-1);
	}


	double horizon = .3;
	PdQuickGrid::TensorProduct3DMeshGenerator cellPerProcIter(numProcs,horizon,xSpec,ySpec,zSpec);
	gridData =  PdQuickGrid::getDiscretization(myRank, cellPerProcIter);

	// Initialize UTF
	return unit_test_main( init_unit_test, argc, argv );
}