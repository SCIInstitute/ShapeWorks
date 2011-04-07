//---------------------------------------------------------------------------
// Implementation of 
//
//   Efficiently Combining Positions and Normals for Precise 3D Geometry
//   Nehab, D.; Rusinkiewicz, S.; Davis, J.; Ramamoorthi, R.
//   ACM Transactions on Graphics - SIGGRAPH 2005
//   Los Angeles, California, July 2005, Volume 24, Issue 3, pp. 536-543
// 
// Diego Nehab, Princeton, NJ, 5/2007
//---------------------------------------------------------------------------
#include <cstdio> 
#include <vector> 
#include <cstdarg> 

#include "cholmod.h"
#include "TriMesh.h"
#include "TriMesh_algo.h"

// CHOLMOD error handler
static void handler(int status, char *file, int line, char *message) {
    fprintf(stderr, "\ncholmod error: file: %s line: %d status: %d: %s\n\n",
        file, line, status, message);
    exit(1);
}

static void usage(const char *myname) {
    fprintf(stderr, "Usage: %s infile [options] [outfile]\n", myname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "   -fc file.fc     Range grid camera intrinsics (i.e. fx fy cx cy)\n");
    fprintf(stderr, "   -lambda l       Geometry weight\n");
    fprintf(stderr, "   -blambda b      Boundary geometry weight\n");
    fprintf(stderr, "   -fixnorm s[:n]  Fix normals by smoothing n times with sigma=s*edgelength\n");
    fprintf(stderr, "   -smooth s[:n]   Smooth positions n times with sigma=s*edgelength\n");
    fprintf(stderr, "   -opt            Run one optimization round\n");
    fprintf(stderr, "   -noopt          Do not optimize\n");
    fprintf(stderr, "   -noconf         Remove per-vertex confidence\n");
    fprintf(stderr, "   -nogrid         Unpack range grid to faces\n");
}

static void usage_error(const char *myname, const char *fmt = NULL, ...) {
    if (fmt) {
        fprintf(stderr, "\n");
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, "\n\n");
    }
    usage(myname);
    exit(1);
}

// Derivative types possible at a range grid vertex
typedef enum _e_di {
    NONE, // No derivative possible
    LO,   // Left one-sided
    HI,   // Right one-sided
    TWO,  // Two sided
    ALL   // Rotationally invariant O(h^3) derivative
} e_di;

// Number of normal constraint non-zero coefficients produced 
// by each derivative type
static int nz(e_di d) {
    switch (d) {
        case LO: 
        case HI: 
            return 2;
        case TWO: 
            return 3;
        case ALL:
            return 7;
        default:
            return 0;
    }
}

// Context for each optimization variable 
typedef struct _t_var {
    // Derivative types in x an y directions
    e_di dx, dy;  
    // Index of vertex associated to optimization variable
    int i;
} t_var;
typedef std::vector<t_var> t_map;

// Intrinsics
typedef struct _t_fc {
    float fx, fy, cx, cy;
} t_fc;

// Checks if two verticdes are neighbors 
static int isneighbor(TriMesh *mesh, int i, int j) {
    if (i == j) return 1;
    for (int v = 0; v < mesh->neighbors[i].size(); v++)
        if (mesh->neighbors[i][v] == j) return 1;
    return 0;
}

// Determine types of derivatives possible given a neighborhood
static void dxdy(int *n, e_di *dx, e_di *dy) {
    // HORIZONTAL
    if (n[2*3+0]&&n[2*3+2]&&n[1*3+0]&&n[1*3+2]&&n[0*3+0]&&n[0*3+2]) *dx = ALL;
    else if (n[1*3+0]&&n[1*3+2]) *dx = TWO;
    else if (n[1*3+0]) *dx = LO;
    else if (n[1*3+2]) *dx = HI;
    else *dx = NONE; 
    // VERTICAL
    if (n[0*3+2]&&n[2*3+2]&&n[0*3+1]&&n[2*3+1]&&n[0*3+0]&&n[2*3+0]) *dy = ALL;
    else if (n[0*3+1]&&n[2*3+1]) *dy = TWO;
    else if (n[0*3+1]) *dy = LO;
    else if (n[2*3+1]) *dy = HI;
    else *dy = NONE;
}           

// Set matrix entry in triplet representation
static void set(cholmod_triplet *t, int i, int j, double x) {
    ((int *)(t->i))[(int) t->nnz] = i;
    ((int *)(t->j))[(int) t->nnz] = j;
    ((double*) (t->x))[(int) t->nnz] = x;
    t->nnz++;
}

