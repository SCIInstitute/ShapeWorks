#include "math3d.h"

//-----------------------------------------------------------------------------
mat3d::mat3d(double a00, double a01, double a02, double a10, double a11, double a12, double a20, double a21, double a22)
{
	m_data[0][0] = a00; m_data[0][1] = a01; m_data[0][2] = a02;
	m_data[1][0] = a10; m_data[1][1] = a11; m_data[1][2] = a12;
	m_data[2][0] = a20; m_data[2][1] = a21; m_data[2][2] = a22;
}

//-----------------------------------------------------------------------------
double mat3d::Invert()
{
	// calculate determinant
	double det = 0;
	det += m_data[0][0]*m_data[1][1]*m_data[2][2];
	det += m_data[0][1]*m_data[1][2]*m_data[2][0];
	det += m_data[0][2]*m_data[1][0]*m_data[2][1];
	det -= m_data[0][2]*m_data[1][1]*m_data[2][0];
	det -= m_data[0][1]*m_data[1][0]*m_data[2][2];
	det -= m_data[0][0]*m_data[1][2]*m_data[2][1];

	assert(det != 0);

	double deti = 1 / det;

	// calculate conjugate matrix
	double mi[3][3];

	mi[0][0] =  (m_data[1][1]*m_data[2][2] - m_data[1][2]*m_data[2][1]);
	mi[0][1] = -(m_data[1][0]*m_data[2][2] - m_data[1][2]*m_data[2][0]);
	mi[0][2] =  (m_data[1][0]*m_data[2][1] - m_data[1][1]*m_data[2][0]);

	mi[1][0] = -(m_data[0][1]*m_data[2][2] - m_data[0][2]*m_data[2][1]);
	mi[1][1] =  (m_data[0][0]*m_data[2][2] - m_data[0][2]*m_data[2][0]);
	mi[1][2] = -(m_data[0][0]*m_data[2][1] - m_data[0][1]*m_data[2][0]);

	mi[2][0] =  (m_data[0][1]*m_data[1][2] - m_data[0][2]*m_data[1][1]);
	mi[2][1] = -(m_data[0][0]*m_data[1][2] - m_data[0][2]*m_data[1][0]);
	mi[2][2] =  (m_data[0][0]*m_data[1][1] - m_data[0][1]*m_data[1][0]);

	// divide by det and transpose
	m_data[0][0] = mi[0][0]*deti; m_data[1][0] = mi[0][1]*deti; m_data[2][0] = mi[0][2]*deti;
	m_data[0][1] = mi[1][0]*deti; m_data[1][1] = mi[1][1]*deti; m_data[2][1] = mi[1][2]*deti;
	m_data[0][2] = mi[2][0]*deti; m_data[1][2] = mi[2][1]*deti; m_data[2][2] = mi[2][2]*deti;

	// return determinant
	return det;
}

//-----------------------------------------------------------------------------
quatd quatd::slerp(quatd &q1, quatd &q2, double t) 
{
	quatd q3;
	double dot = quatd::dot(q1, q2);

	/*	dot = cos(theta)
		if (dot < 0), q1 and q2 are more than 90 degrees apart,
		so we can invert one to reduce spinning	*/
	if (dot < 0)
	{
		dot = -dot;
		q3 = -q2;
	} else q3 = q2;
		
	if (dot < 0.95f)
	{
		double angle = acos(dot);
		return (q1*sin(angle*(1-t)) + q3*sin(angle*t))/sin(angle);
	} else // if the angle is small, use linear interpolation								
		return quatd::lerp(q1,q3,t);
}


//-----------------------------------------------------------------------------
bool matrix::solve(vector<double>& x, vector<double>& b)
{
	// check sizes
	if (m_nr != m_nc) return false;
	if (((int)x.size() != m_nr) || ((int)b.size() != m_nr)) return false;

	// step 1. Factorization

	const double TINY = 1.0e-20;
	int i, imax, j, k;
	double big, dum, sum, temp;

	matrix& a = *this;

	int n = a.Rows();
	
	// create index vector
	vector<int> indx(n);

	vector<double> vv(n);
	for (i=0; i<n; ++i)
	{
		big = 0;
		for (j=0; j<n; ++j)
			if ((temp=fabs(a(i,j))) > big) big = temp;
		if (big == 0) return false; // singular matrix
		vv[i] = 1.0 / big;
	}

	for (j=0; j<n; ++j)
	{
		for (i=0; i<j; ++i)
		{
			sum = a(i,j);
			for (k=0; k<i; ++k) sum -= a(i,k)*a(k,j);
			a(i,j) = sum;
		}
		big = 0;
		imax = j;
		for (i=j;i<n;++i)
		{
			sum = a(i,j);
			for (k=0; k<j; ++k) sum -= a(i,k)*a(k,j);
			a(i,j) = sum;
			if ((dum=vv[i]*fabs(sum))>=big)
			{
				big = dum;
				imax = i;
			}
		}

		if (j != imax)
		{
			for (k=0; k<n; ++k)
			{
				dum = a(imax,k);
				a(imax,k) = a(j,k);
				a(j,k) = dum;
			}
			vv[imax] = vv[j];
		}

		indx[j] = imax;
		if (a(j,j) == 0) a(j,j) = TINY;
		if (j != n-1)
		{
			dum = 1.0/a(j,j);
			for (i=j+1;i<n; ++i) a(i,j) *= dum;
		}
	}

	// step 2. back-solve
	x = b;

	int ii=0, ip;

	n = a.Rows();
	for (i=0; i<n; ++i)
	{
		ip = indx[i];
		sum = x[ip];
		x[ip] = x[i];
		if (ii != 0)
			for (j=ii-1;j<i;++j) sum -= a(i,j)*x[j];
		else if (sum != 0)
			ii = i+1;
		x[i] = sum;
	}

	for (i=n-1; i>=0; --i)
	{
		sum = x[i];
		for (j=i+1; j<n; ++j) sum -= a(i,j)*x[j];
		x[i] = sum/a(i,i);
	}

	return true;
}
//-----------------------------------------------------------------------------
void matrix::mult_transpose_self(matrix& AAt)
{
	matrix& A = *this;
	int N = m_nc;
	int R = m_nr;
	for (int i=0; i<N; ++i)
		for (int j=0; j<N; ++j)
		{
			double& aij = AAt[i][j];
			aij = 0.0;
			for (int k=0; k<R; ++k) aij += A[k][i]*A[k][j];
		}
}
//-----------------------------------------------------------------------------
matrix::matrix(int r, int c) : m_nr(r), m_nc(c)
{
	m_ne = r*c;
	if (m_ne > 0) d = new double[m_ne];
	else d = 0;
}

