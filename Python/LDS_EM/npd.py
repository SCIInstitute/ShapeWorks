import numpy as np
import torch
DEVICE = 'cuda:0'
# torch.set_default_dtype(torch.float64)

def nearestPD(A):
    B = (A + A.T) / 2
    _, s, V = torch.linalg.svd(B)
    H = V.T @ (torch.diag(s) @ V)
    A2 = (B + H) / 2
    A3 = (A2 + A2.T) / 2
    if isPD(A3):
        return A3
    spacing = np.spacing(torch.linalg.norm(A).to('cpu').numpy())
    I = torch.eye(A.size()[0]).to(DEVICE)
    k = 1
    # print('here waiting for conversion to PSD----')
    while not isPD(A3):
        mineig = torch.min(torch.real(torch.linalg.eigvals(A3)))
        A3 += I * (-mineig * k**2 + spacing)
        k += 1
    # print('------Nearest PD found for Matrix-------')
    return A3


def isPD(B):
    """Returns true when input is positive-definite, via Cholesky_ex"""
    try:
        _, info = torch.linalg.cholesky_ex(B, check_errors=True)
        return True
    except:
        return False
