import matplotlib.pyplot as plt
import argparse
def parse_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
        x = []
        for line in lines:
            line = line.strip().split()
            x.append(int(line[0]))
        return x

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--prefix', type=str, default='data', help='data prefix')
    parser.add_argument('-n', type=int, default=100, help='data number')
    args = parser.parse_args()

    prefix = args.prefix
    n = args.n
    data = []
    max_length = 0

    legends = []

    for i in range(n):
        filename = prefix + str(i)
        x = parse_file(filename)
        max_length = max(max_length, len(x))
        data.append(x)
        legends.append(str(i))

    for i in range(n):
        x = data[i]
        if (len(x) < max_length):
            x += [x[-1]] * (max_length - len(x))
        plt.plot(x, range(max_length))

    plt.show()
