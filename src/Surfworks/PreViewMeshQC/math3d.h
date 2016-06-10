#pragma once
#include <math.h>
#include <assert.h>
#include <vector>
using namespace std;
#define PI 3.1415926
#define RAD2DEG (180.0/PI)
#define DEG2RAD (PI/180.0)

//-----------------------------------------------------------------------------
// class vec2d defines a 2D vector
class vec2d
{
public:
	vec2d() { x = y = 0.0; }
	vec2d(double X, double Y) { x = X; y = Y; }

	vec2d operator - () { return vec2d(-x, -y); }

	vec2d operator - (const vec2d& r) { return vec2d(x - r.x, y - r.y); }
	vec2d operator + (const vec2d& r) { return vec2d(x + r.x, y + r.y); }
	vec2d operator * (double g) { return vec2d(x*g, y*g); }

	double operator * (const vec2d& r) { return (x*r.x + y*r.y); }

	double norm() { return sqrt(x*x + y*y); }
	double unit() { double R = sqrt(x*x + y*y); if (R != 0) {x /= R; y/= R; }; return R; }

	bool operator == (const vec2d& r) const { return (x==r.x)&&(y==r.y); }

public:
	double	x, y;
};

//-----------------------------------------------------------------------------
// class vec3d defines a 3D vector
//
class vec3d
{
public:
	vec3d() { x = y = z = 0; }
	vec3d(double rx, double ry, double rz) { x = rx; y = ry; z = rz; }
	vec3d(const vec2d& r) { x = r.x; y = r.y; z = 0; }

	vec3d operator + (const vec3d& v) const { return vec3d( x + v.x, y + v.y, z + v.z); }
	vec3d operator - (const vec3d& v) const { return vec3d( x - v.x, y - v.y, z - v.z); }
	vec3d operator ^ (const vec3d& v) const
	{ 
		return vec3d( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); 
	}

	double operator * (const vec3d& v) const { return (x*v.x + y*v.y + z*v.z); }

	vec3d operator * (const double g) const { return vec3d(x*g, y*g, z*g); }
	vec3d operator / (const double g) const { return vec3d(x/g, y/g, z/g); }

	const vec3d& operator += (const vec3d& v) { x += v.x; y += v.y; z += v.z; return (*this); }
	const vec3d& operator -= (const vec3d& v) { x -= v.x; y -= v.y; z -= v.z; return (*this); }
	const vec3d& operator /= (const double f) { x /= f; y /= f; z /= f; return (*this); }
	const vec3d& operator /= (const int n) { x /= n; y /= n; z /= n; return (*this); }
	const vec3d& operator *= (const double f) { x*=f; y*=f; z*=f; return (*this); }

	vec3d operator - () const { return vec3d(-x, -y, -z); }

	double Length() const { return (double) sqrt(x*x + y*y + z*z); }
	double SqrLength() const { return x*x + y*y + z*z; }

	vec3d& Normalize()
	{
		double L = Length();
		if (L != 0) { x /= L; y /= L; z /= L; }

		return (*this);
	}

public:
	double x, y, z;
};

///////////////////////////////////////////////////////////////////
// vec6d

class vec6d
{
public:
	vec6d() { x = y = z = xy = yz = xz = 0; }

public:
	double x, y, z;
	double xy, yz, xz;
};


///////////////////////////////////////////////////////////////////
// mat3d

class mat3d
{
public:
	mat3d() 
	{
		zero();
	}

	mat3d(double a00, double a01, double a02, double a10, double a11, double a12, double a20, double a21, double a22);

	double* operator [] (int i) { return m_data[i]; }
	double& operator () (int i, int j) { return m_data[i][j]; }

	mat3d operator * (mat3d& m)
	{
		mat3d a;

		int k;
		for (k=0; k<3; k++)
		{
			a[0][0] += m_data[0][k]*m[k][0]; a[0][1] += m_data[0][k]*m[k][1]; a[0][2] += m_data[0][k]*m[k][2];
			a[1][0] += m_data[1][k]*m[k][0]; a[1][1] += m_data[1][k]*m[k][1]; a[1][2] += m_data[1][k]*m[k][2];
			a[2][0] += m_data[2][k]*m[k][0]; a[2][1] += m_data[2][k]*m[k][1]; a[2][2] += m_data[2][k]*m[k][2];
		}

		return a;
	}

