import sys

with open('test_suite_template.txt', 'r') as f:
    testcases = sys.stdin.read().split('\n')[:-1]
    statement = '    set_up(); mu_run_test(%s); tear_down();'
    testcases = [statement % test for test in testcases]
    print f.read().replace('{{ testcases }}', '\n'.join(testcases))
