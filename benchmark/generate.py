import sys
import random

def valid(word, length):
    return (len(word) == length and word.isalpha() and word.islower()
                                and len(word) == len(set(word)))

def generate_word_func(words):
    func = 'tuple Word getWord(int n) {\n        tuple Word result;\n'
    func += '        result = initWord();\n'
    for index, word in enumerate(words):
        s = '        if (n == %d) {\n' % index
        s += '            result.word = "%s";\n' % word
        for i, l in enumerate(word):
            s += '            result.index%s = %d;\n' % (l, i)
        s += '        }\n'
        func += s
    func += '        return result;\n    }\n'
    return func
    
if __name__ == '__main__':
    usage = 'usage: python generate.py word-length amount-to-generate'
    if len(sys.argv) != 3:
        print usage
        sys.exit(1)
    length = int(sys.argv[1])
    amount = int(sys.argv[2])
    with open('/usr/share/dict/words', 'r') as f:
        words = [w.upper() for w in f.read().split() if valid(w, length)]
        words = random.sample(words, amount)
    with open('hangman.wig', 'w') as out, open('hangman.stub', 'r') as stub:
        print >>out, stub.read().replace('<<W>>', generate_word_func(words))

