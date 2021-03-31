import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import pandas as pd
import sys
df=pd.read_csv(sys.argv[1])
ax = plt.gca()
df.plot(kind='line',x='pthread',y='GPU-utilisation', color = 'blue' , ax=ax)
plt.show()
df.plot(kind='line',x='pthread',y='error', color = 'red' , ax=ax , style = '.-')
plt.show()
df.plot(kind='line',x='pthread',y='runtime', color = 'blue' , ax=ax ,style = '.-')
plt.show(