	mat3d& operator *=(mat3d& m)
	{
		mat3d a;

		int k;
		for (k=0; k<3; k++)
		{
			a[0][0] += m_data[0][k]*m[k][0]; a[0][1] += m_data[0][k]*m[k][1]; a[0][2] += m_data[0][k]*m[k][2];
			a[1][0] += m_data[1][k]*m[k][0]; a[1][1] += m_data[1][k]*m[k][1]; a[1][2] += m_data[1][k]*m[k][2];
			a[2][0] += m_data[2][k]*m[k][0]; a[2][1] += m_data[2][k]*m[k][1]; a[2][2] += m_data[2][k]*m[k][2];
		}

		m_data[0][0] = a.m_data[0][0]; m_data[0][1] = a.m_data[0][1]; m_data[0][2] = a.m_data[0][2];
		m_data[1][0] = a.m_data[1][0]; m_data[1][1] = a.m_data[1][1]; m_data[1][2] = a.m_data[1][2];
		m_data[2][0] = a.m_data[2][0]; m_data[2][1] = a.m_data[2][1]; m_data[2][2] = a.m_data[2][2];

		return (*this);
	}

	mat3d& operator +=(mat3d& m)
	{
		m_data[0][0] += m[0][0]; m_data[0][1] += m[0][1]; m_data[0][2] += m[0][2];
		m_data[1][0] += m[1][0]; m_data[1][1] += m[1][1]; m_data[1][2] += m[1][2];
		m_data[2][0] += m[2][0]; m_data[2][1] += m[2][1]; m_data[2][2] += m[2][2];

		return (*this);
	}
	mat3d& operator -=(mat3d& m)
	{
		m_data[0][0] -= m[0][0]; m_data[0][1] -= m[0][1]; m_data[0][2] -= m[0][2];
		m_data[1][0] -= m[1][0]; m_data[1][1] -= m[1][1]; m_data[1][2] -= m[1][2];
		m_data[2][0] -= m[2][0]; m_data[2][1] -= m[2][1]; m_data[2][2] -= m[2][2];

		return (*this);
	}
	
	mat3d& operator /=(const double f)
	{
		m_data[0][0] /= f; m_data[0][1] /= f; m_data[0][2] /= f;
		m_data[1][0] /= f; m_data[1][1] /= f; m_data[1][2] /= f;
		m_data[2][0] /= f; m_data[2][1] /= f; m_data[2][2] /= f;

		return (*this);
	}

	vec3d operator * (vec3d b)
	{
		vec3d r;
		r.x = m_data[0][0]*b.x + m_data[0][1]*b.y + m_data[0][2]*b.z;
		r.y = m_data[1][0]*b.x + m_data[1][1]*b.y + m_data[1][2]*b.z;
		r.z = m_data[2][0]*b.x + m_data[2][1]*b.y + m_data[2][2]*b.z;

		return r;
	}

	double det()
	{
		double det = 0;
		det += m_data[0][0]*m_data[1][1]*m_data[2][2];
		det += m_data[0][1]*m_data[1][2]*m_data[2][0];
		det += m_data[0][2]*m_data[1][0]*m_data[2][1];
		det -= m_data[0][2]*m_data[1][1]*m_data[2][0];
		det -= m_data[0][1]*m_data[1][0]*m_data[2][2];
		det -= m_data[0][0]*m_data[1][2]*m_data[2][1];
		return det;
	}

	double Invert();

	void zero()
	{
		m_data[0][0] = m_data[0][1] = m_data[0][2] = 0;
		m_data[1][0] = m_data[1][1] = m_data[1][2] = 0;
		m_data[2][0] = m_data[2][1] = m_data[2][2] = 0;
	}

	void unit()
	{
		m_data[0][0] = m_data[1][1] = m_data[2][2] = 1;
		m_data[0][1] = m_data[0][2] = m_data[1][2] = 0;
		m_data[1][0] = m_data[2][0] = m_data[2][1] = 0;
	}

	mat3d transpose();

protected:
	double	m_data[3][3];
};



///////////////////////////////////////////////////////////////////
// matrix

class matrix
{
public:
	matrix(int r, int c);
	~matrix() { delete [] d; }

	void zero();

