from numpy.linalg import solve
import numpy as np
from matplotlib import pyplot as plt


def get_phi(u, i, n): return 0.5 * (u[i] + u[i-1]) * (u[i] - u[i-1])

def get_F(u, i, n): return 0.5 * (u[i]**2)

def get_b(n, u, w):
    # zakładam, że b[0] i b[-1] = 0 i i=j; TODO: nk zweryfikuje
    b = [0] + [
        u[i] - 0.5 * (dt/dx) * (get_F(u, i+1, n) - get_F(u, i, n)) + 0.5*w * (dt/dx) * (get_phi(u, i+1, n) - get_phi(u, i-1, n)) for i in range(1,n-1)
    ] + [0]

    return np.array(b)

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

def get_init_u(n, dx, a, mu):
    x = np.array([np.pi * i * dx for i in range(n)])

    return (2 * mu * np.pi) * np.sin(x) / (a + np.cos(x))

def main(n, mu, dt, dx, w=0.5, a=8):
    u = get_init_u(n, dx, a, mu)
    aggregated_result = [u]
    print(u)

    r = get_r(mu, dt, dx)
    for _ in range(1,2*n): # time steps
        A = get_A(n, r)
        print(A)
        b = get_b(n, u, w)
        print(b)
        u = np.linalg.solve(A, b)
        print(u)
        aggregated_result.append(u)

    return np.array(aggregated_result)

def plot_result(result):
    fig = plt.figure()
    ax = plt.axes(projection="3d")
    y = np.arange(len(result))
    x = np.arange(len(result[0]))
    (x,y) = np.meshgrid(x,y)
    ax.plot_surface(x,y,result)
    plt.show()

if __name__ == "__main__":
    n = 10
    mu = 0.5
    dt = 1/n
    dx = 1/n
    result = main(n, mu, dt, dx)
    plot_result(result)