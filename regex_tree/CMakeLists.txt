set(TREE_INCLUDE regex_tree.h)
set(TREE_SOURCE  regex_tree.cpp)

add_executable(test_tree test_tree.cpp ${TREE_INCLUDE} ${TREE_SOURCE})

