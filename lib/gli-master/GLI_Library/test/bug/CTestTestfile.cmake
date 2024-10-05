# CMake generated Testfile for 
# Source directory: C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug
# Build directory: C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/GLI_Library/test/bug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(test-bug "C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/GLI_Library/test/bug/Debug/test-bug.exe")
  set_tests_properties(test-bug PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/CMakeLists.txt;22;add_test;C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug/CMakeLists.txt;1;glmCreateTestGTC;C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(test-bug "C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/GLI_Library/test/bug/Release/test-bug.exe")
  set_tests_properties(test-bug PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/CMakeLists.txt;22;add_test;C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug/CMakeLists.txt;1;glmCreateTestGTC;C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(test-bug "C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/GLI_Library/test/bug/MinSizeRel/test-bug.exe")
  set_tests_properties(test-bug PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/CMakeLists.txt;22;add_test;C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug/CMakeLists.txt;1;glmCreateTestGTC;C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(test-bug "C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/GLI_Library/test/bug/RelWithDebInfo/test-bug.exe")
  set_tests_properties(test-bug PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/CMakeLists.txt;22;add_test;C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug/CMakeLists.txt;1;glmCreateTestGTC;C:/Users/User/Documents/GitHub/Boof-Woof/lib/gli-master/gli-master/test/bug/CMakeLists.txt;0;")
else()
  add_test(test-bug NOT_AVAILABLE)
endif()