	double* operator [] (int i) { return d + i*m_nc; }
	double& operator () (int i, int j) { return d[i*m_nc + j]; }

	bool solve(vector<double>& x, vector<double>& b);

	bool lsq_solve(vector<double>& x, vector<double>& b);
	bool eigen_vectors(matrix& Eigen,vector<double>& eigen_values);
	int Rows() { return m_nr; }

	void mult_transpose(vector<double>& x, vector<double>& y);

	void mult_transpose_self(matrix& AAt);

private:
	double*	d;
	int		m_nr, m_nc;
	int		m_ne;
};


///////////////////////////////////////////////////////////////////
// quatd

class quatd
{
public:
	// constructors
	quatd () { x = y = z = 0; w = 1; }

	quatd( const double angle, vec3d v)
	{
		w = (double) cos(angle * 0.5);

		double sina = (double) sin(angle * 0.5);

		v.Normalize();
		
		x = v.x*sina;
		y = v.y*sina;
		z = v.z*sina;
	}

	quatd (vec3d v1, vec3d v2)
	{
		vec3d n = v1^v2;
		n.Normalize();

		double d = v1*v2;

		double sina = (double) sqrt((1.0-d)*0.5);
		double cosa = (double) sqrt((1.0+d)*0.5);

		w = cosa;

		x = n.x*sina;
		y = n.y*sina;
		z = n.z*sina;

	}

	quatd(const double qx, const double qy, const double qz, const double qw = 1.0)
	{
		w = qw;
		x = qx;
		y = qy;
		z = qz;
	}

	bool operator != (const quatd& q) { return ((x!=q.x) || (y!=q.y) || (z!=q.z) || (w!=q.w)); }

	quatd operator - () { return quatd(-x, -y, -z, -w); }

	// addition and substraction

	quatd operator + (const quatd& q) const
	{
		return quatd(x + q.x, y + q.y, z + q.z, w + q.w);
	}

	quatd operator - (const quatd& q) const
	{
		return quatd(x - q.x, y - q.y, z - q.z, w - q.w);
	}

	quatd& operator += (const quatd& q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;

		return *this;
	}

	quatd& operator -= (const quatd& q)
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;

		return *this;
	}


	// multiplication

	quatd operator * (const quatd& q) const
	{
		double qw = w*q.w - x*q.x - y*q.y - z*q.z;
		double qx = w*q.x + x*q.w + y*q.z - z*q.y;
		double qy = w*q.y + y*q.w + z*q.x - x*q.z;
		double qz = w*q.z + z*q.w + x*q.y - y*q.x;

		return quatd(qx, qy, qz, qw);
	}

	quatd& operator *= (const quatd& q)
	{
		double qw = w*q.w - x*q.x - y*q.y - z*q.z;
		double qx = w*q.x + x*q.w + y*q.z - z*q.y;
		double qy = w*q.y + y*q.w + z*q.x - x*q.z;
		double qz = w*q.z + z*q.w + x*q.y - y*q.x;

		x = qx;
		y = qy;
		z = qz;
		w = qw;

		return *this;
	}

	quatd operator*(const double a) const
	{
		return quatd(x*a, y*a, z*a, w*a);
	}

	// division

	quatd operator / (const double a) const
	{
		return quatd(x/a, y/a, z/a, w/a);
	}

	quatd& operator /= (const double a)
	{
		x /= a;
		y /= a;
		z /= a;
		w /= a;

		return *this;
	}

	// Special ops

	quatd Conjugate() const { return quatd(-x, -y, -z, w); }

	double Norm() const { return w*w + x*x + y*y + z*z; } 

	void MakeUnit() 
	{
		double N = (double) sqrt(w*w + x*x + y*y + z*z);

		if (N != 0)
		{
			x /= N;
			y /= N;
			z /= N;
			w /= N;
		}
		else w = 1.f;
	}

	quatd Inverse() const
	{
		double N = w*w + x*x + y*y + z*z;

		return quatd(-x/N, -y/N, -z/N, w/N);
	}

	double DotProduct(const quatd& q) const
	{
		return w*q.w + x*q.x + y*q.y + z*q.z;
	}

	vec3d GetVector() const
	{
		return vec3d(x, y, z).Normalize();
	}

	double GetAngle() const
	{
		return (double)(acos(w)*2.0);
	}

