/*
Szymon Rusinkiewicz
Princeton University

Benedict Brown
Katholieke Universiteit Leuven

mesh_info.cc
Query various information about ply files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TriMesh.h"
#include <vector>
using namespace std;


void usage(const char *myname)
{
	fprintf(stderr, "Usage: %s infile desired_info\n", myname);
	fprintf(stderr, "Info:\n");
	fprintf(stderr, "	faces		Number of faces\n");
	fprintf(stderr, "	vertices	Number of vertices\n");
	fprintf(stderr, "	bbox		Bounding box\n");
	fprintf(stderr, "	area		Surface area\n");
	exit(1);
}


int main(int argc, char *argv[])
{
	// Don't clutter the output
	TriMesh::set_verbose(0);

	// Parse command line and read mesh
	if (argc != 3)
		usage(argv[0]);

	TriMesh *mesh = TriMesh::read(argv[1]);
	if (!mesh)
		usage(argv[0]);

	// Display information
	if (!strncmp(argv[2], "faces", 1)) {
		mesh->need_faces();
		printf("%d\n", (int) mesh->faces.size());
	} else if (!strncmp(argv[2], "vertices", 1)) {
		printf("%d\n", (int) mesh->vertices.size());
	} else if (!strncmp(argv[2], "bbox", 1)) {
		mesh->need_bbox();
		printf("%g %g %g\n%g %g %g\n",
			mesh->bbox.min[0], mesh->bbox.min[1], mesh->bbox.min[2],
			mesh->bbox.max[0], mesh->bbox.max[1], mesh->bbox.max[2]);
	} else if (!strncmp(argv[2], "area", 1)) {
		mesh->need_faces();
		int nf = mesh->faces.size();
		float a = 0.0f;
		for (int i = 0; i < nf; i++)
			a += len(mesh->trinorm(i));
		printf("%g\n", a);
	} else {
		usage(argv[0]);
	}

	return 0;
}

