import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("table.csv")
#x = range(0, len(data['total']))
plt.plot(data['total'], data['hit']+data['miss'])
plt.plot(data['total'], data['hit'])
plt.plot(data['total'], data['miss'])
plt.xticks(rotation=30)
plt.xlabel('Reference')
plt.ylabel('hit/miss')
plt.show()
#print(data.head(12))