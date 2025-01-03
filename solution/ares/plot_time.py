from matplotlib import pyplot as plt

def get_S(T_1, T_P): return T_1 / T_P

def get_E(S_P, P): return S_P / P

def get_f(S_P, P): return (1/S_P - 1/P) / (1 - 1/P)

def main():
    # paste execution times here in order starting from 1 task
    t = [
        24.871830,
        14.341179,
        10.489308,
        8.855650,
        7.904733,
        7.325112,
        7.195594,
        6.548185,
        6.631589,
        6.579050,
        5.904610,
        5.713260,
    ]
    s = [get_S(t[0], t[i]) for i in range(len(t))]
    # max tasks here
    p = [i+1 for i in range(12)]
    e = [get_E(s[i], p[i]) for i in range(len(t))]
    f = [1] + [get_f(s[i], p[i]) for i in range(1,len(t))]
    
    X = [p[i] for i in range(len(t))]

    plt.title("speed up")
    plt.plot(X, s)
    plt.xticks(X)
    plt.xlabel("n_tasks")
    plt.ylabel("value")
    plt.show()
    plt.clf()

    plt.title("efficiency")
    plt.plot(X, e)
    plt.xticks(X)
    plt.xlabel("n_tasks")
    plt.ylabel("value")
    plt.show()
    plt.clf()

    plt.title("f function")
    plt.plot(X, f)
    plt.xticks(X)
    plt.xlabel("n_tasks")
    plt.ylabel("value")
    plt.show()
    plt.clf()

if __name__ == "__main__":
    main()