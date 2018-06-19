import matplotlib.pyplot as plt
import numpy as np

a = [1,2,4,8,16]
b = [385.205,200.829,106.433,59.5898,37.2913]

plt.xscale('log')
plt.yscale('log')
plt.plot(a,b,'ro')
plt.show()
