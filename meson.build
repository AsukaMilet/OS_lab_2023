project('Scheduling', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c17'])

incdir = include_directories('include')

scheduler = executable('scheduler', 'src/scheduler/main.c', 
  'src/argparse.c', 'src/scheduler/scheduler.c',
  include_directories: incdir, install : false)

test('scheduler_test', scheduler)