import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from os import getenv


df = pd.read_csv(sys.argv[1], header=None)

x = np.linspace(0, 1, int(getenv("SIDE_LENGTH")) - 2)
y = np.linspace(0, 1, int(getenv("SIDE_LENGTH")) - 2)
X, Y = np.meshgrid(x, y)

plt.figure(figsize=(8, 6))
levels = np.linspace(0.0, 0.5, 51)
plt.contourf(X, Y, df, levels=levels, cmap='viridis')
plt.colorbar(label='u')
plt.xlabel('x')
plt.ylabel('y')

# c = plt.imshow(df, cmap='hot', interpolation='nearest')
# plt.colorbar(c)
plt.savefig('out.png')
