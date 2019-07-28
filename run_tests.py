import glob
import json
import pathlib
import struct
import subprocess
import sys

import math

hide_successes = True
test_dir = 'test/'
test_build_dir = 'test/build/'
test_binary = 'cmake-build-debug/wasm_interpreter'

skip_suites = [
    'names',
]


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


PREFIX_OK = "[" + bcolors.OKGREEN + " OK " + bcolors.ENDC + "]"
PREFIX_WARN = "[" + bcolors.WARNING + "WARN" + bcolors.ENDC + "]"
PREFIX_FAIL = "[" + bcolors.FAIL + "FAIL" + bcolors.ENDC + "]"

counts = {
    'total': 0,
    'skipped': 0,
    'failed': 0,
    'successful': 0,
}


def invoke_test(test_binary, module, fn, args):
    runner_args = [test_binary]
    runner_args += ["-p", module]
    runner_args += ["-f", fn]

    for arg in args:
        runner_args += ["-a", arg['type'] + ":" + arg['value']]

    try:
        result = subprocess.run(runner_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=1)
        if result.returncode != 0:
            return runner_args, result.stderr.decode('utf-8').rstrip("\r\n\t "), True
        else:
            return runner_args, result.stdout.decode('utf-8').rstrip("\r\n\t "), False
    except subprocess.TimeoutExpired as e:
        return runner_args, "invocation timed out", True


def stringify_call(fn, args):
    return fn + '(' + ', '.join([arg['type'] + ':' + arg['value'] for arg in args]) + ')'


def stringify_result(command):
    if command['type'] == 'assert_return':
        if len(command['expected']) == 0:
            return 'void'
        return command['expected'][0]['type'] + ':' + command['expected'][0]['value']
    if command['type'] == 'assert_return_arithmetic_nan':
        return 'arithmetic_nan'
    if command['type'] == 'assert_return_canonical_nan':
        return 'canonical_nan'


def check_result(actual, command):
    if command['type'] == 'assert_return':
        if len(command['expected']) == 0:
            return actual == 'void'
        return actual == (command['expected'][0]['type'] + ':' + command['expected'][0]['value'])
    if command['type'] == 'assert_return_arithmetic_nan' or command['type'] == 'assert_return_canonical_nan':
        t = actual.split(":")[0]
        v = int(actual.split(":")[1])
        if t == 'f32':
            f = struct.unpack('f', struct.pack('I', v))[0]
        elif t == 'f64':
            f = struct.unpack('d', struct.pack('Q', v))[0]
        else:
            return False
        return math.isnan(f)


def run_suite(name, test_suite, test_binaries):
    global counts

    source_filename = test_suite['source_filename']
    current_module = None

    for command in test_suite['commands']:
        line = command['line']
        line_str = source_filename + ':' + str(line) + ': '

        if command['type'] == 'module':
            current_module = command['filename']
            continue

        counts['total'] += 1

        if name in skip_suites:
            print(PREFIX_WARN + " " + line_str + "Skipping test due to exclude " + name)
            counts['skipped'] += 1
            continue

        if command['type'] == 'assert_return' or command['type'] == 'assert_return_canonical_nan' or command[
            'type'] == 'assert_return_arithmetic_nan':
            action = command['action']
            if action['type'] == 'invoke':
                run_cmd, result, failed = invoke_test(test_binary, test_binaries + current_module, action['field'],
                                                      action['args'])

                if not check_result(result, command):
                    failed = True

                if failed:
                    counts['failed'] += 1
                    prefix = PREFIX_FAIL
                else:
                    counts['successful'] += 1
                    prefix = PREFIX_OK

                if failed or (not failed and not hide_successes):
                    print(prefix + " " + line_str + stringify_call(action['field'],
                                                                   action['args']) + ": expected=" + stringify_result(
                        command) + ", actual=" + result + ". Command: \"" + " ".join(run_cmd) + "\"")
            else:
                print(PREFIX_WARN + " " + line_str + "Skipping test with action type " + action[
                    'type'] + "(" + action.__str__() + ")" + bcolors.ENDC)
                counts['skipped'] += 1

            continue

        print(PREFIX_WARN + " " + line_str + "Skipping test with command type " + command[
            'type'] + " (" + command.__str__() + ")" + bcolors.ENDC)
        counts['skipped'] += 1


def main():
    included_modules = []

    if len(sys.argv) == 2:
        included_modules = sys.argv[1].split(",")

    for wast_file in glob.glob(test_dir + "*.wast"):
        suite_name = pathlib.Path(wast_file).stem
        if len(included_modules) == 0 or suite_name in included_modules:
            with open(test_build_dir + suite_name + '/' + suite_name + '.json') as test_suite:
                run_suite(suite_name, json.load(test_suite), test_build_dir + suite_name + '/')

    print("Found " + str(counts['total']) + " tests")
    print("\tSkipped: " + str(counts['skipped']))
    print("\tFailed: " + str(counts['failed']))
    print("\tSuccessful: " + str(counts['successful']))


if __name__ == "__main__":
    main()
