import numpy as np
import matplotlib.pyplot as plt

f = open('data.txt', 'r')

L = 1
N = 400
x = np.linspace(0,L,N+1)
xmid = 1/2*(x[0:N]+x[1:N+1])
y = np.linspace(0,L,N+1)
ymid = 1/2*(x[0:N]+x[1:N+1])
X,Y = np.meshgrid(xmid, ymid)

data = np.loadtxt(f)

initial = data[0:400]
middle = data[400:800]
last = data[800:1200]

plt.figure(figsize = (15,5))
plt.subplot(131)
plt.pcolor(X, Y, initial, cmap='RdBu', vmax=abs(initial).max(), vmin=0)

plt.subplot(132)
plt.pcolor(X, Y, middle, cmap='RdBu', vmax=abs(middle).max(), vmin=0)

plt.subplot(133)
plt.pcolor(X, Y, last, cmap='RdBu', vmax=abs(last).max(), vmin=0)

plt.colorbar()
#plt.show()
plt.savefig('parallel.png')
