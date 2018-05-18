from tqdm import tqdm

def main():
  data_path = './data.txt'
  # data_path = './test_data.txt'
  with open(data_path) as f:
    rows = f.read().splitlines()
  data = []
  for r in rows:
    tmp = []
    left = 0
    for right, ch in enumerate(r):
      if ch == ',' and r[right+1] != ' ':
        tmp.append(r[left:right])
        left = right + 1
    data.append(tmp)
  for i in range(1, len(data)):
    assert len(data[i]) == len(data[i-1])

  lhs = [1, 2, 12]
  rhs = 0
  mem = {}
  for ridx, r in tqdm(enumerate(data)):
    key = []
    for col in lhs:
      key.append(r[col])
    key = tuple(key)
    if key in mem.keys() and mem[key] != r[rhs]:
      print('ERROR: line %d' % ridx)
      break
    mem[key] = r[rhs]

if __name__ == '__main__':
  main()

