from numpy.linalg import solve
import numpy as np


def get_phi(u, i, n): return 0.5 * (u[i,n] + u[i-1,n]) * (u[i,n] - u[i-1,n])

def get_F(u, i, n): return 0.5 * (u[i,n]**2)

def get_b(n, u):
    w = 0.5
    b = [0]
    for i in range(1,n-1):
        b_i = u[i,n] - 0.5 * (dt/dx) * (get_F(u, i+1, n) - get_F(u, i, n)) + 0.5*w * (dt/dx) * (get_phi(u, i+1, n) - get_phi(u, i-1, n)) # phi j
        b.append(b_i)

    return np.array(b + [0])

def get_A(n, r):
    A = np.zeros((n,n))

    # diagonal
    for i in range(n):
        A[i,i] = 1 + 2*r
    
    # sub- and super-diagonal
    for i in range(n):
        if i-1 >= 0 and i-1 < n:
            A[i,i-1] = -r
        if i+1 >= 0 and i+1 < n:
            A[i,i+1] = -r

    return A

def get_r(mu, dt, dx): return mu * dt / (dx*dx)

def main(n, mu, dt, dx):
    r = get_r(mu, dt, dx)

    u = np.zeros(n)
    A = get_A(n, r)
    print(A)
    # b = get_b(n, u)
    # u = np.linalg.solve(A, b)
    print(u)

if __name__ == "__main__":
    n = 10
    mu = 0.5
    dt = 1/n
    dx = 1/n
    main(n, mu, dt, dx)