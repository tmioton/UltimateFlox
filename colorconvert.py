#!/usr/bin/python3

from sys import argv, stderr


def convert(code, floating=False) -> int:
    if len(code) != 6:
        print("Proper hex color code expected.", file=stderr)
        return 1

    codes = tuple(map('0x{0}'.format, tuple(code[i * 2:i * 2 + 2] for i in range(3))))
    values = []
    for i, individual in enumerate(codes):
        try:
            values.append(round(int(individual, base=16) / 0xFF, 5))
        except ValueError:
            print(f"Invalid hex code:\n{' '.join(codes)}\n{(' ' * (i * 5))}^^^^", file=stderr)
            return 1
    print(f", ".join(map(('{0}f' if floating else '{0}').format, values)))
    return 0


if __name__ == '__main__':
    if len(argv) < 2:
        print("Hex color code expected as argument.", file=stderr)
        exit(1)
    else:
        floating = False
        if argv[1] == "-f":
            floating = True
            argv.pop(1)
        elif argv[1] == "-d":
            floating = False
            argv.pop(1)
        exit(convert(argv[1], floating))
