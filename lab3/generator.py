import random
import string
import sys


def random_text(len_of_input):
    res = ''
    for i in range(len_of_input):
        res += random.choice(string.ascii_letters)
    return res


def get_random_key():
    return ''.join(random.choice(string.ascii_letters) for _ in range(3))


if __name__ == "__main__":
    len_of_pattern = random.randint(0, 100)
    pattern = random_text(len_of_pattern)
    count_of_tests = int(sys.argv[1]) + 1
    keys = dict()
    test_file_name = "test.txt"
    len_of_text = 10**count_of_tests
    text = random_text(len_of_text)
    with open(f'{test_file_name}', 'w') as ftest:
        ftest.write(f'{len_of_text}\n{len_of_pattern}\n{text}\n{pattern}\n')

