import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


df = pd.read_csv(sys.argv[1], header=None)

x = np.linspace(0, 1, 21) # This 21 need to be same as 'SIDE_LENGTH' minus 2 from .c file
y = np.linspace(0, 1, 21) # This 21 need to be same as 'SIDE_LENGTH' minus 2 from .c file
X, Y = np.meshgrid(x, y)

plt.figure(figsize=(8, 6))
levels = np.linspace(0.0, 0.4, 51)
plt.contourf(X, Y, df, levels=levels, cmap='viridis')
plt.colorbar(label='u')
plt.xlabel('x')
plt.ylabel('y')

# c = plt.imshow(df, cmap='hot', interpolation='nearest')
# plt.colorbar(c)
plt.savefig('out.png')
