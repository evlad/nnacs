# -*- coding: utf-8 -*-
"""
Created on Sun Oct 28 14:08:03 2018

@author: evlad
"""

import numpy as np
import matplotlib.pyplot as plt

x=np.linspace(0,1,5)
y=np.linspace(0,1,5)
mx,my = np.meshgrid(x,y)

io_dist = np.loadtxt('xor5_in_out_dist.dat')
InErr=io_dist[:,1]
OutErr=io_dist[:,2]

InMSE=np.square(InErr).mean()
OutMSE=np.square(OutErr).mean()
# Equal to
#TarNN=np.loadtxt('xor5_out.dat')
#OutMSE=np.square(TarNN - OutNN).mean()

ie=InErr.reshape(5,5)
oe=OutErr.reshape(5,5)

OutNN=np.loadtxt('xor5_res.dat')
nno=OutNN.reshape(5,5)

ix=np.loadtxt('xor5_in.dat')
dx=np.loadtxt('xor5_in_err.dat')

ticks=np.arange(0,1.1,0.25)

fig=plt.figure()
plt.suptitle('XOR input and output errors', fontsize=16)

plt.subplot(2,2,1)
plt.title('NN output, input pairs and desired input pairs')
plt.contourf(mx,my,nno,20,cmap='RdBu')
for i in np.arange(0,25):
    plt.arrow(ix[i,0], ix[i,1], dx[i,0], dx[i,1])
plt.grid()
plt.xlim(-0.1, 1.1)
plt.ylim(-0.1, 1.1)
plt.xticks(ticks)
plt.yticks(ticks)
plt.colorbar()

plt.subplot(2,2,2)
plt.title('Input error (MSE={:g})'.format(InMSE))
plt.contourf(mx,my,ie,20,cmap='jet')
plt.grid()
plt.xticks(ticks)
plt.yticks(ticks)
plt.colorbar()

plt.subplot(2,2,4)
plt.title('Output error (MSE={:g})'.format(OutMSE))
plt.contourf(mx,my,oe,20,cmap='jet')
plt.grid()
plt.xticks(ticks)
plt.yticks(ticks)
plt.colorbar()

plt.subplot(2,2,3)
plt.title('Input error/output error ratio')
plt.contourf(mx,my,ie/oe,20,cmap='YlGn')
plt.grid()
plt.xticks(ticks)
plt.yticks(ticks)
plt.colorbar()

plt.show()
#fig.savefig('xor5.png')
