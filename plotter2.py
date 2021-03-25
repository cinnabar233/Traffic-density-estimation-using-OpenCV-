import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import pandas as pd
df=pd.read_csv("out_2(1).txt")
ax = plt.gca()
#df.plot(kind='line',x='frame',y='dynamic density', color = 'blue' , ax=ax)
df.plot(kind='line',x='frame',y='queue density', color = 'red' , ax=ax)
plt.show()


