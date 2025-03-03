import numpy as np
import matplotlib.pyplot as plt


c1x=np.array([-35.0,-50.0,-75.0,-100.0])
c1z=3.0+10.0*c1x/100.0
print("c1x= ",c1x)
print("c1z= ",c1z)

cryz=c1z-2.5
print("cryz= ",cryz)

dz=10.0-cryz
print("dz=",dz)

dx=abs(c1x)-10.0
print("dx=",dx)

theta=np.arctan(dz/dx)*180.0/3.14
print("theta=",theta)

th2=theta+5.0
print("th2=",th2)



