import sys


def create_pattern(length):
    pattern = ""
    for upper in range(ord("A"), ord("Z") + 1):
        for lower in range(ord("a"), ord("z") + 1):
            for digit in range(ord("0"), ord("9") + 1):
                if len(pattern) < length:
                    pattern += chr(upper) + chr(lower) + chr(digit)
                else:
                    return pattern[:length]
    return pattern[:length]


def find_offset(substring, length=10000):
	pattern = create_pattern(length)

	if substring.startswith("0x"):
		try:
			val = int(substring, 16)
			substring = val.to_bytes(4, 'little').decode('latin1')
		except Exception:
			pass
	offset = pattern.find(substring)
	return offset if offset != -1 else None


if __name__ == "__main__":
	mode = sys.argv[1]
	arg = sys.argv[2]
	if mode == "-g":
		print(create_pattern(int(arg)))
	elif mode == "-f":
		print(find_offset(sys.argv[2]))
	else:
		print("Invalid mode. Use -g to generate a pattern or -f to find an offset.")