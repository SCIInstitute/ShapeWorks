/*
Szymon Rusinkiewicz
Princeton University

mesh_cc.cc
Determine the connected components of a mesh, and possibly write
out only selected components of the object.

Does the same thing as "plycomps", part of the plytools package by Greg Turk.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "TriMesh.h"
#include "TriMesh_algo.h"
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;


#define BIGNUM 2147483647
#define NO_COMP -1
#define FOR_EACH_ADJACENT_FACE(mesh,v,f) \
	for (int f_ind = 0, f = mesh->adjacentfaces[v][0]; \
	     (f_ind < mesh->adjacentfaces[v].size()) && \
	     ((f = mesh->adjacentfaces[v][f_ind]) || 1); \
	     f_ind++)


bool conn_vert = false;	// Consider connectivity along vertices? (else edges)
int nprint = 20;	// Default # of CCs to print


// Are two faces connected along an edge (or vertex)?
bool connected(const TriMesh *in, int f1, int f2)
{
	int f10=in->faces[f1][0], f11=in->faces[f1][1], f12=in->faces[f1][2];
	int f20=in->faces[f2][0], f21=in->faces[f2][1], f22=in->faces[f2][2];

	if (conn_vert)
		return f10 == f20 || f10 == f21 || f10 == f22 ||
		       f11 == f20 || f11 == f21 || f11 == f22 ||
		       f12 == f20 || f12 == f21 || f12 == f22;
	else
		return (f10 == f20 && (f11 == f22 || f12 == f21)) ||
		       (f10 == f21 && (f11 == f20 || f12 == f22)) ||
		       (f10 == f22 && (f11 == f21 || f12 == f20)) ||
		       (f11 == f20 && f12 == f22) ||
		       (f11 == f21 && f12 == f20) ||
		       (f11 == f22 && f12 == f21);
}


// Helper function for find_comps, below.  Finds and marks all the faces
// connected to f.
void find_connected(const TriMesh *in,
		    vector<int> &comps, vector<int> &compsizes,
		    int f, int whichcomponent)
{
	stack<int> s;
	s.push(f);
	while (!s.empty()) {
		int currface = s.top();
		s.pop();
		for (int i = 0; i < 3; i++) {
			int vert = in->faces[currface][i];
			FOR_EACH_ADJACENT_FACE(in, vert, adjface) {
				if (comps[adjface] != NO_COMP ||
				    !connected(in, adjface, currface))
					continue;
				comps[adjface] = whichcomponent;
				compsizes[whichcomponent]++;
				s.push(adjface);
			}
		}
	}
}


// Find the connected components of TriMesh "in".
// Outputs:
//  comps is a vector that gives a mapping from each face to its
//   associated connected component.
//  compsizes holds the size of each connected component
void find_comps(TriMesh *in, vector<int> &comps, vector<int> &compsizes)
{
	if (in->vertices.empty())
		return;
	if (in->faces.empty())
		return;
	in->need_adjacentfaces();

	int nf = in->faces.size();
	comps.clear();
	comps.reserve(nf);
	comps.resize(nf, NO_COMP);
	compsizes.clear();

	for (int i = 0; i < nf; i++) {
		if (comps[i] != NO_COMP)
			continue;
		int comp = compsizes.size();
		comps[i] = comp;
		compsizes.push_back(1);
		find_connected(in, comps, compsizes, i, comp);
	}
}


// Helper class for comparing two integers by finding the elements at those
// indices within some array and comparing them
template <class Array>
class CompareArrayElements {
private:
	const Array &a;
public:
	CompareArrayElements(const Array &_a) : a(_a)
		{}
	bool operator () (int i1, int i2) const
	{
		return (a[i1] > a[i2]);
	}
};


// Sorts the connected components from largest to smallest.  Renumbers the
// elements of compsizes to reflect this new numbering.
void sort_comps(vector<int> &comps, vector<int> &compsizes)
{
	if (compsizes.size() < 1)
		return;

	int i;
	vector<int> comp_pointers(compsizes.size());
	for (i = 0; i < comp_pointers.size(); i++)
		comp_pointers[i] = i;

	sort(comp_pointers.begin(), comp_pointers.end(),
	     CompareArrayElements< vector<int> >(compsizes));

	vector<int> remap_table(comp_pointers.size());
	for (i = 0; i < comp_pointers.size(); i++)
		remap_table[comp_pointers[i]] = i;
	for (i = 0; i < comps.size(); i++)
		comps[i] = remap_table[comps[i]];

	vector<int> newcompsizes(compsizes.size());
	for (i = 0; i < compsizes.size(); i++)
		newcompsizes[i] = compsizes[comp_pointers[i]];
	compsizes = newcompsizes;
}


// Print out the connected components that are bigger than morethan and
// smaller than lessthan.  The largest min(nprint, total) components are
// printed out, unless morethan == 0 and lessthan != BIGNUM, in which case
// the smallest min(nprint, total) components are printed
void print_comps(const vector<int> &comps, const vector<int> &compsizes,
		 int morethan, int lessthan, int total)
{
	printf("%lu connected components total.\n",
		(unsigned long) compsizes.size());

	if (compsizes.size() < 1 || total < 1)
		return;
	int numtoprint = min(min(nprint, total), (int)compsizes.size());

	if (morethan == 0 && lessthan != BIGNUM) {
		// Print numtoprint smallest components
		for (int i = 0; i < numtoprint; i++)
			printf(" Component #%d - %d faces\n", i+1, compsizes[compsizes.size()-1-i]);
	} else {
		// Print numtoprint largest components
		for (int i = 0; i < numtoprint; i++)
			printf(" Component #%d - %d faces\n", i+1, compsizes[i]);
	}
	if (numtoprint != compsizes.size())
		printf(" ...\n");
}


// Select a particular connected component, and delete all other vertices from
// the mesh.
void select_comp(TriMesh *in, const vector<int> &comps, int whichcc)
{
	int numfaces = in->faces.size();
	vector<bool> toremove(numfaces, false);
	for (int i = 0; i < in->faces.size(); i++) {
		if (comps[i] != whichcc)
			toremove[i] = true;
	}

	remove_faces(in, toremove);
	remove_unused_vertices(in);
}


// Select the requested connected components (as given by the "morethan",
// "lessthan", and "total" parameters), and delete all other vertices from
// the mesh.
void select_comps_by_size(TriMesh *in, const vector<int> &comps,
			  const vector<int> &compsizes,
			  int morethan, int lessthan, int total)
{
	if (compsizes.size() < 1 || total < 1)	
		return;
	if (compsizes.size() == 1 &&
	    (compsizes[0] > lessthan || compsizes[0] < morethan)) {
		in->faces.clear();
		in->vertices.clear();
		return;
	}

	int keep_first = 0;
	while (keep_first < compsizes.size() && compsizes[keep_first] > lessthan)
		keep_first++;
	int keep_last = compsizes.size()-1;
	while (keep_last > -1 && compsizes[keep_last] < morethan)
		keep_last--;
	if (keep_last-keep_first+1 > total) {
		if (morethan == 0 && lessthan != BIGNUM) {
			// Keep the smallest components
			keep_first = keep_last+1-total;
		} else {
			// Keep the largest components
			keep_last = keep_first+total-1;
		}
	}

	int numfaces = in->faces.size();
	vector<bool> toremove(numfaces, false);
	for (int i = 0; i < numfaces; i++) {
		if (comps[i] < keep_first || comps[i] > keep_last)
			toremove[i] = true;
	}

	remove_faces(in, toremove);
	remove_unused_vertices(in);
}


void usage(const char *myname)
{
	fprintf(stderr, "Usage: %s [options] in.ply [out.ply]\n", myname);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "	-v		Base connectivity on vertices, not edges\n");
	fprintf(stderr, "    	-a		Print sizes of ALL the components\n");
	fprintf(stderr, "    	-s		Split components into separate files\n");
	fprintf(stderr, "    	-m n		Select components with >= n faces\n");
	fprintf(stderr, "    	-l n		Select components with <= n faces\n");
	fprintf(stderr, "    	-t n		Select the largest n components\n");
	exit(1);
}


int main(int argc, char *argv[])
{
	// Parse command line
	int morethan = 0;
	int lessthan = BIGNUM;
	int total = BIGNUM;
	bool splitcc = false;
	const char *infilename=NULL, *outfilename=NULL;

	int c;
	while ((c = getopt(argc, argv, "hvasm:l:t:")) != EOF) {
		switch (c) {
			case 'v': conn_vert = true; break;
			case 'a': nprint = BIGNUM; break;
			case 's': splitcc = true; break;
			case 'm': morethan = atoi(optarg); break;
			case 'l': lessthan = atoi(optarg); break;
			case 't': total = atoi(optarg); break;
			default: usage(argv[0]);
		}
	}
	if (argc - optind < 1)
		usage(argv[0]);
	infilename = argv[optind];
	if (argc - optind >= 2)
		outfilename = argv[optind+1];

	// Read input file
	TriMesh *in = TriMesh::read(infilename);
	if (!in) {
		fprintf(stderr, "Couldn't read input %s\n", infilename);
		exit(1);
	}
	bool had_tstrips = !in->tstrips.empty();
	in->need_faces();
	in->tstrips.clear();

	// Find connected components
	vector<int> comps;
	vector<int> compsizes;
	find_comps(in, comps, compsizes);
	sort_comps(comps, compsizes);

	// Print out the top few components
	print_comps(comps, compsizes, morethan, lessthan, total);

	// Exit here if just printing things out, and not saving anything
	if (!outfilename)
		exit(0);

	// Get rid of the junk we don't want...
	select_comps_by_size(in, comps, compsizes, morethan, lessthan, total);

	if (splitcc) {
		// Hack...
		find_comps(in, comps, compsizes);
		sort_comps(comps, compsizes);

		// Split into separate files, if requested
		for (int i = 0; i < compsizes.size(); i++) {
			TriMesh *tmp = new TriMesh(*in);
			select_comp(tmp, comps, i);
			if (had_tstrips)
				tmp->need_tstrips();
			char filename[1024];
			sprintf(filename, "cc%d-%s", i+1, outfilename);
			tmp->write(filename);
			delete tmp;
		}
	} else {
		// Write the requested components to a file
		if (had_tstrips)
			in->need_tstrips();
		in->write(outfilename);
	}
}
