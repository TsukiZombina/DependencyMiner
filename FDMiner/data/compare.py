path_a = './result.txt'
path_b = './output.txt'

with open(path_a) as f:
  lines_a = set(f.read().splitlines())

with open(path_b) as f:
  lines_b = set(f.read().splitlines())

if lines_a == lines_b:
  print('Okay')
else:
  print("Missed:")
  for a in lines_a:
    if a not in lines_b:
      print(a)
  print("Wrong:")
  for b in lines_b:
    if b not in lines_a:
      print(b)

print(len(lines_a))
print(len(lines_b))

