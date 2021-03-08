import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import pandas as pd
df=pd.read_csv("out.csv")
ax = plt.gca()
df.plot(kind='line',x='Frame',y='Dynamic density', color = 'red' , ax=ax)
df.plot(kind='line',x='Frame',y='Queue density', color = 'red' , ax=ax)
plt.show()


