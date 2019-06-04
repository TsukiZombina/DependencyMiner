import multiprocessing
import subprocess


def worker(id, l, t, d, w):
    print("Executing {} with l = {}, t = {}, d = {}, w = {}".format(id, l, t, d, w))
    prefix   = "sim_"
    filename = prefix + "w=" + str(w) + "_l=" + str(l) + "_t=" + str(t) + "_d=" + str(d)
    subprocess.run(["../build/DependencyMiner.exe",
        "-f", "../Delitos_cleaned.csv",
        "-o", "../../../../Dropbox/Masters/Resultados/Similarity/" + filename + "_" + str(id),
        "-s", "1",
        "-a", "0,1,2,3,4,5,6,7,8,9,10,11,12,13",
        "-m", "t,t,d,l,l,l,l,l,l,l,l,l,l,w",
        "-v", "{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13}".format(t,t,d,l,l,l,l,l,l,l,l,l,l,w)
        ])


if __name__ == '__main__':
    jobs = []
    args = [[0,1,10,3,0.00594],[1,1,10,2,0.00396],[2,1,10,2,0.00594],[3,1,10,2,0.00792],[4,1,10,2,0.01],[5,1,10,3,0.0001],[6,1,10,3,0.00396]]
    for i in range(7):
        p = multiprocessing.Process(target=worker, args=args[i])
        jobs.append(p)
        p.start()
