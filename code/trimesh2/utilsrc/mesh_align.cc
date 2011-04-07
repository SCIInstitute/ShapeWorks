/*
Szymon Rusinkiewicz
Princeton University

mesh_align.cc
Minimal interface to ICP: register two meshes given an initial guess
for their alignment.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "TriMesh.h"
#include "ICP.h"
#include <string>
using std::string;


void usage(const char *myname)
{
	fprintf(stderr, "Usage: %s [-options] mesh1.ply mesh2.ply\n", myname);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "	-a		Align using affine xform\n");
	fprintf(stderr, "	-r		Align using rigid-body transform (default)\n");
	fprintf(stderr, "	-s		Align using rigid + isotropic scale\n");
	fprintf(stderr, "	-v		Verbose\n");
	fprintf(stderr, "Reads transforms in mesh1.xf and mesh2.xf, updates the latter\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int verbose = 1;
	bool do_scale = false;
	bool do_affine = false;

	int c;
	while ((c = getopt(argc, argv, "harsv")) != EOF) {
		switch (c) {
			case 'a': do_affine = true; do_scale = false; break;
			case 'r': do_affine = do_scale = false; break;
			case 's': do_scale = true; do_affine = false; break;
			case 'v': verbose = 2; break;
			default: usage(argv[0]);
		}
	}
	if (argc - optind < 2)
		usage(argv[0]);
	const char *filename1 = argv[optind], *filename2 = argv[optind+1];

	TriMesh *mesh1 = TriMesh::read(filename1);
	if (!mesh1)
		usage(argv[0]);
	TriMesh *mesh2 = TriMesh::read(filename2);
	if (!mesh2)
		usage(argv[0]);

	xform xf1;
	string xffilename1 = xfname(filename1);
	xf1.read(xffilename1);

	xform xf2;
	string xffilename2 = xfname(filename2);
	xf2.read(xffilename2);

	float err = ICP(mesh1, mesh2, xf1, xf2, verbose, do_scale, do_affine);
	if (err >= 0.0f)
		err = ICP(mesh1, mesh2, xf1, xf2, verbose, do_scale, do_affine);

	if (err < 0.0f) {
		fprintf(stderr, "ICP failed\n");
	} else {
		fprintf(stderr, "ICP succeeded - distance = %f\n", err);
		xf2.write(xffilename2);
	}
}

