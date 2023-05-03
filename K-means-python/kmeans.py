import sys

print("K:", sys.argv[1])
print("iter:", sys.argv[2])
with open(sys.argv[3]) as file:
    for line in file:
        print(line.rstrip())
