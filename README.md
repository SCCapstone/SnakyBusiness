# Glass Opus

Glass Opus is an open source rotoscoping software for students and artists. The software will provide a variety of features to allow the users to see their work from start to finish. Drawing with a variety brushes and vectors, image manipuilation and object tracking via an included AI package, and filtering are among the many features than one can employ to create their vision.

The focus of Glass Opus, and the team behind it, is to provide a free software that students and artists can use to further they work and portfolio. This is often a difficult endeavor for artists due to the restrictive cost of major softwares. Since Glass Opus is open source, users can tweak features or add their own to suit specific needs. It will also serve as a foundation for those who seek to improve their knowledge in image processing and manipulation, as well as basic graphics programming.

The team is using Microsoft coding guideleines. Found [here](https://www.cise.ufl.edu/~mschneid/Research/C++%20Programming%20Style%20Guidelines.htm) and implemented via [this](https://docs.microsoft.com/en-us/cpp/code-quality/using-the-cpp-core-guidelines-checkers?view=vs-2019https://docs.microsoft.com/en-us/cpp/code-quality/using-the-cpp-core-guidelines-checkers?view=vs-2019).

## External Requirements

The app should be able to be run by using the executable in the bin folder.

## Setup

1. Install C++ libraries, specifically <!--either -->[MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/)<!-- or [MinGW](http://www.mingw.org/)-->.
2. Install [Qt and Qt Creator](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
3. Install [OpenCV](https://opencv.org/) libraries. [Tutorial](https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows) if needed.
4. Make sure system path aligns with current installation of OpenCV and MSVC. Verify that the includes and libs within the project file are correct.
5. Copy the .libs from the OpenCV build folder to the Qt app build folder.

## Running

There are two options to running the software:  
1. Once Qt and the OpenCV libs have been correctly installed, one must open the project in Qt and press the run button in the bottom left of the Qt gui, or run the executable in the bin folder. 
2. Open the bin folder and run the Roto.exe file.
# Testing

In 492 you will write automated tests. When you do you will need to add a 
section that explains how to run them.

The unit tests are in `/test/unit`.

The behavioral tests are in `/test/casper/`.

## Testing Technology

In some cases you need to install test runners, etc. Explain how.

## Running Tests

Explain how to run the automated tests.

# Authors

- Auden Childress, geistfulautomaton@gmail.com, auden@email.sc.edu
- Ben Kronemeyer, benjamink1409@gmail.com, kronemeb@email.sc.edu  
- Your names and emails