// Set vector entry in dense representation
static void set(cholmod_dense *v, int i, double x) {
    ((double *)(v->x))[i] = x;
}

// Range grid optimizer
static void optimize_grid(TriMesh *mesh, float lambda, float blambda, 
        const t_fc &fc) {
    mesh->need_neighbors();
    fprintf(stderr, "Range grid optimization... \n");
    int w = mesh->grid_width;
    int h = mesh->grid_height;
    const std::vector<int> &g = mesh->grid;
    t_map map(w*h);
    int nvars = 0, neqns = 0, nnzs = 0;
    float fx = fc.fx, fy = fc.fy, cx = fc.cx, cy = fc.cy;
    fprintf(stderr, "  Analyzing range grid... ");
    // Find out where each vertex goes in the optimization
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            map[i*w+j].i = -1;
            map[i*w+j].dx = NONE;
            map[i*w+j].dy = NONE;
            if (g[i*w+j] >= 0) {
                int n[9];
                for (int u = -1; u <= 1; u++) {
                    for (int v = -1; v <= 1; v++) {
                        n[(u+1)*3+v+1] = 0;
                        if (i+u >= 0 && i+u < h && j+v >= 0 && j+v < w) {
                            if (g[(i+u)*w+j+v] >= 0 &&
                                    isneighbor(mesh,g[i*w+j], g[(i+u)*w+j+v])) {
                                n[(u+1)*3+v+1] = 1;
                            } 
                        }
                    }
                }
                e_di dx, dy;
                dxdy(n, &dx, &dy);
                if (dx != NONE || dy != NONE) {
                    t_var &v = map[i*w+j];
                    v.dx = dx; v.dy = dy; 
                    v.i = nvars++;
                    neqns += (dx != NONE) + (dy != NONE) + 1;
                    nnzs += nz(dx) + nz(dy) + 1;
                }
            }
        }
    }
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Building system (%dx%d)... ", nvars, neqns);
    cholmod_common c;
    cholmod_start(&c);
    c.error_handler = handler;
    cholmod_triplet *Tt = cholmod_allocate_triplet(nvars, neqns, nnzs,
            0, CHOLMOD_REAL, &c);
    cholmod_dense *b = cholmod_allocate_dense(neqns, 1, neqns, 
            CHOLMOD_REAL, &c);
    // Fillout triplet and rhs 
    int row = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            const t_var &v = map[i*w+j];
            if (v.dx == NONE && v.dy == NONE) continue; 
            double x = j - cx;
            double y = i - cy;
            double mu = sqrt(pow(x/fx, 2)+pow(y/fy, 2)+1);
            // Use vertex confidence
            float conf = 0.5;
            if (!mesh->confidences.empty())
                conf = mesh->confidences[g[i*w+j]];
            // Use boundary confidence
            float geom = lambda;
            float mult = 1.0f;
            if (v.dx == NONE || v.dy == NONE)
                geom = blambda;
            else
                mult = 0.5;
            // Add position constraint
            set(Tt, v.i, row, conf*geom*mu);
            // Fill out corresponding rhs entry
            double Z = mesh->vertices[g[i*w+j]][2];
            set(b, row, conf*geom*mu*Z);
            row++;
            vec n = mesh->normals[g[i*w+j]];
            // Horizontal normal constraint
            double xZ = (-n[0]/fx)*(1-geom)*(1-conf)*mult;
            double xdZ = (n[2] - n[1]*y/fy - n[0]*x/fx)*(1-geom)*(1-conf)*mult;
            switch (v.dx) {
                case ALL:
                    set(Tt, v.i, row, xZ);
                    set(Tt, map[(i)*w+j-1].i,   row, -4*xdZ/12);
                    set(Tt, map[(i)*w+j+1].i,   row,  4*xdZ/12);
                    set(Tt, map[(i-1)*w+j-1].i, row,   -xdZ/12);
                    set(Tt, map[(i-1)*w+j+1].i, row,    xdZ/12);
                    set(Tt, map[(i+1)*w+j-1].i, row,   -xdZ/12);
                    set(Tt, map[(i+1)*w+j+1].i, row,    xdZ/12);
                    set(b, row, 0);
                    break;
                case TWO:
                    set(Tt, v.i, row, xZ);
                    set(Tt, map[(i)*w+j-1].i, row,     -xdZ/2);
                    set(Tt, map[(i)*w+j+1].i, row,      xdZ/2);
                    set(b, row, 0);
                    break;
                case HI:
                    set(Tt, v.i, row, xZ-xdZ);
                    set(Tt, map[(i)*w+j+1].i, row,      xdZ);
                    set(b, row, 0);
                    break;
                case LO:
                    set(Tt, v.i, row, xZ+xdZ);
                    set(Tt, map[(i)*w+j-1].i, row,     -xdZ);
                    set(b, row, 0);
                    break;
                case NONE:
                    row--;
                    break;
            }
            row++;
            // Vetical normal constraint
            double yZ = (-n[1]/fy)*(1-geom)*(1-conf)*mult;
            double ydZ = (n[2] - n[1]*y/fy - n[0]*x/fx)*(1-geom)*(1-conf)*mult;
            switch (v.dy) {
                case ALL:
                    set(Tt, v.i, row, yZ);
                    set(Tt, map[(i-1)*w+j].i,   row, -4*ydZ/12);
                    set(Tt, map[(i+1)*w+j].i,   row,  4*ydZ/12);
                    set(Tt, map[(i-1)*w+j-1].i, row,   -ydZ/12);
                    set(Tt, map[(i+1)*w+j-1].i, row,    ydZ/12);
                    set(Tt, map[(i-1)*w+j+1].i, row,   -ydZ/12);
                    set(Tt, map[(i+1)*w+j+1].i, row,    ydZ/12);
                    set(b, row, 0);
                    break;
                case TWO:
                    set(Tt, v.i, row, yZ);
                    set(Tt, map[(i-1)*w+j].i, row,     -ydZ/2);
                    set(Tt, map[(i+1)*w+j].i, row,      ydZ/2);
                    set(b, row, 0);
                    break;
                case HI:
                    set(Tt, v.i, row, yZ-ydZ);
                    set(Tt, map[(i+1)*w+j].i, row,      ydZ);
                    set(b, row, 0);
                    break;
                case LO:
                    set(Tt, v.i, row, yZ+xdZ);
                    set(Tt, map[(i-1)*w+j].i, row,     -ydZ);
                    set(b, row, 0);
                    break;
                case NONE:
                    row--;
            }
            row++;
        }
    }
    // Produce At from Triplet and Atb from At and b.
    cholmod_sparse *At = cholmod_triplet_to_sparse(Tt, nnzs, &c);
    cholmod_free_triplet(&Tt, &c);
    double one[2] = {1, 0}, zero[2] = {0, 0};
    cholmod_dense *Atb = cholmod_zeros(At->nrow, 1, At->xtype, &c);
    cholmod_sdmult(At, 0, one, zero, b, Atb, &c);
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Analyzing matrix... ");
    cholmod_factor *L = cholmod_analyze (At, &c) ;   
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Factoring matrix... ");
    cholmod_factorize (At, L, &c);
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Back substituting... ");
    cholmod_dense *z = cholmod_solve(CHOLMOD_A, L, Atb, &c);
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Updating range grid... ");
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int v = map[i*w+j].i;
            if (v >= 0) {
                double oldZ = mesh->vertices[g[i*w+j]][2];
                double Z = ((double *) z->x)[v];
                float s = (float) (Z/oldZ);
                mesh->vertices[g[i*w+j]] = s*mesh->vertices[g[i*w+j]];
            }
        }
    }
    // Cleanup
    cholmod_free_dense(&b, &c);
    cholmod_free_sparse(&At, &c);
    cholmod_free_dense(&Atb, &c);
    cholmod_free_factor(&L, &c);
    cholmod_free_dense(&z, &c);
    cholmod_finish(&c);
    fprintf(stderr, "Done.\n");
}

