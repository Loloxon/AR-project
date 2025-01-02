import numpy as np
import matplotlib.pyplot as plt

# Parameters
nx, ny = 21, 21
Lx, Ly = 1.0, 1.0
dx, dy = Lx / nx, Ly / ny
T = 6.5
cfl = 0.3

x = np.linspace(0, Lx, nx)
y = np.linspace(0, Ly, ny)
X, Y = np.meshgrid(x, y)


def initial_condition(x, y, R=100):
    # print(1. / (1. + np.exp(R * (x + y) * 0.5)))
    return 1. / (1. + np.exp(R * (x + y) * 0.5))
    # return x + y
    # return np.exp(-10 * ((x - Lx / 2) ** 2 + (y - Ly / 2) ** 2))


u = np.empty((nx + 2, ny + 2))
u[1:-1, 1:-1] = initial_condition(X, Y)
u[0, :] = u[1, :]
u[-1, :] = u[-2, :]
u[:, 0] = u[:, 1]
u[:, -1] = u[:, -2]


def lax_friedrichs_2d(u, dx, dy, dt):
    nx, ny = u.shape
    u_new = np.zeros_like(u)
    alpha_x = np.max(np.abs(u))
    alpha_y = np.max(np.abs(u))
    # print(alpha_x, alpha_y)
    # alpha_x = 0.002
    # alpha_y = 0.002
    for i in range(1, nx - 1):
        for j in range(1, ny - 1):
            a = 0.25 * (u[i, j] ** 2 + u[i + 1, j] ** 2)
            b = 0.25 * (u[i - 1, j] ** 2 + u[i, j] ** 2)
            c = 0.25 * (u[i, j] ** 2 + u[i, j + 1] ** 2)
            d = 0.25 * (u[i, j - 1] ** 2 + u[i, j] ** 2)

            a2 = 0.5 * alpha_x * (u[i + 1, j] - u[i, j])
            b2 = 0.5 * alpha_x * (u[i, j] - u[i - 1, j])
            c2 = 0.5 * alpha_y * (u[i, j + 1] - u[i, j])
            d2 = 0.5 * alpha_y * (u[i, j] - u[i, j - 1])

            Fx_p = 0.25 * (u[i, j] ** 2 + u[i + 1, j] ** 2) - 0.5 * alpha_x * (u[i + 1, j] - u[i, j])
            Fx_m = 0.25 * (u[i - 1, j] ** 2 + u[i, j] ** 2) - 0.5 * alpha_x * (u[i, j] - u[i - 1, j])
            Gy_p = 0.25 * (u[i, j] ** 2 + u[i, j + 1] ** 2) - 0.5 * alpha_y * (u[i, j + 1] - u[i, j])
            Gy_m = 0.25 * (u[i, j - 1] ** 2 + u[i, j] ** 2) - 0.5 * alpha_y * (u[i, j] - u[i, j - 1])
            u_new[i, j] = u[i, j] - dt / dx * (Fx_p - Fx_m) - dt / dy * (Gy_p - Gy_m)
            # u_new[i, j] = u[i, j] - dt / dx * (a - b) - dt / dy * (c - d) - (a2 - b2) - (c2 - d2)

            # u_new[i, j] = u[i, j] - dt / dx * (Fx_p - Fx_m) - dt / dy * (Gy_p - Gy_m)
    u_new[0, :] = u_new[1, :]
    u_new[-1, :] = u_new[-2, :]
    u_new[:, 0] = u_new[:, 1]
    u_new[:, -1] = u_new[:, -2]

    return u_new


def plot():
    # c = plt.imshow(u[1:-1, 1:-1], cmap='hot', interpolation='nearest')
    # plt.colorbar(c)
    # plt.show()
    # return
    # fig = plt.figure()
    # ax = fig.add_subplot(projection='3d')
    # ax.set_title(f"T = {time}")
    # ax.plot_surface(X, Y, u[1:-1, 1:-1])
    # ax.axes.set_zlim3d(bottom=0.0, top=0.4)
    # ax.set_xlabel('X')
    # ax.set_ylabel('Y')
    # ax.set_zlabel('U')
    # plt.show()
    # return
    plt.figure(figsize=(8, 6))
    levels = np.linspace(0.0, 0.4, 51)
    plt.contourf(X, Y, u[1:-1, 1:-1], levels=levels, cmap='viridis')
    plt.colorbar(label='u')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title(f"T = {time}")
    plt.show()


next_t = 1
time = 0.0
while time < T:
    # dt = cfl * min(dx, dy) / np.max(np.abs(u))
    dt = 0.01
    if time + dt > T:
        dt = T - time
    u = lax_friedrichs_2d(u, dx, dy, dt)
    if time >= next_t:
        plot()
        next_t += 1
    time += dt
plot()
# plt.imshow(u[1:-1, 1:-1], cmap='hot', interpolation='nearest', origin='lower')
# plt.colorbar()
# plt.show()
