project('AsukaOSLab', 'c',
  version : '0.6',
  default_options : ['warning_level=3', 'c_std=c17'])

incdir = include_directories('include')
thirdparty = include_directories('third_party')

if meson.get_compiler('c').get_id() == 'clang'
  extra_args = ['-fsanitize=address', '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls']
else
  extra_args = []
endif

executable('scheduler', 'src/scheduler/process.c', 
  'src/argparse.c', 'src/scheduler/scheduler.c',
  include_directories: [incdir, thirdparty], c_args: extra_args)

executable('memory', 'src/memory/memory.c', 'src/memory/replacer.c',
  'src/argparse.c', 'src/memory/buffer_manager.c',
  include_directories: [incdir, thirdparty], c_args: extra_args)

disk_test = executable('disk_test', 'tests/disk_test.c', 'src/sfs/disk.c',
  include_directories: [incdir, thirdparty], c_args: extra_args)

test('BasicDiskTest', disk_test)