//-----------------------------------------------------------------------------
void matrix::zero()
{
	for (int i=0; i<m_ne; ++i) d[i] = 0.0;
}

//-----------------------------------------------------------------------------
void matrix::mult_transpose(vector<double>& x, vector<double>& y)
{
	for (int i=0; i<m_nc; ++i) y[i] = 0.0;

	for (int i=0; i<m_nr; ++i)
	{
		double* di = d + i*m_nc;
		for (int j=0; j<m_nc; ++j) y[j] += di[j]*x[i];
	}
}

//-----------------------------------------------------------------------------
bool matrix::lsq_solve(vector<double>& x, vector<double>& b)
{
	if ((int) x.size() != m_nc) return false;
	if ((int) b.size() != m_nr) return false;

	vector<double> y(m_nc);
	mult_transpose(b, y);

	matrix AA(m_nc, m_nc);
	mult_transpose_self(AA);

	AA.solve(x, y);

	return true;
}

#define ROTATE(a, i, j, k, l) g=a[i][j]; h=a[k][l];a[i][j]=g-s*(h+g*tau); a[k][l] = h + s*(g - h*tau);

bool matrix::eigen_vectors(matrix& Eigen, vector<double>& eigen_values){
	matrix& A = *this;
	int N = m_nc;
	int R = m_nr;
	const int NMAX = 50;
	double sm, tresh, g, h, t, c, tau, s, th;
	const double eps = 0;//1.0e-15;
	int k ;

	//initialize Eigen to identity
	for(int i = 0; i< R ; i++)
	{
		for(int j = 0;j<N;j++) Eigen[i][j] = 0;
		Eigen[i][i] = 1;
	}
	vector<double> b;
	b.reserve(R);
	vector<double> z;
	z.reserve(R);

	// initialize b and eigen_values to the diagonal of A
	for(int i = 0; i<R;i++)
	{
		b.push_back(A[i][i]);
		eigen_values.push_back(A[i][i]);
		z.push_back(0);
	}
	// loop
	int n, nrot = 0;
	for (n=0; n<NMAX; ++n)
	{
		// sum off-diagonal elements
		sm = 0;
		for(int i = 0; i<N-1;i++){
			for(int j = i+1; j<N; j++)
				sm += fabs(A[i][j]);
		}
		if (sm <= eps) {
			break;
		}
		// set the treshold
		if (n < 3) tresh = 0.2*sm/(R*R); else tresh = 0.0;

		// loop over off-diagonal elements
		for(int i = 0; i<N-1;i++){
			for(int j = i+1; j<N; j++){

				g = 100.0*fabs(A[i][j]);
				// after four sweeps, skip the rotation if the off-diagonal element is small
				if ((n > 3) && ((fabs(eigen_values[i])+g) == fabs(eigen_values[i])) && ((fabs(eigen_values[j])+g) == fabs(eigen_values[j])))
				{
					A[i][j] = 0.0;
				}
				else if (fabs(A[i][j]) > tresh){
					h = eigen_values[j] - eigen_values[i];
					if ((fabs(h)+g) == fabs(h))
						t = A[i][j]/h;
					else
					{
						th = 0.5*h/A[i][j];
						t = 1.0/(fabs(th) + sqrt(1+th*th));
						if (th < 0.0) t = -t;
					}
					c = 1.0/sqrt(1.0 + t*t);
					s = t*c;
					tau = s/(1.0+c);
					h = t*A[i][j];
					z[i] -= h;
					z[j] += h;
					eigen_values[i] -= h;
					eigen_values[j] += h;
					A[i][j] = 0;
					for (k=  0; k<=i-1; ++k) { ROTATE(A, k, i, k, j) }
					for (k=i+1; k<=j-1; ++k) { ROTATE(A, i, k, k, j) }
					for (k=j+1; k<N; ++k) { ROTATE(A, i, k, j, k) }
					for (k=  0; k<N; ++k) { ROTATE(Eigen, k, i, k, j) }
					++nrot;
				}
			}
		}//end of for loop

		//Update eigen_values with the sum. Reinitialize z.
		for (int i=0; i<R; ++i) 
		{
			b[i] += z[i];
			eigen_values[i] = b[i];
			z[i] = 0.0;
		}
	}

	// we sure we converged
	assert(n < NMAX);
	return true;
}
//-----------------------------------------------------------------------------
mat3d mat3d::transpose()
{
	return mat3d(m_data[0][0], m_data[1][0], m_data[2][0], m_data[0][1], m_data[1][1], m_data[2][1], m_data[0][2], m_data[1][2], m_data[2][2]);
}