/*	quatd& MultiplyAngle(double fa)
	{
		double angle = fa*acos(w)*2.0;

		w = cos(angle * 0.5);

		double sina = sin(angle * 0.5);

		x *= sina;
		y *= sina;
		z *= sina;
	}
*/


	// use only when *this is unit vector
	void RotateVector(vec3d& v) const
	{
		if ((w == 0) || ((x==0) && (y==0) && (z==0))) return;

		// v*q^-1
		double qw = v.x*x + v.y*y + v.z*z;
		double qx = v.x*w - v.y*z + v.z*y;
		double qy = v.y*w - v.z*x + v.x*z;
		double qz = v.z*w - v.x*y + v.y*x;

		// q* (v* q^-1)
		v.x = (double) (w*qx + x*qw + y*qz - z*qy);
		v.y = (double) (w*qy + y*qw + z*qx - x*qz);
		v.z = (double) (w*qz + z*qw + x*qy - y*qx);
	}

	// use only when *this is unit vector
	vec3d operator * (const vec3d& r)
	{
		vec3d n = r;

		// v*q^-1
		double qw = n.x*x + n.y*y + n.z*z;
		double qx = n.x*w - n.y*z + n.z*y;
		double qy = n.y*w - n.z*x + n.x*z;
		double qz = n.z*w - n.x*y + n.y*x;

		// q* (v* q^-1)
		n.x = (w*qx + x*qw + y*qz - z*qy);
		n.y = (w*qy + y*qw + z*qx - x*qz);
		n.z = (w*qz + z*qw + x*qy - y*qx);

		return n;
	}

	mat3d operator * (mat3d m)
	{
		mat3d a;
		double qw, qx, qy, qz;
		for (int i=0; i<3; ++i)
		{
			// v*q^-1
			qw = m[0][i]*x + m[1][i]*y + m[2][i]*z;
			qx = m[0][i]*w - m[1][i]*z + m[2][i]*y;
			qy = m[1][i]*w - m[2][i]*x + m[0][i]*z;
			qz = m[2][i]*w - m[0][i]*y + m[1][i]*x;

			// q* (v* q^-1)
			a[0][i] = (w*qx + x*qw + y*qz - z*qy);
			a[1][i] = (w*qy + y*qw + z*qx - x*qz);
			a[2][i] = (w*qz + z*qw + x*qy - y*qx);
		}

		return a;
	}

	void RotateVectorP(double* v, double* r) const
	{
		static double fx, fy, fz, fw;
		static double qw, qx, qy, qz;
		
		fx = (double) x;
		fy = (double) y;
		fz = (double) z;
		fw = (double) w;

		qw = v[0]*fx + v[1]*fy + v[2]*fz;
		qx = v[0]*fw - v[1]*fz + v[2]*fy;
		qy = v[1]*fw - v[2]*fx + v[0]*fz;
		qz = v[2]*fw - v[0]*fy + v[1]*fx;

		r[0] = (double) (fw*qx + fx*qw + fy*qz - fz*qy);
		r[1] = (double) (fw*qy + fy*qw + fz*qx - fx*qz);
		r[2] = (double) (fw*qz + fz*qw + fx*qy - fy*qx);
	}

	static double dot(quatd &q1, quatd &q2) 
	{ return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w; }

	static quatd lerp(quatd &q1, quatd &q2, double t) 
	{ quatd q = (q1*(1-t) + q2*t); q.MakeUnit(); return q; }

	static quatd slerp(quatd &q1, quatd &q2, double t) ;

public:
	double x, y, z;
	double w;
};

inline quatd operator * (const double a, const quatd& q)
{
	return q*a;
}

typedef unsigned char uchar;

class GLCOLOR
{
public:
	uchar	a, b, g, r;

public:
	GLCOLOR() : a(255), b(0), g(0), r(0){}
	GLCOLOR(uchar ur, uchar ug, uchar ub, uchar ua = 255)
	{
		r = ur;	g = ug;	b = ub;	a = ua;
	}

	GLCOLOR operator * (double f)
	{
		return GLCOLOR((uchar) (r*f), (uchar) (g*f), (uchar) (b*f));
	}

	GLCOLOR operator + (GLCOLOR& c)
	{
		return GLCOLOR(r+c.r, g+c.g, b+c.b);
	}
};
