# CMake generated Testfile for 
# Source directory: /Users/liuhanyu1030/Documents/audiosoftware/AudioSoftwareHW/Assignment_2
# Build directory: /Users/liuhanyu1030/Documents/audiosoftware/AudioSoftwareHW/Assignment_2/bld
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(RingBuff "/Users/liuhanyu1030/Documents/audiosoftware/AudioSoftwareHW/Assignment_2/bin/TestExec" "RingBuff")
add_test(AudioIo "/Users/liuhanyu1030/Documents/audiosoftware/AudioSoftwareHW/Assignment_2/bin/TestExec" "AudioIo" "/Users/liuhanyu1030/Documents/audiosoftware/AudioSoftwareHW/Assignment_2/src/Tests/TestData/")
add_test(CombFilter "/Users/liuhanyu1030/Documents/audiosoftware/AudioSoftwareHW/Assignment_2/bin/TestExec" "CombFilter")
add_test(Dtw "/Users/liuhanyu1030/Documents/audiosoftware/AudioSoftwareHW/Assignment_2/bin/TestExec" "Dtw")
add_test(Vibrato "/Users/liuhanyu1030/Documents/audiosoftware/AudioSoftwareHW/Assignment_2/bin/TestExec" "Vibrato")
subdirs("MUSI6106Exec")
subdirs("AudioFileIO")
subdirs("CombFilter")
subdirs("TestExec")
