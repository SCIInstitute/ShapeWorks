import numpy as np
from numpy import linalg as la
np.random.seed(0)

def random_rotation(n, theta=None):
    if theta is None:
        theta = 0.5 * np.pi * np.random.rand()
    if n == 1:
        return np.random.rand() * np.eye(1)

    rot = np.array([[np.cos(theta), -np.sin(theta)],
                    [np.sin(theta), np.cos(theta)]])
    out = np.zeros((n, n))
    out[:2, :2] = rot
    q = np.linalg.qr(np.random.randn(n, n))[0]
    return q.dot(out).dot(q.T)

def fix_covariance2(A):
    if isPD(A):
        return A
    else:
        epsilon = 1e-6
        u, s, vh = np.linalg.svd(A, full_matrices=True)
        s[s<epsilon] = epsilon
        A2 = np.dot(u * s, vh)
        return A2

def fix_covariance(A):
    if isPD(A):
        return A
    else:
        """Find the nearest positive-definite matrix to input

        A Python/Numpy port of John D'Errico's `nearestSPD` MATLAB code [1], which
        credits [2].

        [1] https://www.mathworks.com/matlabcentral/fileexchange/42885-nearestspd

        [2] N.J. Higham, "Computing a nearest symmetric positive semidefinite
        matrix" (1988): https://doi.org/10.1016/0024-3795(88)90223-6
        """

        B = (A + A.T) / 2
        _, s, V = la.svd(B)

        H = np.dot(V.T, np.dot(np.diag(s), V))

        A2 = (B + H) / 2

        A3 = (A2 + A2.T) / 2

        if isPD(A3):
            return A3

        spacing = np.spacing(la.norm(A))
        # The above is different from [1]. It appears that MATLAB's `chol` Cholesky
        # decomposition will accept matrixes with exactly 0-eigenvalue, whereas
        # Numpy's will not. So where [1] uses `eps(mineig)` (where `eps` is Matlab
        # for `np.spacing`), we use the above definition. CAVEAT: our `spacing`
        # will be much larger than [1]'s `eps(mineig)`, since `mineig` is usually on
        # the order of 1e-16, and `eps(1e-16)` is on the order of 1e-34, whereas
        # `spacing` will, for Gaussian random matrixes of small dimension, be on
        # othe order of 1e-16. In practice, both ways converge, as the unit test
        # below suggests.
        I = np.eye(A.shape[0])
        k = 1
        while not isPD(A3):
            mineig = np.min(np.real(la.eigvals(A3)))
            A3 += I * (-mineig * k**2 + np.spacing(la.norm(A)))
            k += 1
            print(k)
    return A3


def isPD(B):
    """Returns true when input is positive-definite, via Cholesky"""
    try:
        _ = la.cholesky(B)
        if np.linalg.det(B) > 0:
            return True
        else:
            return False
    except la.LinAlgError:
        return False

def fix_covariance1(A):
    # # symmetrize A into B
    B = (A + A.T)/2
    # # Compute the symmetric polar factor of B. Call it H.
    # # Clearly H is itself SPD.
    # if any(isnan(B(:))) || any(isinf(B(:)))
    #     tt= 0;
    # end
    
    U, sigma, V = la.svd(B)
    H = np.dot(V, np.dot(np.diag(sigma), V.T))

    # get Ahat in the above formula
    Ahat = (B+H)/2;

    # ensure symmetry
    Ahat = (Ahat + Ahat.T)/2

    # test that Ahat is in fact PD. if it is not so, then tweak it just a bit.
    k = 1
    while not isPD(Ahat):
        # Ahat failed the chol test. It must have been just a hair off,
        # due to floating point trash, so it is simplest now just to
        # tweak by adding a tiny multiple of an identity matrix.
        print(k)
        mineig = np.min(np.real(la.eigvals(Ahat)))
        Ahat = Ahat + (-mineig*k**2 + np.finfo(mineig).eps)*np.eye(A.shape[0])
        k = k + 1
    return Ahat

if __name__ == '__main__':
    import numpy as np
    for i in range(10):
        for j in range(2, 100):
            A = np.random.randn(j, j)
            B = fix_covariance(A)
            assert (isPD(B))
    print('unit test passed!')