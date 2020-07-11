TEMPLATE = subdirs

unix:!macx:SUBDIRS += \
    cli \
    gui \
    lib

else:SUBDIRS += \
    gui \
    lib

cli.depends = lib
gui.depends = lib