// Get the edge opposite to a vertex
void opposite_edge(const TriMesh::Face &f, int v, int *a, int *b) {
    if (f[0] ==  v) {
        *a = f[1]; *b = f[2];
    } else if (f[1] ==  v) {
        *a = f[0]; *b = f[2];
    } else {
        *a = f[0]; *b = f[1];
    }
}

// Arbitrary mesh optimizer
static void optimize_mesh(TriMesh *mesh, float lambda, float blambda) {
    mesh->need_adjacentfaces();
    mesh->need_neighbors();
    fprintf(stderr, "Arbitrary mesh optimization... \n");
    // Compute size of the optimization problem
    int nvars = mesh->vertices.size(); 
    // Count normal constraints
    int neqns = 0;
    for (int i = 0; i < nvars; i++)
        neqns += mesh->adjacentfaces[i].size(); 
    // One coefficient per position constraint, two per normal constraint
    int nnzs = nvars + neqns*2;
    // Add position constraints
    neqns += nvars;
    fprintf(stderr, "  Building system (%dx%d)... ", nvars, neqns);
    cholmod_common c;
    cholmod_start(&c);
    c.error_handler = handler;
    cholmod_triplet *Tt = cholmod_allocate_triplet(nvars, neqns, nnzs,
            0, CHOLMOD_REAL, &c);
    cholmod_dense *b = cholmod_allocate_dense(neqns, 1, neqns, 
            CHOLMOD_REAL, &c);
    // Position constraints
    for (int v = 0; v < nvars; v++) {
        float conf = 0.5;
        if (!mesh->confidences.empty())
            conf = mesh->confidences[v];
        float geom = lambda;
        if (mesh->is_bdy(v))
            geom = blambda;
        set(Tt, v, v, conf*geom);
    }
    // Normal constraints
    int row = nvars;
    for (int v = 0; v < nvars; v++) {
        const vector<int> &af = mesh->adjacentfaces[v];
        int nf = af.size();
        float conf = 0.5;
        if (!mesh->confidences.empty())
            conf = mesh->confidences[v];
        float geom = lambda;
        if (mesh->is_bdy(v))
            geom = blambda;
        float weight = (1-geom)*(1-conf)/sqrt((float)nf);
        for (int f = 0; f < nf; f++) {
            int u, w;
            opposite_edge(mesh->faces[af[f]], v, &u, &w);
            vec dwu = mesh->vertices[w] - mesh->vertices[u];
            set(b, row, weight*(mesh->normals[v] DOT dwu));
            float vu = weight*(mesh->normals[v] DOT mesh->normals[u]);
            float vw = -weight*(mesh->normals[v] DOT mesh->normals[w]);
            set(Tt, u, row, vu);
            set(Tt, w, row, vw);
            row++;
        }
    }
    cholmod_sparse *At = cholmod_triplet_to_sparse(Tt, nnzs, &c);
    cholmod_free_triplet(&Tt, &c);
    double one[2] = {1, 0}, zero[2] = {0, 0};
    cholmod_dense *Atb = cholmod_zeros(At->nrow, 1, At->xtype, &c);
    cholmod_sdmult(At, 0, one, zero, b, Atb, &c);
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Analyzing matrix... ");
    cholmod_factor *L = cholmod_analyze (At, &c) ;   
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Factoring matrix... ");
    cholmod_factorize (At, L, &c);
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Back substituting... ");
    cholmod_dense *d = cholmod_solve(CHOLMOD_A, L, Atb, &c);
    fprintf(stderr, "Done.\n");
    fprintf(stderr, "  Updating mesh... ");
    for (int v = 0; v < nvars; v++) {
        double delta = ((double *) d->x)[v];
        mesh->vertices[v] += ((float) delta)*mesh->normals[v];
    }
    // Cleanup
    cholmod_free_dense(&b, &c);
    cholmod_free_sparse(&At, &c);
    cholmod_free_dense(&Atb, &c);
    cholmod_free_factor(&L, &c);
    cholmod_free_dense(&d, &c);
    cholmod_finish(&c);
    fprintf(stderr, "Done.\n");
} 

