import subprocess
import numpy as np

prefix = "Sim_"

L = np.array([1, 2, 3])

T = np.array([10, 20, 30, 40, 50, 60])

D = np.array([1, 2, 7, 15, 30])

w_start = 0.0001
w_end   = 0.01
W = np.linspace(w_start, w_end, 5)

print(W)

for l in L:
    for w in W:
        for t in T:
            for d in D:
                filename = prefix + "w=" + str(w) + "_l=" + str(l) + "_t=" + str(t) + "_d=" + str(d)
                subprocess.run(["../build/DependencyMiner.exe",
                    "-f", "../Delitos_cleaned.csv",
                    "-o", "../../../../Dropbox/Masters/Resultados/Similarity/" + filename,
                    "-s", "1",
                    "-a", "0,1,2,3,4,5,6,7,8,9,10,11,12,13",
                    "-m", "t,t,d,l,l,l,l,l,l,l,l,l,l,w",
                    "-v", "{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13}".format(t,t,d,l,l,l,l,l,l,l,l,l,l,w)])
                    