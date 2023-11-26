project('Scheduling', 'c',
  version : '0.3',
  default_options : ['warning_level=3', 'c_std=c17'])

incdir = include_directories('include')
thirdparty = include_directories('third_party')

executable('scheduler', 'src/scheduler/process.c', 
  'src/argparse.c', 'src/scheduler/scheduler.c',
  include_directories: [incdir, thirdparty])

executable('memory', 'src/memory/memory.c', 'src/memory/replacer.c',
  'src/argparse.c',
  include_directories: [incdir, thirdparty])