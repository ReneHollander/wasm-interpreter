import glob
import json
import pathlib
import subprocess
import sys

test_dir = 'test/'
test_build_dir = 'test/build/'
test_binary = 'cmake-build-debug/test_runner'

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


counts = {
    'total': 0,
    'skipped': 0,
    'failed': 0,
    'successful': 0,
}


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
            print(bcolors.WARNING + line_str + "Skipping test due to exclude " + name + bcolors.ENDC)
            counts['skipped'] += 1
            continue

        if command['type'] == 'assert_return':
            action = command['action']
            if action['type'] == 'invoke':
                runner_args = [
                    test_binary
                ]
                runner_args += [
                    "-p", test_binaries + current_module
                ]
                field = action['field']
                runner_args += ["-f", field]
                return_value_string = 'void'
                if len(command['expected']) != 0:
                    expected_type = command['expected'][0]['type']
                    expected_value = command['expected'][0]['value']
                    return_value_string = expected_type + ':' + expected_value

                args = []
                for arg in action['args']:
                    type = arg['type']
                    value = arg['value']
                    args.append(type + ':' + value)
                    runner_args += ["-a", type + ":" + value]

                printed_call = line_str + field + '(' + ', '.join(args) + ') != ' + return_value_string

                try:
                    result = subprocess.run(runner_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=1)
                    if result.returncode != 0:
                        counts['failed'] += 1
                        return_str = result.stderr.decode('utf-8').rstrip("\r\n\t ")
                        print(bcolors.FAIL + printed_call + bcolors.ENDC + ", actual " + return_str)
                    else:
                        return_value = result.stdout.decode('utf-8').rstrip("\r\n\t ")
                        if return_value_string != return_value:
                            counts['failed'] += 1
                            print(bcolors.FAIL + printed_call + bcolors.ENDC + ", actual " + return_value)
                        else:
                            counts['successful'] += 1
                            print(bcolors.OKGREEN + printed_call + bcolors.ENDC)
                except subprocess.TimeoutExpired as e:
                    counts['failed'] += 1
                    print(bcolors.FAIL + printed_call + bcolors.ENDC + ", test timed out")
            else:
                print(bcolors.WARNING + line_str + "Skipping test with action type " + action['type'] + bcolors.ENDC)
                counts['skipped'] += 1

            continue

        print(bcolors.WARNING + line_str + "Skipping test with command type " + command['type'] + bcolors.ENDC)
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
