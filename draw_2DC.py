import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

with open('result2.txt', 'r') as f:
    lines = f.readlines()
f.close()
line = []
for item in lines:
    raw = item.split()
    line.append([float(x) for x in raw])
del item, lines, raw
plt.imshow(line)
plt.show()