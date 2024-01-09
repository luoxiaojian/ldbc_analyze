import matplotlib.pyplot as plt
import argparse
def parse_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
        x = []
        val = 0
        for line in lines:
            if val == 0:
                x.append(1.0)
                val = float(line.strip())
            else:
                cur = float(line.strip())
                x.append(float(cur) / float(val))
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
        filename = prefix + str(i + 1)
        x = parse_file(filename)
        max_length = max(max_length, len(x))
        data.append(x)
        legends.append(str(i + 1))

    for i in range(n):
        y = data[i]
        if (len(y) < max_length):
            y += [y[-1]] * (max_length - len(y))
        plt.plot(range(max_length), y)

    plt.legend(legends)

    plt.show()
