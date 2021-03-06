Prayer Times Plasmoid
=====================


This project brings you a plasmoid + data engine for calculating and displaying the Islamic
prayer times and qibla (direction to Makkah) for your current location.

Please visit the [[Screenshots]] page for getting a glimpse of how it actually looks.



## Features ##

* Prayer times based on your current location
* Qibla direction
* Notifications before a new prayer and upon entering

Please have a look at the [[ToDo]]s for features, that are missing.



## Build instructions ###

For compiling and using this plasmoid you will need:

* KDE 4.6
* Marble
* Islamic Tools and Libraries (ITL)

### Install needed packages ###

When using Debian or (K)Ubuntu you can install the necessary packages with
    sudo aptitude install build-essential debhelper devscripts cmake kdelibs5-dev libmarble-dev libitl-dev

### Build it ###

If you are using Debian or (K)Ubuntu you can build a .dep package and install it:

    cd /where/you/downloaded/the/plasmoid
    ./builddeb.sh
    sudo dpkg -i ../prayer-times-plasmoid*.deb

Users of other systems please follow the general recipe:

    cd /where/you/downloaded/the/plasmoid
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ..
    make
    make install

### Use it ###

To load the applet you need to restart Plasma.

    kbuildsycoca4
    kquitapp plasma-desktop
    plasma-desktop

Now you can add it to your desktop or you can also view it with the Plasmoid Viewer.

    plasmoidviewer prayertimes



## Code documentation ##

To get a better overview over the source code you can have the code documentation generated in HTML format.

### Install needed packages ###

When using Debian or (K)Ubuntu you can install the necessary packages with
    sudo aptitude install doxygen

### Generate it ###

    cd /where/you/downloaded/the/plasmoid
    doxygen Doxyfile

You can now browse the documentation starting in the doc/html/index.html file.



## Contact and Support ##

You can find the main repository at:

https://github.com/riyad/prayer-times-plasmoid

If have come a across any bug or have suggestions please feel free to mail the authors.
