# FILENAME: plot.py
import numpy as np
import matplotlib.pyplot as plt 
import sys

def plot_data(data):
  plt.clf()
  plt.plot(data)
  plt.draw()
  plt.show(block = False)
  plt.pause(0.01)
 
if __name__ == "__main__":
  cnt = 0
  tmp = ''
  plt.ion()
  while True:
    try:
        line = input().strip()
        if line.startswith('{'):
            tmp = line.replace('{','').replace('}','');

        else:
            tmp += line.replace('{','').replace('}','');

        if line.endswith('}'):
            array = np.array(tmp.split(','))
            # print("Array length = %d"%len(array))
            plot_data(array) # Array complete -> plot it
            cnt += 1

    except EOFError:
      print ("Input has terminated! Exiting")
      # exit()
      break
    except ValueError:
      print ("Invalid input, skipping.  Input was: %s"%tmp)
      continue

plt.show(block=True)
print("Exit plot")
