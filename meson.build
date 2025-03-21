project('irlightcontrol', 'c', 'cpp',
  version : '0.1',
  default_options : ['werror=true', 'warning_level=3', 'cpp_std=c++17', 'c_std=c11', 'buildtype=release'])

headersDir = include_directories(['src/h', '../boost_1_82_0'])

pigpio_dep = dependency('libpigpio', required : true)

irlightcontrol_dep = [pigpio_dep]

irlightcontrol_src = [
   'src/cpp/Main.cpp',
   'src/cpp/network/Connection.cpp',
   'src/cpp/IrLightControl.cpp',
   'src/cpp/network/TcpConnection.cpp',
   'src/cpp/network/TcpServer.cpp',
   'src/cpp/Logging.cpp']

executable(
   'irlightcontrol', 
   irlightcontrol_src, 
   dependencies : irlightcontrol_dep,
   include_directories : headersDir)
