# Glass Opus

Glass Opus is an open source rotoscoping software for students and artists. The software will provide a variety of features to allow the users to see their work from start to finish. Drawing with a variety brushes and vectors, image manipulation, and filtering are among the many features than one can employ to create their vision.

The focus of Glass Opus, and the team behind it, is to provide a free software that students and artists can use to further their work and portfolio. This is often a difficult endeavor for artists due to the restrictive cost of major softwares. Since Glass Opus is open source, users can tweak features or add their own to suit specific needs. It will also serve as a foundation for those who seek to improve their knowledge in image processing and manipulation, as well as basic graphics programming.

The team is using Microsoft coding guidelines. Found [here](https://www.cise.ufl.edu/~mschneid/Research/C++%20Programming%20Style%20Guidelines.htm) and implemented via [this](https://docs.microsoft.com/en-us/cpp/code-quality/using-the-cpp-core-guidelines-checkers?view=vs-2019https://docs.microsoft.com/en-us/cpp/code-quality/using-the-cpp-core-guidelines-checkers?view=vs-2019).

Visit our [website](https://sccapstone.github.io/SnakyBusiness/) to learn more about Glass Opus and view the demo video. 

## External Requirements

The app be run via the executable in the zip folder under the most recent release.

## Dev Setup

1. Install C++ libraries, specifically <!--either -->[MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/)
2. Install [Qt and Qt Creator](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
<!--3. Install [OpenCV](https://opencv.org/) libraries. [Tutorial](https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows_with_MSVC2017)(https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows) if needed.-->
3. Make sure system path aligns with current installation of <!--OpenCV and--> MSVC. <!--Verify that the includes and libs within the project file are correct.-->
<!--5. Copy the .libs from the OpenCV build folder to the highest/top directory of the build folder.-->
4. Copy the mainMenubar.txt into the Menu folder. If the menu folder is absent, redownload it from the repo. The icons can be accessed the same way.

## Running

There are two options to running the software:  
1. Once Qt <!--and the OpenCV libs have-->has been correctly installed, one must open the project in Qt and press the run button in the bottom left of the Qt gui. (This option requires the Qt libraries to be on your path.)
2. Launch Glass Opus via the Windows start menu, desktop shortcut, or by opening the executable in the installation (or zip folder if used).  

# Testing

Tests are run automatically, verified by the user. Test are used to verify that basic functionality has not been altered or damaged.

## Testing Technology

Testing can only be done in the Debug Release of Glass Opus.

## Running Tests

After launching the Debug Release, add a new Layer, then press the F7 Key. A prompt appears warning the user that running tests will wipe their project and ask them if
they wish to continue. Upon selecting Yes, a list will be brought up with all tests that can be run. Clicking on the name of a test will run that test, then re-prompt
for further testing. Selecting Run All Tests will go through each test one by one. When using Run All Tests, or certain other tests, there will be pop up windows at 
certain intervals, clicking OK on these windows will allow the test to progress. 

When finished testing, simply close the prompt box or click Cancel. 

# Authors

- Auden Childress,  geistfulautomaton@gmail.com,  auden@email.sc.edu
- Ben Kronemeyer,   benjamink1409@gmail.com,      kronemeb@email.sc.edu
- Matthew Pollard,  mattpollard44@gmail.com,      pollarm@email.sc.edu
- Malik Melvin,     melvinmalik15@gmail.com,      mmmelvin@email.sc.edu
- Thomas Wilks,     tpwilk@hotmail.com,           twilks@email.sc.edu

# Examples
![glassOpusGirl](https://user-images.githubusercontent.com/44931507/116170700-4f15b680-a6d5-11eb-851d-850b10630e07.png)
![demo](https://user-images.githubusercontent.com/44931507/116170714-54730100-a6d5-11eb-87f5-9ba522397bc3.png)
![shadowStreet](https://user-images.githubusercontent.com/44931507/116170747-605ec300-a6d5-11eb-832d-2af2a5faf98c.png)