// Rodrigues formula for rotation
static vec rotate(vec v, vec u, float cs, float s) {
    return cs * v + s * (u CROSS v) + (1.0f - cs) * (u DOT v) * u;
}

// Replace low frequency in normal field with that from geometry
static void fix_normals(TriMesh *themesh, float s, int n) {
    fprintf(stderr, "Fixing normals (%g:%d)... \n", s, n);
    float amount = s * themesh->feature_size();
    // Save measured normals
    std::vector<vec> measured = themesh->normals;
    // Smooth measured normals and save
    for (int i = 0; i < n; i++)
        diffuse_normals(themesh, amount);
    std::vector<vec> smeasured = themesh->normals;
    // Compute and smooth normals from triangulation
    themesh->normals.clear();
    themesh->need_normals();
    for (int i = 0; i < n; i++)
        diffuse_normals(themesh, amount);
    for (int i = 0; i < themesh->normals.size(); i++) {
        vec st = themesh->normals[i];
        vec sm = smeasured[i];
        vec m = measured[i];
        // Rotation axis and angles
        vec u = sm CROSS m; 
        float l = len(u);
        if (l > 0.01) { // Make sure axis is not degenerate
            u = u/l;
            float cs = sm DOT m;
            float s = sqrt(1.0f-cs*cs);
            themesh->normals[i] = rotate(st, u, cs, s);
        } 
    }
    fprintf(stderr, "Done. \n");
}

static void smooth(TriMesh *themesh, float s, int n) {
    fprintf(stderr, "Smoothing positions (%g:%d)... \n", s, n);
    float amount = s * themesh->feature_size();
    std::vector<vec> backup = themesh->normals;
    for (int i = 0; i < n; i++) {
        smooth_mesh(themesh, amount);
        themesh->pointareas.clear();
    }
    themesh->normals = backup;
    fprintf(stderr, "Done.\n");
}

