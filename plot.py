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
    parser.add_argument('path', type=str, default='data', help='data prefix')
    args = parser.parse_args()

    filename = args.path
    x = parse_file(filename)
    plt.plot(x, range(len(x)))

    plt.show()