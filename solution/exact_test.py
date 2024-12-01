import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm


def get_exact(x_space, time_spec, a, mu):
    t_term = np.exp((-np.pi * np.pi * mu) * time_spec)
    x_term = np.pi * x_space
    return (2. * mu * np.pi) * np.outer(t_term, np.sin(x_term)) / (a + np.outer(t_term, np.cos(x_term)))


def get_initial(x_space, a, mu):
    x_term = np.pi * x_space
    return (2. * mu * np.pi) * np.sin(x_term) / (a + np.cos(x_term))


def main():
    mu = 0.1
    a = 8
    dx = 1e-3
    dt = 1e-3
    x_spec = np.linspace(start=0., stop=1., num=int(1. / dx))
    time_spec = np.linspace(start=0., stop=2., num=int(1. / dt))

    result = get_exact(x_spec, time_spec, a, mu)
    print(f'Error with initial conditions: {np.sum((result[0] - get_initial(x_spec, a, mu)) ** 2)} (should be 0)')

    X, T = np.meshgrid(x_spec, time_spec)
    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')
    ax.plot_surface(X, T, result, cmap=cm.coolwarm)
    ax.set_xlabel('X')
    ax.set_ylabel('T')
    ax.set_zlabel('U')
    plt.show()


if __name__ == '__main__':
    main()
