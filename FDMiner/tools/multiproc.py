import multiprocessing
import subprocess


def worker(id, l, t, d, w):
    print("Executing {} with l = {}, t = {}, d = {}, w = {}".format(id, l, t, d, w))
    prefix   = "sim_"
    filename = prefix + "w=" + str(w) + "_l=" + str(l) + "_t=" + str(t) + "_d=" + str(d)
    subprocess.run(["../build/DependencyMiner.exe",
        "-f", "../Delitos_cleaned.csv",
        "-o", "../../../../Dropbox/Masters/Resultados/Similarity/" + filename,
        "-s", "1",
        "-a", "0,1,2,3,4,5,6,7,8,9,10,11,12,13",
        "-m", "t,t,d,l,l,l,l,l,l,l,l,l,l,w",
        "-v", "{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13}".format(t,t,d,l,l,l,l,l,l,l,l,l,l,w)
        ])


if __name__ == '__main__':
    jobs = []
    args = [[0,2,10,2,0.00396],[1,2,10,2,0.00594],[2,2,10,2,0.00792],[3,2,10,2,0.01],[4,2,10,3,0.0001],[5,2,10,3,0.00396],[6,2,10,2,0.0001]]
    [0,]
    for i in range(7):
        p = multiprocessing.Process(target=worker, args=args[i])
        jobs.append(p)
        p.start()
