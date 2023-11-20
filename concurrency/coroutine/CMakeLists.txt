
message(STATUS "coroutine")

add_executable(eagerFuture eagerFuture.cpp)
#  add_executable(lazyGenerator lazyGenerator.cpp)

add_executable(startJob startJob.cpp)
add_executable(startJobWithComments startJobWithComments.cpp)
add_executable(startJobWithAutomaticResumption startJobWithAutomaticResumption.cpp)
add_executable(startJobWithAutomaticResumptionOnThread startJobWithAutomaticResumptionOnThread.cpp)
add_executable(senderReceiver senderReceiver.cpp)
add_executable(lazy lazy.cpp)
 
add_executable(simple simple.cpp)