static int issmootharg(const char *c, float *s, int *n) {
    int e = 0;
    *s = 2.f; *n = 1;
    if (sscanf(c, "%f:%d%n", s, n, &e) == 2 && c[e] == '\0') return 1;
    else if (sscanf(c, "%f%n", s, &e) == 1 && c[e] == '\0') return 1;
    else return 0;
}

static int isanumber(const char *c, float *s) {
    int e = 0;
    *s = 0.f;
    if (sscanf(c, "%f%n", s, &e) == 1 && c[e] == '\0') return 1;
    else return 0;
}

int main(int argc,char *argv[]) {
    if (argc < 3) 
        usage_error(argv[0]);
    const char *filename = argv[1];
    TriMesh *themesh = TriMesh::read(filename);
    if (!themesh) 
        usage_error(argv[0]);
    if (themesh->vertices.size() != themesh->normals.size())
        usage_error(argv[0], "need vertex normals");
    if (!themesh->confidences.empty() && 
            themesh->vertices.size() != themesh->confidences.size()) 
        usage_error(argv[0], "not enough vertex confidence values");
    bool has_intrinsics = false, no_optimize = false, has_blambda = false;
    t_fc fc; 
    float lambda = 0.1, blambda = 0.1;
    bool optimized = false;
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "-noopt")) {
            no_optimize = true;
        } else if (!strcmp(argv[i], "-noconf")) {
            themesh->confidences.clear();
        } else if (!strcmp(argv[i], "-nogrid")) {
            themesh->need_faces();
            themesh->grid.clear();
        } else if (!strcmp(argv[i], "-fixnorm")) {
            i++;
            float s = 1;
            int n = 1;
            if (!(i < argc && issmootharg(argv[i], &s, &n)))
                usage_error(argv[0], "-fixnorm requires a parameter "
                    "in the form: s[:n] (i.e. %%f[:%%d])");
            fix_normals(themesh, s, n);

        } else if (!strcmp(argv[i], "-smooth")) {
            i++;
            float s = 1;
            int n = 1;
            if (!(i < argc && issmootharg(argv[i], &s, &n)))
                usage_error(argv[0], "-fixnorm requires a parameter "
                    "in the form: s[:n] (i.e. %%f[:%%d])");
            smooth(themesh, s, n);
        } else if (!strcmp(argv[i], "-fc")) {
            i++;
            if (!(i < argc))
                usage_error(argv[0], "-fc requires one filename argument");
            FILE *fp = fopen(argv[i], "r");
            if (!fp) usage_error(argv[0], "unable to open -fc '%s'", argv[i]);
            if (fscanf(fp,"%f %f %f %f",&fc.fx, &fc.fy, &fc.cx, &fc.cy) != 4) {
                fclose(fp);
                usage_error(argv[0], "invalid fc file");
            }
            fclose(fp);
            has_intrinsics = true;
        } else if (!strcmp(argv[i], "-lambda")) {
            i++;
            if (!(i < argc && isanumber(argv[i], &lambda)))
                usage_error(argv[0], "-lambda requires one float parameter");
            else if (lambda < 0 || lambda > 1)
                usage_error(argv[0], "lambda value must be within [0,1]");
        } else if (!strcmp(argv[i], "-blambda")) {
            i++;
            if (!(i < argc && isanumber(argv[i], &blambda)))
                usage_error(argv[0], "-blambda requires one float parameter");
            else if (blambda < 0 || blambda > 1)
                usage_error(argv[0], "blambda value must be within [0,1]");
            has_blambda = true;
        } else if (!strcmp(argv[i], "-opt")) {
            if (!has_blambda) 
                blambda = lambda;
            optimized = true;
            if (has_intrinsics) {
                if (themesh->grid.empty())
                    usage_error(argv[0], "fc requires a range grid");
                optimize_grid(themesh, lambda, blambda, fc);
            } else optimize_mesh(themesh, lambda, blambda);
        } else if (i == argc - 1 &&
               (argv[i][0] != '-' || argv[i][1] == '\0')) {
            if (!has_blambda) 
                blambda = lambda;
            if (!no_optimize && !optimized) {
                if (has_intrinsics) {
                    if (themesh->grid.empty())
                        usage_error(argv[0], "fc requires a range grid");
                    optimize_grid(themesh, lambda, blambda, fc);
                } else optimize_mesh(themesh, lambda, blambda);
            }
            optimized = true;
            themesh->write(argv[i]);
        } else
            usage_error(argv[0], "unrecognized option [%s]", argv[i]);
    }
    return 0;
}
