cmake_minimum_required(VERSION 2.8)
project( tictactoe )
find_package( OpenCV REQUIRED )
add_executable( tictactoe PlayGameMain.cpp zrarm.c iic.c uArmMovements.c arucofidmarkers.cpp board.cpp boarddetector.cpp cameraparameters.cpp cvdrawingutils.cpp marker.cpp markerdetector.cpp uarm.c xil_io.c )
target_link_libraries( tictactoe ${OpenCV_LIBS} )

